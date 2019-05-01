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
import sys

from pydbg import *
from pydbg.defines import *

debug = False
graph = True

if(debug == True):
    import pdb
    pdb.set_trace()

def debug_print(string):
    if(debug == True):
        print(string)

def format_empty_node(ea, my_mod, dis, tmod, color = None):
    if(color == None):
        return "<node TEXT=\"" + my_mod.szModule + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + tmod.szModule + ")" + "\"/>\n"
    else:
        return "<node COLOR=\""+ color +"\" TEXT=\"" + my_mod.szModule + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + tmod.szModule + ")" + "\"/>\n"
        

def format_node(ea, my_mod, dis, tmod, color = None):
    if(color == None):
        return "<node FOLDED=\"true\" TEXT=\"" + my_mod.szModule + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + tmod.szModule + ")" + "\">\n"
    else:
        return "<node FOLDED=\"true\" COLOR=\""+ color +"\" TEXT=\"" + my_mod.szModule + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + tmod.szModule + ")" + "\">\n"

def format_empty_node_text(text, color = None):
    if(color == None):
        return "<node TEXT=\"" + text + "\"/>\n"
    else:
        return "<node COLOR=\""+ color +"\" TEXT=\"" + text + "\"/>\n"

samples_dir = "Z:\\"
crashed_dir = "Z:\\crashed"
hanged_dir = "Z:\\hanged"
clean_dir = "Z:\\clean"
graph_file = "Z:\\graph-"

class AttachFail(Exception):
    pass

def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

cb = utils.crash_binning.crash_binning()

def handle_av(dbg):
    ea = dbg.get_register("EIP")
    dbg.walk.current_ea = ea
    if(ea == 0x50f8e14):
        return DBG_CONTINUE
    else:
        my_module = dbg.addr_to_module(int(ea & 0xffffffff))
        if(my_module == None):
            my_module_name = "[UKNOWN]"
        else:
            my_module_name = my_module.szModule
        my_dis = dbg.disasm(ea)
        print("Crashed")
        print(my_module_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + my_dis)
        gf.write(format_empty_node_text("[CRASH] " + my_module_name + ":" + hex(int(ea & 0xffffffff)) + ": " + my_dis, "#ff0000"))
        dbg.terminate_process()
        dbg.walk.running = False
        return DBG_CONTINUE

graph_file += time.strftime("%Y%m%d-%H%M%S")
graph_file += ".mm" 

if(graph == True):
    gf = open(graph_file, "w")
    gf.write("<map version=\"0.8.0\">\n")

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

regs = ["EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EBP", "ESP", "EIP"]

def decode_op1(dbg, op1):
    my_op = op1
    if(my_op[0] == '['):
        my_op = decode_op1(dbg, my_op[1:-1])
        my_op = int(struct.unpack("<i", "".join(dbg.read(my_op, 4)))[0]) & 0xffffffff
        return my_op
    for reg in regs:
        if(my_op.upper() == reg):
            my_op = dbg.get_register(reg)
            return my_op & 0xffffffff
    if(len(my_op.split("*")) >1):
        (a,b) = my_op.split("*")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a*b
        return my_op & 0xffffffff
    if(len(my_op.split("+")) >1):
        (a,b) = my_op.split("+")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a+b
        return my_op & 0xffffffff
    if(len(my_op.split("-")) >1):
        (a,b) = my_op.split("-")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a-b
        return my_op & 0xffffffff
    return int(my_op, 16) & 0xffffffff
        
def handle_ss(dbg):
    return handle_bp(dbg)

def handle_empty(dbg):
    pass

def handle_ret(dbg):
    ea = dbg.get_register("EIP")
    dbg.bp_del(ea)
    dbg.single_step(True)
    return DBG_CONTINUE

def handle_ret_surface(dbg):
    ea = dbg.get_register("EIP")
    dbg.bp_del(ea)
    dbg.walk.surface()
    dbg.single_step(True)
    return DBG_CONTINUE
    

