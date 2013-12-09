import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

import win32pipe, win32file, win32gui
import time
import binner
import settings
from functions import *
#from socket import socket, AF_INET, SOCK_STREAM
import socket
from subprocess import call, Popen

PIPE_NAME = "\\\\.\\pipe\\control"
PIPE_BUFF_SIZE = 4096

if(defined("settings.log_file")):
    log_file = settings.log_file
else:
    log_file = "z:\\samples\\log-"

def getPipe(name):
    dlog("In getPipe", 1)
    ph = win32file.CreateFile(name, win32file.GENERIC_READ | win32file.GENERIC_WRITE | win32pipe.PIPE_TYPE_MESSAGE, 0, None, win32file.OPEN_EXISTING, 0, None)
    return ph

def readPipe():
    global ph
    data = ""
    while True:
        data += ph.recv(1)
        
        if(data[-6:] == "-=OK=-"): 
            break
    return data[:-6]

def writePipe(data):
    global ph
    ph.send(data + "-=OK=-")

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

def startLog():
    global logStarted
    global log_file
    global logf

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

def verify():
    dlog("Please verify process")
    time.sleep(5)

### binner commands
def execute(cmds):
    global main_binner

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


        elif(cmd == "testFile"):
            dlog("In waitTest", 1)
#            verify()
            main_binner.stop_debuggers()
            main_binner.attach_st_markers()

#            verify()
            status = ""
            # test is about to start, expecting ST, CR
            while(status != "ST"):
                main_binner.loop_debuggers(invocation = "powershell -command \"& { invoke-expression z:\\samples\\shared\\%s }\"" % args) 
                while(not main_binner.status.empty()):
                    status = main_binner.status.get()[1]
                    if(status == "ST"):
                        break
                    if(status == "SR"):
                        main_binner.dlog("Requested script: %s" % main_binner.reqScript)
                        main_binner.writePipe("Status: %s\n" % status)
                        main_binner.writePipe("Script: %s\n" % main_binner.reqScript)
                        main_binner.ok()
                        # time for reaction to SR
                        main_binner.start_debuggers()
                        main_binner.readPipe()
                        main_binner.stop_debuggers()
                    if(status == "CR"):
                        dlog("CRASH!!!!!!!!!!!!!!!!")
                        handle_crash()
                        main_binner.writePipe("Status: %s" % status)
                        main_binner.ok()
                        return

#            verify()

            # test has started, expecting MA, TO, CR, SR
            main_binner.detach_st_markers()
            main_binner.attach_end_markers()

#            verify()
            status = ""
            while(status == "SR" or status == ""):
                main_binner.loop_debuggers()
                # process status queue:
                while(not main_binner.status.empty()):
                    status = main_binner.status.get()[1]
                    if(status == "MA"):
                        break
                    if(status == ""):
                        status = "TO" # TO is overriden by all
                        break
                    if(status == "SR"):
                        main_binner.dlog("Requested script: %s" % main_binner.reqScript)
                        main_binner.writePipe("Status: %s\n" % status)
                        main_binner.writePipe("Script: %s\n" % main_binner.reqScript)
                        main_binner.ok()
                        # time for reaction to SR
                        main_binner.start_debuggers()
                        main_binner.readPipe()
                        main_binner.stop_debuggers()
                    if(status == "CR"):
                        dlog("CRASH!!!!!!!!!!!!!!!!")
                        handle_crash()
                        main_binner.writePipe("Status: %s" % status)
                        main_binner.ok()
                        return

#            verify()
            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()
            # time for reaction to test end
            main_binner.detach_end_markers()
            main_binner.attach_rd_markers()
            main_binner.start_debuggers()
            main_binner.readPipe()
            main_binner.stop_debuggers()

            # test has ended, expecting RD, SR
