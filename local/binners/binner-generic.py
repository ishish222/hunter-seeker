import sys

sys.path.append("e:\\server\\paimei")
sys.path.append("e:\\common")

import serial
import win32pipe, win32file, win32gui
import time
import binner
import settings
from functions import *
#from socket import socket, AF_INET, SOCK_STREAM
import socket
from subprocess import call, Popen
from glob import glob


PIPE_NAME = "\\\\.\\pipe\\control"
PIPE_BUFF_SIZE = 4096

if(defined("settings.log_file")):
    log_file = settings.log_file
else:
    log_file = "e:\\logs\\log-"

def getPipe(name):
#    dlog("In getPipe", 2)
    ph = win32file.CreateFile(name, win32file.GENERIC_READ | win32file.GENERIC_WRITE | win32pipe.PIPE_TYPE_MESSAGE, 0, None, win32file.OPEN_EXISTING, 0, None)
    return ph

def readPipe():
    global ph
    data = ""
    while True:
#        data += ph.recv(1)
        data += ph.read(1)
        
        if(data[-6:] == "-=OK=-"): 
            break
    return data[:-6]

def writePipe(data):
    global ph
#    ph.send(data + "-=OK=-")
    ph.write(data + "-=OK=-")

### TODO: trzeba dopracowac!

def handle_crash():
#    global main_binner
#    global crash_bin
#    global crash_reason

#    e_addr = main_binner.crash_bin.last_crash.exception_address

    #create dossier
#    if(reason == "hc"):
#        crash_reason = "hc"
#        writePipe("Probable crash reason: hc (crash info needs reinterpretation)\n")
#        writePipe("")
#    if((disasm(dbg, e_addr) == "[INVALID]") or (get_module(dbg, e_addr) == "[INVALID]")):
#        crash_reason = "uaf"
#        main_binner.writePipe("Probable crash reason: uaf\n")
#        main_binner.writePipe("")
#    else:
#        crash_reason = "unk"
#
#    print("5")
#    main_binner.writePipe(crash_bin.crash_synopsis())
#    main_binner.get_synopsis()
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
    logf.write(data)

def verify():
#    dlog("Please verify process")
    time.sleep(5)

def process_status_queue(satisfying = None):
    global main_binner
    global status

    status = ""
    satisfied = False
    
    while(not main_binner.status.empty()):
        main_binner.dlog("Queue size before get: %d" % main_binner.status.qsize())
        item = main_binner.status.get()
        main_binner.dlog("Queue size after get : %d" % main_binner.status.qsize())
        status = item[1]
        main_binner.dlog("Processing %s" % status)
        if(status == "TO"):
            if(main_binner.prev_status == "MA"):
                status = "PTO"
            if(main_binner.prev_status == "TO"):
                status = "PTO"
            if(main_binner.prev_status == "RD"):
                status = "PTO"
        if(status == "EX"):
            main_binner.writePipe("Status: %s\n" % status)
            main_binner.writePipe("Encountered exception: %s\n" % item[2])
            main_binner.ok()
        if(status == "SR"):
            main_binner.dlog("Requested script: %s" % item[2])
            main_binner.writePipe("Status: %s\n" % status)
            main_binner.writePipe("Script: %s\n" % item[2])
            main_binner.ok()
            # time for reaction to SR
            main_binner.start_debuggers("Script execution")
            main_binner.readPipe()
            main_binner.stop_debuggers("Script execution finished") #could add statuses into queue
            main_binner.dlog("Queue size after script: %d" % main_binner.status.qsize())
            continue

#        print(status)
        if(satisfying != None):
            if(status in satisfying):
                print("Status %s is satisfying" % status)
                satisfied = True
            else:
                print("Status %s is not satisfying" % status)
                print("Satisfying statuses:")
                print(satisfying)
        main_binner.prev_status = status

    return satisfied

def window_check(hwnd, lParam):
    global window_name
    global found

    writePipe(win32gui.GetWindowText(hwnd))
    if(win32gui.GetWindowText(hwnd).find(window_name) != -1):
        found = True

### binner commands
def execute(cmds):
    global main_binner
    global status

    cmd = cmds[0]
    args = " ".join(cmds[1:])

