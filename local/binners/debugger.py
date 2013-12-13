import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

import settings
from threading import Thread, Lock
from multiprocessing import Process, Pipe
from pydbg import *
from pydbg.defines import *
from functions import *
from subprocess import Popen
import utils
import win32pipe, win32file
import time
from select import select
import socket
from random import random
import struct

### functions
# unable to move cause settings module is not visible

HIT_COUNT = 1
PASS_COUNT = 0

### default blacklists

default_module_blacklist = []
default_module_blacklist.append("ntdll.dll")
default_module_blacklist.append("kernel32.dll")
default_module_blacklist.append("user32.dll")
default_module_blacklist.append("xpsp2res.dll")
default_module_blacklist.append("glu32.dll")
default_module_blacklist.append("advapi32.dll")
default_module_blacklist.append("comdlg.dll")
default_module_blacklist.append("crypt32.dll")
default_module_blacklist.append("dnsapi.dll")
default_module_blacklist.append("gdi.dll")
default_module_blacklist.append("lz32.dll")
default_module_blacklist.append("msctf.dll")
default_module_blacklist.append("msvcrt.dll")
default_module_blacklist.append("ole32.dll")
default_module_blacklist.append("urlmon.dll")
default_module_blacklist.append("wininet.dll")
default_module_blacklist.append("ws2_32.dll")
default_module_blacklist.append("MSVCP90.dll")
default_module_blacklist.append("MSVCR100.dll")
default_module_blacklist.append("mfc90u.dll")
default_module_blacklist.append("shlwapi.dll")
default_module_blacklist.append("uxtheme.dll")
default_module_blacklist.append("shimeng.dll")
default_module_blacklist.append("AcGeneral.dll")
default_module_blacklist.append("userenv.dll")
default_module_blacklist.append("winmm.dll")
default_module_blacklist.append("oleaut32.dll")
default_module_blacklist.append("comclt32.dll")
default_module_blacklist.append("msacm32.dll")
default_module_blacklist.append("version.dll")
default_module_blacklist.append("msvcrt.dll")
default_module_blacklist.append("rpcrt.dll")
default_module_blacklist.append("rpcrt4.dll")
default_module_blacklist.append("gdi32.dll")
default_module_blacklist.append("secur32.dll")
default_module_blacklist.append("user32.dll")
default_module_blacklist.append("shell32.dll")
default_module_blacklist.append("mswsock.dll")
default_module_blacklist.append("ws2_help.dll")


def defined(name):
    if(name in globals()):
        return True
    if(name in locals()):
        return True
    if(name in vars()):
        return True
    names = name.split(".")
    if(len(names)>1):
        if(names[1] in dir(globals()[names[0]])):
            return True
    return False

### default handlers

def phony_handler(dbg):
    dbg.dlog("Phony handler!!")
    dbg.ok()
    return DBG_CONTINUE

def default_st_handler(dbg):
    if(dbg.check_counters(dbg.exception_address)):
        dbg.dlog("ST marker reached")
        dbg.signal_st()
        dbg.ok()

    return DBG_CONTINUE


def default_end_handler(dbg):
    if(dbg.check_counters(dbg.exception_address)):
        dbg.dlog("END marker reached")
        dbg.signal_ma()
        dbg.ok()

    return DBG_CONTINUE

def default_rd_handler(dbg):
    if(dbg.check_counters(dbg.exception_address)):
        dbg.dlog("RD marker reached")
        dbg.signal_rd()
        dbg.ok()

    return DBG_CONTINUE

def default_bp_handler(dbg):
    dlog("EXCEPTION_BREAKPOINT")

    dbg.signal_ex()
    dbg.ok()
    return DBG_CONTINUE
    
def default_ss_handler(dbg):
    dlog("EXCEPTION_SINGLE_STEP")

    dbg.signal_ex()
    dbg.ok()
    return DBG_CONTINUE
    
def default_av_handler(dbg):
    dlog("avThread")

    dbg.crash_bin.record_crash(dbg)
    dbg.signal_cr()
    dbg.ok()
    return DBG_CONTINUE
    
    dbg.binner.test_lock.acquire()
    dlog("test locked by AV")
    dbg.binner.crash_bin.record_crash(dbg)
    e_addr = dbg.binner.crash_bin.last_crash.exception_address
    if(cbg.check_blacklists(e_addr) == False):
        dbg.binner.status = "CR"
    dbg.binner.test_lock.release()
    return DBG_CONTINUE

