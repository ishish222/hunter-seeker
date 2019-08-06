import sys
sys.path += ["../common"]

import statemachine
import globs
import os
import queue 

options = globs.state.options

def reset_time(args=None):
    import time
    state = globs.state
    options = globs.state.options

    state.time = time.time()

    return

def read_time(args=None):
    import time
    state = globs.state
    options = globs.state.options

    elapsed = time.time() - state.time
    print('Elapsed: {}'.format(elapsed))

    return

def host_print_result(args=None):
    options = globs.state.options

    if(args is None):
        args = globs.state.stack.pop()

    if(type(args) == int):
        args = '0x%08x' % args

    if(hasattr(options, 'host_result_file') == False):
        options.host_result_file = open("results.txt", "w", 0)
    options.host_result_file.write(args)
    options.host_result_file.write('\n')

    return

def host_print(args=None):
    options = globs.state.options

    if(args is None):
        args = globs.state.stack.pop()

    if(type(args) == int):
        args = '0x%08x' % args

    print(('%s%s%s' % ('\033[91m', args, '\033[0m')))
    print('\n')

    return

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
        print("Failed")
        state.failed = False
        state.failure_reason = ""
        return False

def fuzzing_loop(options, state):
    stats = state.stats
    print(("Current stats (SA/MA/TO): %d/%d/%d" % (stats.sample_count, stats.ma_count, stats.to_count)))

def noop(args = None):
    pass

def nop(options, state):
    pass

def exit_failure(options, state):
    print((state.failure_info))
    state.failure_count +=1
    if(state.failure_count > 3): 
        print("Too many failures, exiting")
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

def clear_stack(args = None):
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

    globs.state.ret = globs.state.ret + args

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

def print_stack(args = None):
    print('Current values on external stack:')
    for val in globs.state.stack:
        print('%s' % val)

def print_stack_2(args = None):
    print('Current values on external stack2:')
    for val in globs.state.stack2:
        print('%s' % val)

def push2(args = None):
    if(args == None):
        val = globs.state.ret
    else:
        val = args
    globs.state.stack2.append(val)

def print_queue2(args = None):
    print("Current queue content:")
    for item in globs.state.queue2:
        print(item)

def print_queue(args = None):
    print("Current queue content:")
    for item in globs.state.queue:
        print(item)

def enqueue(args = None):
    if(args == None):
        val = globs.state.ret
    else:
        val = args
    globs.state.queue.insert(0, val)

def enqueue2(args = None):
    if(args == None):
        val = globs.state.ret
    else:
        val = args
    globs.state.queue2.insert(0, val)

def pop(args = None):
    globs.state.ret = globs.state.stack.pop()
    print("Popped: %s" % globs.state.ret)

def pop2(args = None):
    globs.state.ret = globs.state.stack2.pop()
    print("Popped: %s" % globs.state.ret)

def dequeue(args = None):
    globs.state.ret = globs.state.queue.pop()
    print("Dequeued: %s" % globs.state.ret)

def dequeue2(args = None):
    globs.state.ret = globs.state.queue2.pop()
    print("Dequeued: %s" % globs.state.ret)

def str_cat(args = None):
    if(args == None):
        globs.state.ret = '%s%s' % (globs.state.ret, globs.state.stack.pop())
    else:
        globs.state.ret = '%s%s' % (globs.state.ret, args)
    print('Result string: %s' % globs.state.ret)

def str_extract_extension(args = None):
    globs.state.ret = '%s' % (globs.state.ret.split(".")[-1:][0])
    print('Result string: %s' % globs.state.ret)

def str_cat_queue(args = None):
    globs.state.ret = '%s%s' % (globs.state.ret, globs.state.queue2.pop())
    print('Result string: %s' % globs.state.ret)

def strr(args):
    if(args == None):
        args = globs.state.ret
    globs.state.ret = '%s' % (args)
    print('Result string: %s' % globs.state.ret)

def int10(args = None):
    if(args == None):
        globs.state.ret = int(globs.state.ret, 10)
    else:
        globs.state.ret = int(args, 10)

def int16(args = None):
    if(args == None):
        globs.state.ret = int(globs.state.ret, 0x10)
    else:
        globs.state.ret = int(args, 0x10)

