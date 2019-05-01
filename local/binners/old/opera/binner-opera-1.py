#!c:\python\python.exe

import sys

sys.path.append("z:\\server\\paimei")

import struct
import utils
import subprocess
import time
import os
import time
import signal
from threading import Thread
import _thread
from threading import Lock
import ctypes
import win32pipe, win32file, win32gui
import random

from pydbg import *
from pydbg.defines import *

from debuggee_procedure_call import dpc

START_SLEEP = 8
WAIT_SLEEP = 3
HC_ADDR = 0x770627e4
HC_CODE = 0xc0000374

samples_dir = "z:\\samples"
crashed_dir = samples_dir + "\\crashed"
hanged_dir = samples_dir + "\\hanged"
clean_dir = samples_dir + "\\clean"
log_file = samples_dir + "\\log-"
log_file = "log-"

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

def handle_crash(dbg, reason=""):
    global status
    global crash_bin
    global crash_reason

#    l.acquire()
    crash_bin = cb
    crash_bin.record_crash(dbg)

    #create dossier
    if(reason == "hc"):
        crash_reason = "hc"
        writePipe("Probable crash reason: hc (crash info needs reinterpretation)\n")
        writePipe("")
    if(reason == "uaf"):
        crash_reason = "uaf"
        writePipe("Probable crash reason: uaf\n")
        writePipe("")
    else:
        crash_reason = "unk"

    writePipe(crash_bin.crash_synopsis())
    status = "CR"

    #save sample
    #separate dirs for uafs & hcs

    #release
#    ok()
#    l.release()
#    dbg.terminate_process()    
    return DBG_CONTINUE


def handle_av(dbg):
    global status

    crash_bin = cb
    crash_bin.record_crash(dbg)
    e_addr = crash_bin.last_crash.exception_address

    if(check_blacklists(dbg, e_addr)):
        return DBG_EXCEPTION_NOT_HANDLED

    if((disasm(dbg, e_addr) == "[INVALID]") or (get_module(dbg, e_addr) == "[INVALID]")):
        handle_crash(dbg, "uaf")
        return DBG_CONTINUE

    handle_crash(dbg)
    return DBG_CONTINUE

def handle_bp(dbg):
    b_addr = dbg.get_register("EIP")

    #heap addr?
    if(b_addr == HC_ADDR):
        esp = dbg.get_register("ESP")
        dw = dbg.read_process_memory(esp + 0x4, 4)
        code = struct.unpack("<I", dw)[0]
        if(code == HC_CODE):
            handle_crash(dbg, "hc")
            return DBG_CONTINUE

    return DBG_EXCEPTION_NOT_HANDLED
    
def file_run(filee, dbg):
    global status

    status = "hanged"
    os.system("start " + samples_dir + "\\" + filee)
    logf.write("start " + samples_dir + "\\" + filee + "\n")
    logf.flush()
    time.sleep(my_length)
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

attached=False
imagename = "opera.exe"

def attach(dbg, imagename):
    for (pid, name) in dbg.enumerate_processes():
        if imagename in name:
            print((imagename + " in " + name))
            try:
                print("[*] Attaching to %s (%d)" % (name, pid))
                log_write("[*] Attaching to " + name + " " + str(pid) + "\n")
                dbg.attach(pid)
            except Exception as e:
                print("[!] Problem attaching to %s" % name)
                log_write("[*] Problem attaching to " + name)
                log_write(e)
    #                windows_kill(pid)
                continue
            break

def setup_dbg(dbg):
    dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, handle_av)
#    dbg.bp_set(0x770627e4, handler=handle_bp)

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

def startLog():
    global logStarted
    global log_file
    global logf

    log_file += time.strftime("%Y%m%d-%H%M%S")
    log_file += ".txt" 
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

def bad_handler(dbg):
    global status

#    l.acquire()
#    if(status == ""):
#        status = "BH"
    status = "BH"

    return DBG_CONTINUE

