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

samples_dir = "X:\\"
crashed_dir = "X:\\crashed"
hanged_dir = "X:\\hanged"
clean_dir = "X:\\clean"
#log_file = "X:\\log-"
log_file = "log-"

bl_instr = []
bl_instr.append("")

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

def handle_av(dbg):
    global status

#    if dbg.ignore_first_chance and dbg.dbg.u.Exception.dwFirstChance:
#        return DBG_EXCEPTION_NOT_HANDLED
    instr = ""
    crash_bin = cb
    crash_bin.record_crash(dbg)
    addr = crash_bin.last_crash.exception_address
    binn = hex(crash_bin.last_crash.exception_address)
    print(binn)
    print("--")
    # wrap in try! (sometimes executes non-existent code)
    instr = dbg.disasm(crash_bin.last_crash.exception_address)
    print(instr)
    if(instr == "mov eax,[ecx+0x4]"):
        return DBG_EXCEPTION_NOT_HANDLED
#    print(crash_bin.crash_synopsis())

    return DBG_CONTINUE
    l.acquire()
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
    print("got it!")
    code = "invalid"
    eip = dbg.get_register("EIP")
    if(int(eip) != 0x770627e4):
        return DBG_EXCEPTION_NOT_HANDLED
    esp = dbg.get_register("ESP")
    print("ESP: " + hex(esp))
    print("EIP: " + hex(eip))
    dw = dbg.read_process_memory(esp + 0x4, 4)
    print(hex(struct.unpack("<I", dw)[0]))
    return DBG_CONTINUE    
    

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

imagename = "acad.exe"
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


