import time
import sys
sys.path += ["../../common"]
import common

write_socket = common.write_socket
read_socket = common.read_socket
report = common.report

def binner_kill_explorer(options, state):
    write_socket(options.s, "killExplorer")
    read_socket(options.s)

def binner_spawn(options, state):
    write_socket(options.s, "startBinner")
    read_socket(options.s)

def binner_spawn_app(options, state):
    write_socket(options.s, "spawn " + options.settings.app_path)
    read_socket(options.s)

#    print("Will sleep for %d" % options.settings.start_sleep)
    time.sleep(options.settings.start_sleep)
#    print "Finished sleeping"
#    read_socket(options.s)

def binner_configure(options, state):
    write_socket(options.s, "binTest")
    read_socket(options.s)

    write_socket(options.s, "attachBinner " + options.settings.app_module)
    read_socket(options.s)

    write_socket(options.s, "setupMarkers")
    read_socket(options.s)

    write_socket(options.s, "installHandlers")
    read_socket(options.s)

    #TODO: we want to eliminate this with tests
    if(options.wait_sleep != options.settings.wait_sleep):
        write_socket(options.s, "setupWaitSleep {0}".format(options.wait_sleep))
        read_socket(options.s)

    #TODO: we want to eliminate this with tests
    if(options.slowdown != options.settings.slowdown):
        write_socket(options.s, "setupSlowdown {0}".format(options.slowdown))
        read_socket(options.s)

def binner_start_logs(options, state):
    write_socket(options.s, "logStart e:\\logs\\log-%s-%s.txt" % (options.fuzzbox_name, common.timestamp2()))
    read_socket(options.s)

def binner_start_profiling(options, state):
    write_socket(options.s, "start_profiling")

def binner_check_ready(options, state):
    write_socket(options.s, "checkReady")

    #TODO: should this be here?
    state.status = ""

def binner_key_wait(options, state):
    if(options.wait_key):
        print("Press enter")
        sys.stdin.read(1)

def binner_save(options, state):
    write_socket(options.s, "getSynopsis")
    dossier, _, _ = read_socket(options.s)
    options.log.write("[%s], registered, binned\n" % state.status)
    options.log.flush()
    print("CR in: %s" % options.tmp_disk_img)
    report("CR")
    print("Got crash, restarting")
    report("Got crash, restarting")
    time.sleep(30)