#    try:
    if(True):
        if(cmd == "settle"):
            main_binner.status = ""
    
            watchThread = Thread(target = watchThread_routine, args=(float(cmds[1]),))
            watchThread.start()
            main_binner.start_debuggers()
            main_binner.ok()

        if(cmd == "windowExists"):
            global window_name
            global found

            found = False
            window_name = args

            win32gui.EnumWindows(window_check, None)

            if(found == True):
                main_binner.writePipe("FOUND")
            else:
                main_binner.writePipe("NOT FOUND")
            main_binner.ok()


        
        if(cmd == "startCPUMon"):
            from functions import QueryCPUUsageThread
            global usage
            usage = QueryCPUUsageThread()
#            usage.start()


        if(cmd == "stopCPUMon"):
            global usage
            if usage == None:
                return
            else:
                usage.stop()

        if(cmd == "getCPUUsage"):
            print "1"
            from functions import getCPU
            print "2"
            val = getCPU()
            print "3"
            main_binner.writePipe("Usage: %d" % val)
            main_binner.ok()
            print "4"
               

        if(cmd == "cooldown"):
            pass

        elif(cmd == "testAll"):
            for sample in glob("e:\\samples\\shared\\*.*"):
                main_binner.attach_st_markers()
                main_binner.loop_debuggers(invocation = "powershell -command \"& { invoke-expression %s }\"" % sample)
                while(process_status_queue(["ST", "CR", "TO", "CR"]) != True):
                    main_binner.loop_debuggers()
                if(status == "TO"):
                    print("ST TO")
                    continue
                if(status == "CR"):
                    print("CRrrrrrrrrrr")
                    break
    
                print("Status: %s" % status)
    
                main_binner.detach_st_markers()
                main_binner.attach_end_markers()
    
    #            process_status_queue()
    #            main_binner.loop_debuggers(settings.wait_sleep)
                while(process_status_queue(["MA", "CR", "TO"]) != True):
                    main_binner.loop_debuggers(settings.wait_sleep)
                if(status == "CR"):
                    print("CRrrrrrrrrrr")
                    break
    
                print("Status: %s" % status)

                main_binner.detach_end_markers()
                main_binner.attach_rd_markers()
                # time for reaction to test end
                main_binner.start_debuggers("Closing execution")
                main_binner.readPipe()
                main_binner.stop_debuggers("Closing execution finished")
    
    #            main_binner.loop_debuggers()
                if(settings.needs_ready):
                    while(process_status_queue(["RD", "CR", "TO"]) != True):
                        main_binner.loop_debuggers()
                    if(status == "TO"):
                        print("RD TO")
                        continue
                    if(status == "CR"):
                        print("CRrrrrrrrrrr")
                        break
    
            main_binner.writePipe("Done")
            main_binner.ok()
            main_binner.detach_rd_markers()

### test a file

        elif(cmd == "testFile"):
            main_binner.attach_st_markers()

            main_binner.loop_debuggers(invocation_args=args)
            while(process_status_queue(["ST", "CR", "PTO"]) != True):
                main_binner.loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
#                dlog("Processing CR1")
                main_binner.get_ea()
                main_binner.test_bin_dir()
                main_binner.save_synopsis(args)
                main_binner.save_sample(args)
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            main_binner.detach_st_markers()
            main_binner.attach_end_markers()

            while(process_status_queue(["MA", "CR", "TO", "PTO"]) != True):
                main_binner.loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
#                dlog("Processing CR2")
                main_binner.get_ea()
                main_binner.test_bin_dir()
                main_binner.save_synopsis(args)
                main_binner.save_sample(args)
#                dlog("Hereeeeeee")
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            main_binner.detach_end_markers()
            if(settings.needs_ready):
                main_binner.attach_rd_markers()
            # time for reaction to test end
            main_binner.start_debuggers("Closing execution")
            main_binner.readPipe()
            main_binner.stop_debuggers("Closing execution finished")

#            main_binner.loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    main_binner.loop_debuggers(settings.wait_sleep)
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                main_binner.get_ea()
                main_binner.test_bin_dir()
                main_binner.save_synopsis(args)
                main_binner.save_sample(args)
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
            main_binner.detach_rd_markers()

### test a directory

        elif(cmd == "testDir"):
            args2 = args.split("\"")
            cmds[1] = args2[1]
            cmds[2] = args2[3]
            print("0: %s\n1: %s\n2: %s\n" % (cmds[0], cmds[1], cmds[2]))
            real_target = cmds[2]
            main_binner.attach_st_markers()

            main_binner.loop_debuggers(invocation_args=cmds[2])
            while(process_status_queue(["ST", "CR", "PTO"]) != True):
                main_binner.loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