def increase_counter(args = None):
    if(args == None):
        args = 0x1

    globs.state.counter = globs.state.counter +args
    print("0x%08x" % globs.state.counter)

def decrease_counter(args = None):
    if(args == None):
        args = 0x1

    globs.state.counter = globs.state.counter -args
    print("0x%08x" % globs.state.counter)

def compare_counter(args = None):
    if(type(args) != int):
        args = int(args, 0x10)

    if(not hasattr(globs.state, 'counter')):
        globs.state.counter = 0

    if(globs.state.counter == args):
        return "Y"
    else:
        return "N"

def set_counter(args = None):
    if(args == None):
        args = globs.state.stack.pop()

    if(type(args) == 'str'):
        args = int(args, 0x10)

    globs.state.counter = args
    print("0x%08x" % globs.state.counter)

def get_counter(args = None):
    print("0x%08x" % globs.state.counter)
    globs.state.ret = globs.state.counter
    #globs.state.counter = globs.state.counter-1

def check_counter(args = None):
    print("0x%08x" % globs.state.counter)
    if(globs.state.counter == 0x0):
        #globs.state.counter = globs.state.counter-1
        return "Y"
    else:
        #globs.state.counter = globs.state.counter-1
        return "N"

def get_qemu_cmdline(args=None):

    options.fuzzbox_name = sys.argv[before_last]
    if(options.hda is None):
        options.hda = settings.machines[options.fuzzbox_name]['disk']


    #qemu settings
    options.qemu_command = settings.host_qemu_command
    options.qemu_drive_options = settings.qemu_drive_options

    options.use_taskset = False

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
    #qemu_args += settings.qemu_additional

    qemu_args += ['-monitor', "unix:%s" % settings.machines[options.fuzzbox_name]['monitor']]
    qemu_args += ['-serial', "unix:%s" % settings.machines[options.fuzzbox_name]['serial']]
    qemu_args += ['-serial', "unix:%s" % settings.machines[options.fuzzbox_name]['serial']+'-log']
    qemu_args += ['-serial', "file:%s" % settings.machines[options.fuzzbox_name]['serial']+'3']
#    qemu_args += ['-smp', str(options.smp)]
    
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
    globs.state.queue = []
    globs.state.queue2 = []
    globs.state.ret = ""
    globs.state.eip = ""
    globs.state.ep = ""
    return


def sigkill_handler(signum, frame):
    options = globs.state.options

    print("Detected sigkill from user")
#    raise statemachine.MachineExit
    from . import qemu_parts
    qemu_parts.poweroff_no_revert()
    
    return

def register_signals(args="exception"):
    globs.exc_label = args

def wait(args=None):
    import time
    if(args == None):
        args = globs.state.stack.pop()

    time.sleep(float(args))

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
    print()

def noop(args=None):
    pass

def choosing_saved_disk_procedure(args=None):
    options = globs.state.options

    if(options.samples_source == "autogenerated_batch"):
        return "AM"
    else:
        return "GM"

def check_strstr_w_queue(args=None):
    options = globs.state.options

    if(args == ""):
        args = globs.state.stack.pop()

    for item in globs.state.queue:
        print("Checking strstr %s in %s" % (args, item))
        if(args in item):
            return "Y"

    return "N"

def check_strstr_w_queue2(args=None):
    options = globs.state.options

    if(args == ""):
        args = globs.state.stack.pop()

    for item in globs.state.queue2:
        print("Checking strstr %s in %s" % (args, item))
        if(args in item):
            return "Y"

    return "N"

def check_strstr(args=None):
    options = globs.state.options

    ret = globs.state.ret
    if(type(ret) is not str):
        ret = str(ret)

    print("Checking strstr %s in %s" % (args, ret))

    if(args in ret):
        return "Y"
    else:
        return "N"

def check_equal(args=None):
    options = globs.state.options

    if(type(globs.state.ret) != 'int'):
        ret = int(globs.state.ret, 0x10)
    else:
        ret = globs.state.ret

    args = int(args, 0x10)

    print("Checking 0x%08x == 0x%08x" % (ret, args))

    if(ret == args):
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

    return

def pause(aegs = None):
    import os
    os.system('read -s -n 1 -p "Press any key to continue..."')
    print()
    return

