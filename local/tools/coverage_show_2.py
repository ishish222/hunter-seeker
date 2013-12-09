#!/usr/bin/env python

from PIL import Image, ImageDraw
import sys
import fileinput
from subprocess import call
from random import random
import time
from threading import Thread
from glob import glob

if(len(sys.argv) <2):
    exit()

def routine(fname):

    pic_fname = fname[:-6] + "-trace.jpg"
    l_fname = fname[:-6] + "-legend.jpg"

    color = [int(random()*255)-100 for i in range(0,3)]

    length = 0x1000
    height = 100

    legend = Image.new('RGBA', (100, 20), (color[0], color[1], color[2], 0)) 
    ldraw = ImageDraw.Draw(legend) 
    ldraw.text((10, 5), fname, fill = (255,255,255,0))
    legend.save(l_fname, "JPEG")

    im = Image.new('RGBA', (length, height), (255, 255, 255, 0)) 

    draw = ImageDraw.Draw(im) 

    f = open(fname, "r")

    line_count = 0

    while True:
        line = f.readline()
        if(line == ""):
            time.sleep(5.0)
            continue
        try:
            scaled = int(int(line, 16) * length / 0x80000000)
        except Exception:
            print("Got exception")
            continue
        draw.line((scaled, 0, scaled, height), fill=(color[0], color[1], color[2]), width=1)
        line_count += 1

        if(line_count % 50000 == 0):
            im.save(pic_fname, "JPEG")
#        call(["/usr/bin/firefox", "./test.jpg"])
#        im.show()
    im.save(pic_fname, "JPEG")
    f.close()

dirname = sys.argv[1]
files = glob("./%s/*.trace" % dirname)

print(files)

for filee in files:
    print("Generating for %s" % filee)
    Thread(target=routine, args=(filee, )).start()

while True:
    pass


