#!/usr/bin/env python 

from __future__ import print_function
import sys
from mmap import mmap
import struct

class EndOfSections(Exception):
    pass

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
    if(header == 0x4163043b):
        return True
    else:
        return False    

class Section(object):
    def __init__(self, offset_ = 0x0):
        self.offset = offset_

class SystemSection(Section):
    def decode(self):
#        print("Offset: " + hex(self.offset))
#        print("Type: System")
        dArr = []

        ptr = self.offset

        for i in range(0, 0x5):
            ptr = self.offset + (i * 0x4)
            cur = int(struct.unpack("<i", "".join(fmap[ptr+0x0:ptr+0x4]))[0]) & 0xffffffff
            dArr.append(cur)

        self.dArr = dArr

        self.signature = self.dArr[0x0]
        self.dSize = self.dArr[0x1]
        self.cSize = self.dArr[0x2]
        self.checksum = self.dArr[0x4]
        self.dataOffset = self.offset + 0x14

    def calc_checksum(self):
        global fmap
            
        #stage 1

        hdr = []
        hdr += fmap[self.offset:self.offset+0x10]
        hdr += ["\x00", "\x00", "\x00", "\x00"]
        
        seed = 0x0
        size = 0x14

        sum1 = seed & 0xffff
        sum2 = seed >> 0x10

        ptr = 0x0

        while (size != 0x0):
            chunkSize = min(0x15b0, size)
            size -= chunkSize
            for i in range(0, chunkSize):
                byte = ord(struct.unpack("<c", "".join(hdr[ptr]))[0])
                ptr += 1
                sum1 += byte
                sum2 += sum1
            sum1 %= 0xfff1
            sum2 %= 0xfff1

        summ = (sum2 << 0x10) | (sum1 & 0xffff)

        #stage 2

        seed = summ
        size = self.cSize
        ptr = self.offset + 0x14

        while (size != 0x0):
            chunkSize = min(0x15b0, size)
            size -= chunkSize
            for i in range(0, chunkSize):
                byte = ord(struct.unpack("<c", "".join(fmap[ptr]))[0])
                ptr += 1
                sum1 += byte
                sum2 += sum1
            sum1 %= 0xfff1
            sum2 %= 0xfff1
        
        summ2 = (sum2 << 0x10) | (sum1 & 0xffff)
        self.calcChecksum = summ2 
        return summ2

    def set_new_checksum(self):
        global fmap

        new_sum = self.calcChecksum

        #convert to signed
        if(new_sum & 0x80000000):
            new_sum = -0x100000000 + new_sum

        pack = struct.pack("<i", new_sum)
        fmap[self.offset + 0x10 + 0x0] = pack[0x0]
        fmap[self.offset + 0x10 + 0x1] = pack[0x1]
        fmap[self.offset + 0x10 + 0x2] = pack[0x2]
        fmap[self.offset + 0x10 + 0x3] = pack[0x3]
        
class DataSection(Section):
    def decode(self):
#        print("Offset: " + hex(self.offset))
#        print("Type: Data")
        self.decrypt()
#        print(self.dArr)
        self.signature = self.dArr[0x0]
        self.number = self.dArr[0x1]
        self.cSize = self.dArr[0x2]
        self.dSize = self.dArr[0x3]
        self.checksum = self.dArr[0x7]
        self.dataOffset = self.offset + 0x20

    def calc_checksum(self):
        global fmap
        
        seed = 0x0

        sum1 = seed & 0xffff
        sum2 = seed >> 0x10
        s_size = self.cSize

        ptr = self.dataOffset

        while (s_size != 0x0):
            chunkSize = min(0x15b0, s_size)
            s_size -= chunkSize
            for i in range(0, chunkSize):
                byte = ord(struct.unpack("<c", "".join(fmap[ptr]))[0])
                ptr += 1
                sum1 += byte
                sum2 += sum1
            sum1 %= 0xfff1
            sum2 %= 0xfff1

        summ = (sum2 << 0x10) | (sum1 & 0xffff)
        self.calcChecksum = summ & 0xffffffff
        return summ & 0xffffffff

    def set_new_checksum(self):
        global fmap
        
        new_sum = self.calcChecksum ^ self.mask
    
        #convert to signed
        if(new_sum & 0x80000000):
            new_sum = -0x100000000 + new_sum

        pack = struct.pack("<i", new_sum)
        fmap[self.offset + 0x1c + 0x0] = pack[0x0]
        fmap[self.offset + 0x1c + 0x1] = pack[0x1]
        fmap[self.offset + 0x1c + 0x2] = pack[0x2]
        fmap[self.offset + 0x1c + 0x3] = pack[0x3]
        
    def decrypt(self):
        global fmap

        self.mask = 0x4164536b ^ self.offset
        dArr = []
        
        ptr = self.offset

        for i in range(0, 0x8):
            ptr = self.offset + (i * 0x4)
            cur = int(struct.unpack("<i", "".join(fmap[ptr+0x0:ptr+0x4]))[0]) & 0xffffffff
            cur ^= self.mask
            dArr.append(cur)

        self.dArr = dArr
        return dArr

first_sect_off = 0x100

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
            sect = DataSection(cCur)
            sect.decode()
            sect.calc_checksum()
            print("Data section at " + hex(sect.offset) + " ", end="")
            if(sect.calcChecksum == sect.checksum):
                print("\t[x] ")
            else:
                print("\t[i] ", end="")
                print("- should be: " + hex(sect.calcChecksum), end="")
                sect.set_new_checksum()
                print(" (corrected)")
            cCur += sect.dSize
        else:
            if(check_s_sect()):
                sect = SystemSection(cCur)
                sect.decode()
                sect.calc_checksum()
                print("System section at " + hex(sect.offset) + " ", end="")
                if(sect.calcChecksum == sect.checksum):
                    print("\t[x] ")
                else:
                    print("\t[i] ", end=" ")
                    print("- should be: " + hex(sect.calcChecksum), end="")
                    sect.set_new_checksum()
                    print(" (corrected)")
                cCur += sect.dSize 
            else:
                print("Looks like final section, bye")
                break
#        print("Next: " + hex2(cCur))
    except EndOfSections as e:
        print("Finished")
        break
    except Exception as e:
        print(e)
        print(type(e))
        break

