#!c:\python\python.exe

# $Id: heap_trace.py 231 2008-07-21 22:43:36Z pedram.amini $

# TODO - need to add race condition testing and hook de-activation testing.

from pydbg import *
from pydbg.defines import *

import pgraph
import utils

import sys
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


def access_violation (dbg):
    crash_bin = utils.crash_binning.crash_binning()
    crash_bin.record_crash(dbg)

    log("***** process access violated *****")
    log(crash_bin.crash_synopsis())
    dbg.terminate_process()


def dll_load_handler (dbg):
    global hooks

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
            print((hex(addrRtlCreateHeap)))
        if(addrRtlDestroyHeap != None):
            print((hex(addrRtlDestroyHeap)))
        if(addrRtlAllocateHeap != None):
            print((hex(addrRtlAllocateHeap)))
        if(addrRtlFreeHeap != None):
            print((hex(addrRtlFreeHeap)))
        if(addrRtlReAllocateHeap != None):
            print((hex(addrRtlReAllocateHeap)))


        hooks.add(dbg, addrRtlCreateHeap,     6, None, RtlCreateHeap)
        hooks.add(dbg, addrRtlDestroyHeap,    1, None, RtlDestroyHeap)
        hooks.add(dbg, addrRtlAllocateHeap,   3, None, RtlAllocateHeap)
        hooks.add(dbg, addrRtlFreeHeap,       3, None, RtlFreeHeap)
        hooks.add(dbg, addrRtlReAllocateHeap, 4, None, RtlReAllocateHeap)

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
    for addr, alloc in allocs.items():
        if addr + alloc.size < dbg.violation_address < addr + alloc.size + 4:
            violated_chunk = addr
            break

    log("violated chunk:")

    log("0x%08x: %d" % (violated_chunk, allocs[violated_chunk].size))

    for call in allocs[violated_chunk].call_stack:
        log("\t%08x" % call)

    input("")

    # XXX - add check for Rtl addresses in call stack and ignore


def monitor_print():
    for addr, alloc in allocs.items():
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

    for node in list(graph.nodes.values()):
        if hasattr(node, "size"):
            outstanding += node.size

    return outstanding


def log(line):
    global logf

    if(logf):
        logf.write(line + "\n")
    else:
        print(line)

def RtlCreateHeap (dbg, args, ret):
    global graph
    log("%08x: RtlCreateHeap(%08x, %08x, %08x, %08x, %08x, %08x) == %08x" % (dbg.context.Eip, args[0], args[1], args[2], args[3], args[4], args[5], ret))

def RtlDestroyHeap (dbg, args, ret):
    global graph
    log("%08x: RtlDestroyteHeap(%08x) == %08x" % (dbg.context.Eip, args[0], ret))

def RtlAllocateHeap (dbg, args, ret):
    global graph

    # heap id, flags, size
    log("[%04d] %08x: RtlAllocateHeap(%08x, %08x, %d) == %08x" % (len(graph.nodes), dbg.context.Eip, args[0], args[1], args[2], ret))

    monitor_add(dbg, ret, args[2])

    graph_connect(dbg, ret, args[2])
    graph_update(dbg.context.Eip)


def RtlFreeHeap (dbg, args, ret):
    global graph

    # heap id, flags, address
    log("[%04d] %08x: RtlFreeHeap(%08x, %08x, %08x) == %08x" % (len(graph.nodes), dbg.context.Eip, args[0], args[1], args[2], ret))
    log("%d bytes outstanding" % outstanding_bytes())

    monitor_remove(dbg, args[2])

    for edge in graph.edges_to(args[2]):
        graph.del_edge(edge.id)

    graph.del_node(args[2])
    graph_update(args[2], True)


def RtlReAllocateHeap (dbg, args, ret):
    global graph

    # heap id, flags, address, new size
    log("[%04d] %08x: RtlReAllocateHeap(%08x, %08x, %08x, %d) == %08x" % (len(graph.nodes), dbg.context.Eip, args[0], args[1], args[2], args[3], ret))

    monitor_remove(dbg, args[2])
    monitor_add(dbg, ret, args[3])

    graph.del_node(args[2])
    graph_connect(dbg, ret, args[3], realloc=True)
    graph_update(dbg.context.Eip)


# parse command line options.
try:
    opts, args = getopt.getopt(sys.argv[1:], "gh:o:l:mp:f:", ["graph", "host=", "monitor", "port=", "pid=", "logfile"])
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

for opt, arg in opts:
    if opt in ("-g", "--graph"):   udraw    = True
    if opt in ("-h", "--host"):    host     = arg
    if opt in ("-o", "--port"):    port     = int(arg)
    if opt in ("-l", "--load"):    filename = arg
    if opt in ("-p", "--pid"):     pid      = int(arg)
    if opt in ("-m", "--monitor"): monitor = True
    if opt in ("-f", "--logfile"):    logfile  = arg

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
