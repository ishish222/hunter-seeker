import sys

#sys.path.append("e:\\server\\paimei")

import serial
import win32pipe, win32file, win32gui
import time
#import binner
import TraceController
#from socket import socket, AF_INET, SOCK_STREAM
import socket
from subprocess import call, Popen, PIPE
from glob import glob
import pefile
from functions import *

PIPE_NAME = "\\\\.\\pipe\\control"
PIPE_BUFF_SIZE = 4096

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    OK_STR = BOLD+OKGREEN+"OK"+ENDC
    ERR_STR = BOLD+FAIL+"ERROR"+ENDC

class SerialWrap(serial.Serial):
    def write(self, data):
        super(SerialWrap, self).write(data+'\n')
#        super(SerialWrap, self).write('-=OK=-')

#else:
#    log_file = "e:\\logs\\log-"

def getPipe(name):
#    dlog("In getPipe", 2)
    ext_pipe = win32file.CreateFile(name, win32file.GENERIC_READ | win32file.GENERIC_WRITE | win32pipe.PIPE_TYPE_MESSAGE, 0, None, win32file.OPEN_EXISTING, 0, None)
    return ext_pipe

def readPipe(pipe):
    data = ""
    while True:
        data += pipe.read(1)
        
        if(data[-6:] == "-=OK=-"): 
            break
    return data[:-6]

def writePipe(pipe, data):
#    pipe.write(data + "-=OK=-")
    pipe.write(data)

def ok(pipe):
    pipe.write("-=OK=-")

### TODO: trzeba dopracowac!

def handle_crash():
#    global main_binner
#    global crash_bin
#    global crash_reason

#    e_addr = crash_bin.last_crash.exception_address

    #create dossier
#    if(reason == "hc"):
#        crash_reason = "hc"
#        writePipe(ext_pipe, "Probable crash reason: hc (crash info needs reinterpretation)\n")
#        writePipe(ext_pipe, "")
#    if((disasm(dbg, e_addr) == "[INVALID]") or (get_module(dbg, e_addr) == "[INVALID]")):
#        crash_reason = "uaf"
#        writePipe(ext_pipe, "Probable crash reason: uaf\n")
#        writePipe(ext_pipe, "")
#    else:
#        crash_reason = "unk"
#
#    print("5")
#    writePipe(ext_pipe, crash_bin.crash_synopsis())
#    get_synopsis()
#    data = 
#
#    return DBG_CONTINUE
    return

logStarted = False

def startLog(name):
    global logStarted
#    global log_file
    global logf

#    log_file = "%s" % name
#    logf = open(log_file, "a", 0)
#    logf.write("test\n")
#    logStarted = True

def stopLog():
    global logStarted
    global logf

    logf.close()
    logStarted = False

def dlog(data, level=0):
    global logStarted
    global logf

    if(log_level <0):
        return
    if(level > log_level):
        return

    if(logStarted == False):
        print("[internal]: %s" % data)
    logf.write("[internal]: %s" % data)

def verify():
#    dlog("Please verify process")
    time.sleep(5)

def process_status_queue(satisfying = None):
    global ext_pipe
    global status

    status = ""
    satisfied = False
    
    while(not status.empty()):
        dlog("Queue size before get: %d" % status.qsize())
        item = status.get()
        dlog("Queue size after get : %d" % status.qsize())
        status = item[1]
        dlog("Processing %s" % status)
        if(status == "TO"):
            if(prev_status == "MA"):
                status = "PTO"
            if(prev_status == "TO"):
                status = "PTO"
            if(prev_status == "RD"):
                status = "PTO"
        if(status == "EX"):
            writePipe(ext_pipe, "Status: %s\n" % status)
            writePipe(ext_pipe, "Encountered exception: %s\n" % item[2])
            ok(ext_pipe)
        if(status == "SR"):
            dlog("Requested script: %s" % item[2])
            writePipe(ext_pipe, "Status: %s\n" % status)
            writePipe(ext_pipe, "Script: %s\n" % item[2])
            ok(ext_pipe)
            # time for reaction to SR
            start_debuggers("Script execution")
            readPipe()
            stop_debuggers("Script execution finished") #could add statuses into queue
            dlog("Queue size after script: %d" % status.qsize())
            continue

        print("Got status: %s " % status)
        if(satisfying != None):
            if(status in satisfying):
                print("Status %s is satisfying" % status)
                satisfied = True
            else:
                print("Status %s is not satisfying" % status)
                print("Satisfying statuses:")
                print(satisfying)
        prev_status = status

    return satisfied

