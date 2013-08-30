#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

import os
import sys
import signal
import time
import settings

pidfile = "/tmp/hs-2nd.pid"
pid = None



#cd
os.chdir("/home/ish/projects/2012-08-02-korrino/implementation-2/python")

while(True):
    if os.path.isfile(pidfile):
        #exists, send signal and exit
        f = file(pidfile, "r")
        pid = f.read()
        f.close()
        os.kill(int(pid), signal.SIGUSR1)
        break
    else:
        command = ["fuzzer-acad-2nd-loop.py", settings.second]
        print("1")
        os.spawnv(os.P_NOWAIT, "./fuzzer-acad-2nd-loop.py", command)
        print("2")
        time.sleep(30)
    

