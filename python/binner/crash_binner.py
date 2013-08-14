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
crashed_dir = "Z:\\crashed"
hanged_dir = "Z:\\hanged"
clean_dir = "Z:\\clean"
log_file = "Z:\\log-"

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
        return DBG_CONTINUE
    else:
        status = "crashed"
        dbg.terminate_process()
        binn = hex(crash_bin.last_crash.exception_address)
        logf.write("Bin: " + binn)
        testdir(crashed_dir + "\\" + binn)
        os.rename(samples_dir + "\\" + filee, crashed_dir + "\\" + binn + "\\" + filee)
        logf.write("status: crashed")
        logf.write(crash_bin.crash_synopsis())
        return DBG_CONTINUE

def file_run(filee, dbg):
    global status

    status = "hanged"
    os.system("start " + samples_dir + "\\" + filee)
    logf.write("start " + samples_dir + "\\" + filee + "\n")
    logf.flush()
    time.sleep(3)
    if(status == "hanged"):
        print("status: hanged")
        logf.write("status: hanged\n")
        dbg.terminate_process()
        os.rename(samples_dir + "\\" + filee, hanged_dir + "\\" + filee)
        

def debug_loop(dbg):
    dbg.debug_event_loop()

imagename = "acad.exe"
count = 0

cb_file = samples_dir + time.strftime("%Y%m%d-%H%M%S") + ".crash"
log_file += time.strftime("%Y%m%d-%H%M%S")
log_file += ".txt" 
status = "hang"

logf = open(log_file, "w")
logf.write("test\n")

for filee in os.listdir(samples_dir):
    if(filee[-4:] != ".dwg"):
        continue
    #spawn app & wait to load
    proc = subprocess.Popen("C:\\Program Files\\AutoCAD 2010\\acad.exe")
    time.sleep(3)

    #install hook
    subprocess.Popen("W:\\p023-standalone.exe " + str(proc.pid))

    #attach
    dbg = pydbg()      # globally accessible pydbg instance.
    dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, handle_av)
    for (pid, name) in dbg.enumerate_processes():
        if imagename in name:
            try:
                print "[*] Attaching to %s (%d)" % (name, pid)
                logf.write("[*] Attaching to " + name + " " + str( pid) + "\n")
                dbg.attach(pid)
            except:
                print "[!] Problem attaching to %s" % name
                logf.write("[*] Problem attaching to " + name)
                continue

    #load file
    thread = Thread(target = file_run, args = (filee, dbg, ))
    thread.start()
    
    dbg.debug_event_loop()

    #clean
    dbg.detach()
    dbg = None
    count += 1
#    break

print("Got " + str(count) + " crashes, exporting to: " + cb_file)
logf.write("Got " + str(count) + " crashes, exporting to: " + cb_file)

cb.export_file(cb_file)
logf.close()


