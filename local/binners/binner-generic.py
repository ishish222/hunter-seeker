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

def process_reactions():
    global main_binner
    global status

    status = ""
    for i in range(0, main_binner.status.qsize()):
#    while(not main_binner.status.empty()):
#        main_binner.dlog("Queue size before get: %d" % main_binner.status.qsize())
        item = main_binner.status.get()
#        main_binner.dlog("Queue size after get : %d" % main_binner.status.qsize())
        status = item[1]
#        main_binner.dlog("Processing %s" % status)
#        if(status == ""):
#            status = "TO" # TO is overriden by all
        if(status == "SR"):
#            main_binner.dlog("Requested script: %s" % item[2])
            main_binner.writePipe("Status: %s\n" % status)
            main_binner.writePipe("Script: %s\n" % item[2])
            main_binner.ok()
            # time for reaction to SR
            main_binner.start_debuggers("Script execution")
            main_binner.readPipe()
            main_binner.stop_debuggers("Script execution finished")
#            main_binner.dlog("Queue size after script: %d" % main_binner.status.qsize())
            continue
        else:
            # put in back!
            main_binner.status.put(item)
            continue


def process_status_queue(satisfactory = None):
    global main_binner
    global status

    status = ""
    while(not main_binner.status.empty()):
#        main_binner.dlog("Queue size before get: %d" % main_binner.status.qsize())
        item = main_binner.status.get()
#        main_binner.dlog("Queue size after get : %d" % main_binner.status.qsize())
        status = item[1]
#        main_binner.dlog("Processing %s" % status)
        if(status == "TO"):
            if(main_binner.prev_status == "MA"):
                status = "RDTO"
            if(main_binner.prev_status == "TO"):
                status = "RDTO"
            if(main_binner.prev_status == "RD"):
                status = "STTO"
        if(status == "SR"):
#            main_binner.dlog("Requested script: %s" % item[2])
            main_binner.writePipe("Status: %s\n" % status)
            main_binner.writePipe("Script: %s\n" % item[2])
            main_binner.ok()
            # time for reaction to SR
            main_binner.start_debuggers("Script execution")
            main_binner.readPipe()
            main_binner.stop_debuggers("Script execution finished")
#            main_binner.dlog("Queue size after script: %d" % main_binner.status.qsize())
            continue
        elif(status == "CR"):
            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
#            print(main_binner.last_data)
            #main_binner.stop_debuggers()
            #handle_crash()
        print(status)
        if(satisfactory != None):
            if(status in satisfactory):
                return True
        main_binner.prev_status = status
    return False


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


        elif(cmd == "testAll"):
            for sample in glob("e:\\samples\\shared\\*.*"):
                main_binner.attach_st_markers()
                main_binner.loop_debuggers(invocation = "powershell -command \"& { invoke-expression %s }\"" % sample)
                while(process_status_queue(["ST", "CR", "TO"]) != True):
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
                #process_reactions()
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
                #process_reactions()
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

        elif(cmd == "testFile"):
            main_binner.attach_st_markers()

#            process_status_queue()
            #process_reactions()
            main_binner.loop_debuggers(invocation = "powershell -command \"& { invoke-expression e:\\samples\\shared\\%s }\"" % args)
            while(process_status_queue(["ST", "CR", "STTO"]) != True):
                main_binner.loop_debuggers(settings.wait_sleep)
#            if(status == "TO"):
#                status = "STTO"
            if(status == "CR"):
                print("CRrrrrrrrrrr")
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            main_binner.detach_st_markers()
            main_binner.attach_end_markers()

#            process_status_queue()
#            main_binner.loop_debuggers(settings.wait_sleep)
            #process_reactions()
            while(process_status_queue(["MA", "CR", "TO"]) != True):
                main_binner.loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
                print("CRrrrrrrrrrr")
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

            main_binner.detach_end_markers()
            main_binner.attach_rd_markers()
            # time for reaction to test end
            main_binner.start_debuggers("Closing execution")
            main_binner.readPipe()
            main_binner.stop_debuggers("Closing execution finished")

#            main_binner.loop_debuggers()
            #process_reactions()
            while(process_status_queue(["RD", "CR", "RDTO"]) != True):
                main_binner.loop_debuggers(settings.wait_sleep)
            if(status == "CR"):
                print("CRrrrrrrrrrr")
                return

            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
            main_binner.detach_rd_markers()

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
f = open("e:\\logs\\stats-binner", "a")

if __name__ == '__main__':
    binner_routine()
#    profile.run("binner_routine()", "e:\\logs\\stats-binner")

pr.disable()
s = StringIO.StringIO()
sortby = 'cumulative'
ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
f.write("test")
f.close()

