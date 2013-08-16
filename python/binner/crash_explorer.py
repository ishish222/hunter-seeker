#!c:\python\python.exe

from __future__ import print_function
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
import sys

from pydbg import *
from pydbg.defines import *

debug = False

if(debug == True):
    import pdb
#    pdb.set_trace()

def debug_print(string):
    if(debug == True):
        print(string)

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
        

log_file += time.strftime("%Y%m%d-%H%M%S")
log_file += ".txt" 

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

PHASE_WALKING=0
PHASE_DIVING=1
PHASE_SURFACING=2

class walk():
    def __init__(self, app="", imagename="", filee="", addr=0x0, max_level=3):
        self.app = app
        self.imagename = imagename
        self.filee = filee
        self.level = 0
        self.max_level = max_level
        self.phase = 0
        self.running = True
        self.dbg = pydbg()
        self.dbg.walk = self
        self.walk_addr = addr
        self.walk_end_addr = 0x0
        self.dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, handle_av)
        self.addr_blacklist = []
        self.current_ea = 0x0
        self.delete_next_bp = False
        
    def attach(self):
        for (pid, name) in self.dbg.enumerate_processes():
            if imagename in name:
                try:
                    print("[*] Attaching to " + str(pid))
                    logf.write("[*] Attaching to " + name + " " + str( pid) + "\n")
                    self.dbg.attach(pid)
                except:
                    print("[!] Problem attaching to " + str(pid))
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
            print("[*] Loading file")
            thread = Thread(target = file_run, args = (self.filee, self.dbg, ))
            thread.start()
            return self.dbg
        except Exception as e:
            print(e)

    def install_bp(self, addr):
        try:
            debug_print("[d] Installing bp at: " + hex(addr))
            self.dbg.bp_set(addr, "Working bp")
        except Exception as e:
            print(e)
 

    def register_callbacks(self):
        try:
            print("[*] Registering callbacks")
            self.dbg.set_callback(EXCEPTION_BREAKPOINT, handle_bp)
            self.dbg.set_callback(EXCEPTION_SINGLE_STEP, handle_ss)
        except Exception as e:
            print(e)
       
    def unregister_callbacks(self):
        try:
            print("[*] Unregistering callbacks")
            self.dbg.set_callback(EXCEPTION_BREAKPOINT, handle_empty)
            self.dbg.set_callback(EXCEPTION_SINGLE_STEP, handle_empty)
        except Exception as e:
            print(e)

    def install_walk_bp(self):
        try:
            print("[*] Installing walk handlers")
            self.dbg.bp_set(self.walk_addr, "Walked function")
        except Exception as e:
            print(e)

    def install_walk_end_bp(self):
        try:
            print("[*] Installing walk end handler")
            self.dbg.bp_set(self.walk_end_addr, "Walked function finished")
        except Exception as e:
            print(e)

    def run(self):
        try:
            print("[*] Running")
#            thread = Thread(target = self.dbg.run)
#            thread.start()
            self.dbg.run()
        except Exception as e:
            print(e)

    def dive(self):
        if(self.level > self.max_level): 
            debug_print("Max level reached, not diving")
            debug_print("[d] Until " + hex(self.current_ea + self.current_instr.length))
            self.install_bp(self.current_ea + self.current_instr.length)
            self.delete_next_bp = True
            self.dbg.single_step(False)
        else:
            self.dbg.single_step(True)
            self.level += 1
            debug_print("[d] Diving, level: " + hex(self.level))

    def surface(self):
        self.dbg.single_step(True)
        self.level -= 1
        debug_print("Surfacing, level: " + hex(self.level))

    def detach(self):
        dbg.detach()

    def kill(self):
        print("[*] Killing")
        try:
            self.dbg.terminate_process()
        except Exception as e:
            print(e)
 
def handle_ss(dbg):
    return handle_bp(dbg)

def handle_empty(dbg):
    pass

def handle_bp(dbg):
    ea = dbg.get_register("EIP")
    dbg.walk.current_ea = ea
    dbg.walk.current_dis = dbg.disasm(ea)
    dbg.walk.current_instr = dbg.get_instruction(dbg.get_register("EIP"))
    debug_print("in handle_bp at: " + hex(ea) + ": " + dbg.walk.current_dis)

    if(dbg.walk.delete_next_bp == True):
        dbg.bp_del(ea)
        dbg.walk.delete_next_bp = False
        debug_print("returned, deleting bp at: " + hex(ea))

    if(dbg.mnemonic == "int3"):
        return DBG_CONTINUE

    for addr in dbg.walk.addr_blacklist:
        if(ea == addr):
            print("[*] Blacklisted, ignoring")
            return DBG_CONTINUE

    if(ea == dbg.walk.walk_addr):
        debug_print("[*] Reached walk start")

        # handle walk end
        dbg.walk.walk_end_addr = dbg.walk.walk_addr + dbg.walk.current_instr.length
        dbg.walk.install_walk_end_bp()

        # dive
        dbg.walk.dive()
        print("--cut here--")

    if(ea == dbg.walk.walk_end_addr):
        print("--cut here--")
        debug_print("[*] Reached walk end")
        dbg.single_step(False)
        dbg.walk.unregister_callbacks()
        dbg.walk.running = False
        dbg.walk.kill()
        return DBG_CONTINUE

    else:
#        print("simple step")
        if(dbg.mnemonic == "call"):
            for i in range(0, dbg.walk.level):
                print(" ", end="")
            print(hex(int(ea & 0xffffffff)) + ": " + dbg.walk.current_dis)

            dbg.walk.dive()
            return DBG_CONTINUE

        if((dbg.mnemonic == "ret") or (dbg.mnemonic == "retn")):
            dbg.walk.surface()
            return DBG_CONTINUE

        dbg.single_step(True)

    return DBG_CONTINUE

if(len(sys.argv)>1):
    depth = int(sys.argv[1])
else:
    depth = 2

my_walk = walk(app, imagename, filee, 0x0049ac33, depth)
my_walk.addr_blacklist.append(0x7c90120f)
my_walk.spawn()
my_walk.attach()
my_walk.register_callbacks()
my_walk.install_walk_bp()
my_walk.run()
#my_walk.load_file()

while(my_walk.running == True):
    pass

logf.close()