### main routines

def readline(stream):
    data = ""
    while True:
        c = stream.recv(1)
        if(c == "\n"):
            return data
        data += c

def comm_routine(dbg):
    try:
        while True:
            cmd = readline(dbg.binner)
            dbg.dlog("Received: %s" % cmd, 2)
            if(cmd == "exit"):
                break
            dbg.execute(cmd)
#            dbg.ok()
    except Exception, e:
        dbg.dlog("Got exception in debugger_routine")
        dbg.dlog(e)
        dbg.ok()

def debugger_routine():
    global l
    l = Lock()
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("127.0.0.1", 12347))
    dbg = debugger(s)
    dbg.dlog("Spawned & constructed", 2)
    dbg.ok()
    dbg.dlog("Will accept attach now")
    dbg.ok()
    cmd = readline(dbg.binner)
    dbg.execute(cmd)
    dbg.ok()
    Thread(target=comm_routine, args=(dbg,)).start()

    # dbg in main thread, all commands in additional threads
    time.sleep(5)
    while True:
        l.acquire()
        dbg.debug_event_loop()
        l.release()

### debugger class
class debugger(pydbg):

    def __init__(self, binner):
        pydbg.__init__(self)
        self.binner = binner
        self.preparation_lock = Lock()
        self.crash_bin = utils.crash_binning.crash_binning()
        #blacklists
        self.bl_modules = []
        self.bl_instructions = []
        self.bl_addresses = []
        self.bl_rvas = []
        #markers
        self.markers = []
        self.st_markers = []
        self.end_markers = []
        self.react_markers = []
        #handlers
        self.marker_handler = phony_handler
        self.st_marker_handler = phony_handler
        self.end_marker_handler = phony_handler
        self.react_marker_handlers = {}
        self.rd_marker_handler = phony_handler
        self.av_handler = default_av_handler
        self.bp_handler = default_bp_handler
        self.ss_handler = default_ss_handler
        self.counters = {}
        self.tracked_threads = []

        if(defined("settings.log_level") == True):
            self.log_level = settings.log_level
        else:
            self.log_level = 0

        # will switch after attach
        self.debug = False

    def dlog(self, data, level=0):
#        dlog("[binner] %s" % data, level)
        if(self.debug == True):
            if(self.log_level <0):
                return
            if(level > self.log_level):
                return
