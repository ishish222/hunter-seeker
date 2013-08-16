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
        self.app = app
        self.imagename = imagename
        self.filee = filee
        self.level = 0
        self.phase = 1
        self.running = True
        self.dbg = pydbg()
        self.dbg.walk = self
        self.walk_addr = addr
        self.walk_end_addr = 0x0
        self.dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, handle_av)
        self.addr_blacklist = []
        
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
        self.pid = pid


    def spawn(self):
        #spawn app & wait to load
        self.proc = subprocess.Popen(self.app)
        time.sleep(3)
    
        #install hooks
#        subprocess.Popen("W:\\p023-standalone.exe " + str(self.proc.pid))
    
    def load_file(self):
        #load file
        try:
            print("Loading file")
            thread = Thread(target = file_run, args = (self.filee, self.dbg, ))
            thread.start()
            return self.dbg
        except Exception as e:
            print(e)

    def install_bp(self, addr):
        try:
#            print("Installing bp at: " + hex(addr))
            self.dbg.set_callback(EXCEPTION_BREAKPOINT, walk_function)
            self.dbg.bp_set(addr, "Working bp")
        except Exception as e:
            print(e)
 

    def install_walk_bp(self):
        try:
            print("Installing walk handlers")
            self.dbg.set_callback(EXCEPTION_BREAKPOINT, walk_function)
#            self.dbg.set_callback(EXCEPTION_SINGLE_STEP, walk_ss_routine)
            self.dbg.bp_set(self.walk_addr, "Walked function")
        except Exception as e:
            print(e)

    def install_walk_end_bp(self):
        try:
            print("Installing walk end handler")
#            self.dbg.set_callback(EXCEPTION_BREAKPOINT, walk_function)
            self.dbg.bp_set(self.walk_end_addr, "Walked function finished")
        except Exception as e:
            print(e)

    def run(self):
        try:
            print("Running")
#            thread = Thread(target = self.dbg.run)
#            thread.start()
            self.dbg.run()
        except Exception as e:
            print(e)

    def go_deeper(self):
        pass

    def detach(self):
        dbg.detach()

    def kill(self):
        print("Killing")
        try:
            self.dbg.terminate_process()
        except Exception as e:
            print(e)
 
def walk_ss_routine(dbg):
    ea = dbg.get_register("EIP")
    instr = dbg.disasm(ea)
    if(dbg.mnemonic == "call"):
        print(hex(ea) + ": " + instr)
    dbg.single_step(True)
    return DBG_CONTINUE

def walk_function(dbg):
    ea = dbg.get_register("EIP")
#    print(hex(ea))
    for addr in dbg.walk.addr_blacklist:
        if(ea == addr):
            print("Blacklisted, ignoring")
            return DBG_CONTINUE
    if(ea == dbg.walk.walk_addr):
        print("Reached walk start")
        instr = dbg.get_instruction(dbg.get_register("EIP"))
        print(dir(instr))
        print(instr.opcode)
        print(instr.length)
        print(instr.op1)
        dbg.disasm(ea)
        print(dbg.mnemonic)
        print(dbg.op1)
        dbg.walk.install_bp(int(dbg.op1,16)) 
        dbg.walk.walk_end_addr = dbg.walk.walk_addr + instr.length
        dbg.walk.install_walk_end_bp()
    if(ea == dbg.walk.walk_end_addr):
        print("Reached walk end")
        print(dbg.disasm(ea))
        dbg.walk.running = False
    else:
        print(dbg.disasm(ea))
        instr = dbg.get_instruction(dbg.get_register("EIP"))
        dbg.walk.install_bp(ea + instr.length)
    return DBG_CONTINUE

my_walk = walk(app, imagename, filee, 0x0049ac33)
my_walk.addr_blacklist.append(0x7c90120f)
my_walk.spawn()
my_walk.attach()
my_walk.install_walk_bp()
my_walk.run()
#my_walk.load_file()

while(my_walk.running):
    pass

my_walk.kill()
logf.close()