#                dlog("Processing CR1")
                main_binner.get_ea()
                main_binner.test_bin_dir()
                main_binner.save_synopsis(args)
                main_binner.save_sample(cmds[1])
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            main_binner.detach_st_markers()
            main_binner.attach_end_markers()

            while(process_status_queue(["MA", "CR", "TO", "PTO"]) != True):
                main_binner.loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
#                dlog("Processing CR2")
                main_binner.get_ea()
                main_binner.test_bin_dir()
                main_binner.save_synopsis(args)
                main_binner.save_sample(cmds[1])
#                dlog("Hereeeeeee")
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            main_binner.detach_end_markers()
            if(settings.needs_ready):
                main_binner.attach_rd_markers()
            # time for reaction to test end
            main_binner.start_debuggers("Closing execution")
            main_binner.readPipe()
            main_binner.stop_debuggers("Closing execution finished")

#            main_binner.loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    main_binner.loop_debuggers(settings.wait_sleep)
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                main_binner.get_ea()
                main_binner.test_bin_dir()
                main_binner.save_synopsis(args)
                main_binner.save_sample(cmds[1])
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
            main_binner.detach_rd_markers()

### walk a file

        elif(cmd == "walk"):
            filee, depth, gf_file = args.split(" ")
            depth = int(depth)
            print("Walking %s for %d levels, generating graph into %s" % (filee, depth, gf_file))
            
            main_binner.take_a_walk("%s %s" % (depth, gf_file))
            time.sleep(2)

            main_binner.loop_debuggers(invocation_args=filee)
            while(process_status_queue(["WS"]) != True):
                main_binner.loop_debuggers()
            if(status == "CR"):
                # CR before ST
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            while(process_status_queue(["WE"]) != True):
                main_binner.loop_debuggers()
            if(status == "CR"):
                # CR before ST
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            if(settings.needs_ready):
                main_binner.attach_rd_markers()
            # time for reaction to test end
            main_binner.start_debuggers("Closing execution")
            main_binner.readPipe()
            main_binner.stop_debuggers("Closing execution finished")

#            main_binner.loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    main_binner.loop_debuggers(settings.wait_sleep)
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                main_binner.get_ea()
                main_binner.test_bin_dir()
                main_binner.save_synopsis(args)
                main_binner.save_sample(args)
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
            main_binner.detach_rd_markers()

### walk a file from new addr

        elif(cmd == "walk2"):
            filee, depth, gf_file, addr = args.split(" ")
            depth = int(depth)
            print("Walking %s for %d levels, generating graph into %s" % (filee, depth, gf_file))
            
            main_binner.take_a_walk2("%s %s %s" % (depth, gf_file, addr))
            time.sleep(2)

            main_binner.loop_debuggers(invocation_args=filee)
            while(process_status_queue(["WS"]) != True):
                main_binner.loop_debuggers()
            if(status == "CR"):
                # CR before ST
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            while(process_status_queue(["WE"]) != True):
                main_binner.loop_debuggers()
            if(status == "CR"):
                # CR before ST
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            if(settings.needs_ready):
                main_binner.attach_rd_markers()
            # time for reaction to test end
            main_binner.start_debuggers("Closing execution")
            main_binner.readPipe()
            main_binner.stop_debuggers("Closing execution finished")

#            main_binner.loop_debuggers()
            if(settings.needs_ready):
                while(process_status_queue(["RD", "CR", "PTO"]) != True):
                    main_binner.loop_debuggers(settings.wait_sleep)
            else:
                process_status_queue(["RD", "CR", "PTO"])
                if(status != "CR"): status = "RD"

            if(status == "CR"):
#                dlog("Processing CR3")
                main_binner.get_ea()
                main_binner.test_bin_dir()
                main_binner.save_synopsis(args)
                main_binner.save_sample(args)
                main_binner.writePipe("Status: CR")
                main_binner.ok()
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
            main_binner.detach_rd_markers()

### other

        elif(cmd == "observe"):