def window_check(hwnd, lParam):
    global window_name
    global found

    writePipe(ext_pipe, win32gui.GetWindowText(hwnd))
    if(win32gui.GetWindowText(hwnd).find(window_name) != -1):
        found = True

### binner commands
def execute(cmds, ext_pipe):
#    global ext_pipe
    global status
    global trace_controller
    global responder
    global mailslot_client

    cmd = cmds[0]
    args = " ".join(cmds[1:])

#    try:
    if(True):
        if(cmd == "settle"):
            status = ""
    
            watchThread = Thread(target = watchThread_routine, args=(float(cmds[1]),))
            watchThread.start()
            start_debuggers()
            ok(ext_pipe)

        if(cmd == "windowExists"):
            global window_name
            global found

            found = False
            window_name = args

            win32gui.EnumWindows(window_check, None)

            if(found == True):
                writePipe(ext_pipe, "FOUND")
            else:
                writePipe(ext_pipe, "NOT FOUND")
            ok(ext_pipe)

        if(cmd == "getCPUUsage"):
            from functions import getCPU
            val = getCPU()
            writePipe(ext_pipe, "Usage: %d\n" % val)
            ok(ext_pipe)

        if(cmd == "cooldown"):
            print "here2"
            from functions import getCPU

            print("Waiting for cooldown")
            cool_count = 5
            cool_level = 10
            cool_wait = 1

            count = 0
            while(count < cool_count):
                time.sleep(cool_wait)
                val = getCPU()
                print("CPU usage: %d" % val)
                if val < cool_level:
                    count = count+1
                else:
                    count = 0

            writePipe(ext_pipe, "We're cool\n")
            ok(ext_pipe)

        if(cmd == "cooldown2"):
            from functions import getCPU

            cool_count, cool_level = args.split(" ")
            cool_count = int(cool_count)
            cool_level = int(cool_level)
            cool_wait = 5

            writePipe(ext_pipe, "Waiting for cooldown: level %d, count %d\n" % (cool_level, cool_count))
            print("Waiting for cooldown: level %d, count %d\n" % (cool_level, cool_count))

            count = 0
            while(count < cool_count):
                time.sleep(cool_wait)
                val = getCPU()
                writePipe(ext_pipe, "CPU usage: %d\n" % val)
                print 'val: %d, cool_level: %d' % (val, cool_level)
                if val < cool_level:
                    count = count+1
                else:
                    count = 0

            writePipe(ext_pipe, "We're cool\n")
            ok(ext_pipe)

        if(cmd == "cooldown3"):
            from functions import getCPU

            cool_count, cool_level = args.split(" ")
            cool_count = int(cool_count)
            cool_level = int(cool_level)
            cool_wait = 1

            writePipe(ext_pipe, "Waiting for cooldown: level %d, count %d\n" % (cool_level, cool_count))
            print("Waiting for cooldown: level %d, count %d\n" % (cool_level, cool_count))

            count = 0
            while(count < cool_count):
                time.sleep(cool_wait)
                val = getCPU()
                writePipe(ext_pipe, "CPU usage: %d\n" % val)
                print 'val: %d, cool_level: %d' % (val, cool_level)
                if val < cool_level:
                    count = count+1
                else:
                    count = 0

            writePipe(ext_pipe, "We're cool\n")
            ok(ext_pipe)

            writePipe(ext_pipe, "Done")
            ok(ext_pipe)
            detach_rd_markers()

        elif(cmd == "openserialport"):
#            dlog("In getSynopsis", 2)
            try:
                ext_pipe2 = serial.Serial(1)
            except Exception:
                print "No second serial"
            ext_pipe2.write("test-=OK=-") 
            print("done")
            ok(ext_pipe)

        elif(cmd == "getSynopsis"):
#            dlog("In getSynopsis", 2)
            writePipe(ext_pipe, last_data)
            ok(ext_pipe)
