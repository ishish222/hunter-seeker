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

length = 0x1000
height = 100

def routine(fname):
    global t_files
    global worky 

    print("Received: %s" % fname)
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

    f = t_files[fname]

    line_count = 0

    while(worky == True):
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
    f.close()
    exit()

def generate_page(pid):
    content = "<div>"

    legends = [l for l in glob("%s/*-legend.jpg" % pid)]
    imgs = [i for i in glob("%s/*-trace.jpg" % pid)]
    #print(legends)
    #print(imgs)
    for (legend, img) in zip(sorted(legends), sorted(imgs)):
        content += "<div><img src=\"%s\"></img></div> <div><img src =\"%s\" height=10%% width=100%%></img></div><br>" % (legend, img)
    content += "</div>"
    f = open(pid + ".html", "w")

    f.write("""
<!doctype html>
<html>
<script>
setInterval(function() 
{
    location.reload();
}, 1000);
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
            n_im = Image.new('RGBA', (length, height), (255, 255, 255, 0)) 
            n_draw = ImageDraw.Draw(n_im)
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
            scaled = int(int(addr, 16) * length / 0x80000000)
        except Exception:
            print("Got exception")
            continue

        filee, pic_fname, im, draw, l_fname, color, l_count = t_files[fname]
#        print(pic_fname + "-" + hex(scaled))
        filee.write(addr)
        filee.flush()
        draw.line((scaled, 0, scaled, height), fill=(color[0], color[1], color[2]), width=3)

        l_count += 1
        if(l_count % 50000 == 0):
            im.save(pic_fname, "JPEG")

        t_files[fname] = (filee, pic_fname, im, draw, l_fname, color, l_count)
#        time.sleep(3)

def main():
    if(len(sys.argv)<2):
        exit()

    global t_files
    global j_files
    global line_count
    global worky

    t_files = {}
    j_files = {}
    worky = True
    pid = sys.argv[1]

    line_count = 0
    testdir2("./%s" % pid)

    signal(SIGINT, kill_handle)

    #split
    Thread(target=splitting, args=(pid, )).start()

    while True:
        time.sleep(60)

main()
