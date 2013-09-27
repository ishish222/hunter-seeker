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
from threading import Lock
import ctypes

from pydbg import *
from pydbg.defines import *

HC_ADDR = 0x770627e4
HC_CODE = 0xc0000374

samples_dir = "X:\\"
crashed_dir = "X:\\crashed"
hanged_dir = "X:\\hanged"
clean_dir = "X:\\clean"
#log_file = "X:\\log-"
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
'acad2010' : 'C:\\Program Files\\AutoCAD 2010\\acad.exe',
'acad2014' : 'C:\\Program Files\\Autodesk\\AutoCAD 2014\\acad.exe'
}


my_path = app_path["acad2014"]
my_length = 15

def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

def testfile(x):
    return os.path.exists(x)
    

#import pdb
#pdb.set_trace()

#testdir(crashed_dir)
#testdir(hanged_dir)
#testdir(clean_dir)

cb = utils.crash_binning.crash_binning()

l = Lock()

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
    #lock

    l.acquire()
    crash_bin = cb
    crash_bin.record_crash(dbg)

    #create dossier
    if(reason == "hc"):
        print("Probable crash reason: hc (crash info needs reinterpretation)")
        print("")
    if(reason == "uaf"):
        print("Probable crash reason: uaf")
        print("")

    print(crash_bin.crash_synopsis())

    #save sample
    #separate dirs for uafs & hcs

    #release
    l.release()
    dbg.terminate_process()    
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

#    crash_bin = utils.crash_binning.crash_binning()
    crash_bin = cb
    crash_bin.record_crash(dbg)
    
    if(testfile(samples_dir + "\\" + filee)):
        status = "crashed"
        dbg.terminate_process()
        binn = hex(crash_bin.last_crash.exception_address)
        logf.write("Bin: " + binn)
        testdir(crashed_dir + "\\" + binn)
        if(not testfile(crashed_dir + "\\" + binn + "\\" + binn + ".txt")):
            so = open(crashed_dir + "\\" + binn + "\\" + binn + ".txt", "w")
            so.write(crash_bin.crash_synopsis())
            so.close()
        os.rename(samples_dir + "\\" + filee, crashed_dir + "\\" + binn + "\\" + filee)
        logf.write("status: crashed")
        if(testfile(samples_dir + "\\" + filee + ".bak")):
            os.remove(samples_dir + "\\" + filee + ".bak")
    l.release()
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
            if(testfile(samples_dir + "\\" + filee + ".bak")):
                os.remove(samples_dir + "\\" + filee + ".bak")
    l.release()
    

def debug_loop(dbg):
    dbg.debug_event_loop()

imagename = "opera.exe"
count = 0

cb_file = samples_dir + time.strftime("%Y%m%d-%H%M%S") + ".crash"
log_file += time.strftime("%Y%m%d-%H%M%S")
log_file += ".txt" 
status = "hang"

#logf = open(log_file, "w")
#logf.write("test\n")

def windows_kill(pid):
    kernel32 = ctypes.windll.kernel32
    handle = kernel32.OpenProcess(1, 0, pid)
    return (0 != kernel32.TerminateProcess(handle, 0))

#for filee in os.listdir(samples_dir):
#    if(filee[-4:] != ".dwg"):
#        continue
    #spawn app & wait to load
filee = sys.argv[1]
proc = subprocess.Popen(my_path)
time.sleep(3)


#logf.write("test2\n")
#install hook
#    subprocess.Popen("W:\\p023-standalone.exe " + str(proc.pid))

    #attach
dbg = pydbg()      # globally accessible pydbg instance.
dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, handle_av)
#dbg.bp_set("0x770627e4")
#dbg.set_callback(EXCEPTION_BREAKPOINT, handle_bp)
#dbg.bp_set(0x770627e4, handler=handle_bp)
#dbg.bp_set(0x76a0f1a6, handler=handle_bp)
#dbg.bp_set(0x70e00c31)
#logf.write("test3\n")
for (pid, name) in dbg.enumerate_processes():
    if imagename in name:
        print(imagename + " in " + name)
        try:
            print "[*] Attaching to %s (%d)" % (name, pid)
#            logf.write("[*] Attaching to " + name + " " + str( pid) + "\n")
            dbg.attach(pid)
        except:
            print "[!] Problem attaching to %s" % name
#            logf.write("[*] Problem attaching to " + name)
#                windows_kill(pid)
            continue

#time.sleep(3)
#print("--")
#print(hex(dbg.h_process))
#for t in dbg.enumerate_threads():
#    print(t)
#print("--")
#try:
#    dbg.write_process_memory(0x77063846, "\xCC")
#    instr = dbg.read_process_memory(0x77063846, 1)
#    print(hex(ord(instr)))
#except:
#    print("exc")
#print(dbg.h_thread)
#print(pid)
#dbg.dbg_print_all_debug_registers()
#print("--")
dbg.bp_set(0x770627e4, handler=handle_bp)
#load file
#thread = Thread(target = file_run, args = (filee, dbg, ))
#thread.start()

#os.system("start " + samples_dir + "\\" + filee)
os.system("start " + filee)
dbg.debug_event_loop()

#print(dbg.callbacks)

#clean
#dbg.detach()
#dbg = None

#count += 1
#    break

#print("Got " + str(count) + " crashes, exporting to: " + cb_file)
#logf.write("Got " + str(count) + " crashes, exporting to: " + cb_file)

#cb.export_file(cb_file)
#logf.close()


