from subprocess import Popen, PIPE, STDOUT
import socket
import time
from script import rs, rss, runscriptq, write_monitor, write_monitor_2, read_monitor
import sys
sys.path += ["../../common"]
import common
import signal
import os
import globs
from statemachine import MachineError

report = common.report
read_socket = common.read_socket
write_socket = common.write_socket

def qemu_prepare_pipes(args=None):
    options = globs.state.options

    options.ms = common.prepare_monitor(options.ms_path)
    options.ss = common.prepare_serial(options.ss_path)
    options.ss_log = common.prepare_serial(options.ss_path + '-log')


def qemu_ready(args=None):
    globs.state.reqScript = ""
    globs.state.status = "RD"
#    for s in globs.state.samples_list:
#        print s

def qemu_prepare(options, state):
    global signaled
    signaled = False

    options = globs.state.options
    state = globs.state

    #necessary?
    options.log = open("./log-%s-%s-%s" % (options.fuzzbox_name, common.timestamp2(), options.origin), "a")
    report("Starting fuzzer")
    print("[%s] Generic fuzzer" % common.timestamp())

    state.reqScript = ""
    state.status = "RD"

    signal.signal(signal.SIGINT, common.sigkill_handler)

    options.ms = common.prepare_monitor(options.ms_path)
    options.ss = common.prepare_serial(options.ss_path)

    state.stats = statistics(options.metric_res)


def log_loop(arg1, shutdown):
    from select import select
    options = globs.state.options
    
    log_data = ""

    while not(shutdown.is_set()):
        options.log_file.write('%s\n' % options.shutting_down.is_set())
        ready, _, _ = select([options.s_log], [], [], 5.0)
        if not(ready):
            continue
        log_data, _, _ = common.read_socket_q(options.s_log)
        options.log_file.write(log_data)
        shutdown.wait(0.1)

    options.log_file.write('finishing\n')

import signal
import os

def preexec_function():
#    signal.signal(signal.SIGINT, signal.SIG_IGN)
    os.setpgrp()


def qemu_connect_log(args=None):
    from threading import Thread

    options = globs.state.options
    options.s_log, _ = options.ss_log.accept()
    options.log_file = open("%s/internal.log" % options.settings.log_dir, "w+", 0)
    options.log_thread = Thread(target = log_loop, args = (10, options.shutting_down))
    options.log_thread.start()
    options.threads.append(options.log_thread)

def qemu_args_kvm(args=None):
    options = globs.state.options

    options.qemu_args += ['--enable-kvm']

def qemu_args_research_dir(args=None):
    options = globs.state.options

    options.qemu_args += ['-net', 'nic,model=rtl8139', '-net', 'user,smb=%s' % globs.state.research_dir]

def qemu_start(args=None):
    options = globs.state.options

    print("[%s] Starting" % common.timestamp())
    print " ".join(options.qemu_args)

    myErr = open("./err", "w+")

    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE, stderr=myErr.fileno(), env=os.environ, preexec_fn=preexec_function)
    time.sleep(3)
    options.m, _ = options.ms.accept()
    options.s, _ = options.ss.accept()

#    time.sleep(options.boot_wait)
    #TODO!!! wee nedd to perform test, not wait!

    print("[%s] Qemu full boot finished" % common.timestamp())
#    for s in globs.state.samples_list:
#        print s
    read_monitor(options.m)

def qemu_start_full(args=None):
    options = globs.state.options

    if(hasattr(options, 'tmp_disk_img')):
        options.log.write("[%s]\n" % options.tmp_disk_img)
        options.log.flush()
        print("Spawning fuzz for batch: %s" % options.tmp_disk_img)

    print("[%s] Starting" % common.timestamp())
    print " ".join(options.qemu_args)

    myErr = open("./err", "w+")

    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE, stderr=myErr.fileno(), env=os.environ, preexec_fn=preexec_function)
    time.sleep(3)
    options.m, _ = options.ms.accept()
    options.s, _ = options.ss.accept()

#    time.sleep(options.boot_wait)
    #TODO!!! wee nedd to perform test, not wait!

    print("[%s] Qemu full boot finished" % common.timestamp())
#    for s in globs.state.samples_list:
#        print s
    read_monitor(options.m)