#            self.last_log_file.write("[%d] %s\n" % (self.pid, data))
            self.last_log_file.write("[%s] %s\n" % (timestamp(), data))

    def signal_st(self):
        self.dlog("Signaled: ST", 1)
        self.binner.send("Status: ST")

    def signal_ma(self):
        self.dlog("Signaled: MA", 1)
        self.binner.send("Status: MA")

    def signal_rd(self):
        self.dlog("Signaled: RD", 1)
        self.binner.send("Status: RD")

    def signal_rs(self):
        self.dlog("Signaled: RS", 1)
        self.binner.send("Status: RS")

    def signal_cr(self):
        self.dlog("Signaled: CR", 1)
        self.binner.send("Status: CR")

    def signal_ex(self):
        self.dlog("Signaled: EX", 1)
        self.binner.send("Status: EX")

    def ok(self):
        self.binner.send("=[OK]=")

    def reqScript(self, script):
        self.dlog("Signaled: SR", 1)
        self.binner.send("Status: SR\nScript: %s\n" % script)

    def execute(self, cmd):
        cmds = cmd.split(" ")
        cmd = cmds[0]
        args = " ".join(cmds[1:])

        if(cmd == "read_config"):
            self.read_config()
            self.dlog("config read", 2)

        if(cmd == "attach"):
            self.attach(int(args))
            if(defined("settings.debug") == True):
                if(settings.debug == True):
                    self.debug = True
                    self.last_log_file = open("z:\\server\\debugger-%s-last_log.txt" % self.pid, "w")

            self.dlog("attached to %d" % int(args))

        if(cmd == "start"):
            if(len(cmds) > 1):
                self.start(float(args))
            else:
                self.start()
            self.dlog("Started", 2)

        if(cmd == "stop"):
            self.stop()
            self.dlog("Stopped", 2)

        if(cmd == "attach_bp_handler"):
            self.attach_bp_handler()
            self.dlog("BP handler attached", 2)

        if(cmd == "attach_ss_handler"):
            self.attach_ss_handler()
            self.dlog("SS handler attached", 2)

        if(cmd == "attach_av_handler"):
            self.attach_av_handler()
            self.dlog("AV handler attached", 2)

        if(cmd == "attach_markers"):
            self.attach_markers()
            self.dlog("markers attached", 2)

        if(cmd == "attach_st_markers"):
            self.attach_st_markers()
            self.dlog("ST markers attached", 2)

        if(cmd == "attach_end_markers"):
            self.attach_end_markers()
            self.dlog("END markers attached", 2)

        if(cmd == "attach_react_markers"):
            self.attach_react_markers()
            self.dlog("REACT markers attached", 2)

        if(cmd == "attach_rd_markers"):
            self.attach_rd_markers()
            self.dlog("RD markers attached", 2)

        if(cmd == "detach_markers"):
            self.detach_markers()
            self.dlog("markers detached", 2)

        if(cmd == "detach_st_markers"):
            self.detach_st_markers()
            self.dlog("ST markers detached", 2)

        if(cmd == "detach_end_markers"):
            self.detach_end_markers()
            self.dlog("END markers detached", 2)

        if(cmd == "detach_react_markers"):
            self.detach_react_markers()
            self.dlog("RECT markers detached", 2)

        if(cmd == "detach_rd_markers"):
            self.detach_rd_markers()
            self.dlog("RD markers detached", 2)

        if(cmd == "set_callback"):
            self.set_callback(cmds[1], cmds[2])

        if(cmd == "dump_modules"):
            self.dump_modules()

        if(cmd == "dump_threads"):
            self.dump_threads()

        if(cmd == "track_all_threads"):
            self.track_all_threads()

        if(cmd == "start_tracking_all_threads"):
            self.start_tracking_all_threads()

        if(cmd == "stop_tracking_all_threads"):
            self.stop_tracking_all_threads()

        if(cmd == "start_log"):
            self.start_log(args)

        if(cmd == "stop_log"):
            self.stop_log()

        if(cmd == "get_synopsis"):
            self.get_synopsis()

    def read_config(self):
        #blacklists
        self.dlog("Reading config")
        if(defined("settings.bl_modules")):
            self.bl_modules = settings.bl_modules
        else:
            self.bl_modules = default_module_blacklist

        if(defined("settings.bl_instructions")):
            self.bl_instructions = settings.bl_instructions
        else:
            self.bl_instructions = []

        if(defined("settings.bl_addresses")):
            self.bl_addresses = settings.bl_addresses
        else:
            self.bl_addresses = []

        if(defined("settings.bl_rvas")):
            self.bl_rvas = settings.bl_rvas
        else:
            self.bl_rvas = []

        #markers
        self.dlog("Reading markers")

        if(defined("settings.ma_addrs")):
            self.dlog("generic markers found", 2)
            self.markers = settings.ma_addrs
        else:
            self.markers = []

        if(defined("settings.ma_st_addrs")):
            self.dlog("ST markers found", 2)
            self.st_markers = settings.ma_st_addrs
        else:
            self.st_markers = []

        if(defined("settings.ma_end_addrs")):
            self.dlog("END markers found", 2)
            self.end_markers = settings.ma_end_addrs
        else:
            self.end_markers = []

        if(defined("settings.ma_react_addrs")):
            self.dlog("react markers found", 2)
            self.react_markers = settings.ma_react_addrs
        else:
            self.react_markers = []

        if(defined("settings.ma_rd_addrs")):
            self.dlog("RD markers found", 2)
            self.rd_markers = settings.ma_rd_addrs
        else:
            self.rd_markers = []

        #rvas
        self.dlog("Loading RVAs")
        if(defined("settings.ma_rvas")):
            self.dlog("generic RVAs found", 2)
            self.markers += self.resolve_rvas(settings.ma_rvas)
        if(defined("settings.ma_st_rvas")):
            self.dlog("ST RVAs found", 2)
            self.st_markers += self.resolve_rvas(settings.ma_st_rvas)
        if(defined("settings.ma_end_rvas")):
            self.dlog("END RVAs found", 2)
            self.end_markers += self.resolve_rvas(settings.ma_end_rvas)
        if(defined("settings.ma_react_rvas")):
            self.dlog("REACT RVAs found", 2)
            self.react_markers += self.resolve_rvas(settings.ma_react_rvas)
        if(defined("settings.ma_rd_rvas")):
            self.dlog("RD RVAs found", 2)
            self.rd_markers += self.resolve_rvas(settings.ma_rd_rvas)

        #handlers
        self.dlog("Loading handlers")
        if(defined("settings.marker_handler")):
            self.dlog("generic marker handler found", 2)
            self.marker_handler = settings.marker_handler
        else:
            self.marker_handler = phony_handler

        if(defined("settings.st_marker_handler")):
            self.dlog("ST marker handler found", 2)
            self.st_marker_handler = settings.st_marker_handler
        else:
            self.st_marker_handler = default_st_handler

        if(defined("settings.end_marker_handler")):
            self.dlog("END marker handler found", 2)
            self.end_marker_handler = settings.end_marker_handler
        else:
            self.end_marker_handler = default_end_handler

        if(defined("settings.react_marker_handlers")):
            self.dlog("REACT marker handlers found", 2)
            self.react_marker_handlers = settings.react_marker_handlers
        else:
            self.react_marker_handlers = {}
        
        if(defined("settings.rd_marker_handlers")):
            self.dlog("RD marker handler found", 2)
            self.rd_marker_handler = settings.rd_marker_handler
        else:
            self.rd_marker_handler = default_rd_handler
        
        if(defined("settings.bp_handler")):
            self.dlog("BP handler found", 2)
            self.bp_handler = settings.bp_handler
        else:
            self.bp_handler = default_bp_handler
        
        if(defined("settings.ss_handler")):
            self.dlog("SS handler found", 2)
            self.ss_handler = settings.ss_handler
        else:
            self.ss_handler = default_ss_handler
        
        if(defined("settings.av_handler")):
            self.dlog("AV handler found", 2)
            self.av_handler = settings.av_handler
        else:
            self.av_handler = default_av_handler
        
        #directories
        self.dlog("Reading directories")
        if(defined("settings.samples_dir")):
            self.dlog("samples dir found", 2)
            self.samples_dir = settings.samples_dir
        else:
            self.samples_dir = samples_dir = "z:\\samples"
        self.crashed_dir = samples_dir + "\\crashed"
        self.hanged_dir = samples_dir + "\\hanged"
        self.clean_dir = samples_dir + "\\clean"
        self.log_file = samples_dir + "\\log-"

        self.dlog("After config:")
        self.dlog("generic markers: %s" % self.markers)
        self.dlog("ST markers: %s" % self.st_markers)
        self.dlog("END markers: %s" % self.end_markers)
        self.dlog("REACT markers: %s" % self.react_markers)
        self.dlog("RD markers: %s" % self.rd_markers)

    def stop(self):
        if(self.debugger_active == True):
            self.debugger_active = False
            l.acquire()
        self.dlog("Stopped")

    def start(self, to = None):
        self.dlog("About to start dbg", 2)
        if(to != None):
            Thread(target=self.stopping_routine, args=(to,)).start()
        if(settings.breaking == True):
            Thread(target=self.break_things, args=()).start()
        if(self.debugger_active == False):
            self.debugger_active = True
            l.release()
        self.dlog("Started")

    def stopping_routine(self, to):
        time.sleep(to)
        self.debugger_active = False
        self.dlog("Stopped after %d s" % to)

    def looping_routine(self):
        self.dlog("Entering looping routine", 2)
        self.debugger_active = True
        self.debug_event_loop()
        self.dlog("Leaving looping routine", 2)

    def resolve_rvas(self, rvas):
        for ma_rva in rvas:
            self.dlog("Resolving RVA at {0} + {1}".format(ma_rva[0], hex(ma_rva[1])), 2)
            mod_addr = 0x0
            for mod in self.enumerate_modules():
