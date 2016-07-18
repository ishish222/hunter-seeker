import sys

sys.path.append("e:\\server\\paimei")
sys.path.append("e:\\common")

import serial
import win32pipe, win32file, win32gui
import time
import binner
import TraceController
import settings
from functions import *
#from socket import socket, AF_INET, SOCK_STREAM
import socket
from subprocess import call, Popen
from glob import glob


PIPE_NAME = "\\\\.\\pipe\\control"
PIPE_BUFF_SIZE = 4096

class SerialWrap(serial.Serial):
    def write(self, data):
        super(SerialWrap, self).write(data+'\n')
#        super(SerialWrap, self).write('-=OK=-')

if(defined("settings.log_file")):
    log_file = settings.log_file
else:
    log_file = "e:\\logs\\log-"

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
    global log_file
    global logf

    log_file = "%s.txt" % name
    logf = open(log_file, "a", 0)
    logf.write("test\n")
    logStarted = True

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
        return
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
def execute(cmds):
    global ext_pipe
    global status
    global trace_controller

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
            from functions import getCPU

            writePipe(ext_pipe, "Waiting for cooldown\n")
            cool_count = 5
            cool_level = 10
            cool_wait = 1

            count = 0
            while(count < cool_count):
                time.sleep(cool_wait)
                val = getCPU()
                writePipe(ext_pipe, "CPU usage: %d\n" % val)
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

        elif(cmd == "testAll"):
            for sample in glob("e:\\samples\\shared\\*.*"):
                attach_st_markers()
                loop_debuggers(invocation = "powershell -command \"& { invoke-expression %s }\"" % sample)
                while(process_status_queue(["ST", "CR", "TO", "CR"]) != True):
                    loop_debuggers()
                if(status == "TO"):
                    print("ST TO")
                    continue
                if(status == "CR"):
                    print("CRrrrrrrrrrr")
                    break
    
                print("Status: %s" % status)
    
                detach_st_markers()
                attach_end_markers()
    
    #            process_status_queue()
    #            loop_debuggers(settings.wait_sleep)
                while(process_status_queue(["MA", "CR", "TO"]) != True):
                    loop_debuggers(settings.wait_sleep)
                if(status == "CR"):
                    print("CRrrrrrrrrrr")
                    break
    
                print("Status: %s" % status)

                detach_end_markers()
                attach_rd_markers()
                # time for reaction to test end
                start_debuggers("Closing execution")
                readPipe()
                stop_debuggers("Closing execution finished")
    
    #            loop_debuggers()
                if(settings.needs_ready):
                    while(process_status_queue(["RD", "CR", "TO"]) != True):
                        loop_debuggers()
                    if(status == "TO"):
                        print("RD TO")
                        continue
                    if(status == "CR"):
                        print("CRrrrrrrrrrr")
                        break
    
            writePipe(ext_pipe, "Done")
            ok(ext_pipe)
            detach_rd_markers()

### test a file

        elif(cmd == "openFile"):
            settings.runner(args)
            ok(ext_pipe)
            return

### test a file

        elif(cmd == "testFile"):
            attach_st_markers()

            loop_debuggers(invocation_args=args)
            while(process_status_queue(["ST", "CR", "PTO"]) != True):
                loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
#                dlog("Processing CR1")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(args)
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)
            print "Send: %s" % status

            detach_st_markers()
            attach_end_markers()

            while(process_status_queue(["MA", "CR", "TO", "PTO"]) != True):
                loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
#                dlog("Processing CR2")
                print "CR handling 1"
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(args)
#                dlog("Hereeeeeee")
                print "CR handling 2"
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                print "Send: %s" % status
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            detach_end_markers()
            if(settings.needs_ready):
                attach_rd_markers()
            # time for reaction to test end
            start_debuggers("Closing execution")
            readPipe()
            stop_debuggers("Closing execution finished")

#            loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    loop_debuggers(settings.wait_sleep)
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(args)
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)
            detach_rd_markers()