#            dlog("In observe")
            main_binner.stop_debuggers()
            main_binner.attach_st_markers()
            main_binner.attach_end_markers()
            main_binner.loop_debuggers(invocation = "powershell -command \"& { invoke-expression d:\\samples\\shared\\%s }\"" % args)
            # waiting for ST
            main_binner.attach_bp_handler()
            main_binner.attach_ss_handler()
            main_binner.dump_modules()
            main_binner.dump_threads()
            main_binner.track_all_threads()
            main_binner.start_tracking_all_threads()
            main_binner.loop_debuggers()
            #waiting for END
            main_binner.stop_tracking_all_threads()

        elif(cmd == "testReactMarkers"):
#            dlog("In testReactMarkers", 2)
            main_binner.attach_st_markers()
            main_binner.loop_debuggers(invocation = "powershell -command \"& { invoke-expression d:\\samples\\shared\\%s }\"" % args)
            main_binner.detach_st_markers()
            main_binner.attach_end_markers()
            main_binner.attach_react_markers()
            status == ""
            while(status != "MA"):
                main_binner.loop_debuggers()
                status = main_binner.status.get()[1]
                if(status == "SR"):
                    main_binner.writePipe("Status: SR\n Script: %s\n" % main_binner.reqScript)
                    main_binner.ok()
#                    dlog("Verified SR marker")
                # react
            main_binner.detach_react_markers()
            main_binner.detach_end_markers()
            main_binner.ok()

        elif(cmd == "testStEndMarkers"):
#            dlog("In testStEndMarkers", 2)

            # ST markers
            main_binner.attach_st_markers()
            main_binner.loop_debuggers(invocation = "powershell -command \"& { invoke-expression d:\\samples\\shared\\%s }\"" % args)
            while(process_status_queue(["ST"]) != True):
                main_binner.loop_debuggers()
#            main_binner.writePipe("Verified ST marker\n")
#            dlog("Verified ST marker")
            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
            main_binner.detach_st_markers()

            # END markers
            main_binner.attach_end_markers()
            main_binner.loop_debuggers(settings.wait_sleep)
            while(process_status_queue(["MA", "CR", "TO"]) != True):
                main_binner.loop_debuggers(settings.wait_sleep)
#            dlog("Verified END marker")
            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
            main_binner.detach_end_markers()


            main_binner.attach_rd_markers()
            main_binner.loop_debuggers()
            while(process_status_queue(["RD"]) != True):
                main_binner.loop_debuggers()
            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
#            dlog("Verified RD marker")
            main_binner.detach_rd_markers()

        elif(cmd == "getSynopsis"):
#            dlog("In getSynopsis", 2)
            main_binner.writePipe(main_binner.last_data)
            main_binner.ok()
#            main_binner.get_synopsis()
#            main_binner.ok()

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStart"):
            global log_file

#            if(len(cmds) > 1):
#                startLog(cmds[1])
#            else:
#                startLog(log_file)
            startLog(args)
            main_binner.start_log("%s" % (args))
            main_binner.ok()

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStop"):
            stopLog()
            main_binner.stop_log()
            main_binner.ok()
    
        elif(cmd == "ps"):
            for (ppid, pid, name) in main_binner.enumerate_processes_custom():
                main_binner.writePipe("0x%x 0x%x %s\n" % (ppid, pid, name))
            main_binner.ok()

        elif(cmd == "listTebs"):
            main_binner.list_tebs()
            main_binner.ok()

        elif(cmd == "cbEip"):
            main_binner.writePipe("0x%x" % main_binner.crash_bin.last_crash.exception_address)
            main_binner.ok()

        elif(cmd == "cbCrashSynopsis"):
            main_binner.writePipe(main_binner.crash_bin.crash_synopsis())
            main_binner.ok()
    
        elif(cmd == "cbReason"):
            main_binner.writePipe(main_binner.crash_reason)
            main_binner.ok()
    
        elif(cmd == "cbStackUnwind"):
            for call_frame in main_binner.crash_bin.last_crash.stack_unwind:
                main_binner.writePipe(call_frame + "\n")
            main_binner.ok()

        # TODO: sprawdz, czy dziala!
        elif(cmd == "attachBinner"):
            imagename = args
            for (pid, name) in main_binner.enumerate_processes():
                if imagename in name:
#                    try:
                    if True:
#                        main_binner.dlog("[*] Attaching to %s (%d)" % (name, pid))
                        main_binner.attach(pid)
