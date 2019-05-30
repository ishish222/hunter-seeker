import time
import sys
sys.path += ["../../common"]
sys.path += ["../client"]
import common
from script import rs, runscriptq
import globs

write_socket = common.write_socket
read_socket = common.read_socket
report = common.report

#def internal_kill_explorer(options, state):
def internal_kill_host(args=None):
    options = globs.state.options

    write_socket(options.s, "killHost")
    read_socket(options.s)

def internal_kill_explorer(args=None):
    options = globs.state.options

    write_socket(options.s, "killExplorer")
    read_socket(options.s)

#def internal_spawn_python_server(options, state):
def internal_spawn_python_server(args=None):
    options = globs.state.options

    rs("lclick", options.m)
    rs("python_server_spawn_args", options.m)

def internal_spawn(args=None):
    options = globs.state.options

    write_socket(options.s, "startBinner")
    read_socket(options.s)

'''
def internal_attach_app(args = None):
    options = globs.state.options

    if(args == None):
        args = globs.state.ret

    write_socket(options.s, "attach 0x%08x" % args)
    read_socket(options.s)
'''

def internal_spawn_app(args=None):
    options = globs.state.options

    write_socket(options.s, "spawn " + options.settings.app_path)
    read_socket(options.s)

#    print("Will sleep for %d" % options.settings.start_sleep)
#    time.sleep(options.settings.start_sleep)
#    print "Finished sleeping"
#    read_socket(options.s)

def internal_configure(args=None):
    options = globs.state.options

    write_socket(options.s, "binTest")
    read_socket(options.s)

    write_socket(options.s, "attachBinner " + options.settings.app_module)
    read_socket(options.s)

    time.sleep(1)

    write_socket(options.s, "setupMarkers")
    read_socket(options.s)

    time.sleep(1)

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

    time.sleep(1)

def internal_start_logs(args=None):
    options = globs.state.options

    write_socket(options.s, "logStart e:\\logs\\log-%s-%s.txt" % (options.fuzzbox_name, common.timestamp2()))
    read_socket(options.s)

def internal_start_profiling(args=None):
    options = globs.state.options

    write_socket(options.s, "start_profiling")

def internal_check_ready(args=None):
    options = globs.state.options

    write_socket(options.s, "checkReady")

    #TODO: should this be here?
    state.status = ""

def internal_key_wait(args=None):
    options = globs.state.options

    if(options.wait_key):
        print("Press enter")
        sys.stdin.read(1)

def internal_save(args=None):
    options = globs.state.options

#    write_socket(options.s, "getSynopsis")
#    dossier, _, _ = read_socket(options.s)
#    options.log.write("[%s], registered, binned\n" % state.status)
#    options.log.flush()
#    print("CR in: %s" % options.tmp_disk_img)
#    report("CR")
#    print("Got crash, restarting")
#    report("Got crash, restarting")
#    time.sleep(30)
#    read_socket(options.s) #waiting for "saved"
    
    print("Saved automatically")

def internal_close_sample(args=None):
    options = globs.state.options

    runscriptq(options.settings.closing_plugin_name, options.m)
    write_socket(options.s, "")

def cooldown(args=None):
    options = globs.state.options

    write_socket(options.s, "cooldown")
    read_socket(options.s)

def cooldown2(args=None):
    options = globs.state.options

    write_socket(options.s, "cooldown2 10 30")
    read_socket(options.s)

def cooldown3(args=None):
    options = globs.state.options

    write_socket(options.s, "cooldown3 5 10")
    read_socket(options.s)

def cooldown4(args=None):
    options = globs.state.options

    write_socket(options.s, "cooldown3 3 101")
    read_socket(options.s)