#            get_synopsis()
#            ok(ext_pipe)

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStart"):
            startLog(args)
            writePipe(ext_pipe, "logStart OK")
            ok(ext_pipe)

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStop"):
            stopLog()
            stop_log()
            ok(ext_pipe)
    
        elif(cmd == "tracer_ps"):
            for (ppid, pid, name) in enumerate_processes_custom():
                writePipe(ext_pipe, "0x%x 0x%x %s\n" % (ppid, pid, name))
            writePipe(ext_pipe, "tracer_ps OK")
            ok(ext_pipe)

        elif(cmd == "listTebs"):
            list_tebs()
            ok(ext_pipe)

        elif(cmd == "cbEip"):
            writePipe(ext_pipe, "0x%x" % crash_bin.last_crash.exception_address)
            ok(ext_pipe)

        elif(cmd == "cbCrashSynopsis"):
            writePipe(ext_pipe, crash_bin.crash_synopsis())
            ok(ext_pipe)
    
        elif(cmd == "cbReason"):
            writePipe(ext_pipe, crash_reason)
            ok(ext_pipe)
    
        elif(cmd == "cbStackUnwind"):
            for call_frame in crash_bin.last_crash.stack_unwind:
                writePipe(ext_pipe, call_frame + "\n")
            ok(ext_pipe)

        # TODO: sprawdz, czy dziala!
        elif(cmd == "attachBinner"):
            imagename = args
            for (pid, name) in enumerate_processes():
                if imagename in name:
#                    try:
                    if True:
#                        dlog("[*] Attaching to %s (%d)" % (name, pid))
                        attach(pid)
#                    except Exception, e:
#                        dlog("[!] Problem attaching to %s" % name)
#                        dlog(e)
                    continue

            writePipe(ext_pipe, "Attached to " + str(args))
            ok(ext_pipe)

        elif(cmd == "killHost"):
            terminate_processes()
            ok(ext_pipe)
    
        elif(cmd == "installHandlers"):
            attach_react_markers()
            attach_ex_handler()
            attach_av_handler()
            ok(ext_pipe)

        elif(cmd == "installMarkerAddrs"):
#            for ma_addr in settings.ma_addrs:
#                writePipe(ext_pipe, "Installing bad at " + cmds[1])
#                dbg.bp_set(ma_addr, handler = bad_handler)
            # will do automatically on process attach
            ok(ext_pipe)

        elif(cmd == "binTest"):
#            dlog("binTest")
            writePipe(ext_pipe, "Communication with binner is working")
            ok(ext_pipe)
        
        elif(cmd == "checkReady"):
            writePipe(ext_pipe, "Status: RD")
            ok(ext_pipe)

        elif(cmd == "tracer_spawn"):
            print("Spawning: %s" % args)
            Popen([args], stdin = PIPE, stdout = PIPE, stderr = PIPE)
            print "here1"
            writePipe(ext_pipe, "tracer_spawn "+bcolors.OKGREEN+"OK"+bcolors.ENDC)
            ok(ext_pipe)

        elif(cmd == "startBinner"):
            writePipe(ext_pipe, "OK")
            ok(ext_pipe)

        elif(cmd == "kill"):
            print("Killing %s" % args)
            call("taskkill /F /IM %s" % args)
            writePipe(ext_pipe, "kill "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "killExplorer"):
            call("taskkill /F /IM explorer.exe")
            #writePipe(log_pipe, "Killed explorer")
            writePipe(ext_pipe, "killExplorer "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "release"):
            writePipe(ext_pipe, "Releasing")
            ok(ext_pipe)
            start_debuggers()

        elif(cmd == "start_profiling"):
            start_profiling()

        elif(cmd == "dump_stats"):
            dump_stats(args)

        elif(cmd == "quit"):
            close_logs()
            #log_pipe.write("quit")

        elif(cmd == "read_ep"):
            writePipe(ext_pipe, "read_ep "+bcolors.OK_STR)
            ok(ext_pipe)

