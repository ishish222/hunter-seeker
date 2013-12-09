#!/usr/bin/env python

from signal import signal, SIGINT
from glob import glob
import sys

if(len(sys.argv) <2):
    exit()

fname = sys.argv[1]

content = "<div>"

legends = [l for l in glob("./*-legend.jpg")]
imgs = [i for i in glob("./*-trace.jpg")]
#print(legends)
#print(imgs)
for (legend, img) in zip(sorted(legends), sorted(imgs)):
    content += "<div><img src=\"%s\"></img></div> <div><img src =\"%s\" height=10%% width=100%%></img></div><br>" % (legend, img)
content += "</div>"

filee = open(fname + "/" + fname + ".html", "w")

filee.write("""
<!doctype html>
<html>
<script>
setInterval(function() 
{
    document.reload()
}, 1000);
</script>
<body bgcolor="#002020">
<center><font color="#ffffff" size=14>
%s
</font></center>
%s
</body>
</html>
""" % (fname, content))

filee.close()
