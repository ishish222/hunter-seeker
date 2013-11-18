#!c:\python\python.exe

import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

import struct
import utils
import subprocess
import time
import os
import time
import signal
from threading import Thread
import thread
from threading import Lock
import ctypes
import win32pipe, win32file, win32gui
import random
import settings
from pydbg import *
from pydbg.defines import *
from debuggee_procedure_call import dpc

test_lock = Lock()
preparation_lock = Lock()
attached=False
imagename = "opera.exe"

samples_dir = "z:\\samples"
crashed_dir = samples_dir + "\\crashed"
hanged_dir = samples_dir + "\\hanged"
clean_dir = samples_dir + "\\clean"
log_file = samples_dir + "\\log-"
#log_file = "log-"

module_blacklist = []
module_blacklist.append("ntdll.dll")
module_blacklist.append("kernel32.dll")
module_blacklist.append("user32.dll")
module_blacklist.append("xpsp2res.dll")
module_blacklist.append("glu32.dll")
module_blacklist.append("advapi32.dll")
module_blacklist.append("comdlg.dll")
module_blacklist.append("crypt32.dll")
module_blacklist.append("dnsapi.dll")
module_blacklist.append("gdi.dll")
module_blacklist.append("lz32.dll")
module_blacklist.append("msctf.dll")
module_blacklist.append("msvcrt.dll")
module_blacklist.append("ole32.dll")
module_blacklist.append("urlmon.dll")
module_blacklist.append("wininet.dll")
module_blacklist.append("ws2_32.dll")
module_blacklist.append("MSVCP90.dll")
module_blacklist.append("MSVCR100.dll")
module_blacklist.append("mfc90u.dll")
module_blacklist.append("shlwapi.dll")
module_blacklist.append("uxtheme.dll")
module_blacklist.append("shimeng.dll")
module_blacklist.append("AcGeneral.dll")
module_blacklist.append("userenv.dll")
module_blacklist.append("winmm.dll")
module_blacklist.append("oleaut32.dll")
module_blacklist.append("comclt32.dll")
module_blacklist.append("msacm32.dll")
module_blacklist.append("version.dll")
module_blacklist.append("msvcrt.dll")
module_blacklist.append("rpcrt.dll")
module_blacklist.append("rpcrt4.dll")
module_blacklist.append("gdi32.dll")
module_blacklist.append("secur32.dll")
module_blacklist.append("user32.dll")
module_blacklist.append("shell32.dll")
module_blacklist.append("mswsock.dll")
module_blacklist.append("ws2_help.dll")


bl_instructions = []
#too general, will miss uafs
#bl_instructions.append("mov eax,[ecx+0x4]")
#bl_instructions.append("mov eax,[ecx]")
bl_modules = []
bl_addresses = []
bl_rvas = []
#bl_rvas.append(("Ac1st19.dll", 0x3bd1))
#bl_rvas.append(("AcXtrnal.DLL", 0x8cfb))

app_path={
"acad2010"  : "C:\\Program Files\\AutoCAD 2010\\acad.exe",
"acad2014"  : "C:\\Program Files\\Autodesk\\AutoCAD 2014\\acad.exe",
"opera"     : "C:\\Program Files\\Opera\\16.0.1196.80\\opera.exe"
}


my_path = app_path["opera"]
my_length = 15
my_pipe = "\\\\.\\pipe\\control"
my_pipe2 = "control"
pipe_buff_size = 4096

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

def ok():
    time.sleep(0.5)
    writePipe("-=OK=-")
    win32file.FlushFileBuffers(ph)

def getPipe(name):
    ph = win32file.CreateFile(name, win32file.GENERIC_READ | win32file.GENERIC_WRITE | win32pipe.PIPE_TYPE_MESSAGE, 0, None, win32file.OPEN_EXISTING, 0, None)
    return ph

def readPipe():
    global ph
    return win32file.ReadFile(ph, pipe_buff_size)

def writePipe(data):
    global ph
    win32file.WriteFile(ph, data)

def disasm(dbg, e_addr):
    try:
        e_instr = dbg.disasm(e_addr)
    except:
        e_instr = "[INVALID]"
    return e_instr

