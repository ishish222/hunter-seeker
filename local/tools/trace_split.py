#!/usr/bin/env python

from PIL import Image, ImageDraw
import sys
import fileinput
import os
import time
from glob import glob 
from threading import Thread
from random import random
from shutil import rmtree
from signal import signal, SIGINT

def kill_handle(signum, frame):
    global worky
    print("Shutting down")
    worky = False
    exit()

def testdir2(x): 
    if(os.path.isdir(x) == True):
        if(os.path.isdir("%s-old" %x)):
            rmtree("%s-old" %x)
        os.rename(x, "%s-old" %x)
    os.mkdir(x)

#length = 0x1000
#height = 100
width = 100
height = 0x1000

def generate_page(pid):
    content = "<div>"

    legends = [l for l in glob("%s/*-legend.jpg" % pid)]
    imgs = [i for i in glob("%s/*-trace.jpg" % pid)]
    #print(legends)
    #print(imgs)
    for (legend, img) in zip(sorted(legends), sorted(imgs)):
        content += "<div><img src=\"%s\"></img></div> <div><img src =\"%s\" height=100%% width=5%%></img></div><br>" % (legend, img)
    content += "</div>"
    f = open(pid + ".html", "w")

    f.write("""
<!doctype html>
<html>
<script>
setInterval(function() 
{
    location.reload();
}, 10000);
</script>
<body bgcolor="#002020">
<center><font color="#ffffff" size=14>
%s
</font></center>
%s
</body>
</html>
""" % (pid, content))

    f.close()
    os.system("firefox %s.html" % pid)

def splitting(pid):
    print("Starting trace analysis")
    global t_files
    global worky
    global modules

    while True:
        try:
            line = sys.stdin.readline()
            if(line[0] != "["):
                continue
        except Exception:
            continue

        parts = line.split(" ")
        if(len(parts) != 3):
            continue

        fname = "./%s/%s.trace" % (pid, parts[0][1:-1])
        addr = parts[2]
        try:
            filee = t_files[fname][0]
        except KeyError:
            print("Creating new trace file: %s" % fname)
            n_filee = open(fname, "w")
            n_pic_fname = fname[:-6] + "-trace.jpg"
            n_im = Image.new('RGBA', (width, height), (255, 255, 255, 0)) 
            n_draw = ImageDraw.Draw(n_im)

            #draw modules
            for module in modules:
                scaled = int(int(module[1], 16) * height / 0x80000000)
                print("%s at 0x%x" % (module[0], scaled))
                n_draw.line((0, scaled, width, scaled), fill=(0, 0, 0), width=1)
                n_draw.text((0, scaled), module[0], fill=(0, 0, 0))

            n_color = [int(random()*255)-100 for i in range(0,3)]
            n_l_count = 0
            # create legend
            n_l_fname = fname[:-6] + "-legend.jpg"
            n_legend = Image.new('RGBA', (100, 20), (n_color[0], n_color[1], n_color[2], 0)) 
            n_ldraw = ImageDraw.Draw(n_legend) 
            n_ldraw.text((10, 5), fname, fill = (255,255,255,0))
            n_legend.save(n_l_fname, "JPEG")
            #configure new files
            t_files[fname] = (n_filee, n_pic_fname, n_im, n_draw, n_l_fname, n_color, n_l_count)
            #update webpage
            n_im.save(n_pic_fname, "JPEG")
            generate_page(pid)

        try:
            scaled = int(int(addr, 16) * height / 0x80000000)
        except Exception:
            print("Got exception")
            continue

        filee, pic_fname, im, draw, l_fname, color, l_count = t_files[fname]
#        print(pic_fname + "-" + hex(scaled))
        filee.write(addr)
        filee.flush()
        draw.line((0, scaled, width, scaled), fill=(color[0], color[1], color[2]), width=3)

        l_count += 1
        if(l_count % 50000 == 0):
            im.save(pic_fname, "JPEG")

        t_files[fname] = (filee, pic_fname, im, draw, l_fname, color, l_count)
#        time.sleep(3)

def read_modules():
    mods = []
    while True:
        line = sys.stdin.readline()

        if(line[:12] == "Modules map:"):
            break

    while True:
        line = sys.stdin.readline()

        if(line[:2] == "--"):
            break
        elements = line.split(" ")
        if(len(elements) <3):
            continue
        mods.append((elements[0], elements[1], elements[2]))
    return mods

def main():
    if(len(sys.argv)<2):
        exit()

    global t_files
    global j_files
    global line_count
    global worky
    global modules

    print("1")

    t_files = {}
    j_files = {}
    worky = True
    pid = sys.argv[1]

    line_count = 0
    testdir2("./%s" % pid)

    signal(SIGINT, kill_handle)
    print("2")

    modules = read_modules()
    print(modules)

    #split
#    Thread(target=splitting, args=(pid, )).start()
    print("3")
    splitting(pid)

    while True:
        pass
#        time.sleep(60)

main()
