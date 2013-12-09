#!/usr/bin/env python

from socket import socket, AF_INET, SOCK_STREAM, SHUT_RDWR
from signal import signal, SIGINT
from glob import glob

content = "<div>"

legends = [l for l in glob("./*-legend.jpg")]
imgs = [i for i in glob("./*-trace.jpg")]
#print(legends)
#print(imgs)
for (legend, img) in zip(sorted(legends), sorted(imgs)):
    content += "<div><img src=\"%s\"></img></div> <div><img src =\"%s\"></img></div><br>" % (legend, img)
content += "</div>"

s = socket(AF_INET, SOCK_STREAM)
s.bind(("127.0.0.1", 8080))
s.listen(3)

def kill_handle(signum, frame):
    print("Received int")
    s.shutdown(SHUT_RDWR)
    s.close()

signal(SIGINT, kill_handle)

while True:
    try:
        c, a = s.accept()
    except Exception:
        break
    c.recv(1024)
    print("Received req")
    c.send("""
HTTP/1.0 200 Found

<!doctype html>
<html>
<body>
<script>
setInterval(function() 
{
    document.reload()
}, 5000);
</script>
%s
</body>
</html>
\r\n
""" % content)
    c.close()
