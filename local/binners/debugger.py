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
    # increment hit count
    dbg.counters[dbg.exception_address] = (dbg.counters[dbg.exception_address][PASS_COUNT], dbg.counters[dbg.exception_address][HIT_COUNT]+1)
#    dbg.dlog("Current hit no: %d, pass count: %d" % (dbg.counters[dbg.exception_address][HIT_COUNT], dbg.counters[dbg.exception_address][PASS_COUNT]), 1)

    #check hits
    if(dbg.counters[dbg.exception_address][HIT_COUNT] == dbg.counters[dbg.exception_address][PASS_COUNT]+1):
        dbg.dlog("ST marker reached")
        dbg.signal_st()
        dbg.ok()

    return DBG_CONTINUE


def default_end_handler(dbg):
    # increment hit count
    dbg.counters[dbg.exception_address] = (dbg.counters[dbg.exception_address][PASS_COUNT], dbg.counters[dbg.exception_address][HIT_COUNT]+1)
#    dbg.dlog("Current hit no: %d, pass count: %d" % (dbg.counters[dbg.exception_address][HIT_COUNT], dbg.counters[dbg.exception_address][PASS_COUNT]), 1)

    #check hits
    if(dbg.counters[dbg.exception_address][HIT_COUNT] == dbg.counters[dbg.exception_address][PASS_COUNT]+1):
        dbg.dlog("END marker reached")
        dbg.signal_ma()
        dbg.ok()

    return DBG_CONTINUE

def default_rd_handler(dbg):
    # increment hit count
    dbg.counters[dbg.exception_address] = (dbg.counters[dbg.exception_address][PASS_COUNT], dbg.counters[dbg.exception_address][HIT_COUNT]+1)
#    dbg.dlog("Current hit no: %d, pass count: %d" % (dbg.counters[dbg.exception_address][HIT_COUNT], dbg.counters[dbg.exception_address][PASS_COUNT]), 1)

    #check hits
    if(dbg.counters[dbg.exception_address][HIT_COUNT] == dbg.counters[dbg.exception_address][PASS_COUNT]+1):
        dbg.dlog("RD marker reached")
        dbg.signal_rd()
        dbg.ok()

    return DBG_CONTINUE

def default_av_handler(dbg):
    dlog("avThread")
    
    dbg.binner.test_lock.acquire()
    dlog("test locked by AV")
    dbg.binner.crash_bin.record_crash(dbg)
    e_addr = dbg.binner.crash_bin.last_crash.exception_address
    if(cbg.check_blacklists(e_addr) == False):
        dbg.binner.status = "CR"
    dbg.binner.test_lock.release()
    return DBG_CONTINUE

