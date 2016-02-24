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

def qemu_prepare_pipes():
    options = globs.state.options

    options.ms = common.prepare_monitor(options.ms_path)
    options.ss = common.prepare_serial(options.ss_path)
    options.ss2 = common.prepare_serial(options.ss_path + '2')


def qemu_ready():
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


def qemu_start_full():
    options = globs.state.options

    if(hasattr(options, 'tmp_disk_img')):
        options.log.write("[%s]\n" % options.tmp_disk_img)
        options.log.flush()
        print("Spawning fuzz for batch: %s" % options.tmp_disk_img)

    print("[%s] Starting" % common.timestamp())
    print " ".join(options.qemu_args)

    myErr = open("./err", "w+")

    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE, stderr=myErr.fileno(), env=os.environ)
    time.sleep(3)
    options.m, _ = options.ms.accept()
    options.s, _ = options.ss.accept()

#    time.sleep(options.boot_wait)
    #TODO!!! wee nedd to perform test, not wait!

    print("[%s] Qemu full boot finished" % common.timestamp())
#    for s in globs.state.samples_list:
#        print s

def qemu_start_revert():
    options = globs.state.options

    if(hasattr(options, 'tmp_disk_img')):
        options.log.write("[%s]\n" % options.tmp_disk_img)
        options.log.flush()
        print("Spawning fuzz for batch: %s" % options.tmp_disk_img)

    print("[%s] Starting" % common.timestamp())
    print " ".join(options.qemu_args)

    myErr = open("./err", "w+")

    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE, stderr=myErr.fileno(), env=os.environ)
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
def temu_mount_disks():
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

def qemu_mount_disks():
    options = globs.state.options
    #mount_cdrom(options, options.cdrom)
    read_monitor(options.m)

    if(hasattr(options,'tmp_disk_img')):
        print options.tmp_disk_img
        options.slot_shared = common.pci_mount(options, options.tmp_disk_img) #hotplug should be completed during bootup
#        options.slot_shared = common.drive_mount(options, options.tmp_disk_img, 'shared') #hotplug should be completed during bootup

    time.sleep(1)

    if(hasattr(options,'saved_disk_img')):
        print options.saved_disk_img
        options.slot_saved = common.pci_mount(options, options.saved_disk_img) #hotplug should be completed during bootup
#        options.slot_saved = common.drive_mount(options, options.saved_disk_img, 'saved') #hotplug should be completed during bootup

#    for s in globs.state.samples_list:
#        print s
    #TODO:replaced by proceed1, maybe I should replace it with specific parts?

    # executed during each fuzzbox start
#    if(defined("settings.specific_preperations_1")):
#        options.settings.specific_preperations_1(options)
#    if(defined("settings.scripts_1")):
#        rss(options.settings.scripts_1, options.m, options.slowdown)

def qemu_mount_disks_wo_virtio():
    options = globs.state.options
    read_monitor(options.m)

    if(hasattr(options,'tmp_disk_img')):
        options.slot_shared = common.pci_mount_wo_virtio(options, options.tmp_disk_img) #hotplug should be completed during bootup

    time.sleep(1)

    if(hasattr(options,'saved_disk_img')):
        options.slot_saved = common.pci_mount_wo_virtio(options, options.saved_disk_img) #hotplug should be completed during bootup


# was: init
#def qemu_connect_dev_socket(options, state):
def qemu_connect_dev_socket():
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
    except socket.timeout:
        print("Accpet timed out")
        state.initialized = False
        print "raising machine error"
        raise MachineError
    s.settimeout(options.settings.fuzzbox_timeout)
    #trying infinite
    #s.settimeout(None)

def qemu_connect_dev_socket_infinite():
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
    except socket.timeout:
        print("Accpet timed out")
        state.initialized = False
        print "raising machine error"
        raise MachineError
    #s.settimeout(options.settings.fuzzbox_timeout)
    #trying infinite
    s.settimeout(None)

def qemu_umount_disks():
    #move from poweroff
    pass

def temu_umount_disks():
    common.pci_umount('0.1')
#    common.pci_umount(options.slot_saved)

def temu_poweroff_no_revert():
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
    time.sleep(options.shutdown_wait)
    options.m = None
    print("Last batch: %s" % options.tmp_disk_img)
    print("Last saved: %s" % options.saved_disk_img)


def poweroff_no_revert():
    options = globs.state.options
    
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

def offline_revert():
    options = globs.state.options
    state = globs.state
    os.spawnv(os.P_WAIT, "/usr/bin/qemu-img", ["qemu-img", "snapshot", "-a", "clean", options.machines + '/' + options.hda])
    

def shutdown(options, state):
    options.log.close()
    options.s.close()
    common.powerofff(options)
    print("Finished")
    exit()