#                self.dlog(mod[0])
                if(mod[0].upper() == ma_rva[0].upper()):
                    mod_addr = mod[1]
                    break
            if(mod_addr == 0x0):
                self.dlog("Module %s not loaded" % ma_rva[0], 2)
                continue
            off = ma_rva[1]
            #writePipe("Found at {0}\n".format(hex(mod_addr+off)))
            yield (mod_addr+off, ma_rva[2])

    def list_tebs(self):
        for teb in self.tebs.keys():
            yield hex(teb)

    def addr_to_module_name(self, e_addr):
        my_module = self.addr_to_module(e_addr)
        if(my_module != None):
            my_name = my_module.szModule
        else:
            my_name = "[INVALID]"
        return my_name 

    def addr_to_module_name_and_baseaddr(self, e_addr):
        my_module = self.addr_to_module(e_addr)
        if(my_module != None):
            my_name = my_module.szModule
            my_addr = my_module.modBaseAddr
        else:
            my_name = "[INVALID]"
            my_addr = 0x0
        return (my_name, my_addr) 

    def safe_disasm(self, e_addr):
        try:
            e_instr = self.disasm(e_addr)
        except:
            e_instr = "[INVALID]"
        return e_instr

    def decode_op1(self, op1):
        my_op = op1
        if(my_op[0] == '['):
            my_op = self.decode_op1(my_op[1:-1])
            my_op = int(struct.unpack("<i", "".join(self.read(my_op, 4)))[0]) & 0xffffffff
            return my_op
        for reg in regs:
            if(my_op.upper() == reg):
                my_op = self.get_register(reg)
                return my_op & 0xffffffff
        if(len(my_op.split("+")) >1):
            (a,b) = my_op.split("+")
            a = self.decode_op1(a)
            b = self.decode_op1(b)
            my_op = a+b
            return my_op & 0xffffffff
        if(len(my_op.split("-")) >1):
            (a,b) = my_op.split("-")
            a = self.decode_op1(a)
            b = self.decode_op1(b)
            my_op = a-b
            return my_op & 0xffffffff
        if(len(my_op.split("*")) >1):
            (a,b) = my_op.split("*")
            a = self.decode_op1(a)
            b = self.decode_op1(b)
            my_op = a*b
            return my_op & 0xffffffff
        return int(my_op, 16) & 0xffffffff

    def check_blacklists(self, e_addr):

        #addresses
        if(len(self.bl_addresses) > 0):
            for bl_addr in self.bl_addresses:
                if(e_addr == bl_addr):
                    self.dlog("Address blacklisted, won't handle", 3)
                    return True

        #modules
        (mod_name, mod_addr) = self.addr_to_module_name_and_baseaddr(e_addr)
