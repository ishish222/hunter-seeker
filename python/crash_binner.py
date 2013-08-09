#!c:\python\python.exe

import sys
import struct
import utils
import subprocess
import time
import os
import time
from threading import Thread

from pydbg import *
from pydbg.defines import *

samples_dir = "Z:\\"
#crashed_dir = "Z:\\crashed"
#hanged_dir = "Z:\\hanged"
#clean_dir = "Z:\\clean"

crashed_dir = "Z:\\tcrashed"
hanged_dir = "Z:\\thanged"
clean_dir = "Z:\\tclean"


def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

testdir(crashed_dir)
testdir(hanged_dir)
testdir(clean_dir)

cb = utils.crash_binning.crash_binning()

def handle_av(dbg):
    global status

#    crash_bin = utils.crash_binning.crash_binning()
    crash_bin = cb
    crash_bin.record_crash(dbg)
    
    if(crash_bin.last_crash.exception_address == 0x50f8e14):
#        print("pass")
        return DBG_CONTINUE
    else:
        print crash_bin.crash_synopsis()
        status = "crashed"
        dbg.terminate_process()
        print("here")
        return DBG_CONTINUE

def handle_av2(dbg):
    while True:
        crash_bin = utils.crash_binning.crash_binning()
        crash_bin.record_crash(dbg)
        if(crash_bin.last_crash.exception_address == 0x50f8e14):
            continue

    print crash_bin.crash_synopsis()

def file_run(filee, dbg):
    global status
    
    os.system("start " + samples_dir + "\\" + filee)
    print("waiting")
    time.sleep(5)
    print("status: " + status)
    if(status == "hang"):
        dbg.terminate_process()

def debug_loop(dbg):
    dbg.debug_event_loop()

imagename = "acad.exe"
count = 0

cb_file = time.strftime("%Y%m%d-%H%M%S")
status = "hang"

for filee in os.listdir(samples_dir):
    #spawn app & wait to load
    subprocess.Popen("C:\\Program Files\\AutoCAD 2010\\acad.exe")
    time.sleep(3)

    #attach
    dbg = pydbg()      # globally accessible pydbg instance.
    dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, handle_av)
    for (pid, name) in dbg.enumerate_processes():
        if imagename in name:
            try:
                print "[*] Attaching to %s (%d)" % (name, pid)
                dbg.attach(pid)
            except:
                print "[!] Problem attaching to %s" % name
                continue

    #load file
    thread = Thread(target = file_run, args = (filee, dbg, ))
    #thread = Thread(target = debug_loop, args = (dbg, ))
    thread.start()
    
    dbg.debug_event_loop()

    #print("Opening file")
#    file_run(filee)
    
#    print("Waiting")
#    time.sleep(5)

#    dbg.terminate_process()

    #clean
    dbg.detach()
    dbg = None
    count += 1
    break

print("Got " + str(count) + " crashes, exporting to: " + cb_file)
cb.export_file(cb_file)


