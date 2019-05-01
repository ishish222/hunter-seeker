import sys

sys.path.append("e:\\server\\paimei")
sys.path.append("e:\\common")

import cProfile, pstats, io
import settings
from threading import Thread, Lock, ThreadError
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
from mutex_2 import NamedMutex
from trace import trace, add_default_blacklists

### functions
# unable to move cause settings module is not visible

HIT_COUNT = 1
PASS_COUNT = 0

### default blacklists

end = "=[OK]="

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
    dbg.dlog("acquire attempt by phony handler")
    dbg.dbg_lock_acquire(dbg.send_lock)
    dbg.dlog("Phony handler!!")
    dbg.dlog("release attempt by phony handler")
    dbg.dbg_lock_release(dbg.send_lock)
    return DBG_CONTINUE

def default_st_handler(dbg):
    dbg.dlog("acquire attempt by st handler")
    dbg.dbg_lock_acquire(dbg.send_lock)
    if(dbg.check_counters(dbg.exception_address)):
        dbg.dlog("ST marker reached")
        dbg.signal_st()
    dbg.dbg_lock_release(dbg.send_lock)
    dbg.dlog("release attempt by st handler")
    return DBG_CONTINUE


def default_end_handler(dbg):
    dbg.dlog("acquire attempt by end handler")
    dbg.dbg_lock_acquire(dbg.send_lock)
    if(dbg.check_counters(dbg.exception_address)):
        dbg.dlog("END marker reached")
        dbg.signal_ma()
    dbg.dlog("release attempt by end handler")
    dbg.dbg_lock_release(dbg.send_lock)
    return DBG_CONTINUE

def default_rd_handler(dbg):
    dbg.dlog("acquire attempt by rd handler")
    dbg.dbg_lock_acquire(dbg.send_lock)
    if(dbg.check_counters(dbg.exception_address)):
        dbg.dlog("RD marker reached")
        dbg.signal_rd()
    dbg.dlog("release attempt by rd handler")
    dbg.dbg_lock_release(dbg.send_lock)
    return DBG_CONTINUE

def default_bp_handler(dbg):
    dbg.dlog("acquire attempt by bp handler")
    dbg.dbg_lock_acquire(dbg.send_lock)
    dlog("EXCEPTION_BREAKPOINT")

    dbg.signal_ex()
    dbg.dbg_lock_release(dbg.send_lock)
    dbg.dlog("release attempt by bp handler")
    return DBG_CONTINUE
    
def default_ss_handler(dbg):
    dbg.dlog("acquire attempt by ss handler")
    dbg.dbg_lock_acquire(dbg.send_lock)
    dlog("EXCEPTION_SINGLE_STEP")

    dbg.signal_ex()
    dbg.dlog("release attempt by ex handler")
    dbg.dbg_lock_release(dbg.send_lock)
    return DBG_CONTINUE
    
def default_av_handler(dbg):
    dbg.dlog("acquire attempt by av handler")
    dbg.dbg_lock_acquire(dbg.send_lock)
    dbg.dlog("avThread")

    dbg.crash_bin.record_crash(dbg)
    dbg.signal_cr()

    # take over cmd parsing?
    dbg.dlog("release attempt by av handler")
    dbg.dbg_lock_release(dbg.send_lock)

    while True:
        pass
#        cmd, args = readline(dbg.binner)
#        dbg.dlog("Received: %s" % cmd, 2)
#        dbg.execute(cmd, args)

#    dbg.detach_st_markers()
#    dbg.detach_end_markers()
#    dbg.detach_rd_markers()
#    dbg.detach_react_markers()
#    dbg.detach_markers()
#    dbg.detach_bp_handler()
#    dbg.detach_av_handler()
#    dbg.detach_ss_handler()
#    dbg.detach_av_handler()
#    dbg.terminate_process(method = "exitprocess")
#    dbg.schedule_termination = True
#    dbg.get_synopsis()
#    dbg.detach()
#    exit()

    # wait to die?
#    return DBG_EXCEPTION_NOT_HANDLED
    return DBG_CONTINUE