def observer_bp_handler(dbg):
    ea = dbg.exception_address
    dbg.bp_del(dbg.exception_address)
    thread = dbg.dbg.dwThreadId
    thread_handle  = dbg.open_thread(thread)
    log_write("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
    dbg.single_step(True, thread_handle)
    dbg.close_handle(thread_handle)
    return DBG_CONTINUE

def observer_instr_handler(dbg):
    ea = dbg.exception_address
    thread = dbg.dbg.dwThreadId
    log_write("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
#    print("1")
#    log("%x" % ea) #+thread
#    print("%s" % ea)
    dbg.safe_disasm(ea)
    thread_handle  = dbg.open_thread(thread)
#    print("2")
    blacklisted = False
    if(dbg.mnemonic == "call"):
#        print("got call")
#        target_module = dbg.addr_to_module(decode_op1(dbg, dbg.op1))
        
#        if(target_module == None):
#            target_name = "unknown"
#        else:
#            target_name = target_module.szModule
        target_name = get_module(dbg, decode_op1(dbg, dbg.op1))
        log_write("got call to: {0}".format(target_name))
#        print("3")

        for mod in self.bl_modules:
            if(mod.upper() == target_name.upper()):
#                print("4")
                log_write(", skipping, will resume trace at: {1}".format(target_name, hex(ea + dbg.get_instruction(ea).length)))
#                print("skipping call to: %s" % target_name.upper())
                dbg.bp_set(ea + dbg.get_instruction(ea).length, handler = bp_handler)
                dbg.single_step(False, thread_handle)
                blacklisted = True
                break

#    print("5")
    if(blacklisted == False):
        dbg.single_step(True, thread_handle)
#    print("6")
    dbg.close_handle(thread_handle)
    return DBG_CONTINUE

def observer_st_handler(dbg):
    dbg.preparation_lock.acquire()
    dbg.binner.writePipe("reached start marker\n")
    ea = dbg.exception_address
    thread = dbg.dbg.dwThreadId
    print("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
    log_write("--- reached start marker ---")
#    writePipe("Pass count: %x\n" % dbg.breakpoints[dbg.exception_address].pass_count)
#    writePipe("Hit count: %x\n" % counters[dbg.exception_address])
 
    if(dbg.breakpoints[dbg.exception_address].pass_count > dbg.counters[dbg.exception_address]):
        dbg.counters[dbg.exception_address] += 1
#        writePipe("New hit count: %x\n" % counters[dbg.exception_address])
    else:
        print("installing instruction handler2")

        dbg.set_callback(EXCEPTION_BREAKPOINT, observer_bp_handler)
        dbg.set_callback(EXCEPTION_SINGLE_STEP, observer_instr_handler)

#        dbg.debugger_active = False
##        print("dbg thread: [%x]" % dbg.h_thread)
        for thread_id in dbg.enumerate_threads():
            print("Tracking [%x]: " % thread_id)
            thread_handle  = dbg.open_thread(thread_id)
            if(thread_handle == dbg.h_thread):
                continue
            try:
                dbg.single_step(True, thread_handle)
            except Exception, e:
                print("[Error]")
                print(e)
            dbg.close_handle(thread_handle)
#        dbg.debugger_active = True
#        dbg.debug_event_loop()
#        attach_end_markers(dbg)
    dbg.attach_end_markers(dbg)
    dbg.remove_st_markers(dbg)
    dbg.preparation_lock.release()
    print("here1")
    return DBG_CONTINUE

def observer_end_handler(dbg):
    dbg.binner.writePipe("reached end marker\n")
    print("reached end marker")
    dbg.preparation_lock.acquire()
    ea = dbg.exception_address
    thread = dbg.dbg.dwThreadId
    print("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
    for thread_id in dbg.enumerate_threads():
        print("Stop tracking [%x]: " % thread_id)
        thread_handle  = dbg.open_thread(thread_id)
        if(thread_handle == dbg.h_thread):
            continue
        try:
            dbg.single_step(False, thread_handle)
        except Exception, e:
            print("[Error]")
            print(e)
        dbg.close_handle(thread_handle)
        #start observing all threads

    log_write("--- reached end marker ---")
    #TODO u have to move it!
    dbg.binner.writePipe("Status: ME")
    dbg.remove_end_markers(dbg)
    dbg.preparation_lock.release()
    dbg.debugger_active = False
    ok()
    return DBG_CONTINUE

def test_handler(dbg):
    if(dbg.breakpoints[dbg.exception_address].pass_count > dbg.counters[dbg.exception_address]):
        dbg.counters[dbg.exception_address] += 1
        return DBG_CONTINUE

    print("[!0x%x]" % dbg.exception_address)
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
            dbg.dlog("Received: %s" % cmd, 1)
            if(cmd == "exit"):
                break
            dbg.execute(cmd)
            dbg.ok()
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
    dbg.dlog("Spawned & constructed", 1)
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
        self.counters = {}

    def dlog(self, data, level=0):
        dlog("[%d] %s" % (self.pid, data), level)

    def signal_ma(self):
        self.dlog("to binner: Status: MA", 2)
        self.binner.send("Status: MA")

    def signal_rd(self):
        self.dlog("to binner: Status: RD", 2)
        self.binner.send("Status: RD")

    def signal_rs(self):
        self.dlog("to binner: Status: RS", 2)
        self.binner.send("Status: RS")

    def signal_st(self):
        self.dlog("to binner: Status: ST", 2)
        self.binner.send("Status: ST")

    def ok(self):
        self.binner.send("=[OK]=")

    def reqScript(self, script):
        self.dlog("to binner: Status: SR\nScript: %s\n" % script, 2)
        self.binner.send("Status: SR\nScript: %s\n" % script)

    def execute(self, cmds):
        args = cmds.split(" ")
        cmd = args[0]

        if(cmd == "read_config"):
            self.read_config()
            self.dlog("config read", 1)

        if(cmd == "attach"):
            self.attach(int(args[1]))
            self.dlog("attached to %d" % int(args[1]))

        if(cmd == "start"):
            if(len(args) > 1):
                self.start(float(args[1]))
            else:
                self.start()
            self.dlog("Started", 1)

        if(cmd == "stop"):
            self.stop()
            self.dlog("Stopped", 1)

        if(cmd == "attach_markers"):
            self.attach_markers()
            self.dlog("markers attached", 1)

        if(cmd == "attach_st_markers"):
            self.attach_st_markers()
            self.dlog("ST markers attached", 1)

        if(cmd == "attach_end_markers"):
            self.attach_end_markers()
            self.dlog("END markers attached", 1)

        if(cmd == "attach_react_markers"):
            self.attach_react_markers()
            self.dlog("REACT markers attached", 1)

        if(cmd == "attach_rd_markers"):
            self.attach_rd_markers()
            self.dlog("RD markers attached", 1)

        if(cmd == "detach_markers"):
            self.detach_markers()
            self.dlog("markers detached", 1)

        if(cmd == "detach_st_markers"):
            self.detach_st_markers()
            self.dlog("ST markers detached", 1)

        if(cmd == "detach_end_markers"):
            self.detach_end_markers()
            self.dlog("END markers detached", 1)

        if(cmd == "detach_react_markers"):
            self.detach_react_markers()
            self.dlog("RECT markers detached", 1)

        if(cmd == "detach_rd_markers"):
            self.detach_rd_markers()
            self.dlog("RD markers detached", 1)

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
            self.dlog("generic markers found", 1)
            self.markers = settings.ma_addrs
        else:
            self.markers = []

        if(defined("settings.ma_st_addrs")):
            self.dlog("ST markers found", 1)
            self.st_markers = settings.ma_st_addrs
        else:
            self.st_markers = []

        if(defined("settings.ma_end_addrs")):
            self.dlog("END markers found", 1)
            self.end_markers = settings.ma_end_addrs
        else:
            self.end_markers = []

        if(defined("settings.ma_react_addrs")):
            self.dlog("react markers found", 1)
            self.react_markers = settings.ma_react_addrs
        else:
            self.react_markers = []

        if(defined("settings.ma_rd_addrs")):
            self.dlog("RD markers found", 1)
            self.rd_markers = settings.ma_rd_addrs
        else:
            self.rd_markers = []

        #rvas
        self.dlog("Loading RVAs")
        if(defined("settings.ma_rvas")):
            self.dlog("generic RVAs found", 1)
            self.markers += self.resolve_rvas(settings.ma_rvas)
        if(defined("settings.ma_st_rvas")):
            self.dlog("ST RVAs found", 1)
            self.st_markers += self.resolve_rvas(settings.ma_st_rvas)
        if(defined("settings.ma_end_rvas")):
            self.dlog("END RVAs found", 1)
            self.end_markers += self.resolve_rvas(settings.ma_end_rvas)
        if(defined("settings.ma_react_rvas")):
            self.dlog("REACT RVAs found", 1)
            self.react_markers += self.resolve_rvas(settings.ma_react_rvas)
        if(defined("settings.ma_rd_rvas")):
            self.dlog("RD RVAs found", 1)
            self.rd_markers += self.resolve_rvas(settings.ma_rd_rvas)

        #handlers
        self.dlog("Loading handlers")
        if(defined("settings.marker_handler")):
            self.dlog("generic marker handler found", 1)
            self.marker_handler = settings.marker_handler
        else:
            self.marker_handler = phony_handler

        if(defined("settings.st_marker_handler")):
            self.dlog("ST marker handler found", 1)
            self.st_marker_handler = settings.st_marker_handler
        else:
            self.st_marker_handler = default_st_handler

        if(defined("settings.end_marker_handler")):
            self.dlog("END marker handler found", 1)
            self.end_marker_handler = settings.end_marker_handler
        else:
            self.end_marker_handler = default_end_handler

        if(defined("settings.react_marker_handlers")):
            self.dlog("REACT marker handlers found", 1)
            self.react_marker_handlers = settings.react_marker_handlers
        else:
            self.react_marker_handlers = {}
        
        if(defined("settings.rd_marker_handlers")):
            self.dlog("RD marker handlers found", 1)
            self.rd_marker_handler = settings.rd_marker_handler
        else:
            self.rd_marker_handler = default_rd_handler
        
        #directories
        self.dlog("Reading directories")
        if(defined("settings.samples_dir")):
            self.dlog("samples dir found", 1)
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
        self.dlog("About to start dbg", 1)
        if(to != None):
            Thread(target=self.stopping_routine, args=(to,)).start()
        if(self.debugger_active == False):
            self.debugger_active = True
            l.release()
        self.dlog("Started")

    def stopping_routine(self, to):
        time.sleep(to)
        self.debugger_active = False
        self.dlog("Stopped after %d s" % to)

    def looping_routine(self):
        self.dlog("Entering looping routine", 1)
        self.debugger_active = True
        self.debug_event_loop()
        self.dlog("Leaving looping routine", 1)

    def resolve_rvas(self, rvas):
        for ma_rva in rvas:
            self.dlog("Resolving RVA at {0} + {1}".format(ma_rva[0], hex(ma_rva[1])), 1)
            mod_addr = 0x0
            for mod in self.enumerate_modules():
#                self.dlog(mod[0])
                if(mod[0].upper() == ma_rva[0].upper()):
                    mod_addr = mod[1]
                    break
            if(mod_addr == 0x0):
                self.dlog("Module %s not loaded" % ma_rva[0], 1)
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
            my_addr = module_name.modBaseAddr
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

    def check_blacklists(self, e_addr):

        #addresses
        for bl_addr in bl_addresses:
            if(e_addr == bl_addr):
                self.dlog("Address blacklisted, won't handle")
                return True

        #modules
        (mod_name, mod_addr) = self.addr_to_module_name_and_baseaddr(e_addr)
#        dlog("Addr: " + hex(e_addr))
#        dlog("Module: " + my_name)
#        dlog("Module base: " + hex(my_addr))

        if(module_name != "[INVALID]"):
            for bl_mod in bl_modules:
                if(my_name == bl_mod):
                    self.dlog("Address blacklisted, won't handle")
                    return True
    
        #check rvas
        if(my_name != "[INVALID]"):
            for rva in bl_rvas:
                if(rva[0] == my_name):
                    my_off = e_addr - my_addr
                    if(my_off == rva[1]):
                        self.dlog("RVA blacklisted, won't handle")
                        return True
    
        #instructions
        e_instr = self.safe_disasm(e_addr)
        for bl_instr in bl_instructions:
            if(e_instr == bl_instr):
                self.dlog("Instruction blacklisted, won't handle")
                return True
    
        #checks finished, it's ok
        return False

    def attach_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.markers:
#            self.dlog("0x%x: %s" % (ma_addr[0], self.marker_handler))
            self.bp_set(ma_addr[0], handler = self.marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.counters[ma_addr[0]] = 0

        self.preparation_lock.release()

    def attach_st_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.st_markers:
#            self.dlog("0x%x: %s" % (ma_addr[0], self.st_marker_handler))
            self.bp_set(ma_addr[0], handler = self.st_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.preparation_lock.release()

    def attach_end_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.end_markers:
#            self.dlog("0x%x: %s" % (ma_addr[0], self.end_marker_handler))
            self.bp_set(ma_addr[0], handler = self.end_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
#            self.dlog("Pass count: %d" % self.breakpoints[ma_addr[0]].pass_count)
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.preparation_lock.release()

    def attach_react_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.react_markers:
            reaction = ma_addr[1]
#            self.dlog("0x%x: function: %s scripts: %s" % (ma_addr[0], reaction[1], reaction[2]))
            self.bp_set(ma_addr[0], handler = reaction[1])
            self.breakpoints[ma_addr[0]].pass_count = reaction[0]
            self.counters[ma_addr[0]] = 0

        self.preparation_lock.release()

    def attach_rd_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.rd_markers:
#            self.dlog("0x%x: %s" % (ma_addr[0], self.rd_marker_handler))
            self.bp_set(ma_addr[0], handler = self.rd_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
#            self.dlog("Pass count: %d" % self.breakpoints[ma_addr[0]].pass_count)
            self.counters[ma_addr[0]] = (ma_addr[1], 0)

        self.preparation_lock.release()

    def detach_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()

    def detach_st_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.st_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()

    def detach_end_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.end_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()

    def detach_react_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.react_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()

    def detach_rd_markers(self):
        self.preparation_lock.acquire()

        for ma_addr in self.rd_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()

    def break_things(self):
        self.dlog("Breaking stuff")

        addr = int(random.random() * 0xffffffff)
        threads =  self.enumerate_threads()
        thread_num = int(random.random() * len(threads))
        self.dlog(str(thread_num) + ": " + str(threads[thread_num]))
    
        thread_handle = self.open_thread(threads[thread_num])
        thread_context = dbg.get_thread_context(thread_handle)
        thread_context.Eip = addr
        dbg.set_thread_context(thread_context, thread_handle)

if __name__ == '__main__':
#    try:
    debugger_routine()
#    except Exception, e:
#        print(e)
#        print("=[OK]=")
#        while True:
#            pass
