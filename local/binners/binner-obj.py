import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

import win32pipe, win32file, win32gui
import binner


def ok():
    time.sleep(0.5)
    writePipe("-=OK=-")
    win32file.FlushFileBuffers(ph)

def getPipe(name):
    ph = win32file.CreateFile(name, win32file.GENERIC_READ | win32file.GENERIC_WRITE | win32pipe.PIPE_TYPE_MESSAGE, 0, None, win32file.OPEN_EXISTING, 0, None)
    return ph

def readPipe():
    global ph
    return win32file.ReadFile(ph, pipe_buff_size)

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
        writePipe("Probable crash reason: uaf\n")
        writePipe("")
    else:
        crash_reason = "unk"

    print("5")
    writePipe(crash_bin.crash_synopsis())

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

    try:
        if(cmd == "settle"):
            main_binner.status = ""
    
            watchThread = Thread(target = watchThread_routine, args=(float(cmds[1]),))
            watchThread.start()
            main_binner.start_debuggers()
            ok()

        else if(cmd == "waitTest"):
            main_binner.status = ""
            main_binner.attach_markers()
            watchThread = Thread(target = watchThread_routine, args=(settings.wait_sleep * settings.slowdown, ))
            watchThread.start()
            main_binner.start_debuggers()
            # waiting until stop occurs
            writePipe("Status: " + main_binner.status)
            if(status == "CR"):
                handle_crash()
            main_binner.remove_markers()
            ok()

        else if(cmd == "observe"):
            dlog("In observe")
            main_binner.attach_start_markers()
            main_binner.start_debuggers()
            #waiting, end marker handler will ok()

        # TODO: sprawdz ktore logi gdzie maja isc
        else if(cmd == "logStart"):
            global log_file
            if(len(cmds) > 1):
                startLog(cmds[1])
            else:
                startLog(log_file)
            ok()

        # TODO: sprawdz ktore logi gdzie maja isc
        else if(cmd == "logStop"):
            stopLog()
            ok()
    
        else if(cmd == "listTebs"):
            main_binner.list_tebs()
            ok()

        else if(cmd == "cbEip"):
            writePipe("0x%x" % main_binner.crash_bin.last_crash.exception_address)
            ok()

        else if(cmd == "cbCrashSynopsis"):
            writePipe(main_binner.crash_bin.crash_synopsis())
            ok()
    
        else if(cmd == "cbReason"):
            writePipe(main_binner.crash_reason)
            ok()
    
        else if(cmd == "cbStackUnwind"):
            for call_frame in main_binner.crash_bin.last_crash.stack_unwind:
                writePipe(call_frame + "\n")
            ok()

        # TODO: sprawdz, czy dziala!
        else if(cmd == "attachBinner"):
            imagename = args
            for (pid, name) in main_binner.enumerate_processes():
                if imagename in name:
                    try:
                        dlog("[*] Attaching to %s (%d)" % (name, pid))
                        log_write("[*] Attaching to " + name + " " + str(pid) + "\n")
                        main_binner.attach(pid)
                    except Exception, e:
                        dlog("[!] Problem attaching to %s" % name)
                        log_write("[*] Problem attaching to " + name)
                        log_write(e)
                    continue

            writePipe("Attached to " + str(args))
            ok()

        else if(cmd == "killHost"):
            main_binner.terminate_processes()
            ok()
            exit()
    
        else if(cmd == "installHandlers"):
#            setup_dbg(dbg)
#            writePipe("AV handlers in place")
            # will do automatically on process attach
            ok()

        else if(cmd == "installMarkerAddrs"):
#            for ma_addr in settings.ma_addrs:
#                writePipe("Installing bad at " + cmds[1])
#                dbg.bp_set(ma_addr, handler = bad_handler)
            # will do automatically on process attach
            ok()

        else if(cmd == "setupSlowdown"):
            writePipe("Setting slowdown to: {0}\n".format(cmds[1]))
            settings.slowdown = float(cmds[1])
            ok()
    
        else if(cmd == "setupMarkers"):
            writePipe("Setting up markers\n")
#            settings.ma_addrs += resolve_rvas(dbg, settings.ma_rvas)
#            settings.ma_st_addrs += resolve_rvas(dbg, settings.ma_st_rvas)
#            settings.ma_end_addrs += resolve_rvas(dbg, settings.ma_end_rvas)
            # will do automatically on process attach
            writePipe("Marker list: {0}\n".format(main_binner.markers))
            writePipe("ST marker list: {0}\n".format(main_binner.st_markers))
            writePipe("END marker list: {0}\n".format(main_binner.end_markers))
            writePipe("REACT marker list: {0}\n".format(main_binner.end_markers))
            ok()

        if(cmd == "binTest"):
            writePipe("Communication with binner is working")
            ok()
        
        if(cmd == "release"):
            writePipe("Releasing")
            ok()
            main_binner.start_debuggers()

    except Exception, e:
        print(e)
        while True:
            pass
        ok()

### main loop

def main()
    global ph
    global main_binner

    # binner will self-configure based on settings
    main_binner = binner()

    ph = getPipe(my_pipe)
    ok()

    while True:
        cmd = readPipe()
        cmds = str.split(cmd[1])
        execute(cmds)



main()
