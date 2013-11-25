import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

import settings
from threading import Lock
from pydbg import *
from functions import *
from subprocess import Popen
import utils

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

### default handlers

def phony_handler(dbg):
    pass

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
    writePipe("reached start marker\n")
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
    dbg.remove_start_markers(dbg)
    dbg.preparation_lock.release()
    print("here1")
    return DBG_CONTINUE

def observer_end_handler(dbg):
    writePipe("reached end marker\n")
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
    writePipe("Status: ME")
    dbg.remove_end_markers(dbg)
    dbg.preparation_lock.release()
    dbg.debugger_active = False
    ok()
    return DBG_CONTINUE

def test_handler(dbg):
    print("[!0x%x]" % dbg.exception_address)
    return DBG_CONTINUE

### debugger class

class debugger(pydbg):
    def __init__(self):
        self.binner = None
        self.preparation_lock = Lock()

        #configured during attaching

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
        self.react_marker_handlers = []
        self.av_handler = default_av_handler
    
        self.counters = {}

    def stop(self):
        self.debugger_active = False

    def start(self):
        self.debugger_active = True
        self.debug_event_loop()

    def list_tebs(self):
        for teb in self.tebs.keys():
            writePipe(hex(teb))

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
                dlog("Address blacklisted, won't handle")
                return True

        #modules
        (mod_name, mod_addr) = self.addr_to_module_name_and_baseaddr(e_addr)
