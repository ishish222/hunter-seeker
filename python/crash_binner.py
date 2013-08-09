#!c:\python\python.exe

import sys
import struct
import utils
import subprocess
import time

from pydbg import *
from pydbg.defines import *

cb = utils.crash_binning.crash_binning()

def handle_av (dbg):
    '''
    As we are mucking around with process state and calling potentially unknown subroutines, it is likely that we may
    cause an access violation. We register this handler to provide some useful information about the cause.
    '''

#    crash_bin = utils.crash_binning.crash_binning()
    crash_bin = cb
    crash_bin.record_crash(dbg)
    
    if(crash_bin.last_crash.exception_address == 0x50f8e14):
#        print("pass")
        return DBG_CONTINUE
    else:
        print crash_bin.crash_synopsis()
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



imagename = "acad.exe"
count = 0

while True:
    subprocess.Popen("C:\\Program Files\\AutoCAD 2010\\acad.exe")
    time.sleep(3)

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

    dbg.debug_event_loop()
    dbg.detach()
    dbg = None
    count += 1
    if(count > 4):
        print("Got 5 crashes, exporting to: x.crashbin")
        cb.export_file("x.crashbin")
        break
    print("Ready")

for binn in cb.bins:
    print("Bin: " + hex(binn))
#    for crash in binn:
#        print(crash)

