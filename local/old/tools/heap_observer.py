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

USAGE = "USAGE: heap_trace.py <-p|--pid PID> | <-l|--load filename>"                \
        "\n    [-g|--graph]            enable graphing"                             \
        "\n    [-m|--monitor]          enabe heap integrity checking"               \
        "\n    [-h|--host udraw host]  udraw host (for graphing), def:127.0.0.1"    \
        "\n    [-o|--port udraw port]  udraw port (for graphing), def:2542"

ERROR = lambda msg: sys.stderr.write("ERROR> " + msg + "\n") or sys.exit(1)

class __alloc:
    call_stack = []
    size       = 0

HEAP_OFF_TOTAL_FREE = 0x28
HEAP_OFF_SEGMENT    = 0x58
HEAP_OFF_BITMAP     = 0x158
HEAP_OFF_FREE_LISTS = 0x178
HEAP_OFF_LAL        = 0x688

SEGMENT_OFF_BASE_ADDR   = 0x18
SEGMENT_OFF_FIRST_ENTRY = 0x20
SEGMENT_OFF_LAST_VALID_ENTRY  = 0x24
SEGMENT_OFF_LAST_ENTRY  = 0x38

def access_violation (dbg):
    crash_bin = utils.crash_binning.crash_binning()
    crash_bin.record_crash(dbg)

    log("***** process access violated *****")
    log(crash_bin.crash_synopsis())
    dbg.terminate_process()


def dll_load_handler (dbg):
    global hooks
    global observed_heaps

    try:
        last_dll = dbg.get_system_dll(-1)
    except:
        return

    if last_dll.name.lower() == "ntdll.dll":
        addrRtlCreateHeap     = dbg.func_resolve_debuggee("ntdll", "RtlCreateHeap")
        addrRtlDestroyHeap    = dbg.func_resolve_debuggee("ntdll", "RtlDestroyHeap")
        addrRtlAllocateHeap   = dbg.func_resolve_debuggee("ntdll", "RtlAllocateHeap")
        addrRtlFreeHeap       = dbg.func_resolve_debuggee("ntdll", "RtlFreeHeap")
        addrRtlReAllocateHeap = dbg.func_resolve_debuggee("ntdll", "RtlReAllocateHeap")

        if(addrRtlCreateHeap != None):
            print(hex(addrRtlCreateHeap))
        if(addrRtlDestroyHeap != None):
            print(hex(addrRtlDestroyHeap))
        if(addrRtlAllocateHeap != None):
            print(hex(addrRtlAllocateHeap))
        if(addrRtlFreeHeap != None):
            print(hex(addrRtlFreeHeap))
        if(addrRtlReAllocateHeap != None):
            print(hex(addrRtlReAllocateHeap))

        hooks.add(dbg, addrRtlCreateHeap,     6, RtlCreateHeapEntry, RtlCreateHeapExit)
        hooks.add(dbg, addrRtlDestroyHeap,    1, RtlDestroyHeapEntry, RtlDestroyHeapExit)
        hooks.add(dbg, addrRtlAllocateHeap,   3, RtlAllocateHeapEntry, RtlAllocateHeapExit)
        hooks.add(dbg, addrRtlFreeHeap,       3, RtlFreeHeapEntry, RtlFreeHeapExit)
        hooks.add(dbg, addrRtlReAllocateHeap, 4, RtlReAllocateHeapEntry, RtlReAllocateHeapExit)

        log("rtl heap manipulation routines successfully hooked")

    return DBG_CONTINUE


def graph_connect (dbg, buff_addr, size, realloc=False):
    global count, graph
    count += 1

    eip = dbg.context.Eip

    allocator = pgraph.node(eip)
    allocated = pgraph.node(buff_addr)

    allocator.label = "%08x" % eip
    allocated.label = "%d" % size
    allocated.size  = size

    allocator.color = 0xFFAC59

    if realloc:
        allocated.color = 0x46FF46
    else:
        allocated.color = 0x59ACFF

    graph.add_node(allocator)
    graph.add_node(allocated)

    edge = pgraph.edge(allocator.id, allocated.id)
    edge.label = "%d" % count

    graph.add_edge(edge)


def graph_update (id, focus_first=False):
    global graph, udraw

    if udraw:
        if focus_first:
            udraw.focus_node(id)

        udraw.graph_new(graph)

        if not focus_first:
            udraw.focus_node(id)


def monitor_add (dbg, address, size):
    global monitor, allocs

    if not monitor:
        return

    alloc            = __alloc()
    alloc.size       = size
    alloc.call_stack = dbg.stack_unwind()
    allocs[address]  = alloc

    dbg.bp_set_mem(address+size+1, 1, handler=monitor_bp)