def qemu_save(args=None):
    options = globs.state.options

    from script import write_monitor_2

    # check other save, ask to overwrite
    ret = write_monitor_2(options.m, "info snapshots")
    exploded = ret.split('xxx')
    if(len(exploded) > 1):
        last = exploded[1]
        print 'Will overwrite snapshot: %s' % last
        print 'Kill the process in order to abort'

        import os
        os.system('read -s -n 1 -p "Press any key to continue..."')
        print

        qemu_delete(last)

    write_socket(options.s, "disconnect");
    response, _, _ = read_socket(options.s)

    ret = write_monitor_2(options.m, "savevm xxx%sxxx" % args)
    time.sleep(options.revert_wait)
    print("Saved: %s" % args)

    import pickle

    pickle_path = options.settings.host_machines_path
    pickle_path += '/'
    pickle_path += options.settings.machines[options.fuzzbox_name]['disk']
    pickle_path += '.'
    pickle_path += args
    pickle_path += '.state'

    current_state = {}
    #current_state['options'] = globs.state.options
    current_state['stack'] = globs.state.stack
    current_state['stack2'] = globs.state.stack2
    current_state['queue'] = globs.state.queue
    current_state['queue2'] = globs.state.queue2
    current_state['ret'] = globs.state.ret
    current_state['eip'] = globs.state.eip
    current_state['ep'] = globs.state.ep

    f = open(pickle_path, 'wb')
    pickle.dump(current_state, f)
    f.close()
    print 'Saved pickled state to: %s' % pickle_path

def qemu_delete(args=None):
    options = globs.state.options

    pickle_path = options.settings.host_machines_path
    pickle_path += '/'
    pickle_path += options.settings.machines[options.fuzzbox_name]['disk']
    pickle_path += '.'
    pickle_path += args
    pickle_path += '.state'

    import os
    print 'Deleting pickled state in: %s' % pickle_path

    try:
        os.remove(pickle_path)
    except Exception:
        pass
    
    from script import write_monitor_2

    ret = write_monitor_2(options.m, "delvm xxx%sxxx" % args)
    print ret

    print("Deleted: %s" % args)

def qemu_load(args=None):
    options = globs.state.options

    from script import write_monitor_2

    # check other save, ask to overwrite
    ret = write_monitor_2(options.m, "info snapshots")
    exploded = ret.split('xxx')
    if(len(exploded) > 1):
        last = exploded[1]
        if(last != args):
            print 'Required state %s not found\nAborting' % args
            raise Exception
        else:
            pass
    else:
        print 'Required state %s not found\nAborting' % args
        raise Exception

    import pickle

    pickle_path = options.settings.host_machines_path
    pickle_path += '/'
    pickle_path += options.settings.machines[options.fuzzbox_name]['disk']
    pickle_path += '.'
    pickle_path += args
    pickle_path += '.state'

    print 'Loading pickled state from: %s' % pickle_path
    f = open(pickle_path, 'rb')
    current_state = pickle.load(f)
    f.close()

    #globs.state.options = current_state['options']
    globs.state.stack = current_state['stack']
    globs.state.stack2 = current_state['stack2']
    globs.state.queue = current_state['queue']
    globs.state.queue2 = current_state['queue2']
    globs.state.ret = current_state['ret']
    globs.state.eip = current_state['eip']
    globs.state.ep = current_state['ep']

    ret = write_monitor_2(options.m, "loadvm xxx%sxxx" % args)
    time.sleep(options.revert_wait)
    print ret

    print("Loaded: %s" % args)

def qemu_flush(args=None):
    options = globs.state.options
    common.read_socket(options.s)

def qemu_start_revert(args=None):
    options = globs.state.options

    if(hasattr(options, 'tmp_disk_img')):
        options.log.write("[%s]\n" % options.tmp_disk_img)
        options.log.flush()
        print("Spawning fuzz for batch: %s" % options.tmp_disk_img)

    print("[%s] Starting" % common.timestamp())
    print " ".join(options.qemu_args)

    myErr = open("./err", "w+")

    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE, stderr=myErr.fileno(), env=os.environ, preexec_fn=preexec_function)
    time.sleep(3)
    options.m, _ = options.ms.accept()
    options.s, _ = options.ss.accept()

    common.revert(options)
    time.sleep(options.revert_wait)