### test a directory

        elif(cmd == "testDir"):
            args2 = args.split("\"")
            cmds[1] = args2[1]
            cmds[2] = args2[3]
            print("0: %s\n1: %s\n2: %s\n" % (cmds[0], cmds[1], cmds[2]))
            real_target = cmds[2]
            attach_st_markers()

            loop_debuggers(invocation_args=cmds[2])
            while(process_status_queue(["ST", "CR", "PTO"]) != True):
                loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
#                dlog("Processing CR1")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(cmds[1])
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            detach_st_markers()
            attach_end_markers()

            while(process_status_queue(["MA", "CR", "TO", "PTO"]) != True):
                loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
#                dlog("Processing CR2")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(cmds[1])
#                dlog("Hereeeeeee")
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            detach_end_markers()
            if(settings.needs_ready):
                attach_rd_markers()
            # time for reaction to test end
            start_debuggers("Closing execution")
            readPipe()
            stop_debuggers("Closing execution finished")

#            loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    loop_debuggers(settings.wait_sleep)
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(cmds[1])
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)
            detach_rd_markers()

### trace a file by name

        elif(cmd == "trace3"):
            filee = args
            print("Tracing %s" % (filee))
            spawn(args)
            writePipe(ext_pipe, "OK")
            ok(ext_pipe)
        