### debugger class
class debugger(pydbg):

    def __init__(self, binner):
        pydbg.__init__(self)
        self.start_mutex = NamedMutex("StartMutex")
        self.stop_mutex = NamedMutex("StopMutex")
        self.binner = binner
        self.preparation_lock = Lock()
        self.send_lock = Lock()
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
        self.last_state = ""
        self.schedule_termination = False
        self.debug = False
        self.last_log_file = None
        self.pr = None
        self.log_level = settings.log_level
        if(settings.debug == True):
            self.debug = True
            self.last_log_file = open("e:\\logs\\init_log.txt", "a", 0)
        else:
            self.debug = False
            self.last_log_file = None
        self.walk = None

    def dbg_lock_acquire(self, lock):
        lock.acquire()
        self.dlog("lock acquired")
    
    def dbg_lock_release(self, lock):
        try:
            lock.release()
        except ValueError:
            self.dlog("ValueError")
        except ThreadError:
            self.dlog("ThreadError")
        self.dlog("lock released")
        
    def dlog(self, data, level=0):
        if(self.debug == True):
            if(self.log_level <0):
                return
            if(level > self.log_level):
                return
            self.last_log_file.write("[%s] %s\n" % (timestamp(), data))

    def signal_st(self):
        self.dlog("Signaled: ST", 1)
        self.last_state = "ST"
        self.binner_send("SAST")

    def signal_ma(self):
        self.dlog("Signaled: MA", 1)
        self.last_state = "MA"
        self.binner_send("SAMA")

    def signal_ws(self):
        self.dlog("Signaled: WS", 1)
        self.last_state = "WS"
        self.binner_send("SAWS")

    def signal_we(self):
        self.dlog("Signaled: WE", 1)
        self.last_state = "WE"
        self.binner_send("SAWE")

    def signal_rd(self):
        self.dlog("Signaled: RD", 1)
        self.last_state = "RD"
        self.binner_send("SARD")

    def signal_rs(self):
        self.dlog("Signaled: RS", 1)
        self.last_state = "RS"
        self.binner_send("SARS")

    def signal_cr(self):
        self.dlog("Signaled: CR", 1)
        self.last_state = "CR"
        #self.binner_send("SACR%s%s" % (self.crash_binning.last_crash.exception_address, end))
        self.binner_send("SACR")

    def signal_ex(self, str_ec="unknown_"):
        self.dlog("Signaled: EX", 1)
        self.last_state = "EX"
        self.binner_send("EX%s" % str_ec)

    def ok(self):
        self.binner_send("=[OK]=")

    def reqScript(self, script_code):
        self.dlog("Signaled: SR", 1)
        self.binner_send("SR%s" % script_code) #already locked

    def reqScriptArgs(self, script_code, args):
        self.dlog("Signaled: SL", 1)
        self.binner_send("SL%s %s%s" % (script_code, args, end))

    def execute(self, cmd, args):
        if(args == None):
            dbg_cmds[cmd][0](self)
        else:
            dbg_cmds[cmd][0](self, args)

    def prepare_terminate():
        self.schedule_termination = True
#        self.terminate_process(method = "exitprocess")
        self.start()
        self.stop()

    def attach2(self, pid):
        self.attach(int(pid))

    def binner_send(self, data):
#        self.dbg_lock_acquire(self.send_lock)
        print(data)
        self.binner.send(data)
#        self.dbg_lock_release(self.send_lock)

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

        if(defined("settings.ma_ws_addrs")):
            self.dlog("WS markers found", 2)
            self.ws_markers = settings.ma_ws_addrs
        else:
            self.ws_markers = []

        if(defined("settings.ma_we_addrs")):
            self.dlog("WE markers found", 2)
            self.we_markers = settings.ma_we_addrs
        else:
            self.we_markers = []

        if(defined("settings.ma_bl_addrs")):
            self.dlog("BL markers found", 2)
            self.bl_markers = settings.ma_bl_addrs
        else:
            self.bl_markers = []

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
        if(defined("settings.ma_ws_rvas")):
            self.dlog("WS RVAs found", 2)
            self.ws_markers += self.resolve_rvas(settings.ma_ws_rvas)
        if(defined("settings.ma_we_rvas")):
            self.dlog("WE RVAs found", 2)
            self.we_markers += self.resolve_rvas(settings.ma_we_rvas)
        if(defined("settings.ma_bl_rvas")):
            self.dlog("BL RVAs found", 2)
            self.bl_markers += self.resolve_rvas(settings.ma_bl_rvas)

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
        
        if(defined("settings.ex_handler")):
            self.dlog("EX handler found", 2)
            self.ex_handler = settings.ex_handler
        else:
            self.ex_handler = None
        
        #directories
        self.dlog("Reading directories")
        if(defined("settings.samples_dir")):
            self.dlog("samples dir found", 2)
            self.samples_dir = settings.samples_dir
        else:
            self.samples_dir = samples_dir = "e:\\samples"
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
        self.dlog("Trying to stop")
        if(self.debugger_active == True):
