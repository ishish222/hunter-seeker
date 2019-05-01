#!/usr/bin/env python 


import sys
from mmap import mmap
import struct

class EndOfSections(Exception):
    pass

first_sect_off = 0x100

def hex2(n):
#    ret = hex(n & 0xffffffff)[:-1]
#    if(ret == "0x"):
#        ret = "0x0"
#    return ret
    return hex(n)

def checksum_s_sect(size):
    global cCur
    global fmap
    
    seed = 0x0
    cCur2 = cCur
    cCur2 += 0x20

    print("Checksum")
    print("cCur: " + hex(cCur2))
    print("size: " + hex(size))

    sum1 = seed & 0xffff
    sum2 = seed >> 0x10
    s_size = size
    
    while (s_size != 0x0):
        chunkSize = min(0x15b0, s_size)
        s_size -= chunkSize
        for i in range(0, chunkSize):
            byte = ord(struct.unpack("<c", "".join(fmap[cCur2]))[0])
            cCur2 += 1
            sum1 += byte
            sum2 += sum1
        sum1 %= 0xfff1
        sum2 %= 0xfff1

    summ = (sum2 << 0x10) | (sum1 & 0xffff)
    return summ & 0xffffffff
 

def check_s_sect():
    global cCur
    global fmap

    try:
        header = int(struct.unpack("<i", "".join(fmap[cCur+0x0:cCur+0x4]))[0])
    except struct.error:
        raise EndOfSections
        
    if((header == 0x41630e3b) or (header == 0x4163003b)):
        return True
    else:
        return False    

def check_d_sect():
    global cCur
    global fmap

    try:
        header = int(struct.unpack("<i", "".join(fmap[cCur+0x0:cCur+0x4]))[0])
    except struct.error:
        raise EndOfSections

    mask = 0x4164536b ^ cCur
    header ^= mask
#    print("Header: "+ hex2(header))
    if(header == 0x4163043b):
        return True
    else:
        return False    

def decrypt_d_sect_hdr():
    global fmap
    global cCur
    
    seed = 0x4164536b
    mask = seed ^ cCur
    dArr = []

    for i in range(0, 0x8):
        my_off = cCur + (i * 0x4)
        cur = int(struct.unpack("<i", "".join(fmap[my_off+0x0:my_off+0x4]))[0]) & 0xffffffff
        cur ^= mask
        dArr.append(cur)
#        print(hex2(cur))

    return dArr

def dump_s_sect_hdr():
    global fmap
    global cCur

    dArr = []

    for i in range(0, 0x5):
        my_off = cCur + (i * 0x4)
        cur = int(struct.unpack("<i", "".join(fmap[my_off+0x0:my_off+0x4]))[0]) & 0xffffffff
        dArr.append(cur)
#        print(hex2(cur))

    return dArr


if(len(sys.argv)<2):
    print("File")
    quit()

global fmap
global cCur

cCur = first_sect_off

f = sys.argv[1]
fd = open(f, "r+b")
fmap = mmap(fd.fileno(), 0)

#prev = first = decrypt_d_sect_hdr()

print("")

while True:
    try:
        if(check_d_sect()):
            print("Offset: " + hex2(cCur))
            print("Type: Data")
            nextt = decrypt_d_sect_hdr()
            size = nextt[3]
            cSize = nextt[2]
            checksum1 = nextt[7] 
#            print("Here: " + hex(size))
            checksum2 = checksum_s_sect(cSize)
#            print("Current checksum: " + hex(checksum1))
#            print("Calculated checksum: " + hex(checksum2))
            if(checksum1 == checksum2):
                print("[x] Correct")
                print("Current checksum: " + hex(checksum1))
            else:
                print("[x] Incorrect")
                print("Current checksum: " + hex(checksum1))
                print("Calculated checksum: " + hex(checksum2))
            cCur += size
        else:
            if(check_s_sect()):
                print("Offset: " + hex2(cCur))
                print("Type: System")
                nextt = dump_s_sect_hdr()
                cCur += nextt[1]
            else:
                print("Looks like final section, bye")
                break
#        print("Next: " + hex2(cCur))
        print("- - - -")
    except EndOfSections as e:
        print("Finished")
        break
    except Exception as e:
        print(e)
        print(type(e))
        break