def monitor_bp (dbg):
    global allocs

    log("heap bound exceeded at %08x by %08x" % (dbg.violation_address, dbg.exception_address))

    for call in dbg.stack_unwind():
        log("\t%08x" % call)

    # determine which chunk was violated.
    for addr, alloc in allocs.iteritems():
        if addr + alloc.size < dbg.violation_address < addr + alloc.size + 4:
            violated_chunk = addr
            break

    log("violated chunk:")

    log("0x%08x: %d" % (violated_chunk, allocs[violated_chunk].size))

    for call in allocs[violated_chunk].call_stack:
        log("\t%08x" % call)

    raw_input("")

    # XXX - add check for Rtl addresses in call stack and ignore


def monitor_print():
    for addr, alloc in allocs.iteritems():
        log("0x%08x: %d" % (addr, alloc.size))

        for call in alloc.call_stack:
            log("\t%08x" % call)


def monitor_remove (dbg, address):
    global monitor, allocs

    if not monitor:
        return

    del allocs[address]
    monitor_print()


def outstanding_bytes ():
    outstanding = 0

    for node in graph.nodes.values():
        if hasattr(node, "size"):
            outstanding += node.size

    return outstanding


def log(line):
    global logf

    if(logf):
        logf.write(line + "\n")
    else:
        print(line)

def lookup(a, bm1, bm2):
    if(a < 0x40):
        bm1 >>= a
        bm1 &= 0x1
        if(bm1 == 1):
            return True
    else:
        a -= 0x40 
        bm2 >>= a
        bm2 &= 0x1
        if(bm2 == 1):
            return True
    return False

def read_byte(addr):
    return struct.unpack("<B", dbg.read_process_memory(addr, 1))[0]

def read_short(addr):
    return struct.unpack("<H", dbg.read_process_memory(addr, 2))[0]

def read_long(addr):
    return struct.unpack("<I", dbg.read_process_memory(addr, 4))[0]

def enumeratee(addr):
    curr = addr
    nextt = read_long(curr)
    log("\t" + hex(nextt))
    curr = nextt

    while(curr != addr):
        nextt = read_long(curr)
        log("\t" + hex(nextt))
        curr = nextt

def analyze_segment(seg_addr):
    base = read_long(seg_addr + SEGMENT_OFF_BASE_ADDR)
    first = read_long(seg_addr + SEGMENT_OFF_FIRST_ENTRY)
    last_valid = read_long(seg_addr + SEGMENT_OFF_LAST_VALID_ENTRY)
    last = read_long(seg_addr + SEGMENT_OFF_LAST_ENTRY)
    log("BaseAddress : {0}".format(hex(base)))
    log("FirstEntry : {0}".format(hex(first)))
    log("LastValidEntry : {0}".format(hex(last_valid)))
    log("LastEntry : {0}".format(hex(last)))
    log("Chunks:")

    curr = first
    curr_size = read_short(curr)
    prev_size = read_short(curr+0x2)
    seg_index = read_byte(curr+0x7)
    curr_size_bytes = curr_size*0x8
    nextt = curr + curr_size_bytes

#    log("\tAddr: {0}, Size: {1} ({2}), PrevSize: {3}, SegIxd: {4}".format(hex(curr), hex(curr_size), hex(curr_size_bytes), hex(prev_size), hex(seg_index)))
    log("\tAddr: {0}, Size: {1} ({2}), PrevSize: {3}".format(hex(curr), hex(curr_size), hex(curr_size_bytes), hex(prev_size)))

    while(nextt < last):
        curr = nextt
        curr_size = read_short(curr)
        curr_size_bytes = curr_size*0x8
        prev_size = read_short(curr+0x2)
        seg_index = read_byte(curr+0x7)
        log("\tAddr: {0}, Size: {1} ({2}), PrevSize: {3}".format(hex(curr), hex(curr_size), hex(curr_size_bytes), hex(prev_size)))
        nextt = curr + curr_size_bytes

def analyze_lal(lal_addr):
    nextt = read_long(lal_addr)
    log("\tAddr: {0}".format(hex(nextt)))

    while(nextt != 0x0):
        nextt = read_long(nextt)
        log("\tAddr: {0}".format(hex(nextt)))

def analyze_heap(dbg, addr):
    bm1 = struct.unpack("<Q", dbg.read_process_memory(addr + HEAP_OFF_BITMAP, 8))[0]
    bm2 = struct.unpack("<Q", dbg.read_process_memory(addr + HEAP_OFF_BITMAP+0x8, 8))[0]
    log("bm1 " +hex(bm1))
    log("bm2 " +hex(bm2))

    freelists = addr + HEAP_OFF_FREE_LISTS
    for i in range(0,128):
        if(lookup(i, bm1, bm2) == True):
            log("FreeList[{0}]: {1}".format(hex(i), hex(freelists + i*0x8)))
            enumeratee(freelists + i*0x8)

    totalfree = addr + HEAP_OFF_TOTAL_FREE
    log("TotalFree: {0}".format(struct.unpack("<L", dbg.read_process_memory(totalfree, 4))[0]))

    log("Segments")

    for i in range(0x0, 0x40):
        segment = read_long(addr + HEAP_OFF_SEGMENT + i*0x4)
        if(segment != 0x0):
            log("Segment[{0}]: {1}".format(hex(i), hex(segment)))
            analyze_segment(segment)

    for i in range(0x0, 0x80):
        lal = addr + HEAP_OFF_LAL + i*0x30
        lal_head = read_long(lal)
        if(lal_head != 0x0):
            log("LAL[{0}]: {1}".format(hex(i), hex(lal_head)))
            analyze_lal(lal)
        

