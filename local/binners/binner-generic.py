import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

import win32pipe, win32file, win32gui
import time
import binner
import settings
from functions import *

PIPE_NAME = "\\\\.\\pipe\\control"
PIPE_BUFF_SIZE = 4096

def getPipe(name):
    dlog("In getPipe")
    ph = win32file.CreateFile(name, win32file.GENERIC_READ | win32file.GENERIC_WRITE | win32pipe.PIPE_TYPE_MESSAGE, 0, None, win32file.OPEN_EXISTING, 0, None)
    return ph

def readPipe():
    global ph
    return win32file.ReadFile(ph, PIPE_BUFF_SIZE)

def writePipe(data):
    global ph
    win32file.WriteFile(ph, data)

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


def watchThread_routine(to):
    dlog("watchThread")
    global main_binner

    if(settings.breaking == True):
        breaker = Thread(target = breaking_routine, args=(to,))
        breaker.start()

    timer = Thread(target = timer_routine, args=(to,))
    timer.start()

    while(timer.is_alive() and status == ""):
        pass

    if(main_binner.status != "CR" and main_binner.status != "MA"):
        main_binner.test_lock.acquire()
        dlog("test locked by TO")
        main_binner.status = "TO"
        main_binner.test_lock.release()
    main_binner.stop_debuggers()
    return

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

        elif(cmd == "waitTest"):
            main_binner.status = ""
            main_binner.attach_markers()
            watchThread = Thread(target = watchThread_routine, args=(settings.wait_sleep * settings.slowdown, ))
            watchThread.start()
            main_binner.start_debuggers()
            # waiting until stop occurs
            main_binner.writePipe("Status: " + main_binner.status)
            if(status == "CR"):
                handle_crash()
            main_binner.remove_markers()
            main_binner.ok()

        elif(cmd == "observe"):
            dlog("In observe")
            main_binner.attach_st_markers()
            main_binner.loop_debuggers()
            #waiting, end marker handler will ok()

        elif(cmd == "testMarkers"):
            dlog("In testMarkers")
            main_binner.start_debuggers()
#            time.sleep(5)
#            main_binner.stop_debuggers()
#            time.sleep(5)
#            main_binner.start_debuggers()
#            time.sleep(5)
#            main_binner.stop_debuggers()
#            time.sleep(5)
#            main_binner.start_debuggers()
            time.sleep(50)
            main_binner.stop_debuggers()
            time.sleep(50)
#            main_binner.attach_st_markers()
#            main_binner.loop_debuggers()
            # ST reached
#            main_binner.attach_end_markers()
#            main_binner.loop_debuggers()
            #waiting, end marker handler will ok()

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

    dlog("In main")
    # binner will self-configure based on settings
    ph = getPipe(PIPE_NAME)
    main_binner = binner.binner(ph)
    main_binner.ok()

    while True:
        cmd = readPipe()
        cmds = str.split(cmd[1])
        execute(cmds)

if __name__ == '__main__':
    try:
        binner_routine()
    except Exception, e:
        print(e)
        while True:
            pass