def get_module(dbg, e_addr):
    my_module = dbg.addr_to_module(e_addr)
    if(my_module != None):
        my_name = my_module.szModule
    else:
        my_name = "[INVALID]"
    return my_name 

def check_blacklists(dbg, e_addr):

    #addresses
    for bl_addr in bl_addresses:
        if(e_addr == bl_addr):
            print("Address blacklisted, won't handle")
            return True

    #modules
    my_module = dbg.addr_to_module(e_addr)
#    print(dir(my_module))
    if(my_module != None):
        my_name = my_module.szModule
        my_addr = my_module.modBaseAddr
    else:
        my_name = "[INVALID]"
        my_addr = 0x0

#    print("Addr: " + hex(e_addr))
#    print("Module: " + my_name)
#    print("Module base: " + hex(my_addr))

    if(my_name != "[INVALID]"):
        for bl_mod in bl_modules:
            if(my_name == bl_mod):
                print("Address blacklisted, won't handle")
                return True

    #check rvas
    if(my_name != "[INVALID]"):
        for rva in bl_rvas:
            if(rva[0] == my_name):
                my_off = e_addr - my_addr
                if(my_off == rva[1]):
                    print("RVA blacklisted, won't handle")
                    return True

    #instructions
    e_instr = disasm(dbg, e_addr)
    for bl_instr in bl_instructions:
        if(e_instr == bl_instr):
            print("Instruction blacklisted, won't handle")
            return True

    #checks finished, it's ok
    return False

def handle_crash(dbg):
    global crash_bin
    global crash_reason

    print("4")
    e_addr = crash_bin.last_crash.exception_address

    #create dossier
#    if(reason == "hc"):
#        crash_reason = "hc"
#        writePipe("Probable crash reason: hc (crash info needs reinterpretation)\n")
#        writePipe("")
    if((disasm(dbg, e_addr) == "[INVALID]") or (get_module(dbg, e_addr) == "[INVALID]")):
        crash_reason = "uaf"
        writePipe("Probable crash reason: uaf\n")
        writePipe("")
    else:
        crash_reason = "unk"

    print("5")
    writePipe(crash_bin.crash_synopsis())

    return DBG_CONTINUE


def handle_av(dbg):
    print("avThread")
    global status
    global crash_bin
    
    test_lock.acquire()
    print("test locked by AV")
    crash_bin = cb
    crash_bin.record_crash(dbg)
    e_addr = crash_bin.last_crash.exception_address
    if(check_blacklists(dbg, e_addr) == False):
        status = "CR"
    test_lock.release()
    return DBG_CONTINUE

def file_run(filee, dbg):
    global status

    status = "hanged"
    os.system("start " + samples_dir + "\\" + filee)
    logf.write("start " + samples_dir + "\\" + filee + "\n")
    logf.flush()
    time.sleep(my_length * settings.slowdown)
    l.acquire()
    if(status == "hanged"):
        if(testfile(samples_dir + "\\" + filee)):
            print("status: hanged")
            logf.write("status: hanged\n")
            try:
                dbg.terminate_process()
            except Exception:
                pass
#            os.rename(samples_dir + "\\" + filee, hanged_dir + "\\" + filee)
#            if(testfile(samples_dir + "\\" + filee + ".bak")):
#                os.remove(samples_dir + "\\" + filee + ".bak")
    l.release()
    
def check_var(name):
    if(name in globals()):
        return globals()[name]
    if(name in vars()):
        return vars()[name]
    return None

def debug_loop(dbg):
    dbg.debug_event_loop()

def enum_handler(hwnd, lParam):
#    if win32gui.IsWindowVisible(hwnd):
#    writePipe(win32gui.GetWindowText(hwnd))
#    writePipe("\n")
#    if(win32gui.GetWindowText(hwnd).find("Opera") != -1):
#        writePipe(win32gui.GetWindowText(hwnd))
#        win32gui.EnumChildWindows(0, enum_child_handler, lParam)
    writePipe(hex(hwnd) + " - " + win32gui.GetClassName(hwnd))
    writePipe("\n")
    win32gui.EnumChildWindows(0, enum_child_handler, lParam)

