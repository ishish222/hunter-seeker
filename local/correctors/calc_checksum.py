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

seed = 0x0

sum1 = seed & 0xffff
sum2 = seed >> 0x10

cCur = s_off

while (s_size != 0x0):
    chunkSize = min(0x15b0, s_size)
    s_size -= chunkSize
    for i in range(0, chunkSize):
        byte = ord(struct.unpack("<c", "".join(fmap[cCur]))[0])
        cCur += 1
        sum1 += byte
        sum2 += sum1
    sum1 %= 0xfff1
    sum2 %= 0xfff1

summ = (sum2 << 0x10) | (sum1 & 0xffff)
print(hex(summ))

fmap.close()
fd.close()