#    time.sleep(options.boot_wait)
    #TODO!!! wee nedd to perform test, not wait!

    print("[%s] Qemu full boot finished" % common.timestamp())
#    for s in globs.state.samples_list:
#        print s

#was: proceed
#def qemu_mount_disks(options, state):
def temu_mount_disks(args=None):
    options = globs.state.options
    #mount_cdrom(options, options.cdrom)
    read_monitor(options.m)

    if(hasattr(options,'tmp_disk_img')):
        options.slot_shared = common.mount_via_usb(options, options.tmp_disk_img, 'shared')

    time.sleep(1)

    if(hasattr(options,'saved_disk_img')):
        options.slot_saved = common.mount_via_usb(options, options.saved_disk_img, 'saved')

#    for s in globs.state.samples_list:
#        print s

def qemu_mount_disks(args=None):
    options = globs.state.options
    #mount_cdrom(options, options.cdrom)
    read_monitor(options.m)

    if(hasattr(options,'tmp_disk_img')):
        print options.tmp_disk_img
        if(hasattr(options.settings, 'drive_mount')):
            options.slot_shared = common.drive_mount(options, options.tmp_disk_img, 'shared') #hotplug should be completed during bootup
        else:
            options.slot_shared = common.pci_mount(options, options.tmp_disk_img) #hotplug should be completed during bootup

    time.sleep(1)

    if(hasattr(options,'saved_disk_img')):
        print options.saved_disk_img
        if(hasattr(options.settings, 'drive_mount')):
            options.slot_saved = common.drive_mount(options, options.saved_disk_img, 'shared') #hotplug should be completed during bootup
        else:
            options.slot_saved = common.pci_mount(options, options.saved_disk_img) #hotplug should be completed during bootup

#    for s in globs.state.samples_list:
#        print s
    #TODO:replaced by proceed1, maybe I should replace it with specific parts?

    # executed during each fuzzbox start
#    if(defined("settings.specific_preperations_1")):
#        options.settings.specific_preperations_1(options)
#    if(defined("settings.scripts_1")):
#        rss(options.settings.scripts_1, options.m, options.slowdown)

def qemu_mount_disks_wo_virtio(args=None):
    options = globs.state.options
    read_monitor(options.m)

    if(hasattr(options,'tmp_disk_img')):
        options.slot_shared = common.pci_mount_wo_virtio(options, options.tmp_disk_img) #hotplug should be completed during bootup

    time.sleep(1)

    if(hasattr(options,'saved_disk_img')):
        options.slot_saved = common.pci_mount_wo_virtio(options, options.saved_disk_img) #hotplug should be completed during bootup


# was: init
#def qemu_connect_dev_socket(options, state):
def qemu_connect_dev_socket(args=None):
    options = globs.state.options
    state = globs.state

    s = options.s
#    dt = socket.getdefaulttimeout()
#    socket.setdefaulttimeout(options.init_timeout)
#    dts = s.gettimeout()
    s.settimeout(options.init_timeout)

#    while True:
    try:
        print("Waiting for connection....")
        read_socket(options.s)
        state.initialized = True
        print 'Initialized: %s' % state.initialized
    except socket.timeout:
        print("Accpet timed out")
        state.initialized = False
        print "raising machine error"
        print 'Initialized: %s' % state.initialized
#        raise MachineError
    s.settimeout(options.settings.fuzzbox_timeout)
    #trying infinite
    #s.settimeout(None)

def qemu_connect_dev_socket_infinite(args=None):
    options = globs.state.options
    state = globs.state

    s = options.s
#    dt = socket.getdefaulttimeout()
#    socket.setdefaulttimeout(options.init_timeout)
#    dts = s.gettimeout()
    s.settimeout(options.init_timeout)

#    while True:
    try:
        print("Waiting for connection...")
        read_socket(options.s)
        state.initialized = True
        print 'Initialized: %s' % state.initialized
    except socket.timeout:
        print("Accpet timed out")
        state.initialized = False
        print "raising machine error"
        print 'Initialized: %s' % state.initialized
#        raise MachineError
    #s.settimeout(options.settings.fuzzbox_timeout)
    #trying infinite
    s.settimeout(None)

def qemu_umount_disks(args=None):
    #move from poweroff
    pass

def temu_umount_disks(args=None):
    common.pci_umount('0.1')