def handle_bp(dbg):
    ea = dbg.get_register("EIP")
    dbg.walk.current_ea = ea
    dbg.walk.current_dis = dbg.disasm(ea)
    dbg.walk.current_instr = dbg.get_instruction(dbg.get_register("EIP"))

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
        dbg.walk.level -= 1

        # dive
        dbg.walk.dive()
        print("--cut here--")

    if(ea == dbg.walk.walk_end_addr):
        print("--cut here--")
        debug_print("[*] Reached walk end")
        print("[*] Reached walk end")
        dbg.single_step(False)
        dbg.walk.unregister_callbacks()
        dbg.walk.running = False
        dbg.walk.kill()
        return DBG_CONTINUE

    else:
        if(dbg.mnemonic == "call"):
            for i in range(0, dbg.walk.level):
                print(" ", end="")
            my_module = my_walk.dbg.addr_to_module(int(ea & 0xffffffff))
            target_module = my_walk.dbg.addr_to_module(decode_op1(my_walk.dbg, my_walk.dbg.op1))
            print(my_module.szModule + str(":") + hex(int(ea & 0xffffffff)) + ": " + dbg.walk.current_dis + " (" + target_module.szModule + ")")
            dbg.walk.dive()
            return DBG_CONTINUE

        dbg.single_step(True)

    return DBG_CONTINUE

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
        self.module_blacklist = []
        self.current_ea = 0x0
        self.delete_next_bp = False
        
    def attach(self):
        for (pid, name) in self.dbg.enumerate_processes():
            if imagename in name:
                try:
                    print("[*] Attaching to " + str(pid))
                    self.dbg.attach(pid)
                except:
                    print("[!] Problem attaching to " + str(pid))
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

    def install_bp(self, addr, handler = handle_bp):
        try:
            debug_print("[d] Installing bp at: " + hex(addr))
            self.dbg.bp_set(addr, "Working bp", handler = handler)
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
        ea = self.current_ea
        my_module = self.dbg.addr_to_module(int(ea & 0xffffffff))
        target_module = self.dbg.addr_to_module(decode_op1(self.dbg, self.dbg.op1))
        target_module_name = target_module.szModule
        debug_print("[d] Target module: " + target_module_name)
                
        for mod in self.module_blacklist:
            if(mod.upper() == target_module_name.upper()):
                debug_print("[d] Target module blacklisted, not diving")
                debug_print("[d] Until " + hex(self.current_ea + self.current_instr.length))
                if(graph == True):
                    gf.write(format_empty_node(ea, my_module, self.dbg.walk.current_dis, target_module, "#006699"))
                self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret)
                self.delete_next_bp = True
                self.dbg.single_step(False)
                return
        if(self.max_level > 0x0):
            if(self.level > self.max_level): 
                debug_print("Max level reached, not diving")
                debug_print("[d] Until " + hex(self.current_ea + self.current_instr.length))
                if(graph == True):
                    gf.write(format_empty_node(ea, my_module, self.dbg.walk.current_dis, target_module))
                self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret)
                self.delete_next_bp = True
                self.dbg.single_step(False)
                return
        self.dbg.single_step(True)
        self.level += 1
        debug_print("[d] Diving, level: " + hex(self.level))
        if(graph == True):
            gf.write(format_node(ea, my_module, self.dbg.walk.current_dis, target_module))
        self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret_surface)

    def surface(self):
        self.dbg.single_step(True)
        self.level -= 1
        debug_print("Surfacing, level: " + hex(self.level))
        if(graph == True):
            gf.write("</node>\n")
        if(self.level < 0x0):
            print("[!] Level is signed. Sth went wrong")
            self.running = False
            self.kill()


    def detach(self):
        dbg.detach()

    def kill(self):
        print("[*] Killing")
        try:
            self.dbg.terminate_process()
        except Exception as e:
            print(e)
 

if(len(sys.argv)>1):
    depth = int(sys.argv[1])
else:
    depth = 2

my_walk = walk(app, imagename, filee, 0x0049ac33, depth)

#configure blacklists
my_walk.addr_blacklist.append(0x7c90120f)

my_walk.module_blacklist.append("ntdll.dll")
my_walk.module_blacklist.append("kernel32.dll")
my_walk.module_blacklist.append("user32.dll")
my_walk.module_blacklist.append("xpsp2res.dll")
my_walk.module_blacklist.append("glu32.dll")
my_walk.module_blacklist.append("advapi32.dll")
my_walk.module_blacklist.append("comdlg.dll")
my_walk.module_blacklist.append("crypt32.dll")
my_walk.module_blacklist.append("dnsapi.dll")
my_walk.module_blacklist.append("gdi.dll")
my_walk.module_blacklist.append("lz32.dll")
my_walk.module_blacklist.append("msctf.dll")
my_walk.module_blacklist.append("msvcrt.dll")
my_walk.module_blacklist.append("ole32.dll")
my_walk.module_blacklist.append("urlmon.dll")
my_walk.module_blacklist.append("wininet.dll")
my_walk.module_blacklist.append("ws2_32.dll")
my_walk.module_blacklist.append("MSVCP90.dll")
my_walk.module_blacklist.append("MSVCR90.dll")
my_walk.module_blacklist.append("mfc90u.dll")
my_walk.module_blacklist.append("shlwapi.dll")

my_walk.spawn()
my_walk.attach()
my_walk.register_callbacks()
my_walk.install_walk_bp()
my_walk.run()
#my_walk.load_file()

while(my_walk.running == True):
    pass

if(graph == True):
    for i in range(0, my_walk.level):
        gf.write("</node>\n")
    gf.write("</node>\n")
    gf.write("</map>")
    gf.close()