def thread1_routine():
    global Thread1Active

    print(Thread1Active)
    while (Thread1Active == True):
        print("From thread 1")
        time.sleep(1)
    print("Exiting thread 1")


def goThread_routine():
    global dbg
    global goThreadActive

    print("releasing")

    try:
        print("1")
        dbg.debug_event_loop()
        print("2")
        while (goThreadActive == True):
            print("running")
            time.sleep(1)
    except Exception as e:
        print(e)

def killing_routine():
    global dbg

    print((hex(dbg.h_process)))
#    print(hex(dbg.h_thread))
    dbg.open_process(dbg.pid)
    print((hex(dbg.h_process)))
    time.sleep(3)
#    dbg.terminate_process()
    dbg.debug_set_process_kill_on_exit(True)
    dbg.detach()


def timer_routine():
    time.sleep(WAIT_SLEEP)
    return

def breaking_routine():
    global dbg

    time.sleep(int(WAIT_SLEEP/2))

    print("breaking")

    addr = int(random.random() * 0xffffffff)
    threads =  dbg.enumerate_threads()
    thread_num = int(random.random() * len(threads))
    print((str(thread_num) + ": " + str(threads[thread_num])))
    
    thread_handle = dbg.open_thread(threads[thread_num])
    thread_context = dbg.get_thread_context(thread_handle)
    thread_context.Eip = addr
    dbg.set_thread_context(thread_context, thread_handle)


def watchThread_routine():
    global dbg 
    global status

    breaking = False

    if(breaking == True):
        breaker = Thread(target = breaking_routine)
        breaker.start()

    timer = Thread(target = timer_routine)
    timer.start()

    while(timer.is_alive() and status == ""):
        pass

    if(status == ""):
        status = "TO"

    dbg.debugger_active = False
    return

def execute(cmds):
    global dbg

    cmd = cmds[0]
    args = " ".join(cmds[1:])

    try:
        if(cmd == "waitTest"):
            global status
            status = ""

            watchThread = Thread(target = watchThread_routine)
            watchThread.start()
            dbg.debugger_active = True
            dbg.debug_event_loop()
            writePipe("Status: " + status)
            ok()

        if(cmd == "listClasses"):
            win32gui.EnumWindows(enum_handler, None)
            ok()

        if(cmd == "listTebs"):
            for teb in list(dbg.tebs.keys()):
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


        if(cmd == "installHandlers"):
            setup_dbg(dbg)
            writePipe("AV handlers in place")
            ok()

        if(cmd == "installGood"):
            writePipe("Installing good at " + cmds[1])
            ok()

        if(cmd == "installBad"):
            writePipe("Installing bad at " + cmds[1])
            dbg.bp_set(int(cmds[1], 16), handler = bad_handler)
            ok()

        if(cmd == "binTest"):
            writePipe("Communication with binner is working")
            ok()
        
    except Exception as e:
        print(e)
            # loop so i can read it :)
        while True:
            pass
        writePipe("Error " + e)
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
#    time.sleep(START_SLEEP)


    ph = getPipe(my_pipe)
    writePipe("OK")

    print("test")

    try:
        while True:
            cmd = readPipe()
            cmds = str.split(cmd[1])
            execute(cmds)
    except Exception as e:
        print(e)

    cb_file = samples_dir + time.strftime("%Y%m%d-%H%M%S") + ".crash"
    status = "hang"


    count = 0

    for filee in os.listdir(samples_dir):
        if(filee[-4:] != ".ogv"):
            continue
        #spawn app & wait to load
        check_app()
        count += 1

    dbg.detach()
    dbg = None

    print(("Got " + str(count) + " crashes, exporting to: " + cb_file))
    logf.write("Got " + str(count) + " crashes, exporting to: " + cb_file)

    cb.export_file(cb_file)
    logf.close()

#if __name__ == "main":
main()
