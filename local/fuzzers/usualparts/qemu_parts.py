from subprocess import Popen, PIPE
import socket
import time
from script import rs, rss, runscriptq, write_monitor, write_monitor_2, read_monitor
import sys
sys.path += ["../../common"]
import common
import signal
import os

report = common.report
read_socket = common.read_socket
write_socket = common.write_socket

class statistics(object):
    def __init__(self, metrics):
        self.metric_res = metrics
        self.sample_count = 0
        self.to_count = 0
        self.ma_count = 0
        self.last_time_check = time.localtime()

def qemu_prepare(options, state):
    global signaled
    signaled = False

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


def qemu_start_full(options, state):
    options.log.write("[%s]\n" % options.tmp_disk_img)
    options.log.flush()
    print("Spawning fuzz for batch: %s" % options.tmp_disk_img)

    print("[%s] Starting" % common.timestamp())
    print options.qemu_args
    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE)
    time.sleep(3)
    options.m, _ = options.ms.accept()
    options.s, _ = options.ss.accept()

#    time.sleep(options.boot_wait)
    #TODO!!! wee nedd to perform test, not wait!

    print("[%s] Qemu full boot finished" % common.timestamp())

def qemu_start_revert(options, state):
    options.log.write("[%s]\n" % options.tmp_disk_img)
    options.log.flush()
    print("Spawning fuzz for batch: %s" % options.tmp_disk_img)

    print("[%s] Starting" % common.timestamp())
    print options.qemu_args
    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE)
    time.sleep(3)
    options.m, _ = options.ms.accept()
    options.s, _ = options.ss.accept()

    common.revert(options)
    time.sleep(options.revert_wait)
    #TODO!!! additional wait here isn't so bad, is it?

    print("[%s] Qemu revert boot finished" % common.timestamp())

#was: proceed
def qemu_mount_disks(options, state):
    #mount_cdrom(options, options.cdrom)
    read_monitor(options.m)
    options.slot_shared = common.pci_mount(options, options.tmp_disk_img) #hotplug should be completed during bootup
    time.sleep(1)
    options.slot_saved = common.pci_mount(options, options.saved_disk_img) #hotplug should be completed during bootup

    #TODO:replaced by proceed1, maybe I should replace it with specific parts?

    # executed during each fuzzbox start
#    if(defined("settings.specific_preperations_1")):
#        options.settings.specific_preperations_1(options)
#    if(defined("settings.scripts_1")):
#        rss(options.settings.scripts_1, options.m, options.slowdown)

# was: init
def qemu_connect_dev_socket(options, state):
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
#            raise socket.timeout
#            proceed1(options)
#            os.spawnv(os.P_WAIT, "/bin/ping", ["ping", options.fuzzbox_ip, "-c1"])
#            continue
#    socket.setdefaulttimeout(dt)
    s.settimeout(options.settings.fuzzbox_timeout)

#    print("Connected")
#    socket.setdefaulttimeout(dt)
#    s.settimeout(options.fuzzbox_timeout)
#    print("Socket timeout set to: %f" % options.fuzzbox_timeout)
#    options.s.settimeout(options.fuzzbox_timeout)
#    return options.s

#    try:
#        common.wait_for_init(options.s)
#    except socket.timeout:
#        print("Socket timeout, restarting")
#        report("Socket timeout, restarting")
#        common.write_socket(options.s, "logStop")
#        common.del_mountpoint(options)
#        common.powerofff(options)
#        if(not options.save_disks):
#           os.remove(options.tmp_disk_img)
#        state.initialized = False
#        return
#    state.initialized = True

def poweroff_no_revert(options, state):
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

def shutdown(options, state):
    options.log.close()
    options.s.close()
    common.powerofff(options)
    print("Finished")
    exit()