#        dlog("Addr: " + hex(e_addr))
#        dlog("Module: " + my_name)
#        dlog("Module base: " + hex(my_addr))

        if(module_name != "[INVALID]"):
            for bl_mod in bl_modules:
                if(my_name == bl_mod):
                    dlog("Address blacklisted, won't handle")
                    return True
    
        #check rvas
        if(my_name != "[INVALID]"):
            for rva in bl_rvas:
                if(rva[0] == my_name):
                    my_off = e_addr - my_addr
                    if(my_off == rva[1]):
                        dlog("RVA blacklisted, won't handle")
                        return True
    
        #instructions
        e_instr = self.safe_disasm(e_addr)
        for bl_instr in bl_instructions:
            if(e_instr == bl_instr):
                dlog("Instruction blacklisted, won't handle")
                return True
    
        #checks finished, it's ok
        return False

    def attach_markers(self):
        self.preparation_lock.acquire()
        dlog("preparation locked")

        for ma_addr in self.markers:
            self.bp_set(ma_addr[0], handler = self.marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            counters[ma_addr[0]] = 0

        self.preparation_lock.release()
        dlog("preparation released")

    def attach_st_markers(self):
        self.preparation_lock.acquire()
        dlog("preparation locked")

        for ma_addr in self.st_markers:
            self.bp_set(ma_addr[0], handler = self.st_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            counters[ma_addr[0]] = 0

        self.preparation_lock.release()
        dlog("preparation released")

    def attach_end_markers(self):
        self.preparation_lock.acquire()
        dlog("preparation locked")

        for ma_addr in self.end_markers:
            self.bp_set(ma_addr[0], handler = self.end_marker_handler)
            self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
            counters[ma_addr[0]] = 0

        self.preparation_lock.release()
        dlog("preparation released")

    def attach_react_markers(self):
        self.preparation_lock.acquire()
        dlog("preparation locked")

        for ma_addr in self.react_markers:
            if(self.react_marker_handler.has_key(ma_addr[0])):
                self.bp_set(ma_addr[0], handler = self.react_marker_handler[ma_addr[0]])
                self.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
                counters[ma_addr[0]] = 0

        self.preparation_lock.release()
        dlog("preparation released")

    def detach_markers(self):
        self.preparation_lock.acquire()
        dlog("preparation locked")

        for ma_addr in self.markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        dlog("preparation released")

    def detach_st_markers(self):
        self.preparation_lock.acquire()
        dlog("preparation locked")

        for ma_addr in self.st_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        dlog("preparation released")

    def detach_end_markers(self):
        self.preparation_lock.acquire()
        dlog("preparation locked")

        for ma_addr in self.end_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        dlog("preparation released")

    def detach_react_markers(self):
        self.preparation_lock.acquire()
        dlog("preparation locked")

        for ma_addr in self.react_markers:
            self.bp_del(ma_addr[0])

        self.preparation_lock.release()
        dlog("preparation released")

### binner class

class binner(object):
    def __init__(self):
        self.test_lock = Lock()
        self.debuggers = {}                 # dictionary of debuggers for processes
        self.debuggers['init'] = debugger()  # initial debugger for enumerating processes
        crash_bin = utils.crash_binning.crash_binning()

        #blacklists
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
        dlog("Reading markers")

        if(defined("settings.ma_addrss")):
            dlog("Reading generic markers")
            self.markers = settings.ma_addrss
        else:
            self.markers = []

        if(defined("settings.ma_st_addrss")):
            dlog("Reading ST markers")
            self.st_markers = settings.ma_st_addrss
        else:
            self.st_markers = []

        if(defined("settings.ma_end_addrss")):
            dlog("Reading END markers")
            self.end_markers = settings.ma_end_addrss
        else:
            self.end_markers = []

        if(defined("settings.ma_react_addrss")):
            dlog("Reading REACT markers")
            self.react_markers = settings.ma_react_addrss
        else:
            self.react_markers = []

        #handlers
        dlog("Loading handlers")
        if(defined("settings.marker_handler")):
            dlog("Loading generic marker handler")
            self.marker_handler = settings.marker_handler
        else:
            self.marker_handler = phony_handler

        if(defined("settings.st_marker_handler")):
            dlog("Loading ST marker handler")
            self.st_marker_handler = settings.st_marker_handler
        else:
            self.st_marker_handler = phony_handler

        if(defined("settings.end_marker_handler")):
            dlog("Loading END marker handler")
            self.end_marker_handler = settings.end_marker_handler
        else:
            self.end_marker_handler = phony_handler

        if(defined("settings.react_marker_handlers")):
            dlog("Loading REACT marker handlers")
            self.react_marker_handler = settings.react_marker_handlers
        else:
            self.react_marker_handler = []

        #directories
        dlog("Reading directories")
        if(defined("settings.samples_dir")):
            self.samples_dir = settings.samples_dir
        else:
            self.samples_dir = samples_dir = "z:\\samples"
        self.crashed_dir = samples_dir + "\\crashed"
        self.hanged_dir = samples_dir + "\\hanged"
        self.clean_dir = samples_dir + "\\clean"
        self.log_file = samples_dir + "\\log-"

    def stop_debuggers(self):
        for (pid, dbg) in self.debuggers:
            dbg.stop()

    def start_debuggers(self):
        for (pid, dbg) in self.debuggers:
            dbg.start()

    def list_tebs(self):
        for (pid, dbg) in self.debuggers:
            writePipe("PID: " + pid)
            dbg.list_tebs()

    def terminate_processes(self):
        for (pid, dbg) in self.debuggers:
            dbg.terminate_process()


    def spawn(self, path):
        proc = subprocess.Popen(path)
        return proc

    def attach(self, pid):
        self.debuggers[str(pid)] = debugger()
        
        #copy blacklists
        dlog("Setting blacklists")
        self.debuggers[str(pid)].bl_modules = self.bl_modules
        self.debuggers[str(pid)].bl_instructions = self.bl_instructions
        self.debuggers[str(pid)].bl_addresses = self.bl_addresses
        self.debuggers[str(pid)].bl_rvas = self.bl_rvas

        #copy markers
        dlog("Setting markers")
        self.debuggers[str(pid)].markers = self.markers
        self.debuggers[str(pid)].st_markers = self.st_markers
        self.debuggers[str(pid)].end_markers = self.end_markers
        self.debuggers[str(pid)].react_markers = self.react_markers

        #copy handlers
        dlog("Setting handlers")
        self.debuggers[str(pid)].marker_handler = self.marker_handler
        self.debuggers[str(pid)].st_marker_handler = self.st_marker_handler
        self.debuggers[str(pid)].end_marker_handler = self.end_marker_handler
        self.debuggers[str(pid)].react_marker_handlers = self.react_marker_handlers

        #link self
        self.debuggers[str(pid)].binner = self
        
        return self.debuggers[str(pid)].attach(pid)
        
    def enumerate_processes(self):
        return self.debuggers['init'].enumerate_processes()

    def resolve_rvas(self, rvas):
        for ma_rva in rvas:
        writePipe("Resolving RVA at {0} + {1}\n".format(ma_rva[0], hex(ma_rva[1])))
        mod_addr = 0x0
        for mod in self.debuggers['init'].enumerate_modules():
            if(mod[0] == ma_rva[0]):
                mod_addr = mod[1]
        if(mod_addr == 0x0):
            raise Exception
        off = ma_rva[1]
        writePipe("Found at {0}\n".format(hex(mod_addr+off)))
        yield (mod_addr+off, ma_rva[2])

    def configure_markers(self):
        #addrss
        self.markers = settings.markers
        self.st_markers = settings.st_markers
        self.end_markers = settings.end_markers
        self.react_markers = settings.react_markers

        #rvas
        if(defined("settings.ma_rvas")):
            self.markers += self.resolve_rva(settings.ma_rvas)
        if(defined("settings.ma_st_rvas")):
            self.st_markers += self.resolve_rva(settings.ma_st_rvas)
        if(defined("settings.ma_end_rvas")):
            self.end_markers += self.resolve_rva(settings.ma_end_rvas)
        if(defined("settings.ma_react_rvas")):
            self.react_markers += self.resolve_rva(settings.ma_react_rvas)
    
        #handlers
        if(defined("settings.marker_handler")):
            self.marker_handler = settings.marker_handler
        if(defined("settings.marker_st_handler")):
            self.marker_st_handler = settings.marker_st_handler
        if(defined("settings.marker_end_handler")):
            self.marker_handler = settings.marker_end_handler
        if(defined("settings.react_marker_handlers")):
            self.react_marker_handlers = settings.react_marker_handlers

    def attach_all_markers(self):
        attach_markers()
        attach_st_markers()
        attach_end_markers()
        attach_react_markers()

    def attach_markers(self):
        for dbg in self.debuggers:
            dbg.attach_markers()

    def attach_st_markers(self):
        for dbg in self.debuggers:
            dbg.attach_st_markers()

    def attach_end_markers(self):
        for dbg in self.debuggers:
            dbg.attach_end_markers()

    def attach_react_markers(self):
        for dbg in self.debuggers:
            dbg.attach_react_markers()

    def detach_all_markers(self):
        detach_markers()
        detach_st_markers()
        detach_end_markers()
        detach_react_markers()

    def detach_markers(self):
        for dbg in self.debuggers:
            dbg.attach_markers()

    def detach_st_markers(self):
        for dbg in self.debuggers:
            dbg.attach_st_markers()

    def detach_end_markers(self):
        for dbg in self.debuggers:
            dbg.attach_end_markers()

    def detach_react_markers(self):
        for dbg in self.debuggers:
            dbg.attach_react_markers()