#    common.pci_umount(options.slot_saved)

def temu_poweroff_no_revert(args=None):
    options = globs.state.options
    
    try:
        print("Shutting down s")
        options.s.shutdown(socket.SHUT_RDWR)
        options.s.close()
    except Exception:
        print("Error shutting down s")
        pass
    print("[Powering off]")
    rs("powerdown", options.m)
    options.shutting_down = True
    time.sleep(options.shutdown_wait)
    options.m = None
    print("Last batch: %s" % options.tmp_disk_img)
    print("Last saved: %s" % options.saved_disk_img)

def quit(args=None):
    options = globs.state.options
    
    options.shutting_down.set()

    write_socket(options.s, "logStop")
    if(hasattr(options, 'tmp_mountpoint')):
        common.del_mountpoint(options)

    try:
        common.pci_umount(options.slot_shared)
        common.pci_umount(options.slot_saved)
    except Exception:
        print("Problems unmounting pci")

    rs("quit", options.m)
    import os

    options.ms.close()
    options.ss.close()
    options.ss_log.close()
    if(os.path.exists(options.ms_path)): os.remove(options.ms_path)
    if(os.path.exists(options.ss_path)): os.remove(options.ss_path)
    if(os.path.exists(options.ss_path+'-log')): os.remove(options.ss_path+'-log')


def poweroff_no_revert(args=None):
    options = globs.state.options
    
    options.shutting_down.set()

    write_socket(options.s, "logStop")
    common.del_mountpoint(options)

    try:
        common.pci_umount(options.slot_shared)
        common.pci_umount(options.slot_saved)
    except Exception:
        print("Problems unmounting pci")
    try:
        print("Shutting down s")
        options.s.shutdown(socket.SHUT_RDWR)
        options.s.close()
    except Exception:
        print("Error shutting down s")
        pass
    print("[Powering off]")
    rs("powerdown", options.m)
    time.sleep(options.shutdown_wait)
    print("Shutdown: %s" % options.shutting_down.is_set())
    options.m = None
    print("Last batch: %s" % options.tmp_disk_img)
    print("Last saved: %s" % options.saved_disk_img)

def check_cr(options, state):
    if(state.got_cr):
        os.rename(options.saved_disk_img, "cr-%s" % options.saved_disk_img)
        options.saved_disk_img = "cr-%s" % options.saved_disk_img
        state.got_cr = False
    else:
        os.remove(options.saved_disk_img)
    if(options.settings.save_shared == False):
        os.remove(options.tmp_disk_img)

def poweroff_revert(options, state):
    write_socket(options.s, "logStop")
    common.del_mountpoint(options)

    try:
        common.pci_umount(options.slot_shared)
        common.pci_umount(options.slot_saved)
    except Exception:
        print("Problems unmounting pci")
    try:
        print("Shutting down s")
        options.s.shutdown(socket.SHUT_RDWR)
        options.s.close()
    except Exception:
        print("Error shutting down s")
        pass
    print("[Powering off]")
    rs("quit", options.m)
    options.m = None
    print("Last batch: %s" % options.tmp_disk_img)
    print("Last saved: %s" % options.saved_disk_img)

def offline_revert(args=None):
    options = globs.state.options
    state = globs.state
    print("Reverting to: %s" % options.settings.revert_snapshot)
    os.spawnv(os.P_WAIT, "/usr/bin/qemu-img", ["qemu-img", "snapshot", "-a", options.settings.revert_snapshot, options.machines + '/' + options.hda])
    
def revert_ready(args=None):
    options = globs.state.options
    state = globs.state
    
    if(hasattr(options.settings, 'qemu_revert_ready')):
        if(options.settings.qemu_revert_ready == True):
            rs("load_ready", options.m)
    else:
        print("Not configured")

def shutdown(options, state):
    options.log.close()
    options.s.close()
    common.powerofff(options)
    print("Finished")
    exit()

def revert(options):
    options = globs.state.options

    print("[Reverting]")
#    rs("load_ready", options.m)
    rss(options.settings.revert_scripts, options.m, options.slowdown)

def is_socket_connected(args = None):
    options = globs.state.options
    state = globs.state

    print 'Initialized: %s' % state.initialized

    if(state.initialized == True):    
        return "Y"
    else:
        return "N"