#                    except Exception, e:
#                        dlog("[!] Problem attaching to %s" % name)
#                        dlog(e)
                    continue

            main_binner.writePipe("Attached to " + str(args))
            main_binner.ok()

        elif(cmd == "killHost"):
            main_binner.terminate_processes()
            main_binner.ok()
    
        elif(cmd == "installHandlers"):
            main_binner.attach_react_markers()
            main_binner.attach_ex_handler()
            main_binner.attach_av_handler()
            main_binner.ok()

        elif(cmd == "installMarkerAddrs"):
#            for ma_addr in settings.ma_addrs:
#                main_binner.writePipe("Installing bad at " + cmds[1])
#                dbg.bp_set(ma_addr, handler = bad_handler)
            # will do automatically on process attach
            main_binner.ok()

        elif(cmd == "setupWaitSleep"):
            main_binner.writePipe("Setting wait sleep to: {0}\n".format(cmds[1]))
            settings.wait_sleep = float(cmds[1])
            main_binner.ok()
    
        elif(cmd == "setupSlowdown"):
            main_binner.writePipe("Setting slowdown to: {0}\n".format(cmds[1]))
            settings.slowdown = float(cmds[1])
            main_binner.ok()
    
        elif(cmd == "setupMarkers"):
            main_binner.writePipe("Setting up markers\n")
#            settings.ma_addrs += resolve_rvas(dbg, settings.ma_rvas)
#            settings.ma_st_addrs += resolve_rvas(dbg, settings.ma_st_rvas)
#            settings.ma_end_addrs += resolve_rvas(dbg, settings.ma_end_rvas)
            # will do automatically on process attach
#            main_binner.writePipe("Marker list: {0}\n".format(main_binner.markers))
#            main_binner.writePipe("ST marker list: {0}\n".format(main_binner.st_markers))
#            main_binner.writePipe("END marker list: {0}\n".format(main_binner.end_markers))
#            main_binner.writePipe("REACT marker list: {0}\n".format(main_binner.end_markers))
            main_binner.ok()

        elif(cmd == "binTest"):
#            dlog("binTest")
            main_binner.writePipe("Communication with binner is working")
            main_binner.ok()
        
        elif(cmd == "checkReady"):
            main_binner.writePipe("Status: RD")
            main_binner.ok()

        elif(cmd == "spawn"):
            print("Spawning: %s" % args)
            main_binner.spawn(args)
            main_binner.writePipe("OK")
            main_binner.ok()

        elif(cmd == "startBinner"):
            main_binner.writePipe("OK")
            main_binner.ok()

        elif(cmd == "kill"):
            print("Killing %s" % args)
            call("taskkill /F /IM %s" % args)
            main_binner.writePipe("OK")
            main_binner.ok()

        elif(cmd == "killExplorer"):
            print("Killing explorer")
            call("taskkill /F /IM explorer.exe")
            main_binner.writePipe("OK")
            main_binner.ok()

        elif(cmd == "release"):
            main_binner.writePipe("Releasing")
            main_binner.ok()
            main_binner.start_debuggers()

        elif(cmd == "start_profiling"):
            main_binner.start_profiling()

        elif(cmd == "dump_stats"):
            main_binner.dump_stats(args)

        elif(cmd == "quit"):
            main_binner.close_logs()

#    except Exception, e:
#        print(e)
#        while True:
#            pass
#        main_binner.ok()

### main loop

def binner_routine():
    global ph
    global main_binner

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

    # binner will self-configure based on settings
    #ph = getPipe(PIPE_NAME)

#    ph = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ph = serial.Serial(0)
#    s.bind(("0.0.0.0", 12345))
#    s.listen(3)
#    dlog("Trying to connect...", 2)
#    ph.connect(("10.0.2.100", 12345))
#    ph, addr = s.accept()
#    dlog("Got connection", 2)
#    ph.send("python binner")
    ph.write("python binner")

    main_binner = binner.binner(ph)
    main_binner.ok()

    while True:
        cmd = readPipe()
        cmds = cmd.split(" ")
        execute(cmds)
        if(cmd == "quit"):
            break

import cProfile, pstats, StringIO
pr = cProfile.Profile()
pr.enable()
f = open("e:\\logs\\stats-binner", "a", 0)

if __name__ == '__main__':
    binner_routine()
#    profile.run("binner_routine()", "e:\\logs\\stats-binner")

pr.disable()
s = StringIO.StringIO()
sortby = 'cumulative'
ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
f.write("test")
f.close()