def RtlCreateHeapEntry (dbg, args):
    global graph
    global observed_heaps

    if(observed_heaps == None):
        log("[*] %08x: RtlCreateHeap(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)" % (dbg.context.Eip, args[0], args[1], args[2], args[3], args[4], args[5]))
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("[*] %08x: RtlCreateHeap(0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x)" % (dbg.context.Eip, args[0], args[1], args[2], args[3], args[4], args[5]))

def RtlDestroyHeapEntry (dbg, args):
    global graph
    global observed_heaps

    if(observed_heaps == None):
        log("[/] %08x: RtlDestroyHeap(0x%08x)" % (dbg.context.Eip, args[0]))
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("[/] %08x: RtlDestroyHeap(0x%08x)" % (dbg.context.Eip, args[0]))

def RtlAllocateHeapEntry (dbg, args):
    global graph
    global observed_heaps

    if(observed_heaps == None):
        log("[+] %08x: RtlAllocateHeap(0x%08x, 0x%08x, 0x%x)" % (dbg.context.Eip, args[0], args[1], args[2]))
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("[+] %08x: RtlAllocateHeap(0x%08x, 0x%08x, 0x%x)" % (dbg.context.Eip, args[0], args[1], args[2]))

def RtlFreeHeapEntry (dbg, args):
    global graph
    global observed_heaps

    if(observed_heaps == None):
        log("[-] %08x: RtlFreeHeap(0x%08x, 0x%08x, 0x%08x)" % (dbg.context.Eip, args[0], args[1], args[2]))
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("[-] %08x: RtlFreeHeap(0x%08x, 0x%08x, 0x%08x)" % (dbg.context.Eip, args[0], args[1], args[2]))

def RtlReAllocateHeapEntry (dbg, args):
    global graph
    global observed_heaps

    if(observed_heaps == None):
        log("[=] %08x: RtlReAllocateHeap(0x%08x, 0x%08x, 0x%08x, 0x%x" % (dbg.context.Eip, args[0], args[1], args[2], args[3]))
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("[=] %08x: RtlReAllocateHeap(0x%08x, 0x%08x, 0x%08x, 0x%x" % (dbg.context.Eip, args[0], args[1], args[2], args[3]))

def RtlCreateHeapExit (dbg, args, ret):
    global graph
    global observed_heaps
    global current_count 

    if(observed_heaps == None):
        log("== 0x%08x" % (ret))
        current_count +=  1
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("== 0x%08x [%d]" % (ret, current_count))
                current_count +=  1

def RtlDestroyHeapExit (dbg, args, ret):
    global graph
    global observed_heaps
    global current_count 

    if(observed_heaps == None):
        log("== 0x%08x" % (ret))
        current_count +=  1
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                current_count +=  1
                log("== 0x%08x [%d]" % (ret, current_count))

def RtlAllocateHeapExit (dbg, args, ret):
    global graph
    global observed_heaps
    global current_count 
    global count

    if(observed_heaps == None):
        log("== 0x%08x" % (ret))
        current_count +=  1
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("== 0x%08x [%d]" % (ret, current_count))
                current_count +=  1
                if(count != None and current_count > count):
                    analyze_heap(dbg, args[0])

def RtlFreeHeapExit (dbg, args, ret):
    global graph
    global observed_heaps
    global current_count 
    global count 

    if(observed_heaps == None):
        log("== 0x%08x" % (ret))
        current_count +=  1
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("== 0x%08x [%d]" % (ret, current_count))
                current_count +=  1
                if(count != None and current_count > count):
                    analyze_heap(dbg, args[0])


def RtlReAllocateHeapExit (dbg, args, ret):
    global graph
    global observed_heaps
    global current_count 
    global count 


    if(observed_heaps == None):
        log("== 0x%08x" % (ret))
        current_count +=  1
    else:        
        for observed_heap in observed_heaps:
            if(args[0] == int(observed_heap, 16)):
                log("== 0x%08x [%d]" % (ret, current_count))
                current_count +=  1
                if(count != None and current_count > count):
                    analyze_heap(dbg, args[0])


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

dbg.set_callback(EXCEPTION_ACCESS_VIOLATION, access_violation)
dbg.set_callback(LOAD_DLL_DEBUG_EVENT,       dll_load_handler)

dbg.run()

if(logfile != None):
    logf.close()