### tracer info
        elif(cmd == "extract_ep"):
            p = pefile.PE(args)
            writePipe(ext_pipe, "0x%08x" % p.OPTIONAL_HEADER.AddressOfEntryPoint)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            del p
            writePipe(ext_pipe, "extract_ep "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tc_activate_prev"):
            trace_controller.activate_prev_tracer()
            writePipe(ext_pipe, "[internal] Active tracer: 0x%02x" % trace_controller.tracer_active_id)
            writePipe(ext_pipe, "[internal]: "+bcolors.OK_STR)
            writePipe(ext_pipe, "tc_avtivate_prev "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tc_activate_next"):
            trace_controller.activate_next_tracer()
            writePipe(ext_pipe, "[internal] Active tracer: 0x%02x" % trace_controller.tracer_active_id)
            writePipe(ext_pipe, "[internal]: "+bcolors.OK_STR)
            writePipe(ext_pipe, "tc_avtivate_next "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tc_current"):
            writePipe(ext_pipe, "[internal] Active tracer: 0x%02x" % trace_controller.tracer_active_id)
            writePipe(ext_pipe, "[internal]: "+bcolors.OK_STR)
            writePipe(ext_pipe, "tc_avtivate_next "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_current_tid"):
            trace_controller.tracer_current_tid()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_current_tid "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_set_priority_high"):
            trace_controller.tracer_set_priority_high(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_priority_high "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_suspend_thread"):
            trace_controller.tracer_suspend_thread(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_suspend_thread "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_suspend_all"):
            trace_controller.tracer_suspend_all()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_suspend_all"+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_suspend_all_except"):
            trace_controller.tracer_suspend_all_except(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_suspend_all_except "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_release_thread"):
            trace_controller.tracer_release_thread(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_release_thread "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_release_all"):
            trace_controller.tracer_release_all()
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_release_all "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_release_all_except"):
            trace_controller.tracer_release_all_except(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_release_all_except "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_set_base"):
            trace_controller.set_base(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_set_base"+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_sample_pid"):
            trace_controller.set_sample_pid(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_sample_pid "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_sample_file"):
            trace_controller.set_sample_file(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_sample_file "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_in_dir"):
            trace_controller.configure_in_dir(args)
            trace_controller.in_dir = args
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_in_dir "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_out_dir"):
            trace_controller.configure_out_dir(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_out_dir "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_append_out_prefix"):
            trace_controller.append_out_prefix(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_append_out_prefix "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_out_prefix"):
            trace_controller.configure_out_prefix(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_out_prefix "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_markers"):
            trace_controller.configure_markers(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_markers "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_register_regions"):
            trace_controller.register_regions(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_register_regions "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_register_reactions"):
            trace_controller.register_reactions(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_register_reactions "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_disable_reaction"):
            trace_controller.disable_reaction(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_disable_reaction "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_deautorepeat_reaction"):
            trace_controller.deautorepeat_reaction(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_deautorepeat_reaction "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_autorepeat_reaction"):
            trace_controller.autorepeat_reaction(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_autorepeat_reaction "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_exclusive_reaction"):
            trace_controller.exclusive_reaction(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_exclusive_reaction "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_reaction_counter"):
            trace_controller.reaction_counter(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_reaction_counter "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_raise_reaction"):
            trace_controller.raise_reaction(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_raise_reaction "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_lower_reaction"):
            trace_controller.lower_reaction(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_lower_reaction "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_enable_reaction"):
            trace_controller.enable_reaction(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_enable_reaction "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_enable_all_reactions"):
            trace_controller.enable_all_reactions()
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_enable_all_reactions "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_disable_all_reactions"):
            trace_controller.disable_all_reactions()
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_disable_all_reactions "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_resolve_location"):
            trace_controller.resolve_location(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_resolve_location "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_auto_st"):
            trace_controller.auto_st()
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_auto_st "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_set_parameters"):
            trace_controller.set_parameters(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_set_parameters "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_set_st"):
            trace_controller.set_st(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_set_st "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_activate_markers"):
            trace_controller.activate_markers()
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_activate_markers "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_sample_pname"):
            trace_controller.set_sample_pname(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_sample_pname "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_marker_st"):
            mod, addr = args.split(" ")
            trace_controller.configure_marker_st(mod, addr)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_marker_st "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_configure_marker_end"):
            mod, addr = args.split(" ")
            trace_controller.configure_marker_end(mod, addr)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_configure_marker_end "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_attach_sample"):
            trace_controller.attach_sample()
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_attach_sample "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_debug_sample"):
            trace_controller.debug_sample()
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_debug_sample "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_debug_continue"):
            trace_controller.debug_continue(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_debug_continue "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_debug_continue_time"):
            trace_controller.debug_continue_time(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_debug_continue_time "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_prepare_trace"):
            trace_controller.prepare_trace()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_prepare_trace "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_set_limit"):
            trace_controller.set_limit(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_set_limit "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_dump_memory"):
            trace_controller.dump_memory()
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            ok(ext_pipe)

        elif(cmd == "tracer_start_strace"):
            trace_controller.start_strace()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_start_strace "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_start_trace"):
            trace_controller.start_trace()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_start_trace "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_start_trace_light"):
            trace_controller.start_trace_light()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_start_trace_light "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_start_trace_debug"):
            trace_controller.start_trace_debug()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_start_trace_debug "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_get_exception_code"):
            trace_controller.get_exception_code()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_get_exception_code "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_get_exception_address"):
            trace_controller.get_exception_address()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_get_exception_address "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_stop_trace"):
            trace_controller.stop_trace()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_stop_trace "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_list_markers"):
            trace_controller.list_markers()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_list_markers "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_run_routine"):
            trace_controller.run_routine(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "tracer_run_routine "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_secure_all_sections"):
            trace_controller.secure_all_sections()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_secure_all_sections "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_update_memory_w_zero"):
            trace_controller.update_memory_w_zero(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_string_ansi "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_update_memory"):
            trace_controller.update_memory(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_string_ansi "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_read_string_ansi"):
            trace_controller.read_string_ansi(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_string_ansi "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_write_string_ansi"):
            trace_controller.write_string_ansi(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_string_ansi "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_read_string_unicode"):
            trace_controller.read_string_unicode(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_string_unicode "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_write_string_unicode"):
            trace_controller.write_string_unicode(args)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_write_string_unicode "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_read_arg"):
            trace_controller.read_arg(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_arg "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_read_arg_ansi"):
            trace_controller.read_arg_ansi(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_arg_uni "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_read_arg_uni"):
            trace_controller.read_arg_uni(args)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_arg_uni "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_list_bpts"):
            trace_controller.list_bpts()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_list_bpts "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_list_libs"):
            trace_controller.list_libs()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_list_libs "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_list_all_tebs"):
            trace_controller.list_all_tebs()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_list_all_tebs "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_list_tebs"):
            trace_controller.list_tebs()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_list_tebs "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_list_ps"):
            trace_controller.list_ps()
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_list_ps "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_out_region"):
            trace_controller.out_region(args)
            writePipe(ext_pipe, "%s\n" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_out_region "+bcolors.OK_STR);
            ok(ext_pipe)

        elif(cmd == "tracer_read_region"):
            trace_controller.read_region(args)
            writePipe(ext_pipe, "%s\n" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_region "+bcolors.OK_STR);
            ok(ext_pipe)

        elif(cmd == "tracer_reopen_io"):
            trace_controller.reopen_io()
            ok(ext_pipe)

        elif(cmd == "tracer_read_dword"):
            trace_controller.read_dword(args)
            writePipe(ext_pipe, "%s\n" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_dword "+bcolors.OK_STR);
            ok(ext_pipe)

        elif(cmd == "tracer_write_dword"):
            trace_controller.write_dword(args)
            writePipe(ext_pipe, "%s\n" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_write_dword "+bcolors.OK_STR);
            ok(ext_pipe)

        elif(cmd == "tracer_read_register"):
            trace_controller.read_register(args)
            writePipe(ext_pipe, "%s\n" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_register "+bcolors.OK_STR);
            ok(ext_pipe)

        elif(cmd == "tracer_write_register"):
            trace_controller.write_register(args)
            writePipe(ext_pipe, "%s\n" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_write_register "+bcolors.OK_STR);
            ok(ext_pipe)

        elif(cmd == "tracer_read_stack"):
            trace_controller.read_stack(args)
            writePipe(ext_pipe, "%s\n" % trace_controller.last_answer)
            writePipe(ext_pipe, "[tracer 0x%02x]: %s" % (trace_controller.tracer_active_id , trace_controller.last_report))
            writePipe(ext_pipe, "tracer_read_stack "+bcolors.OK_STR)
            #writeReport(trace_controller)
            ok(ext_pipe)

        elif(cmd == "get_http"):
            import urllib2
            response = urllib2.urlopen(args).read()
            writePipe(ext_pipe, "%s" % response)
            writePipe(ext_pipe, "get_http "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "run_cmd"):
            import subprocess
            cmd = "cmd.exe /c %s" % args
            print 'CMD: %s\n' % cmd
            p = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr = subprocess.PIPE)
            out, err = p.communicate()
            writePipe(ext_pipe, out)
            writePipe(ext_pipe, err)
            writePipe(ext_pipe, "run_cmd "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "get_dns"):
            import socket

            response = socket.gethostbyname(args)
            writePipe(ext_pipe, "%s" % response)
            writePipe(ext_pipe, "get_dns "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "tracer_print"):
            trace_controller.print_sth(args)
            writePipe(ext_pipe, "tracer_print "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "spawn_responder_80"):
            from simple import simple_responder
            responder = simple_responder(80)
            writePipe(ext_pipe, "spawn_responder_80 "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "spawn_responder"):
            from simple import simple_responder
            try:
                responder = simple_responder(int(args,10))
                writePipe(ext_pipe, "spawn_responder "+bcolors.OK_STR)
            except Exception:
                writePipe(ext_pipe, "ERROR")
            ok(ext_pipe)

        elif(cmd == "responder_next_response_from_file"):
            responder.load_next_response_from_file("%s\\%s" % (trace_controller.in_dir, args))
            writePipe(ext_pipe, "responder_next_response "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "responder_next_response"):
            responder.load_next_response(args)
            writePipe(ext_pipe, "responder_next_response "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "spawn_mailslot_client"):
            from mailslot_client import mailslot_client
            mailslot_client = mailslot_client(args)
            writePipe(ext_pipe, "spawn_mailslot_client "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "mailslot_next_response"):
            mailslot_client.load_next_response(args)
            writePipe(ext_pipe, "mailslot_next_response "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "spawn_tracer"):
            new_tracer = trace_controller.spawn_tracer()
            writePipe(ext_pipe, "[Trace controller]: "+bcolors.OK_STR)
            writePipe(ext_pipe, "Active tracers: %d" % trace_controller.trace_count)
            writePipe(ext_pipe, "spawn_tracer "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "spawn_tracer_log"):
            new_tracer = trace_controller.spawn_tracer_log()
            writePipe(ext_pipe, "[Trace controller]: "+bcolors.OK_STR)
            writePipe(ext_pipe, "Active tracers: %d" % trace_controller.trace_count)
            writePipe(ext_pipe, "spawn_tracer_log "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "spawn_tracer_remote_log"):
            new_tracer = trace_controller.spawn_tracer_remote_log()
            writePipe(ext_pipe, "[Trace controller]: "+bcolors.OK_STR)
            writePipe(ext_pipe, "Active tracers: %d" % trace_controller.trace_count)
            writePipe(ext_pipe, "spawn_tracer_remote_log "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "spawn_tracer_no_log"):
            new_tracer = trace_controller.spawn_tracer_no_log()
            writePipe(ext_pipe, "[Trace controller]: "+bcolors.OK_STR)
            writePipe(ext_pipe, "Active tracers: %d" % trace_controller.trace_count)
            writePipe(ext_pipe, "spawn_tracer_no_log "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "close_tracer"):
            trace_controller.close_active_tracer()
            writePipe(ext_pipe, "[Trace controller]: "+bcolors.OK_STR)
            writePipe(ext_pipe, "Remaining tracers: %d" % trace_controller.trace_count)
            writePipe(ext_pipe, "close_tracer "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "read_prefix"):
            trace_controller.read_prefix()
            writePipe(ext_pipe, "%s\n" % trace_controller.last_answer)
            writePipe(ext_pipe, "read_prefix "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "which_tracer"):
            trace_controller.close_active_tracer()
            writePipe(ext_pipe, "%d" % trace_controller.tracer_active_id)
            writePipe(ext_pipe, "which_tracer "+bcolors.OK_STR)
            ok(ext_pipe)

        elif(cmd == "start_trace_controller"):
            writePipe(ext_pipe, "[internal]: "+bcolors.OK_STR)
            trace_controller = TraceController.TraceController(ext_pipe)
            writePipe(ext_pipe, "start_trace_controller "+bcolors.OK_STR)
            ok(ext_pipe)

#    except Exception, e:
#        print(e)
#        while True:
#            pass
#        ok(ext_pipe)

### main loop

class Interrupt(Exception):
    pass

import threading
import inspect

def _async_raise(tid, exctype):
    '''Raises an exception in the threads with id tid'''
    if not inspect.isclass(exctype):
        raise TypeError("Only types can be raised (not instances)")
    res = ctypes.pythonapi.PyThreadState_SetAsyncExc(tid,
                                                  ctypes.py_object(exctype))
    if res == 0:
        raise ValueError("invalid thread id")
    elif res != 1:
        # "if it returns a number greater than one, you're in trouble,
        # and you should call it again with exc=NULL to revert the effect"
        ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, 0)
        raise SystemError("PyThreadState_SetAsyncExc failed")


class ThreadWithExc(threading.Thread):
    '''A thread class that supports raising exception in the thread from
       another thread.
    '''
    def _get_my_tid(self):
        """determines this (self's) thread id

        CAREFUL : this function is executed in the context of the caller
        thread, to get the identity of the thread represented by this
        instance.
        """
        if not self.isAlive():
            raise threading.ThreadError("the thread is not active")

        # do we have it cached?
        if hasattr(self, "_thread_id"):
            return self._thread_id

        # no, look for it in the _active dict
        for tid, tobj in threading._active.items():
            if tobj is self:
                self._thread_id = tid
                return tid

        # TODO: in python 2.6, there's a simpler way to do : self.ident

        raise AssertionError("could not determine the thread's id")

    def raiseExc(self, exctype):
        """Raises the given exception type in the context of this thread.

        If the thread is busy in a system call (time.sleep(),
        socket.accept(), ...), the exception is simply ignored.

        If you are sure that your exception should terminate the thread,
        one way to ensure that it works is:

            t = ThreadWithExc( ... )
            ...
            t.raiseExc( SomeException )
            while t.isAlive():
                time.sleep( 0.1 )
                t.raiseExc( SomeException )

        If the exception is to be caught by the thread, you need a way to
        check that your thread has caught it.

        CAREFUL : this function is executed in the context of the
        caller thread, to raise an excpetion in the context of the
        thread represented by this instance.
        """
        _async_raise( self._get_my_tid(), exctype )

def executing(cmd_q, ext_pipe):
    while True:
        cmds = cmd_q.get()
        execute(cmds, ext_pipe)
        print 'executing: %s\n' % cmds

def executing_thread():
    import time
    global cmd_q2

    while True:
        if(len(cmd_q2) == 0):
            time.sleep(1)
            continue
        cmds = cmd_q2.pop()
        try:
            execute(cmds, ext_pipe)
        except Interrupt:
            print 'Interrupted'
        print 'executing_thread: %s\n' % cmds

def executing_no_thread():
    import time
    global cmd_q2

    if(len(cmd_q2) == 0):
        time.sleep(1)
        return
    cmds = cmd_q2.pop()
    try:
        execute(cmds, ext_pipe)
    except Interrupt:
        print 'Interrupted'
    print 'executing_no_thread: %s\n' % cmds


threading_enabled = True

def internal_routine():
    global ext_pipe
    global cmd_q2
    global log_pipe
    global main_binner
    global trace_controller

#    dlog("In main", 2)
    logo = """
 __                      .__               
|  | ____________________|__| ____   ____  
|  |/ /  _ \_  __ \_  __ \  |/    \ /  _ \ 
|    <  <_> )  | \/|  | \/  |   |  (  <_> )
|__|_ \____/|__|   |__|  |__|___|  /\____/ 
     \/                          \/        
Hunter-Seeker
"""
    from multiprocessing import Process, Queue

    print(logo)

    cmd_q = Queue()
    cmd_q2 = []
    ext_pipe = SerialWrap(0)

    writePipe(ext_pipe, 'Ext_pipe connected')
    ok(ext_pipe)

    if(threading_enabled):
        p = ThreadWithExc(None, executing_thread)
        p.start()

        while True:
            print 'Reading...\n'
            cmd = readPipe(ext_pipe)
            if(cmd == 'disconnect'):
                writePipe(ext_pipe, 'disconnecting')
                ok(ext_pipe)
                ext_pipe.close()
                return
            if(cmd == 'interrupt'):
    #            p.terminate()
                p.raiseExc(Interrupt)
                p = ThreadWithExc(None, executing_thread)
                p.start()
                print 'Interrupted'
                ok(ext_pipe)
                continue
            cmds = cmd.split(" ")
    #        cmd_q.put(cmds)
            cmd_q2.insert(0, cmds)
            print 'Inserted_thread: %s\n' % cmds
    else:
        while True:
            print 'Reading...\n'
            cmd = readPipe(ext_pipe)
            cmds = cmd.split(" ")
            cmd_q2.insert(0, cmds)
            print 'Inserted_no_thread: %s\n' % cmds
            executing_no_thread()


if __name__ == '__main__':
    trace_controller = None
    while True:
        internal_routine()
        time.sleep(3)