### trace a file by PID & wait for sample

        elif(cmd == "trace4"):
            filee = args
            print("Tracing %s" % (filee))
            spawn(args)
            writePipe(ext_pipe, "OK")
            ok(ext_pipe)
        
        elif(cmd == "trace"):
            filee = args
            print("Tracing %s" % (filee))
            
            take_a_trace("%s" % 'NU')

            print "About to release debugger"
            loop_debuggers(invocation_args=filee)
            while(process_status_queue(["WS"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            print "About to release debugger"

            print "here2"
            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            while(process_status_queue(["WE", "CR"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            if(settings.needs_ready):
                attach_rd_markers()
            # time for reaction to test end
            start_debuggers("Closing execution")
            readPipe()
            stop_debuggers("Closing execution finished")

            print "here3"
#            loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    print "waiting for RD"
                    loop_debuggers(settings.wait_sleep)
                    print "got sth"
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(args)
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            if(settings.needs_ready):
                writePipe(ext_pipe, "Status: %s" % status)
                ok(ext_pipe)
                detach_rd_markers()

        elif(cmd == "detachdebugger"):
            detach_all()
            print("Detached")
            ok(ext_pipe)

        elif(cmd == "trace2"):
            print args
            filee, pid = args.split(' ')
            print("Tracing %s" % (filee))
            
            take_a_trace2("%s" % pid)

            settings.runner(filee)
            ok(ext_pipe)
            return
            

            loop_debuggers(invocation_args=filee)
            while(process_status_queue(["WS"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            print "About to release debugger"

            print "here2"
            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            while(process_status_queue(["WE", "CR"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            if(settings.needs_ready):
                attach_rd_markers()
            # time for reaction to test end
            start_debuggers("Closing execution")
            readPipe()
            stop_debuggers("Closing execution finished")

            print "here3"
#            loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    print "waiting for RD"
                    loop_debuggers(settings.wait_sleep)
                    print "got sth"
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(args)
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            if(settings.needs_ready):
                writePipe(ext_pipe, "Status: %s" % status)
                ok(ext_pipe)
                detach_rd_markers()

### walk a file

        elif(cmd == "walk"):
            print "here1"
            filee, depth, gf_file = args.split(" ")
            depth = int(depth)
            print("Walking %s for %d levels, generating graext_pipe into %s" % (filee, depth, gf_file))
            
            take_a_walk("%s %s" % (depth, gf_file))
            time.sleep(2)

            loop_debuggers(invocation_args=filee)
            while(process_status_queue(["WS"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            print "here2"
            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            while(process_status_queue(["WE", "CR"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            if(settings.needs_ready):
                attach_rd_markers()
            # time for reaction to test end
            start_debuggers("Closing execution")
            readPipe()
            stop_debuggers("Closing execution finished")

            print "here3"
#            loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    print "waiting for RD"
                    loop_debuggers(settings.wait_sleep)
                    print "got sth"
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(args)
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            if(settings.needs_ready):
                writePipe(ext_pipe, "Status: %s" % status)
                ok(ext_pipe)
                detach_rd_markers()

### walk a file from new addr

        elif(cmd == "walk2"):
            filee, depth, gf_file, addr = args.split(" ")
            depth = int(depth)
            print("Walking %s for %d levels, generating graext_pipe into %s" % (filee, depth, gf_file))
            
            take_a_walk2("%s %s %s" % (depth, gf_file, addr))
            time.sleep(2)

            loop_debuggers(invocation_args=filee)
            while(process_status_queue(["WS"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            while(process_status_queue(["WE", "CR"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            if(settings.needs_ready):
                attach_rd_markers()
            # time for reaction to test end
            start_debuggers("Closing execution")
            readPipe()
            stop_debuggers("Closing execution finished")

#            loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    print "waiting for RD"
                    loop_debuggers(settings.wait_sleep)
                    print "got sth"
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(args)
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            if(settings.needs_ready):
                writePipe(ext_pipe, "Status: %s" % status)
                ok(ext_pipe)
                detach_rd_markers()

        elif(cmd == "walk3"):
            filee, depth, gf_file, addr = args.split(" ")
            depth = int(depth)
            print("Walking %s for %d levels, generating graext_pipe into %s" % (filee, depth, gf_file))
            
            take_a_walk2("%s %s %s" % (depth, gf_file, addr))
            time.sleep(2)

            loop_debuggers(invocation_args=filee)
            while(process_status_queue(["WS"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            while(process_status_queue(["WE"]) != True):
                loop_debuggers()
            if(status == "CR"):
                # CR before ST
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)

            if(settings.needs_ready):
                attach_rd_markers()
            # time for reaction to test end
            start_debuggers("Closing execution")
            readPipe()
            stop_debuggers("Closing execution finished")

#            loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    loop_debuggers(settings.wait_sleep)
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                get_ea()
                test_bin_dir()
                save_synopsis(args)
                save_sample(args)
                writePipe(ext_pipe, "Status: CR")
                ok(ext_pipe)
                return

            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)
            detach_rd_markers()

### other

        elif(cmd == "observe"):
#            dlog("In observe")
            stop_debuggers()
            attach_st_markers()
            attach_end_markers()
            loop_debuggers(invocation = "powershell -command \"& { invoke-expression d:\\samples\\shared\\%s }\"" % args)
            # waiting for ST
            attach_bp_handler()
            attach_ss_handler()
            dump_modules()
            dump_threads()
            track_all_threads()
            start_tracking_all_threads()
            loop_debuggers()
            #waiting for END
            stop_tracking_all_threads()

        elif(cmd == "testReactMarkers"):
#            dlog("In testReactMarkers", 2)
            attach_st_markers()
            loop_debuggers(invocation = "powershell -command \"& { invoke-expression d:\\samples\\shared\\%s }\"" % args)
            detach_st_markers()
            attach_end_markers()
            attach_react_markers()
            status == ""
            while(status != "MA"):
                loop_debuggers()
                status = status.get()[1]
                if(status == "SR"):
                    writePipe(ext_pipe, "Status: SR\n Script: %s\n" % reqScript)
                    ok(ext_pipe)
#                    dlog("Verified SR marker")
                # react
            detach_react_markers()
            detach_end_markers()
            ok(ext_pipe)

        elif(cmd == "testStEndMarkers"):
#            dlog("In testStEndMarkers", 2)

            # ST markers
            attach_st_markers()
            loop_debuggers(invocation = "powershell -command \"& { invoke-expression d:\\samples\\shared\\%s }\"" % args)
            while(process_status_queue(["ST"]) != True):
                loop_debuggers()
#            writePipe(ext_pipe, "Verified ST marker\n")
#            dlog("Verified ST marker")
            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)
            detach_st_markers()

            # END markers
            attach_end_markers()
            loop_debuggers(settings.wait_sleep)
            while(process_status_queue(["MA", "CR", "TO"]) != True):
                loop_debuggers(settings.wait_sleep)
#            dlog("Verified END marker")
            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)
            detach_end_markers()


            attach_rd_markers()
            loop_debuggers()
            while(process_status_queue(["RD"]) != True):
                loop_debuggers()
            writePipe(ext_pipe, "Status: %s" % status)
            ok(ext_pipe)
#            dlog("Verified RD marker")
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
            global log_file

#            if(len(cmds) > 1):
#                startLog(cmds[1])
#            else:
#                startLog(log_file)
            startLog(args)
            start_log("%s" % (args))
            ok(ext_pipe)

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStop"):
            stopLog()
            stop_log()
            ok(ext_pipe)
    
        elif(cmd == "ps"):
            for (ppid, pid, name) in enumerate_processes_custom():
                writePipe(ext_pipe, "0x%x 0x%x %s\n" % (ppid, pid, name))
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

        elif(cmd == "setupWaitSleep"):
            writePipe(ext_pipe, "Setting wait sleep to: {0}\n".format(cmds[1]))
            settings.wait_sleep = float(cmds[1])
            ok(ext_pipe)
    
        elif(cmd == "setupSlowdown"):
            writePipe(ext_pipe, "Setting slowdown to: {0}\n".format(cmds[1]))
            settings.slowdown = float(cmds[1])
            ok(ext_pipe)
    
        elif(cmd == "setupMarkers"):
            writePipe(ext_pipe, "Setting up markers\n")
#            settings.ma_addrs += resolve_rvas(dbg, settings.ma_rvas)
#            settings.ma_st_addrs += resolve_rvas(dbg, settings.ma_st_rvas)
#            settings.ma_end_addrs += resolve_rvas(dbg, settings.ma_end_rvas)
            # will do automatically on process attach
#            writePipe(ext_pipe, "Marker list: {0}\n".format(markers))
#            writePipe(ext_pipe, "ST marker list: {0}\n".format(st_markers))
#            writePipe(ext_pipe, "END marker list: {0}\n".format(end_markers))
#            writePipe(ext_pipe, "REACT marker list: {0}\n".format(end_markers))
            ok(ext_pipe)

        elif(cmd == "runFile"):
            settings.runner(args)
            ok(ext_pipe)

        elif(cmd == "binTest"):
#            dlog("binTest")
            writePipe(ext_pipe, "Communication with binner is working")
            ok(ext_pipe)
        
        elif(cmd == "checkReady"):
            writePipe(ext_pipe, "Status: RD")
            ok(ext_pipe)

        elif(cmd == "spawn"):
            print("Spawning: %s" % args)
            spawn(args)
            writePipe(ext_pipe, "OK")
            ok(ext_pipe)

        elif(cmd == "startBinner"):
            writePipe(ext_pipe, "OK")
            ok(ext_pipe)

        elif(cmd == "kill"):
            print("Killing %s" % args)
            call("taskkill /F /IM %s" % args)
            writePipe(ext_pipe, "kill OK")
            ok(ext_pipe)

        elif(cmd == "killExplorer"):
            call("taskkill /F /IM explorer.exe")
            writePipe(log_pipe, "Killed explorer")
            writePipe(ext_pipe, "killExplorer OK")
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
            log_pipe.write("quit")

### tracer info
        elif(cmd == "tc_avtivate_prev"):
            trace_controller.activate_prev_tracer()
            writePipe(ext_pipe, "tc_avtivate_prev OK")
            ok(ext_pipe)

        elif(cmd == "tc_avtivate_next"):
            trace_controller.activate_next_tracer()
            writePipe(ext_pipe, "tc_avtivate_next OK")
            ok(ext_pipe)

        elif(cmd == "tracer_configure_sample_file"):
            trace_controller.set_sample_file(args)
            writePipe(ext_pipe, "tracer_configure_sample_file OK")
            ok(ext_pipe)

        elif(cmd == "tracer_configure_research_dir"):
            trace_controller.set_research_dir(args)
            writePipe(ext_pipe, "tracer_configure_research_dir OK")
            ok(ext_pipe)

        elif(cmd == "tracer_configure_sample_pname"):
            trace_controller.set_sample_pname(args)
            writePipe(ext_pipe, "tracer_configure_sample_pname OK")
            ok(ext_pipe)

        elif(cmd == "tracer_configure_marker_st"):
            mod, addr = args.split(" ")
            trace_controller.configure_marker_st(mod, addr)
            writePipe(ext_pipe, "tracer_configure_marker_st OK")
            ok(ext_pipe)

        elif(cmd == "tracer_configure_marker_end"):
            mod, addr = args.split(" ")
            trace_controller.configure_marker_end(mod, addr)
            writePipe(ext_pipe, "tracer_configure_marker_end OK")
            ok(ext_pipe)

        elif(cmd == "tracer_debug_sample"):
            trace_controller.debug_sample()
            writePipe(ext_pipe, "tracer_debug_sample OK")
            ok(ext_pipe)

        elif(cmd == "tracer_debug_continue"):
            trace_controller.debug_continue()
            writePipe(ext_pipe, "Got report: %s" % trace_controller.last_report)
            writePipe(ext_pipe, "tracer_debug_continue OK")
            ok(ext_pipe)

        elif(cmd == "tracer_debug_continue_time"):
            trace_controller.debug_continue_time(args)
            writePipe(ext_pipe, "Got report: %s" % trace_controller.last_report)
            writePipe(ext_pipe, "tracer_debug_continue_time OK")
            ok(ext_pipe)

        elif(cmd == "tracer_list_tebs"):
            trace_controller.list_tebs()
            trace_controller.dlog("Writing to ext pipe: %s" % trace_controller.last_answer)
            writePipe(ext_pipe, "%s" % trace_controller.last_answer)
            writePipe(ext_pipe, "tracer_list_tebs OK")
            ok(ext_pipe)

        elif(cmd == "tracer_print"):
            trace_controller.print_sth(args)
            writePipe(ext_pipe, "tracer_print OK")
            ok(ext_pipe)

        elif(cmd == "spawn_tracer"):
            new_tracer = trace_controller.spawn_tracer()
            writePipe(ext_pipe, "Started: %d" % new_tracer);
            writePipe(log_pipe, "Started: %d" % new_tracer);
            writePipe(ext_pipe, "spawn_tracer OK");
            ok(ext_pipe)

        elif(cmd == "close_tracer"):
            trace_controller.close_tracer(args)
            writePipe(ext_pipe, "close_tracer OK")
            ok(ext_pipe)

        elif(cmd == "start_trace_controller"):
            trace_controller = TraceController.TraceController(ext_pipe)
            writePipe(ext_pipe, "start_trace_controller OK")
            ok(ext_pipe)

#    except Exception, e:
#        print(e)
#        while True:
#            pass
#        ok(ext_pipe)

### main loop


def internal_routine():
    global ext_pipe
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
    print(logo)
    ext_pipe = SerialWrap(0)
    log_pipe = SerialWrap(1)
    # signal ext_pipe connection
    writePipe(ext_pipe, 'Ext_pipe connected')
    ok(ext_pipe)
    # signal log_pipe connection
    writePipe(log_pipe, 'Log_pipe connected')
    ok(log_pipe)
    import sys
    sys.stdout = log_pipe
    sys.stderr = log_pipe

    while True:
        cmd = readPipe(ext_pipe)
        cmds = cmd.split(" ")
        execute(cmds)
        log_pipe.write("CMD executed")
        if(cmd == "quit"):
            break

import cProfile, pstats, StringIO
pr = cProfile.Profile()
pr.enable()
f = open("e:\\logs\\stats-binner", "a", 0)

if __name__ == '__main__':
    internal_routine()
#    profile.run("binner_routine()", "e:\\logs\\stats-binner")

pr.disable()
s = StringIO.StringIO()
sortby = 'cumulative'
ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
f.write("test")
f.close()

