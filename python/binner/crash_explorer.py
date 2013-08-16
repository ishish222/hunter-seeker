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
import ctypes

from pydbg import *
from pydbg.defines import *

samples_dir = "Z:\\"
crashed_dir = "Z:\\crashed"
hanged_dir = "Z:\\hanged"
clean_dir = "Z:\\clean"
log_file = "Z:\\log-"

class AttachFail(Exception):
    pass

def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

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
#        testdir(crashed_dir + "\\" + binn)
#        os.rename(samples_dir + "\\" + filee, crashed_dir + "\\" + binn + "\\" + filee)
        logf.write("status: crashed")
        logf.write(crash_bin.crash_synopsis())
        return DBG_CONTINUE

def file_run(filee, dbg):
    global status

    status = "hanged"
    os.system("start " + filee)
#    logf.write("start " + filee + "\n")
#    os.system("start " + samples_dir + "\\" + filee)
#    logf.write("start " + samples_dir + "\\" + filee + "\n")
#    logf.flush()
#    time.sleep(3)
#    if(status == "hanged"):
#        print("status: hanged")
#        logf.write("status: hanged\n")
#        dbg.terminate_process()
#        os.rename(samples_dir + "\\" + filee, hanged_dir + "\\" + filee)
        

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

app = "C:\\Program Files\\AutoCAD 2010\\acad.exe"
imagename = "acad.exe"
filee = "C:\\test.dwg"

def windows_kill(pid):
    PROCESS_TERMINATE = 1
    handle = ctypes.windll.kernel32.OpenProcess(PROCESS_TERMINATE, False, pid)
    ctypes.windll.kernel32.TerminateProcess(handle, -1)
    ctypes.windll.kernel32.CloseHandle(handle)

#dbg = None

class walk():
    def __init__(self, app="", imagename="", filee="", addr=0x0):
#        global dbg
        self.app = app
        self.imagename = imagename
        self.filee = filee
        self.level = 0
        self.phase = 1
        self.dbg = pydbg()
        self.dbg.walk = self
#        dbg = self.dbg
        self.walk_addr = addr
        self.walk_end_addr = 0x0
        self.dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, handle_av)
        
    def attach(self):
        for (pid, name) in self.dbg.enumerate_processes():
            if imagename in name:
                try:
                    print "[*] Attaching to %s (%d)" % (name, pid)
                    logf.write("[*] Attaching to " + name + " " + str( pid) + "\n")
                    self.dbg.attach(pid)
                except:
                    print "[!] Problem attaching to %s" % name
                    logf.write("[*] Problem attaching to " + name)
                    windows_kill(pid)
                    raise AttachFail


    def spawn(self):
        #spawn app & wait to load
        self.proc = subprocess.Popen(self.app)
        time.sleep(3)
    
        #install hooks
#        subprocess.Popen("W:\\p023-standalone.exe " + str(self.proc.pid))
    
    def load_file(self):
        #load file
        print("Loading file")
        thread = Thread(target = file_run, args = (self.filee, self.dbg, ))
        thread.start()
        return self.dbg

    def install_walk_bp(self):
        self.dbg.set_callback(EXCEPTION_BREAKPOINT, walk_function)
        self.dbg.bp_set(self.walk_addr, "Walked function")

    def run(self):
        self.dbg.run()
    
    def go_deeper(self):
        pass

    def detach(self):
        dbg.detach()



def walk_function(dbg):
    print("hello")
    ea = dbg.get_register("EIP")
    print(hex(ea))
    if(ea != dbg.walk.walk_addr):
        return DBG_CONTINUE
    print("Hello!")
    instr = dbg.get_instruction(dbg.get_register("EIP"))
    print(dir(instr))
    print(instr.opcode)
    print(instr.length)
    print(hex(ea))
    print(dbg.disasm(ea))
    return DBG_CONTINUE

my_walk = walk(app, imagename, filee, 0x0049ac33)
my_walk.spawn()
my_walk.attach()
my_walk.install_walk_bp()
my_walk.load_file()

cb.export_file(cb_file)
logf.close()


