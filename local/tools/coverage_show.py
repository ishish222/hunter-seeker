#!/usr/bin/env python

from PIL import Image, ImageDraw
#import sys
import fileinput
from subprocess import call

length = 0x1000
height = 400

im = Image.new('RGBA', (length, height), (255, 255, 255, 0)) 
draw = ImageDraw.Draw(im) 



#draw banned

#draw input
#while True:
line_count = 0

for line in fileinput.input():
#    addr = sys.stdin.readline()
#    if(addr == "exit"):
#        break
    try:
        scaled = int(int(line, 16) * length / 0x80000000)
    except Exception:
        continue
    draw.line((scaled, 0, scaled, height), fill=(0, 0, 255), width=1)
    line_count += 1

    if(line_count % 50000 == 0):
        im.save("./test.jpg", "JPEG")
        call(["/usr/bin/firefox", "./test.jpg"])
#        im.show()
im.save("./test.jpg", "JPEG")
