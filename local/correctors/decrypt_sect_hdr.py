#!/usr/bin/env python


import sys
from mmap import mmap
import struct


if(len(sys.argv)<3):
    print("File & offset in hex")
    quit()

f = sys.argv[1]
s_off = int(sys.argv[2], 16)
seed = 0x4164536b
mask = seed ^ s_off
print("mask: " + hex(mask))

fd = open(f, "r+b")
fmap = mmap(fd.fileno(), 0)

for i in range(0, 0x8):
    my_off = s_off + (i * 0x4)
    cur = int(struct.unpack("<i", "".join(fmap[my_off+0x0:my_off+0x4]))[0])
#    print(hex(cur) + " ", end='')
    cur ^= mask
    print(hex(cur))



