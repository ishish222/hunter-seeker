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
    global main_binner
    global crash_bin
    global crash_reason

    print("4")
    e_addr = main_binner.crash_bin.last_crash.exception_address

    #create dossier
#    if(reason == "hc"):
#        crash_reason = "hc"
#        writePipe("Probable crash reason: hc (crash info needs reinterpretation)\n")
#        writePipe("")
    if((disasm(dbg, e_addr) == "[INVALID]") or (get_module(dbg, e_addr) == "[INVALID]")):
        crash_reason = "uaf"
        main_binner.writePipe("Probable crash reason: uaf\n")
        main_binner.writePipe("")
    else:
        crash_reason = "unk"

    print("5")
    main_binner.writePipe(crash_bin.crash_synopsis())

    return DBG_CONTINUE

### binner commands

def invoke(f):
    print("powershell -command \"& { invoke-expression z:\\samples\\shared\\%s }\"" % f)
    Popen("powershell -command \"& { invoke-expression z:\\samples\\shared\\%s }\"" % f)

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
            main_binner.status = ""
            main_binner.stop_debuggers()
            main_binner.attach_st_markers()
            invoke(args)
            while(main_binner.status != "ST"):
                main_binner.loop_debuggers() # wait for test start, should add timeout?
            # test started, switch markers
            main_binner.detach_st_markers()
            main_binner.attach_react_markers()
            main_binner.attach_end_markers()
#            main_binner.loop_debuggers(settings.wait_sleep * settings.slowdown)
            while True:
                main_binner.loop_debuggers()
                if(main_binner.status == "MA"):
                    break
                if(main_binner.status == ""):
                    main_binner.status = "TO" # TO is overriden by all
                    break
                if(main_binner.status == "CR"):
                    handle_crash()
                    break
                if(main_binner.status == "SR"):
                    main_binner.dlog("Requested script: %s" % main_binner.reqScript)
#                    main_binner.writePipe("Status: %s\n" % main_binner.status)
#                    main_binner.writePipe("Script: %s\n" % main_binner.reqScript)
                    main_binner.ok()
            main_binner.stop_debuggers()
            main_binner.detach_end_markers()
            dlog("About to send status")
            main_binner.writePipe("Status: %s" % main_binner.status)
            dlog("Sent status")
            main_binner.ok()
            dlog("Sent OK")
            #will wait for RD from marker
            time.sleep(10.0)
            main_binner.start_debuggers()
            dlog("About to send RD")
            main_binner.writePipe("Status: RD")
            dlog("Send RD")
            main_binner.ok()
            dlog("Send RDOK")

        elif(cmd == "observe"):
            dlog("In observe")
            main_binner.attach_st_markers()
            main_binner.loop_debuggers()
            #waiting, end marker handler will ok()

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
            main_binner.stop_debuggers()
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

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStart"):
#            global log_file
#            if(len(cmds) > 1):
#                startLog(cmds[1])
#            else:
#                startLog(log_file)
            main_binner.ok()

        # TODO: sprawdz ktore logi gdzie maja isc
        elif(cmd == "logStop"):
#            stopLog()
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
#            setup_dbg(dbg)
#            main_binner.writePipe("AV handlers in place")
            # will do automatically on process attach
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
    dlog(logo)

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
    try:
        binner_routine()
    except Exception, e:
        print(e)
        while True:
            pass