def enum_child_handler(hwnd, lParam):
#    if(win32gui.GetClassName(hwnd) == "ConsoleWindowClass"):
#        writePipe(hex(hwnd) + " - " + win32gui.GetClassName(hwnd))
#        writePipe("\n")
    writePipe("\t" + hex(hwnd) + " - " + win32gui.GetClassName(hwnd))
    writePipe("\n")

def windows_kill(pid):
    kernel32 = ctypes.windll.kernel32
    handle = kernel32.OpenProcess(1, 0, pid)
    return (0 != kernel32.TerminateProcess(handle, 0))

def attach(dbg, imagename):
    for (pid, name) in dbg.enumerate_processes():
        if imagename in name:
            print(imagename + " in " + name)
            try:
                print "[*] Attaching to %s (%d)" % (name, pid)
                log_write("[*] Attaching to " + name + " " + str(pid) + "\n")
                dbg.attach(pid)
            except Exception, e:
                print "[!] Problem attaching to %s" % name
                log_write("[*] Problem attaching to " + name)
                log_write(e)
    #                windows_kill(pid)
                continue
            break

def setup_dbg(dbg):
    dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, handle_av)

def check_app():
    global dbg
    global imagename

    if(attached == True):
        return True

    dbg = pydbg()      # globally accessible pydbg instance.
    proc = subprocess.Popen(my_path)
    time.sleep(3)
    attach(dbg, imagename)
    attached = True

def startLog(log_file):
    global logStarted
    global logf

    log_file += time.strftime("%Y%m%d-%H%M%S")
    log_file += ".txt" 
    print("log: " + log_file)
    logf = open(log_file, "w")
    logf.write("test\n")
    logStarted = True

def stopLog():
    global logStarted
    global logf

    logf.close()
    logStarted = False

def log_write(data):
    global logStarted
    global logf

    if(logStarted == False):
        return
    logf.write(data)

def spawn():
    proc = subprocess.Popen(my_path)

def good_handler(dbg):
    pass

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
    if(len(my_op.split("*")) >1):
        (a,b) = my_op.split("*")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a*b
        return my_op & 0xffffffff
    return int(my_op, 16) & 0xffffffff

global counters
counters = {}

