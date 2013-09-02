#!c:\python\python.exe

import sys
import struct
import utils
import subprocess
import time
import os
import time
import signal
from threading import Thread
from threading import Lock
import ctypes

from pydbg import *
from pydbg.defines import *

samples_dir = "X:\\"
crashed_dir = "X:\\crashed"
hanged_dir = "X:\\hanged"
clean_dir = "X:\\clean"
log_file = "X:\\log-"

app_path={
'acad2010' : 'C:\\Program Files\\AutoCAD 2010\\acad.exe',
'acad2014' : 'C:\\Program Files\\Autodesk\\AutoCAD 2014\\acad.exe'
}


if(len(sys.argv) < 3):
    print("App & length")
    quit()

my_path = app_path[sys.argv[1]]
my_length = int(sys.argv[2], 10)

def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

def testfile(x):
    return os.path.exists(x)
    

#import pdb
#pdb.set_trace()

testdir(crashed_dir)
testdir(hanged_dir)
testdir(clean_dir)

cb = utils.crash_binning.crash_binning()

l = Lock()

def handle_av(dbg):
    global status

    l.acquire()
#    crash_bin = utils.crash_binning.crash_binning()
    crash_bin = cb
    crash_bin.record_crash(dbg)
    
    if(testfile(samples_dir + "\\" + filee)):
        status = "crashed"
        dbg.terminate_process()
        binn = hex(crash_bin.last_crash.exception_address)
        logf.write("Bin: " + binn)
        testdir(crashed_dir + "\\" + binn)
        if(not testfile(crashed_dir + "\\" + binn + "\\" + binn + ".txt")):
            so = open(crashed_dir + "\\" + binn + "\\" + binn + ".txt", "w")
            so.write(crash_bin.crash_synopsis())
            so.close()
        os.rename(samples_dir + "\\" + filee, crashed_dir + "\\" + binn + "\\" + filee)
        logf.write("status: crashed")
        if(testfile(samples_dir + "\\" + filee + ".bak")):
            os.remove(samples_dir + "\\" + filee + ".bak")
    l.release()
    return DBG_CONTINUE
    

def file_run(filee, dbg):
    global status

    status = "hanged"
    os.system("start " + samples_dir + "\\" + filee)
    logf.write("start " + samples_dir + "\\" + filee + "\n")
    logf.flush()
    time.sleep(my_length)
    l.acquire()
    if(status == "hanged"):
        if(testfile(samples_dir + "\\" + filee)):
            print("status: hanged")
            logf.write("status: hanged\n")
            try:
                dbg.terminate_process()
            except Exception:
                pass
            os.rename(samples_dir + "\\" + filee, hanged_dir + "\\" + filee)
            if(testfile(samples_dir + "\\" + filee + ".bak")):
                os.remove(samples_dir + "\\" + filee + ".bak")
    l.release()
    

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

def windows_kill(pid):
    kernel32 = ctypes.windll.kernel32
    handle = kernel32.OpenProcess(1, 0, pid)
    return (0 != kernel32.TerminateProcess(handle, 0))

for filee in os.listdir(samples_dir):
    if(filee[-4:] != ".dwg"):
        continue
    #spawn app & wait to load
    proc = subprocess.Popen(my_path)
    time.sleep(3)

    #install hook
#    subprocess.Popen("W:\\p023-standalone.exe " + str(proc.pid))

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
#                windows_kill(pid)
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


