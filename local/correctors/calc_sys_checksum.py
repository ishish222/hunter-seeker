#!/usr/bin/env python


import sys
from mmap import mmap
import struct


if(len(sys.argv)>4):
    print("File, offset, size in hex")
    quit()

f = sys.argv[1]
s_off = int(sys.argv[2], 16)
s_size = int(sys.argv[3], 16)

fd = open(f, "r+b")
fmap = mmap(fd.fileno(), 0)

#stage 1

data = []
data += fmap[s_off: s_off + 0x10]
data += ["\x00", "\x00", "\x00", "\x00"]
print(data)

seed = 0x0
size = 0x14

sum1 = seed & 0xffff
sum2 = seed >> 0x10

cCur = 0x0

while (size != 0x0):
    chunkSize = min(0x15b0, size)
    size -= chunkSize
    for i in range(0, chunkSize):
        print(hex(cCur))
        byte = ord(struct.unpack("<c", "".join(data[cCur]))[0])
        cCur += 1
        sum1 += byte
        sum2 += sum1
    sum1 %= 0xfff1
    sum2 %= 0xfff1

summ = (sum2 << 0x10) | (sum1 & 0xffff)
print("stage1 sum: " + hex(summ))

#stage 2

seed = summ

size = s_size

sum1 = seed & 0xffff
sum2 = seed >> 0x10

cCur = s_off + 0x14

while (size != 0x0):
    chunkSize = min(0x15b0, size)
    size -= chunkSize
    for i in range(0, chunkSize):
        print(hex(cCur))
        byte = ord(struct.unpack("<c", "".join(fmap[cCur]))[0])
        cCur += 1
        sum1 += byte
        sum2 += sum1
    sum1 %= 0xfff1
    sum2 %= 0xfff1

summ2 = (sum2 << 0x10) | (sum1 & 0xffff)
print("stage2 sum: " + hex(summ2))

fmap.close()
fd.close()