#            verify()
            status = ""
            while(status == "SR" or status == ""):
                main_binner.loop_debuggers()
                while(not main_binner.status.empty()):
                    status = main_binner.status.get()[1]
                    if(status == "RD"):
                        break
                    if(status == "SR"):
                        main_binner.dlog("Requested script: %s" % main_binner.reqScript)
                        main_binner.writePipe("Status: %s\n" % status)
                        main_binner.writePipe("Script: %s\n" % main_binner.reqScript)
                        main_binner.ok()
                        # time for reaction to SR
                        main_binner.start_debuggers()
                        main_binner.readPipe()
                        main_binner.stop_debuggers()
                    if(status == "CR"):
                        dlog("CRASH!!!!!!!!!!!!!!!!")
                        handle_crash()
                        main_binner.writePipe("Status: %s" % status)
                        main_binner.ok()
                        return

            main_binner.detach_rd_markers()
            main_binner.writePipe("Status: %s" % status)
            main_binner.ok()

        elif(cmd == "observe"):
            dlog("In observe")
            main_binner.stop_debuggers()
            main_binner.attach_st_markers()
            main_binner.attach_end_markers()
            main_binner.loop_debuggers(invocation = "powershell -command \"& { invoke-expression z:\\samples\\shared\\%s }\"" % args)
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
            dlog("In testReactMarkers", 1)
            main_binner.stop_debuggers()
            main_binner.attach_end_markers()
            main_binner.attach_react_markers()
            while(main_binner.status != "MA"):
                main_binner.loop_debuggers()
                if(main_binner.status == "SR"):
                    main_binner.writePipe("Status: SR\n Script: %s\n" % main_binner.reqScript)
                    main_binner.ok()
                # react
            main_binner.detach_react_markers()
            main_binner.ok()

        elif(cmd == "testStEndMarkers"):
            dlog("In testStEndMarkers", 1)
#            main_binner.stop_debuggers()
            # ST markers
            main_binner.attach_st_markers()
            main_binner.loop_debuggers()
            main_binner.writePipe("Verified ST marker\n")
            #main_binner.ok()
            main_binner.detach_st_markers()
            # END markers
            main_binner.attach_end_markers()
            main_binner.loop_debuggers()
            main_binner.writePipe("Verified END marker\n")
            #main_binner.ok()
            main_binner.detach_end_markers()
            # test finished
            main_binner.start_debuggers()
            main_binner.ok()

        elif(cmd == "getSynopsis"):
            dlog("In getSynopsis", 1)
            main_binner.get_synopsis()
            main_binner.ok()

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStart"):
            global log_file
            log_file += time.strftime("%Y%m%d-%H%M%S")

#            if(len(cmds) > 1):
#                startLog(cmds[1])
#            else:
#                startLog(log_file)
            startLog()
            main_binner.start_log(log_file)
            main_binner.ok()

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStop"):
            stopLog()
            main_binner.stop_log()
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
                    try:
                        dlog("[*] Attaching to %s (%d)" % (name, pid))
#                        log_write("[*] Attaching to " + name + " " + str(pid) + "\n")
                        main_binner.attach(pid)
                        #break
                    except Exception, e:
                        dlog("[!] Problem attaching to %s" % name)
                        dlog(e)
#                        log_write("[*] Problem attaching to " + name)
#                        log_write(e)
                    continue

            main_binner.writePipe("Attached to " + str(args))
            main_binner.ok()

        elif(cmd == "killHost"):
            main_binner.terminate_processes()
            main_binner.ok()
            exit()
    
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
            dlog("binTest")
            main_binner.writePipe("Communication with binner is working")
            main_binner.ok()
        
        elif(cmd == "checkReady"):
            main_binner.writePipe("Status: RD")
            main_binner.ok()

        elif(cmd == "spawn"):
            print("Spawning: %s" % args)
            Popen(args)
            main_binner.writePipe("OK")
            main_binner.ok()

        elif(cmd == "startBinner"):
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

#    except Exception, e:
#        print(e)
#        while True:
#            pass
#        main_binner.ok()

### main loop

def binner_routine():
    global ph
    global main_binner

    dlog("In main", 1)
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

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("0.0.0.0", 12345))
    s.listen(3)
    dlog("Waiting for connection", 1)
    ph, addr = s.accept()
    dlog("Got connection", 1)
    ph.send("python binner")

    main_binner = binner.binner(ph)
    main_binner.ok()

    while True:
        cmd = readPipe()
        cmds = cmd.split(" ")
        execute(cmds)

if __name__ == '__main__':
    binner_routine()
