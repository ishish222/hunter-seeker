#!c:\python\python.exe

# $Id: heap_trace.py 231 2008-07-21 22:43:36Z pedram.amini $

# TODO - need to add race condition testing and hook de-activation testing.

import sys
sys.path.append("paimei")
from pydbg import *
from pydbg.defines import *

import pgraph
import utils
import struct

import getopt

regs = ["EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EBP", "ESP", "EIP"]

start_addr = 0x1012475
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
module_blacklist.append("MSVCR90.dll")
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
module_blacklist.append("gdi32.dll")
module_blacklist.append("secur32.dll")
module_blacklist.append("user32.dll")
module_blacklist.append("shell32.dll")


USAGE = "USAGE: heap_trace.py <-p|--pid PID> | <-l|--load filename>"                \
        "\n    [-g|--graph]            enable graphing"                             \
        "\n    [-m|--monitor]          enabe heap integrity checking"               \
        "\n    [-h|--host udraw host]  udraw host (for graphing), def:127.0.0.1"    \
        "\n    [-o|--port udraw port]  udraw port (for graphing), def:2542"

ERROR = lambda msg: sys.stderr.write("ERROR> " + msg + "\n") or sys.exit(1)

def access_violation (dbg):
    crash_bin = utils.crash_binning.crash_binning()
    crash_bin.record_crash(dbg)

    log("***** process access violated *****")
    log(crash_bin.crash_synopsis())
    dbg.terminate_process()

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


def log_eip(dbg):
    ea = dbg.exception_address
    log("%x" % ea)
    dbg.disasm(ea)
    if(dbg.mnemonic == "call"):
#        print("got call")
        target_module = dbg.addr_to_module(decode_op1(dbg, dbg.op1))
        
        if(target_module == None):
            target_name = "unknown"
        else:
            target_name = target_module.szModule
        for mod in module_blacklist:
            if(mod.upper() == target_name.upper()):
#                print("skipping call to: %s" % target_name.upper())
                dbg.bp_set(ea + dbg.get_instruction(ea).length)
                return DBG_CONTINUE

    for thread_id in dbg.enumerate_threads():
        thread_handle  = dbg.open_thread(thread_id)
        dbg.single_step(True, thread_handle)
    return DBG_CONTINUE

def log(line):
    global logf

    if(logf):
        logf.write(line + "\n")
    else:
        print(line)

def read_byte(addr):
    return struct.unpack("<B", dbg.read_process_memory(addr, 1))[0]

def read_short(addr):
    return struct.unpack("<H", dbg.read_process_memory(addr, 2))[0]

def read_long(addr):
    return struct.unpack("<I", dbg.read_process_memory(addr, 4))[0]

def start(dbg):
    print("got entry point")
    dbg.set_callback(EXCEPTION_BREAKPOINT, log_eip)
    dbg.set_callback(EXCEPTION_SINGLE_STEP, log_eip)
    dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, access_violation)
    for thread_id in dbg.enumerate_threads():
        print("Tracking thread " + str(hex(thread_id)))
        thread_handle  = dbg.open_thread(thread_id)
        dbg.single_step(True, thread_handle)
    return DBG_CONTINUE


# parse command line options.
try:
    opts, args = getopt.getopt(sys.argv[1:], "gh:o:l:mp:f:H:A:C:", ["graph", "host=", "monitor", "port=", "pid=", "logfile", "heaps", "addresses", "count"])
except getopt.GetoptError:
    ERROR(USAGE)

count    = 0
udraw    = False
host     = "127.0.0.1"
port     = 2542
filename = None
pid      = None
udraw    = None
graph    = pgraph.graph()
hooks    = utils.hook_container()
monitor  = False
allocs   = {}
logfile  = None
logf     = None

global observed_heaps
global observed_addresses
global current_count
global count
observed_heaps = None
observed_addresses = None
current_count = 0
count = None

for opt, arg in opts:
    if opt in ("-g", "--graph"):   udraw    = True
    if opt in ("-h", "--host"):    host     = arg
    if opt in ("-o", "--port"):    port     = int(arg)
    if opt in ("-l", "--load"):    filename = arg
    if opt in ("-p", "--pid"):     pid      = int(arg)
    if opt in ("-m", "--monitor"): monitor = True
    if opt in ("-f", "--logfile"):    logfile  = arg
    if opt in ("-H", "--heaps"):   observed_heaps  = arg.split(",")
    if opt in ("-A", "--addresses"):   observed_addresses  = arg.split(",")
    if opt in ("-C", "--count"):   count  = int(arg)

if not pid and not filename:
    ERROR(USAGE)

if(logfile != None):
    logf = open(logfile, "w")

if udraw:
    udraw = utils.udraw_connector(host, port)
    log("connection to udraw established...")

dbg = pydbg()

if pid:
    dbg.attach(pid)
else:
    dbg.load(filename)

print("here")

dbg.bp_set(start_addr, handler=start)

#dbg.set_callback(LOAD_DLL_DEBUG_EVENT,       dll_load_handler)

dbg.run()

if(logfile != None):
    logf.close()
