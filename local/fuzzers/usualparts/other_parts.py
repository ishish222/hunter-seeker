import statemachine
import globs
import os
import Queue 

options = globs.state.options

def beep(args = None):
    from subprocess import Popen
    Popen('beeep')
    return

def defined(name):
    if(name in globals()):
        return True
    if(name in locals()):
        return True
    if(name in vars()):
        return True
    names = name.split(".")
    if(len(names)>1):
        if(names[1] in dir(globals()[names[0]])):
            return True
    return False

def always_true(options, state):
    return True

def check_successful(options, state):
    return not state.failed

def fail_check(options, state):
    if(not state.failed):
        return True
    else: 
        print "Failed"
        state.failed = False
        state.failure_reason = ""
        return False

def fuzzing_loop(options, state):
    stats = state.stats
    print("Current stats (SA/MA/TO): %d/%d/%d" % (stats.sample_count, stats.ma_count, stats.to_count))

def noop(args):
    pass

def nop(options, state):
    pass

def exit_failure(options, state):
    print(state.failure_info)
    state.failure_count +=1
    if(state.failure_count > 3): 
        print "Too many failures, exiting"
        exit()

def next_test2(options, state):
    if(state.failed):
        state.failed = False
        return "poweroff_no_revert"
    else:
        return "close"

def next_test(options, state):
    if(state.status == "CR"):
        return "save"
    elif(state.samples_exhausted):
        return "poweroff_regenerate"
    else:
        return "test"

def init_test(options, state):
    if(state.initialized == True):
        return "fuzzing_loop"
    else:
        return "binner_spawn_python_server"

def print_logo(args = None):
    logo = """
 __                      .__               
|  | ____________________|__| ____   ____  
|  |/ /  _ \_  __ \_  __ \  |/    \ /  _ \ 
|    <  <_> )  | \/|  | \/  |   |  (  <_> )
|__|_ \____/|__|   |__|  |__|___|  /\____/ 
     \/                          \/        
"""
    print(logo)

import os 

def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

def testfile(x):
    return os.path.exists(x)