#            self.dlog("acquire attempt by stop")
#            self.dbg_lock_acquire(self.send_lock) #if you have something to send, you'll have to wait for another stop
            self.debugger_active = False
 #           self.dbg_lock_acquire(l)
        self.dlog("Stopped")

    def stop_force(self):
        self.dlog("Trying to stop forced")
        if(self.debugger_active == True):
            self.debugger_active = False
#            self.dlog("acquire attempt by forced stop")
            self.dbg_lock_acquire(self.send_lock) #if you have something to send, you'll have to wait for another stop
            self.dbg_lock_acquire(l)
        self.dlog("Stopped")

    def start(self, to = None):
        self.dlog("About to start dbg", 2)
        if(to != None):
            to = float(to)
            Thread(target=self.stopping_routine, args=(to,)).start()
        if(settings.breaking == True):
            self.break_things()
        if(self.debugger_active == False):
            self.debugger_active = True
#            self.dbg_lock_release(l)
#            self.dbg_lock_release(self.send_lock)
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
                self.dlog(mod[0])
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
        for teb in list(self.tebs.keys()):
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
        dlog("Addr: " + hex(e_addr))
        dlog("Module: " + my_name)
        dlog("Module base: " + hex(my_addr))

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

    def set_callback2(self, argss):
        args = argss.split(" ")
        return self.set_callback(args[0], args[1])

    def attach_bp_handler(self):
        self.set_callback(EXCEPTION_BREAKPOINT, self.bp_handler)
        self.dlog("BP handler installed", 2)

    def attach_ss_handler(self):
        self.set_callback(EXCEPTION_SINGLE_STEP, self.ss_handler)
        self.dlog("SS handler installed", 2)

    def attach_av_handler(self):
        self.set_callback(EXCEPTION_ACCESS_VIOLATION, self.av_handler)
        self.set_callback(STATUS_IN_PAGE_ERROR, self.av_handler)
        self.set_callback(STATUS_ILLEGAL_INSTRUCTION, self.av_handler)
        self.set_callback(STATUS_INVALID_DISPOSITION, self.av_handler)
        self.set_callback(STATUS_INTEGER_DIVIDE_BY_ZERO, self.av_handler)
        self.set_callback(STATUS_INTEGER_OVERFLOW, self.av_handler)
        self.set_callback(STATUS_PRIVILEGED_INSTRUCTION, self.av_handler)
        self.set_callback(STATUS_STACK_OVERFLOW, self.av_handler)
        self.set_callback(STATUS_UNHANDLED_EXCEPTION, self.av_handler)
        self.set_callback(STATUS_PIPE_BROKEN, self.av_handler)
        self.dlog(self.callbacks[EXCEPTION_ACCESS_VIOLATION])
        self.dlog("AV handler installed", 2)

    def attach_ex_handler(self):
        self.default_exc_callback = self.ex_handler
        self.dlog(self.default_exc_callback)

    def detach_bp_handler(self):
        self.set_callback(EXCEPTION_BREAKPOINT, self.exception_handler_breakpoint)
        self.dlog("BP handler uninstalled", 2)

    def detach_ss_handler(self):
        self.set_callback(EXCEPTION_SINGLE_STEP, self.exception_handler_single_step)
        self.dlog("SS handler uninstalled", 2)

    def detach_av_handler(self):
        self.dlog("detaching AV handler")
        self.callbacks.pop(EXCEPTION_ACCESS_VIOLATION)
        self.callbacks.pop(STATUS_IN_PAGE_ERROR)
        self.callbacks.pop(STATUS_ILLEGAL_INSTRUCTION)
        self.callbacks.pop(STATUS_INVALID_DISPOSITION)
        self.callbacks.pop(STATUS_INTEGER_DIVIDE_BY_ZERO)
        self.callbacks.pop(STATUS_INTEGER_OVERFLOW)
        self.callbacks.pop(STATUS_PRIVILEGED_INSTRUCTION)
        self.callbacks.pop(STATUS_STACK_OVERFLOW)
        self.callbacks.pop(STATUS_UNHANDLED_EXCEPTION)
        self.callbacks.pop(STATUS_PIPE_BROKEN)
        self.dlog("AV handler uninstalled", 2)

    def detach_ex_handler(self):
        self.default_exc_callback = None

    def attach_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.markers:
            self.dlog("0x%x: %s" % (ma_addr[0], self.marker_handler))
            self.bp_set(ma_addr[0], handler = self.marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def attach_st_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.st_markers:
            self.dlog("0x%x: %s" % (ma_addr[0], self.st_marker_handler))
            self.bp_set(ma_addr[0], handler = self.st_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def attach_end_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.end_markers:
            self.dlog("0x%x: %s" % (ma_addr[0], self.end_marker_handler))
            self.bp_set(ma_addr[0], handler = self.end_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.dlog("Pass count: %d" % self.breakpoints[ma_addr[0]].pass_count)
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def attach_react_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.react_markers:
            reaction = ma_addr[1]
            self.dlog("0x%x: function: %s scripts: %s" % (ma_addr[0], reaction[1], reaction[2]))
            self.bp_set(ma_addr[0], handler = reaction[1])
            self.breakpoints[ma_addr[0]].pass_count = reaction[0]
            self.counters[ma_addr[0]] = (ma_addr[1][0], 0) #take care of this

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def attach_rd_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.rd_markers:
            self.dlog("0x%x: %s" % (ma_addr[0], self.rd_marker_handler))
            self.bp_set(ma_addr[0], handler = self.rd_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.dlog("Pass count: %d" % self.breakpoints[ma_addr[0]].pass_count)
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.markers:
            self.bp_del(ma_addr[0])

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_st_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.st_markers:
            self.bp_del(ma_addr[0])

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_end_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.end_markers:
            self.bp_del(ma_addr[0])

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_react_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.react_markers:
            self.bp_del(ma_addr[0])

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def detach_rd_markers(self):
        self.dbg_lock_acquire(self.preparation_lock)
        self.dlog("[LOCK] Preparation section", 2)

        for ma_addr in self.rd_markers:
            self.bp_del(ma_addr[0])

        self.dbg_lock_release(self.preparation_lock)
        self.dlog("[UNLOCK] Preparation section", 2)

    def break_things(self):
        # break 1 in 20:
        if(random() > 0.05):
            return
        self.dlog("Breaking random stuff", 2)
        print("Breaking random stuff")

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
            self.counters[ea] = (self.counters[ea][PASS_COUNT], 0) # zeroing
            return True
        else:
            return False

    def dump_modules(self):
        self.write_log("Modules map:") 
        for mod in self.enumerate_modules_w_size():
            self.write_log("%s 0x%x 0x%x" % (mod[0], mod[1], mod[2]))
        self.write_log("--")

    def get_modules(self):
        for mod in self.enumerate_modules_w_size():
            yield (mod[0], mod[1], mod[2])

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
        if(self.debug == False):
            return
        self.logStarted = True
        if(self.last_log_file != None):
            self.last_log_file.close()
        print(("debugger opening: %s-%d.txt" % (name, self.pid)))
        self.last_log_file = open("%s-%d.txt" % (name, self.pid), "a", 0)

    def write_log(self, data):
        self.log.write("%s\n" % data)
        self.log.flush()

    def stop_log(self):
        if(self.last_log_file != None):
            self.last_log_file.close()
            self.last_log_file = None
        self.logStarted = False


    def take_a_trace2(self, pid):
        self.dlog("taking a trace")
        from subprocess import Popen
        st_addr = self.st_markers[0][0]
        end_addr = self.end_markers[0][0]
   
        self.dlog("e:\\server\\trace.exe %d 0x%x 0x%x" % (pid, st_addr, end_addr))
        Popen("e:\\server\\trace.exe %d 0x%x 0x%x" % (pid, st_addr, end_addr))

    def take_a_trace(self, args):
        st_addr = self.st_markers[0][0]
        end_addr = self.end_markers[0][0]
    

        self.dlog("Talking a trace from 0x%x until 0x%x" % (st_addr, end_addr))
    
        self.walk = None
        self.walk = trace(st_addr, end_addr, dbg=self)

        add_default_blacklists(self.walk)

        for bl_marker in self.bl_markers:
            self.walk.addr_blacklist.append(bl_marker[0])

#        self.walk.install_walk_bp()
#        self.walk.install_walk_end_bp()
#        self.walk.register_callbacks()
#
        self.dlog("Trace is prepared")
        print("Trace is prepared")

    def take_a_walk(self, args):
        depth, gf_path = args.split(" ")
        depth = int(depth)

        st_addr = self.ws_markers[0][0]
        end_addr = self.we_markers[0][0]

        self.dlog("Talking a walk from 0x%x until 0x%x, level %d, saving into %s" % (st_addr, end_addr, depth, gf_path))
    
        self.walk = None
        self.walk = walk(st_addr, end_addr, max_level=depth, gf_path=gf_path, dbg=self)

        add_default_blacklists(self.walk)
        for bl_marker in self.bl_markers:
            self.walk.addr_blacklist.append(bl_marker[0])

#        self.walk.install_walk_bp()
#        self.walk.install_walk_end_bp()
#        self.walk.register_callbacks()
#
        self.dlog("Walk is prepared")

    def take_a_walk2(self, args):
        depth, gf_path, addr = args.split(" ")
        depth = int(depth)
        addr = int(addr, 16)

        st_addr = addr
        instr = self.get_instruction(addr)
        end_addr = addr + instr.length

        self.dlog("Talking a walk from 0x%x until 0x%x, level %d, saving into %s" % (st_addr, end_addr, depth, gf_path))
    
        self.walk = None
        self.walk = walk(st_addr, end_addr, max_level=depth, gf_path=gf_path, dbg=self)

        add_default_blacklists(self.walk)
        for bl_marker in self.bl_markers:
            self.walk.addr_blacklist.append(bl_marker[0])

        self.dlog("Walk is prepared")

    def save_synopsis(self, filee):
        f = open("%s.synopsis" % filee, "a", 0)
        f.write(self.crash_bin.crash_synopsis())

        f.write("Modules:")

        #include module info

        for name,off,size in self.get_modules():
            f.write("%s 0x%x 0x%x" % (name,off,size))

        f.flush()
        f.close()
        # + signatures

    def get_ea(self):
        self.dlog("In get_ea")
        ea = self.crash_bin.last_crash.exception_address
        self.dlog("Sending: LO%x%s" % (ea, end))
        self.binner_send("LO%x%s" % (ea, end))

    def get_synopsis(self):
        self.binner_send(self.crash_bin.crash_synopsis())
#        self.ok() 

    def start_profiling(self):
        #if profiling
        self.pr = cProfile.Profile()
        self.pr.enable()
#        self.stats = open("e:\\logs\\stats-debugger", "a")
#        self.stats.write("1\n")
#        self.stats.flush()

    def close_logs(self):
        self.last_log_file.flush()
        self.last_log_file.close()

    def stop_profiling(self):
        if(self.pr != None):
            self.pr.disable()

    def detach(self):
        self.dlog("detaching")
        #super(pydbg, self).detach()
        pydbg.detach()

    def dump_stats(self, fname):
        if(self.pr != None):
            try:
                s = io.StringIO()
                sortby = 'cumulative'
                ps = pstats.Stats(self.pr, stream=s).sort_stats(sortby)
                ps.dump_stats(fname)
            except Exception:
                self.dlog(str(sys.exc_info()))

# protocol def: required operation, need to read args?
dbg_cmds = {}
dbg_cmds["S1"] = (debugger.start, False)
dbg_cmds["S2"] = (debugger.start, True)
dbg_cmds["S3"] = (debugger.stop, False)
dbg_cmds["Q1"] = (debugger.detach, False)
dbg_cmds["PT"] = (debugger.prepare_terminate, False)
dbg_cmds["AT"] = (debugger.attach2, True)
dbg_cmds["RC"] = (debugger.read_config, False)
dbg_cmds["A1"] = (debugger.attach_av_handler, False)
dbg_cmds["A2"] = (debugger.attach_bp_handler, False)
dbg_cmds["A3"] = (debugger.attach_ss_handler, False)
dbg_cmds["A4"] = (debugger.attach_markers, False)
dbg_cmds["A5"] = (debugger.attach_st_markers, False)
dbg_cmds["A6"] = (debugger.attach_end_markers, False)
dbg_cmds["A7"] = (debugger.attach_react_markers, False)
dbg_cmds["A8"] = (debugger.attach_rd_markers, False)
dbg_cmds["A9"] = (debugger.attach_ex_handler, False)
dbg_cmds["D4"] = (debugger.detach_markers, False)
dbg_cmds["D5"] = (debugger.detach_st_markers, False)
dbg_cmds["D6"] = (debugger.detach_end_markers, False)
dbg_cmds["D7"] = (debugger.detach_react_markers, False)
dbg_cmds["D8"] = (debugger.detach_rd_markers, False)
dbg_cmds["D9"] = (debugger.detach_ex_handler, False)
dbg_cmds["T1"] = (debugger.track_all_threads, False)
dbg_cmds["T2"] = (debugger.start_tracking_all_threads, False)
dbg_cmds["T3"] = (debugger.stop_tracking_all_threads, False)
dbg_cmds["DM"] = (debugger.dump_threads, False)
dbg_cmds["DT"] = (debugger.dump_modules, False)
dbg_cmds["SC"] = (debugger.set_callback2, True)
dbg_cmds["LS"] = (debugger.start_log, True)
dbg_cmds["LQ"] = (debugger.stop_log, False)
dbg_cmds["LW"] = (None, True)
dbg_cmds["GS"] = (debugger.get_synopsis, False)
dbg_cmds["RS"] = (debugger.start_profiling, False)
dbg_cmds["RD"] = (debugger.dump_stats, True)
dbg_cmds["SS"] = (debugger.save_synopsis, True)
dbg_cmds["EA"] = (debugger.get_ea, False)
dbg_cmds["CL"] = (debugger.close_logs, False)
dbg_cmds["WK"] = (debugger.take_a_walk, True)
dbg_cmds["WN"] = (debugger.take_a_walk2, True)
dbg_cmds["TC"] = (debugger.take_a_trace, True)
dbg_cmds["TE"] = (debugger.take_a_trace2, True)

### main routines
def readline(stream):
    global dbg
    cmd = stream.recv(2)
    dbg.dlog("Recv: %s" % cmd)
    args = None
    if(dbg_cmds[cmd][1] == True): #has params
        dbg.dlog("reading params")
        data = ""
        while True:
            data += stream.recv(1)
            if(data[-6:] == end):
                args = data[:-6]
                break
    return cmd, args

def comm_routine(dbg):
    global running
    try:
        while True:
            cmd, args = readline(dbg.binner)
            dbg.dlog("Received: %s" % cmd, 2)
            dbg.execute(cmd, args)
    except Exception as e:
        dbg.dlog("Got exception in debugger_routine")
        dbg.dlog(e)
        dbg.dump_stats("e:\\logs\\debugger-%d-error" % dbg.pid)
        #send exception

def stopping_routine(dbg):
    while True:
        print("Waiting for stop mutex")
        dbg.stop_mutex.acquire()
#        print("Got it, releasing")
        dbg.debugger_routine = False
        dbg.stop_mutex.release()
        time.sleep(0.01)

def debugger_routine():
    global l
    global running
    global dbg
    running = True
    l = Lock()
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("127.0.0.1", 12347))
    dbg = debugger(s)
    dbg.dlog("Spawned & constructed", 2)
    dbg.dlog("Will accept attach now")
    cmd, args = readline(dbg.binner)
    dbg.execute(cmd, args)

    st = Thread(target=stopping_routine, args=(dbg,))
    st.start()
    ct = Thread(target=comm_routine, args=(dbg,))
    ct.start()

    # dbg in main thread, all commands in additional threads
    print((dbg.start_mutex))
    while True:
#        print("Waiting for mutex")
#        dbg.dbg_lock_acquire(l)
        dbg.start_mutex.acquire()
        dbg.start_mutex.release()
#        print("Got mutex, starting")
        dbg.debug_event_loop()
#        try:
#            dbg.dbg_lock_release(l)
#        except Exception:
#            pass
        if(dbg.schedule_termination == True):
            break

    ct.terminate()

    #waiting for permission
#    dbg.dlog("Waiting for permission")
    readline(dbg.binner)

    try:
#        self.dlog("Terminating")
        dbg.terminate_process()
    except AssertionError:
        pass
    except Exception:
        pass
#    dbg.dlog("Leaving")

if __name__ == '__main__':
    debugger_routine()

