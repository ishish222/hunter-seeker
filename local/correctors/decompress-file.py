#!/usr/bin/env python


from mmap import mmap
import struct

class TerminateException(Exception):
    pass

class NotLLException(Exception):
    pass

key = [0x29, 0x23, 0xBE, 0x84, 0xE1, 0x6C, 0xD6, 0xAE, 
        0x52, 0x90, 0x49, 0xF1, 0xF1, 0xBB, 0xE9, 0xEB, 
        0xB3, 0xA6, 0xDB, 0x3C, 0x87, 0x0C, 0x3E, 0x99, 
        0x24, 0x5E, 0x0D, 0x1C, 0x06, 0xB7, 0x47, 0xDE, 
        0xB3, 0x12, 0x4D, 0xC8, 0x43, 0xBB, 0x8B, 0xA6, 
        0x1F, 0x03, 0x5A, 0x7D, 0x09, 0x38, 0x25, 0x1F, 
        0x5D, 0xD4, 0xCB, 0xFC, 0x96, 0xF5, 0x45, 0x3B, 
        0x13, 0x0D, 0x89, 0x0A, 0x1C, 0xDB, 0xAE, 0x32, 
        0x20, 0x9A, 0x50, 0xEE, 0x40, 0x78, 0x36, 0xFD, 
        0x12, 0x49, 0x32, 0xF6, 0x9E, 0x7D, 0x49, 0xDC, 
        0xAD, 0x4F, 0x14, 0xF2, 0x44, 0x40, 0x66, 0xD0, 
        0x6B, 0xC4, 0x30, 0xB7]

def litLength(lV, fmap):
    global cCur

    if(0x01 <= lV <= 0x0e):
        return lV + 3

    if((lV & 0xf0) != 0):
        cCur -=1
        return 0

    if(lV == 0x0):
        total = 0xf
        while True:
            nlV = ord(fmap[cCur])
            cCur +=1
            
            if(nlV == 0x0):
                total += 0xff
                continue
            else:
                total += nlV
                break

        return total +3

    return -1

def twoByteOffset(fmap):
    global cCur

    fB = ord(fmap[cCur])
    cCur +=1
    sB = ord(fmap[cCur])
    cCur +=1

    cO = (fB >> 2) | (sB << 6)
    lV = (fB & 0x3)
    return (cO, lV)

def longCompressionOffset(fmap):
    global cCur

    cB = ord(fmap[cCur])
    cCur +=1

    if(cB == 0x0):
        cB = 0xff
        while True:
            nB = ord(fmap[cCur])
            cCur +=1
            if(nB == 0x0):
                cB += 0xff
                continue
            else:
                cB += nB
                break

    return cB

def decode_opcode(fmap):
    global cCur

    opc1 = ord(fmap[cCur])
    print(hex(opc1))
    cCur += 1

    if(opc1 < 0x10):
        cCur += 1
        return (0, 0, opc1 + 3)

    if(opc1 == 0x10):
        cB = longCompressionOffset(fmap) + 9
        (cO, lV) = twoByteOffset(fmap)
        if(lV == 0x0):
            lV = ord(fmap[cCur])
            cCur += 1
            lC = litLength(lV, fmap)
        else:
            lC = lV
        
    if(opc1 == 0x11):
        raise TerminateException

    if(0x12 <= opc1 <= 0x1f):
        print("0x12 <= opc1 <= 0x1f")
        cB = (opc1 & 0xf) +2
        (cO, lV) = twoByteOffset(fmap)
        cO += 0x3fff
        if(lV == 0x0):
            lV = ord(fmap[cCur])
            cCur += 1
            lC = litLength(lV, fmap)
        else:
            lC = lV
        
    if(opc1 == 0x20):
        print("opc1 == 0x20")
        cB = longCompressionOffset(fmap)
        (cO, lV) = twoByteOffset(fmap)
        if(lV == 0x0):
            lV = ord(fmap[cCur])
            cCur += 1
            lC = litLength(lV, fmap)
        else:
            lC = lV

    if(0x21 <= opc1 <= 0x3f):
        print("0x21 <= opc1 <= 0x3f")
        cB = opc1 - 0x1e
        (cO, lV) = twoByteOffset(fmap)
        if(lV == 0x0):
            lV = ord(fmap[cCur])
            cCur += 1
            lC = litLength(lV, fmap)
        else:
            lC = lV

    if(0x40 <= opc1 <= 0xff):
#        print("0x40 <= opc1 <= 0xff")
        opc2 = ord(fmap[cCur])
        cCur += 1

        cB = ((opc1 & 0xf0) >> 4)-1
        cO = (opc2 << 2) | ((opc1 & 0x0c) >> 2)
        lV = (opc1 & 0x03)
        if(lV == 0x0):
            lV = ord(fmap[cCur])
            cCur += 1
            lC = litLength(lV, fmap)
        else: 
            lC = lV

    return (cB, cO, lC)


def decompress2(fmap, off):
    global cCur
    cCur = off
    dArr = []

    cByte = fmap[cCur]
    if(cByte <= 0x11):
        pass
        

def decompress(fmap, off):
    global cCur
    cCur = off
    dArr = []

    lV = ord(fmap[cCur])
    cCur +=1

    lC = litLength(lV, fmap)
    dArr += fmap[cCur:cCur + lC]
    cCur += lC

    while True:
        here = cCur
        try:
            (cB, cO, lC) = decode_opcode(fmap)
        except TerminateException:
            print("Finished")
            break
        except Exception as e:
            print("Error")
            print(e)
            break
#        dArr += dArr[len(dArr)-cO-1:len(dArr)-cO+cB-1]
        for i in range(0, cB):
            dArr += dArr[len(dArr)-cO-1]
#        dArr += fmap[cCur-1:cCur+lC]
        dArr += fmap[cCur:cCur+lC]
        cCur += lC

    print("Length: " + str(hex(len(dArr))))
    print("Decompressed: ")

    for i in range(0, len(dArr)):
        print(hex(ord(dArr[i])) + "\t", end='')
        if((i+1) % 0x8 == 0):
            print("")

    print("")
    return dArr

def extractHdrData(encData):
    decData = []

    for i in range(0, 0x6c):
        print(hex(ord(encData[i]) ^ key[i % len(key)]) + "\t", end='')
        decData.append(chr(ord(encData[i]) ^ key[i % len(key)]))
        if((i+1) % 0x4 == 0):
            print("")

    print("")
    print("")
    return decData

import sys

if(len(sys.argv) < 2):
    print("plik")
    quit()

f = sys.argv[1]
of = f + ".dec"

encData = []

try:
    fd = open(f, "r+b")
    ofd = open(of, "w+b")
    fmap = mmap(fd.fileno(), 0)

    ofd.write("".join(decompress(fmap, 0)))


except Exception as e:
    print(e)

    fd.close()
    ofd.close()    