def clear_stack(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    globs.state.stack = []

    return

def adjust(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    args = int(args, 0x10)
    globs.state.stack.append(globs.state.stack.pop() + args)

    return

def shift(args = 0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    globs.state.stack.append(globs.state.stack[args])

    return

def push(args = None):
    if(args == None):
        val = globs.state.ret
    else:
        val = args
    globs.state.stack.append(val)

def push2(args = None):
    if(args == None):
        val = globs.state.ret
    else:
        val = args
    globs.state.stack2.append(val)

def pop(args = None):
    globs.state.ret = globs.state.stack.pop()

def pop2(args = None):
    globs.state.ret = globs.state.stack2.pop()

    

def get_options(args=None):
    from optparse import OptionParser
    import settings
    import sys
    import logging
    import logging.handlers

    class MyOptionParser(OptionParser):
        def error(self, msg):
            pass

    parser = MyOptionParser()

    parser.add_option("-M", "--machines",       dest="machines", help="Machines path", default=settings.qemu_machines)
    parser.add_option("-a", "--hda",            dest="hda", help="First disk")
    parser.add_option("-b", "--hdb",            dest="hdb", help="Second disk")
    parser.add_option("-c", "--cdrom",          dest="cdrom", help="CD")
    parser.add_option("-m", "--m",              dest="qemu_m", help="Amount of memory", default=settings.qemu_m)
    parser.add_option("-S", "--scripts",        dest="scripts", help="Scripts to start with")
    parser.add_option("-o", "--origin",         dest="origin", help="Original file")
    parser.add_option("-s", "--shared",         dest="shared_folder", help="Folder for shared applications", default=settings.qemu_shared_folder)
    parser.add_option("-d", "--samples-shared", dest="samples_shared", help="Folder for shared samples", default=settings.samples_shared_path)
    parser.add_option("-v", "--samples-saved",  dest="samples_saved", help="Folder for saved samples", default=settings.samples_saved)
    parser.add_option("-B", "--samples-binned", dest="samples_binned", help="Folder for binned samples", default=settings.samples_binned)
    parser.add_option("-i", "--fuzzbox_ip",     dest="fuzzbox_ip", help="Force fuzzbox ip (normally based on hda)")
    parser.add_option("-p", "--fuzzbox_port",   dest="fuzzbox_port", help="Force fuzzbox port (normally based on hda)")
    parser.add_option("-V", "--visible",        dest="visible", help="Should box be visible?", action="store_true", default=settings.visible)
    parser.add_option("-l", "--slowdown",       dest="slowdown", help="Slowdown (default is 1)", default=settings.slowdown)
    parser.add_option("-e", "--extension",      dest="extension", help="Extension of generated sample", default=settings.extension)
    parser.add_option("-n", "--mutation-number", dest="mutations", help="Number of mutations to perform", default=settings.mutations)
    parser.add_option("-D", "--orig-samples-dir", dest="samples_orig", help="Path to original samples", default="")
    parser.add_option("-P", "--sample",         dest="sample", help="Path to sample", default="")
    parser.add_option("-C", "--command",        dest="obs_command", help="Observer command", default="testStEndMarkers")
    parser.add_option("-L", "--timeout",        dest="wait_sleep", help="Timeout for state transitions", default=settings.wait_sleep)
    parser.add_option("-O", "--count",          dest="samples_count", help="Amount of samples in single run", default=10)
    parser.add_option("-T", "--tap",            dest="tap", help="Tap interface", default="tap0")
    parser.add_option("-r", "--metric-res",     dest="metric_res", help="Metrics resolution", default=settings.metric_res)
    parser.add_option("-A", "--save-disks",     action="store_true", dest="save_disks", help="Save disks even if they don't contain crashing", default=settings.save_disks)
    parser.add_option("-N", "--vnc",            action="store_true", dest="vnc", help="Use VNC", default=settings.vnc)
    parser.add_option("-R", "--profiling",      action="store_true", dest="profiling", help="Use profiling", default=settings.profiling)
    parser.add_option("-k", "--taskset",        action="store_true", dest="use_taskset", help="Use taskset", default=settings.use_taskset)
    parser.add_option("-t", "--to-mult-factor", dest="to_mult_factor", help="Factor for calculating SO timeout based on TO (SO=TO*factor)", default=settings.to_mult_factor)
    parser.add_option("-w", "--boot-wait",      dest="boot_wait", help="How long does this system boot", default=settings.boot_wait)
    parser.add_option("-I", "--revert-wait",    dest="revert_wait", help="How long does this system revert", default=settings.revert_wait)
    parser.add_option("-W", "--shutdown-wait",  dest="shutdown_wait", help="How long does this system shutdown", default=settings.shutdown_wait)
    parser.add_option("-f", "--init-timeout",   dest="init_timeout", help="Timeout for binner initiation", default=settings.init_timeout)
    parser.add_option("-z", "--samples-size-margin", dest="samples_size_margin", help="Size marigin of single sample for disk size calculations", default=settings.samples_size_margin)
    parser.add_option("-u", "--smp",            dest="smp", help="Number of vCPUs", default=settings.smp)
    parser.add_option("-K", "--generator",      dest="generator", help="Generator to use", default=settings.generator)
    parser.add_option("-E", "--mutator",        dest="mutator", help="Mutator to use", default=settings.mutator)
    parser.add_option("-G", "--loop-function",  dest="loop", help="Loop function to use", default="dummy")
    parser.add_option("-H", "--samples-source", dest="samples_source", help="Samples source to use", default="autogenerated_batch")
    parser.add_option("-J", "--glob",           dest="glob_pattern", help="Glob pattern to use for glob sample source", default=None)
    parser.add_option("-j", "--wait-key",       action="store_true", dest="wait_key", help="Wait for keystroke", default=False)
    parser.add_option("-U", "--walk-level",     dest="walk_level", help="Walk level", default="3")
    parser.add_option("-X", "--ws",             dest="walk_start", help="Walk start address (should be call)", default=None)
    parser.add_option("-g", "--args",           dest="args", default=None)
    parser.add_option("-F", "--output",         dest="out_path", default='./heis-out')

    (options, args) = parser.parse_args()
    (options, args) = parser.parse_args()

    before_last = len(sys.argv) -2

    options.fuzzbox_name = sys.argv[before_last]
    if(options.hda is None):
        options.hda = settings.machines[options.fuzzbox_name]['disk']

    #qemu settings
    options.qemu_command = settings.qemu_command
    options.qemu_drive_options = settings.qemu_drive_options
    if(options.use_taskset is True):
        qemu_args =  ['taskset', '-c', settings.machines[options.fuzzbox_name]['taskset'], options.qemu_command]
    else:
        qemu_args =  [options.qemu_command]
    qemu_args += ['-m', options.qemu_m]

    if(options.qemu_drive_options != ""):
        qemu_args += ['-drive', 'file=' + options.machines + '/' + options.hda + ',' + options.qemu_drive_options]
        if(options.hdb is not None):
            qemu_args += ['-drive', 'file=', './' + options.hdb + ',' + options.qemu_drive_options]
    else:
        qemu_args += ['-drive', 'file=' + options.machines + '/' + options.hda]
        if(options.hdb is not None):
            qemu_args += ['-drive', 'file=', './' + options.hdb]


    if(options.cdrom is not None):
        qemu_args += ['-cdrom', options.cdrom]

    options.ms_path = settings.machines[options.fuzzbox_name]['monitor']
    options.ss_path = settings.machines[options.fuzzbox_name]['serial']

    if(options.visible == False and options.vnc == True):
        qemu_args += ['-vnc', settings.machines[options.fuzzbox_name]['vnc']]
    qemu_args += settings.qemu_additional

    qemu_args += ['-monitor', "unix:%s" % settings.machines[options.fuzzbox_name]['monitor']]
    qemu_args += ['-serial', "unix:%s" % settings.machines[options.fuzzbox_name]['serial']]
    qemu_args += ['-serial', "unix:%s" % settings.machines[options.fuzzbox_name]['serial']+'-log']
    qemu_args += ['-serial', "file:%s" % settings.machines[options.fuzzbox_name]['serial']+'3']
    qemu_args += ['-smp', str(options.smp)]
    
    if(settings.qemu_env != None):
        for exp in settings.qemu_env:
            os.environ[exp[0]] = exp[1]
#    print os.environ
#        qemu_args.insert(0, settings.qemu_env)
    

    testdir(settings.qemu_shared_folder + "/logs")
    
    options.qemu_args = qemu_args
    options.slowdown = float(options.slowdown)
    options.samples_count = int(options.samples_count)
    options.logger = logging.getLogger('MyLogger')
    options.handler = logging.handlers.SysLogHandler(address = '/dev/log')
    options.logger.setLevel(logging.DEBUG)
    options.logger.addHandler(options.handler)
    options.wait_sleep = float(options.wait_sleep)
    options.metric_res = int(options.metric_res)
    options.smp = int(options.smp)
    options.boot_wait = float(options.boot_wait)
    options.revert_wait = float(options.revert_wait)
    options.shutdown_wait = float(options.shutdown_wait)
    options.to_mult_factor = float(options.to_mult_factor)
    options.fuzzbox_timeout = float(options.wait_sleep*options.to_mult_factor)
    options.init_timeout = float(options.init_timeout)
    options.walk_level = float(options.walk_level)


    from threading import Event

    options.shutting_down = Event()
    options.threads = list()

    # for additional parsing
    options.parser = parser

    #thats right bitches
    options.settings = settings

    globs.state.options = options
    globs.state.stack = []
    globs.state.stack2 = []
    globs.state.ret = ""
    globs.state.eip = ""
    globs.state.ep = ""
    return


def sigkill_handler(signum, frame):
    options = globs.state.options

    print "Detected sigkill from user"
#    raise statemachine.MachineExit
    import qemu_parts
    qemu_parts.poweroff_no_revert()
    
    return

def register_signals(args="exception"):
    globs.exc_label = args

def wait_1_seconds(args=None):
    import time
    time.sleep(1)

def wait_10_seconds(args=None):
    import time
    time.sleep(10)

def wait_100_seconds(args=None):
    import time
    time.sleep(100)

def wait_for_keypress(args=None):
    os.system('read -s -n 1 -p "Press any key to continue..."')
    print

def noop(args=None):
    pass

def choosing_saved_disk_procedure(args=None):
    options = globs.state.options

    if(options.samples_source == "autogenerated_batch"):
        return "AM"
    else:
        return "GM"

def check_equal(args=None):
    options = globs.state.options

    args = int(args, 0x10)

    if(globs.state.ret == args):
        return "Y"
    else:
        return "N"

def check_greater_than(args=None):
    options = globs.state.options

    args = int(args, 0x10)

    if(globs.state.ret > args):
        return "Y"
    else:
        return "N"

def check_less_than(args=None):
    options = globs.state.options

    args = int(args, 0x10)

    if(globs.state.ret < args):
        return "Y"
    else:
        return "N"


def run_cmd_host(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    from subprocess import call
    call(args.split())

    globs.state.ret = response
    return