#        dlog("Addr: " + hex(e_addr))
#        dlog("Module: " + my_name)
#        dlog("Module base: " + hex(my_addr))

        if(mod_name != "[INVALID]"):
            if(len(self.bl_modules) > 0):
                for bl_mod in self.bl_modules:
                    if(mod_name == bl_mod):
                        self.dlog("Address blacklisted, won't handle", 3)
                        return True
    
        #check rvas
        if(mod_name != "[INVALID]"):
            if(len(self.bl_rvas) > 0):
                for rva in self.bl_rvas:
                    if(rva[0] == mod_name):
                        my_off = e_addr - mod_addr
                        if(my_off == rva[1]):
                            self.dlog("RVA blacklisted, won't handle", 3)
                            return True
    
        #instructions
        if(len(self.bl_instructions) > 0):
            e_instr = self.safe_disasm(e_addr)
            for bl_instr in self.bl_instructions:
                if(e_instr == bl_instr):
                    self.dlog("Instruction blacklisted, won't handle", 3)
                    return True
    
        #checks finished, it's ok
        return False

    def attach_bp_handler(self):
        self.set_callback(EXCEPTION_BREAKPOINT, self.bp_handler)
        self.dlog("AV handler installed", 2)

    def attach_ss_handler(self):
        self.set_callback(EXCEPTION_SINGLE_STEP, self.ss_handler)
        self.dlog("AV handler installed", 2)

    def attach_av_handler(self):
        self.set_callback(EXCEPTION_ACCESS_VIOLATION, self.av_handler)
        self.dlog("AV handler installed", 2)

    def attach_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.markers:
            self.dlog("0x%x: %s" % (ma_addr[0], self.marker_handler))
            self.bp_set(ma_addr[0], handler = self.marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.counters[ma_addr[0]] = 0

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def attach_st_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.st_markers:
            self.dlog("0x%x: %s" % (ma_addr[0], self.st_marker_handler))
            self.bp_set(ma_addr[0], handler = self.st_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def attach_end_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.end_markers:
            self.dlog("0x%x: %s" % (ma_addr[0], self.end_marker_handler))
            self.bp_set(ma_addr[0], handler = self.end_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.dlog("Pass count: %d" % self.breakpoints[ma_addr[0]].pass_count)
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def attach_react_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.react_markers:
            reaction = ma_addr[1]
            self.dlog("0x%x: function: %s scripts: %s" % (ma_addr[0], reaction[1], reaction[2]))
            self.bp_set(ma_addr[0], handler = reaction[1])
            self.breakpoints[ma_addr[0]].pass_count = reaction[0]
            self.counters[ma_addr[0]] = 0

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def attach_rd_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.rd_markers:
            self.dlog("0x%x: %s" % (ma_addr[0], self.rd_marker_handler))
            self.bp_set(ma_addr[0], handler = self.rd_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.dlog("Pass count: %d" % self.breakpoints[ma_addr[0]].pass_count)
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_st_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.st_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_end_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.end_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_react_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.react_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_rd_markers(self):
        self.preparation_lock.acquire()
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.rd_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        self.dlog("[UNLOCK] Preparation section", 2)

    def break_things(self):
        if(random() > 0.05):
            return
        self.dlog("Breaking random stuff", 2)

        addr = int(random() * 0xffffffff)
        threads =  self.enumerate_threads()
        thread_num = int(random() * len(threads))
        self.dlog(str(thread_num) + ": " + str(threads[thread_num]))
    
        thread_handle = self.open_thread(threads[thread_num])
        thread_context = self.get_thread_context(thread_handle)
        thread_context.Eip = addr
        self.set_thread_context(thread_context, thread_handle)

    def check_counters(self, ea):
        self.counters[ea] = (self.counters[ea][PASS_COUNT], self.counters[ea][HIT_COUNT]+1)
        self.dlog("Current hit no: %d, pass count: %d" % (self.counters[ea][HIT_COUNT], self.counters[ea][PASS_COUNT]), 2)
    
        if(self.counters[ea][HIT_COUNT] == self.counters[ea][PASS_COUNT]+1):
            return True
        else:
            return False

    def dump_modules(self):
        self.write_log("Modules map:") 
        for mod in self.enumerate_modules_w_size():
            self.write_log("%s 0x%x 0x%x" % (mod[0], mod[1], mod[2]))
        self.write_log("--")

    def dump_threads(self):
        self.write_log("Threads:") 
        for thread in self.enumerate_threads():
            self.write_log("Tracking [%x] " % thread)
            self.tracked_threads.append(thread)
        self.write_log("--")

    def track_all_threads(self):
        for thread in self.enumerate_threads():
            self.tracked_threads.append(thread)

    def start_tracking_all_threads(self):
        for thread_id in self.tracked_threads:
            thread_handle  = self.open_thread(thread_id)
            if(thread_handle == self.h_thread):
                continue
            self.single_step(True, thread_handle)
            self.close_handle(thread_handle)

    def stop_tracking_all_threads(self):
        for thread_id in self.tracked_threads:
            self.dlog("Stop tracking [%x] " % thread_id)
            thread_handle  = self.open_thread(thread_id)
            if(thread_handle == self.h_thread):
                continue
            self.single_step(False, thread_handle)
            self.close_handle(thread_handle)

    def start_log(self, name):
        self.logStarted = True
        self.log_file = "%s-%d" % (name, self.pid)
        self.log = open(self.log_file, "w")

    def write_log(self, data):
        self.log.write("%s\n" % data)
        self.log.flush()

    def stop_log(self):
        self.log.close()

    def get_synopsis(self):
#        self.binner.send(self.crash_bin.export_string())
#        print(self.crash_bin.crash_synopsis())
        self.binner.send(self.crash_bin.crash_synopsis())
        self.ok() 

if __name__ == '__main__':
    debugger_routine()