def bp_handler(dbg):
    ea = dbg.exception_address
    dbg.bp_del(dbg.exception_address)
    thread = dbg.dbg.dwThreadId
    thread_handle  = dbg.open_thread(thread)
    log_write("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
    dbg.single_step(True, thread_handle)
    dbg.close_handle(thread_handle)
    return DBG_CONTINUE

def instr_handler(dbg):
    ea = dbg.exception_address
    thread = dbg.dbg.dwThreadId
    log_write("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
#    print("1")
#    log("%x" % ea) #+thread
#    print("%s" % ea)
    dbg.disasm(ea)
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

        for mod in module_blacklist:
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
 
def st_handler(dbg):
    preparation_lock.acquire()
    writePipe("reached start marker\n")
    ea = dbg.exception_address
    thread = dbg.dbg.dwThreadId
    print("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
    log_write("--- reached start marker ---")
    global status
    global counters
#    writePipe("Pass count: %x\n" % dbg.breakpoints[dbg.exception_address].pass_count)
#    writePipe("Hit count: %x\n" % counters[dbg.exception_address])
 
    if(dbg.breakpoints[dbg.exception_address].pass_count > counters[dbg.exception_address]):
        counters[dbg.exception_address] += 1
#        writePipe("New hit count: %x\n" % counters[dbg.exception_address])
    else:
        print("installing instruction handler2")

        dbg.set_callback(EXCEPTION_BREAKPOINT, bp_handler)
        dbg.set_callback(EXCEPTION_SINGLE_STEP, instr_handler)

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
    attach_end_markers(dbg)
    remove_start_markers(dbg)
    preparation_lock.release()
    print("here1")
    return DBG_CONTINUE

def test_handler(dbg):
    print("[!0x%x]" % dbg.exception_address)
    return DBG_CONTINUE

def end_handler(dbg):
    writePipe("reached end marker\n")
    print("reached end marker")
    preparation_lock.acquire()
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
    remove_end_markers(dbg)
    preparation_lock.release()
    dbg.debugger_active = False
    ok()
    return DBG_CONTINUE

def check_modules(dbg):
    for mod in dbg.enumerate_modules():
        print("[%s] : 0x%x" % (mod[0], mod[1]))
    

def bad_handler(dbg):
    print("badThread")
    global status
    global counters
#    writePipe("Pass count: %x\n" % dbg.breakpoints[dbg.exception_address].pass_count)
#    writePipe("Hit count: %x\n" % counters[dbg.exception_address])
 
    if(dbg.breakpoints[dbg.exception_address].pass_count > counters[dbg.exception_address]):
        counters[dbg.exception_address] += 1
        writePipe("New hit count: %x\n" % counters[dbg.exception_address])
    else:
        if(status != "CR"):
            test_lock.acquire()
            print("test locked by MA")
            if(status == ""):
                status = "MA"
            test_lock.release()
    return DBG_CONTINUE

def timer_routine(to):
    time.sleep(to * settings.slowdown)
    return

def breaking_routine(to):
    print("breakingThread")
    global dbg

    time.sleep(int(to * settings.slowdown /2))

    print("breaking")
    try:
        addr = int(random.random() * 0xffffffff)
        threads =  dbg.enumerate_threads()
        thread_num = int(random.random() * len(threads))
        print(str(thread_num) + ": " + str(threads[thread_num]))
    
        thread_handle = dbg.open_thread(threads[thread_num])
        thread_context = dbg.get_thread_context(thread_handle)
        thread_context.Eip = addr
        dbg.set_thread_context(thread_context, thread_handle)
    except Exception:
        pass


def watchThread_routine(to):
    print("watchThread")
    global dbg 
    global status

    if(settings.breaking == True):
        breaker = Thread(target = breaking_routine, args=(to,))
        breaker.start()

    timer = Thread(target = timer_routine, args=(to,))
    timer.start()

    while(timer.is_alive() and status == ""):
        pass

    if(status != "CR" and status != "MA"):
        test_lock.acquire()
        print("test locked by TO")
        status = "TO"
        test_lock.release()
    dbg.debugger_active = False
    return

def resolve_rvas(dbg, rvas):
    for ma_rva in rvas:
        writePipe("Resolving RVA at {0} + {1}\n".format(ma_rva[0], hex(ma_rva[1])))
        mod_addr = 0x0
        for mod in dbg.enumerate_modules():
            if(mod[0] == ma_rva[0]):
                mod_addr = mod[1]
        if(mod_addr == 0x0):
            raise Exception
        off = ma_rva[1]
        writePipe("Found at {0}\n".format(hex(mod_addr+off)))
        yield (mod_addr+off, ma_rva[2])

def attach_start_markers(dbg):
    global counters
    print("attatch start 1")

    for ma_addr in settings.ma_st_addrs:
        writePipe("Installing start at {0}\n".format(hex(ma_addr[0])))
        dbg.bp_set(ma_addr[0], handler = st_handler)
        dbg.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
        counters[ma_addr[0]] = 0
    print("attach start 2")

def attach_end_markers(dbg):
    global counters
    print("attach end 1")

    for ma_addr in settings.ma_end_addrs:
        writePipe("Installing end at {0}\n".format(hex(ma_addr[0])))
        dbg.bp_set(ma_addr[0], handler = end_handler)
        dbg.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
        counters[ma_addr[0]] = 0
    print("attach end 2")

def attach_markers(dbg):
    global counters

    preparation_lock.acquire()
    print("preparation locked")
    for ma_addr in settings.ma_addrs:
#        writePipe("Installing bad at {0}\n".format(hex(ma_addr)))
        dbg.bp_set(ma_addr[0], handler = bad_handler)
        dbg.breakpoints[ma_addr[0]].pass_count = ma_addr[1]
        counters[ma_addr[0]] = 0
    preparation_lock.release()
    print("preparation released")

def remove_start_markers(dbg):
    print("remove start 1")
    for ma_addr in settings.ma_st_addrs:
#        writePipe("Removing bad at {0}\n".format(hex(ma_addr)))
        dbg.bp_del(ma_addr[0])
    print("remove start 2")

def remove_end_markers(dbg):
    print("remove end 1")
    for ma_addr in settings.ma_end_addrs:
#        writePipe("Removing bad at {0}\n".format(hex(ma_addr)))
        dbg.bp_del(ma_addr[0])
    print("remove end 2")

def remove_markers(dbg):
    preparation_lock.acquire()
    print("preparation locked")
    for ma_addr in settings.ma_addrs:
#        writePipe("Removing bad at {0}\n".format(hex(ma_addr)))
        dbg.bp_del(ma_addr[0])
    preparation_lock.release()
    print("preparation released")

def check_markers(dbg):
    for ma_addr in settings.ma_addrs:
        if(dbg.bp_is_ours(ma_addr)):
            db = dbg.read_process_memory(ma_addr, 1)
            byte = struct.unpack("<B", db)[0]
            writePipe("Marker working at " + str(hex(ma_addr)) + "\n")
            writePipe("Byte: %x\n" % byte)
        else:
            writePipe("Marker not working at " + str(hex(ma_addr)) + "\n")

def execute(cmds):
    global dbg

    cmd = cmds[0]
    args = " ".join(cmds[1:])

    try:
        if(cmd == "settle"):
            global status
            status = ""
    
            watchThread = Thread(target = watchThread_routine, args=(float(cmds[1]),))
            watchThread.start()
            dbg.debugger_active = True
            dbg.debug_event_loop()
            ok()
    
        if(cmd == "waitTest"):
            global status
    
            status = ""
            attach_markers(dbg)
            watchThread = Thread(target = watchThread_routine, args=(settings.wait_sleep * settings.slowdown, ))
            watchThread.start()
            dbg.debugger_active = True
            dbg.debug_event_loop()
            writePipe("Status: " + status)
            if(status == "CR"):
                handle_crash(dbg)
            remove_markers(dbg)
            ok()
    
        if(cmd == "observe"):
            print("In observe")
#            writePipe("In observe\n")
#            check_modules(dbg)
#            dbg.bp_set(0x1386D20, handler=test_handler)
#            dbg.bp_set(0x138FDB0, handler=test_handler)
            attach_start_markers(dbg)
#            attach_end_markers(dbg)
    #        watchThread = Thread(target = watchThread_routine, args=(settings.wait_sleep * settings.slowdown, ))
    #        watchThread.start()
            dbg.debugger_active = True
            dbg.debug_event_loop()
#            remove_start_markers(dbg)
#            remove_end_markers(dbg)
            #end marker handler will ok()
#            ok()
    
        if(cmd == "logStart"):
            global log_file
            if(len(cmds) > 1):
                startLog(cmds[1])
            else:
                startLog(log_file)
            ok()
    
        if(cmd == "logStop"):
            stopLog()
            ok()
    
        if(cmd == "listClasses"):
            win32gui.EnumWindows(enum_handler, None)
            ok()
    
        if(cmd == "listTebs"):
            for teb in dbg.tebs.keys():
                print(teb)
                writePipe(hex(teb))
            ok()
    
    
        if(cmd == "cbEip"):
            global crash_bin
    
            writePipe("0x%x" % crash_bin.last_crash.exception_address)
            ok()
    
        if(cmd == "cbCrashSynopsis"):
            global crash_bin
    
            writePipe(crash_bin.crash_synopsis())
            ok()
    
        if(cmd == "cbReason"):
            global crash_reason
    
            writePipe(crash_reason)
            ok()
    
        if(cmd == "cbStackUnwind"):
            global crash_bin
    
            for call_frame in crash_bin.last_crash.stack_unwind:
                writePipe(call_frame + "\n")
            ok()
    
        if(cmd == "snapshot"):
            dbg.suspend_all_threads()
            dbg.process_snapshot()
            dbg.resume_all_threads()
            ok()
    
        if(cmd == "restore"):
            dbg.suspend_all_threads()
            dbg.process_restore()
            dbg.resume_all_threads()
            ok()
    
        if(cmd == "stop"):
            global goThreadActive 
            goThreadActive = False
            ok()
    
        if(cmd == "attachBinner"):
            attach(dbg, args)
            writePipe("Attached to " + str(dbg.pid))
            ok()
    
        if(cmd == "killHost"):
            dbg.terminate_process()
            dbg.debug_set_process_kill_on_exit(True)
            dbg.detach()
            ok()
            exit()
    
        if(cmd == "installHandlers"):
            setup_dbg(dbg)
            writePipe("AV handlers in place")
            ok()
    
        if(cmd == "installGood"):
            writePipe("Installing good at " + cmds[1])
            ok()
    
        if(cmd == "installMarkerAddrs"):
            for ma_addr in settings.ma_addrs:
                writePipe("Installing bad at " + cmds[1])
                dbg.bp_set(ma_addr, handler = bad_handler)
            ok()

        if(cmd == "installBad"):
            writePipe("Installing bad at " + cmds[1])
            dbg.bp_set(int(cmds[1], 16), handler = bad_handler)
            ok()
    
        if(cmd == "checkBad"):
            if(dbg.bp_is_ours(int(cmds[1], 16))):
                db = dbg.read_process_memory(int(cmds[1], 16), 1)
                byte = struct.unpack("<B", db)[0]
                writePipe("Marker working at " + cmds[1] + "\n")
                writePipe("Byte: %x\n" % byte)
            else:
                writePipe("Marker not working at " + cmds[1] + "\n")
            ok()
    
        if(cmd == "checkBadOff"):
            mod_addr = 0x0
            for mod in dbg.enumerate_modules():
                if(mod[0] == cmds[1]):
                    mod_addr = mod[1]
            if(mod_addr == 0x0):
                raise Exception
            off = int(cmds[2], 16)
    
            if(dbg.bp_is_ours(mod_addr+off) == True):
                db = dbg.read_process_memory(mod_addr+off, 1)
                byte = struct.unpack("<B", db)[0]
                writePipe("Marker working at " + cmds[1] + "\n")
                writePipe("Byte: %x\n" % byte)
                writePipe("Marker working at " + cmds[1] + " +" + cmds[2]+ "\n")
            else:
                writePipe("Marker not working at " + cmds[1] + " +" + cmds[2]+ "\n")
            ok()
    
        if(cmd == "setupSlowdown"):
            writePipe("Setting slowdown to: {0}\n".format(cmds[1]))
            settings.slowdown = float(cmds[1])
            ok()
    
        if(cmd == "setupMarkers"):
            writePipe("Setting up markers\n")
            settings.ma_addrs += resolve_rvas(dbg, settings.ma_rvas)
            settings.ma_st_addrs += resolve_rvas(dbg, settings.ma_st_rvas)
            settings.ma_end_addrs += resolve_rvas(dbg, settings.ma_end_rvas)
            writePipe("Marker list: {0}\n".format(settings.ma_addrs))
            writePipe("ST marker list: {0}\n".format(settings.ma_st_addrs))
            writePipe("END marker list: {0}\n".format(settings.ma_end_addrs))
            ok()
    
        if(cmd == "setupStartMarkers"):
            writePipe("Setting up start markers\n")
    #        settings.ma_addrs += resolve_rvas(dbg, settings.ma_rvas)
    #        writePipe("Marker list: {0}\n".format(settings.ma_addrs))
            ok()

    
        if(cmd == "binTest"):
            writePipe("Communication with binner is working")
            ok()
        
        if(cmd == "release"):
            writePipe("Releasing")
            ok()
            dbg.debugger_active = True
            dbg.debug_event_loop()

    except Exception, e:
        print(e)
        while True:
            pass
        ok()

def main():
    global dbg
    global status
    global ph 
    global my_pipe
    global logStarted

    logStarted = False

    count = 0
    dbg = pydbg()

    ph = getPipe(my_pipe)
    writePipe("OK")

    while True:
        cmd = readPipe()
        cmds = str.split(cmd[1])
        execute(cmds)

#    try:
#        while True:
#            cmd = readPipe()
#            cmds = str.split(cmd[1])
#            execute(cmds)
#    except Exception, e:
#        print(e)

#    cb_file = samples_dir + time.strftime("%Y%m%d-%H%M%S") + ".crash"
#    status = "hang"
#    count = 0

#    for filee in os.listdir(samples_dir):
#        if(filee[-4:] != ".ogv"):
#            continue
#        #spawn app & wait to load
#        check_app()
#        count += 1

#    dbg.detach()
#    dbg = None

#    print("Got " + str(count) + " crashes, exporting to: " + cb_file)
#    logf.write("Got " + str(count) + " crashes, exporting to: " + cb_file)

#    cb.export_file(cb_file)
#    logf.close()

#if __name__ == "main":
main()
