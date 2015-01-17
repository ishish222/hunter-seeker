from optparse import OptionParser
import settings
import sys
import logging
import logging.handlers
import os
from datetime import datetime
from script import rs, rss, runscriptq, write_monitor, write_monitor_2
import generators.generatorCorrected as generator
import socket
from subprocess import Popen, PIPE
import time
import tempfile
import usualparts.globs as globs
options = globs.state.options


def windows_escape(txt):
#    return windows_slashes(txt.replace(" ", "\\ "))
    return windows_slashes(txt)

def windows_slashes(txt):
    return txt.replace("/", "\\")

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

class ErrorDetectedException(Exception):
    pass

def get_options():
    parser = OptionParser()
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

    (options, args) = parser.parse_args()

    options.fuzzbox_name = sys.argv[1]
    if(options.hda is None):
        options.hda = settings.machines[options.fuzzbox_name]['disk']

    #qemu settings
    if(options.use_taskset is True):
        qemu_args =  ['taskset', '-c', settings.machines[options.fuzzbox_name]['taskset'], settings.qemu_command]
    else:
        qemu_args =  [settings.qemu_command]
    qemu_args += ['-m', options.qemu_m]
    qemu_args += ['-drive', 'file=' + options.machines + '/' + options.hda + ',cache=none,if=virtio']
    if(options.hdb is not None):
        qemu_args += ['-drive', 'file=', './' + options.hdb, ',cache=none,if=vitrio']

    if(options.cdrom is not None):
        qemu_args += ['-cdrom', options.cdrom]

    options.ms_path = settings.machines[options.fuzzbox_name]['monitor']
    options.ss_path = settings.machines[options.fuzzbox_name]['serial']

    if(options.visible == False and options.vnc == True):
        qemu_args += ['-vnc', settings.machines[options.fuzzbox_name]['vnc']]
    qemu_args += settings.qemu_additional

    qemu_args += ['-monitor', "unix:%s" % settings.machines[options.fuzzbox_name]['monitor']]
    qemu_args += ['-serial', "unix:%s" % settings.machines[options.fuzzbox_name]['serial']]
    qemu_args += ['-smp', str(options.smp)]

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

    #thats right bitches
    options.settings = settings

    return(options)
    
def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

def testfile(x):
    return os.path.exists(x)

def create_sample_dirs(options):
    testdir(options.settings.samples_binned)
    testdir(options.settings.samples_binned + "/hc")
    testdir(options.settings.samples_binned + "/uaf")
    testdir(options.settings.samples_binned + "/unk")

def report(string):
    options = globs.state.options
    options.logger.info("[" + settings.log_name + ":" + options.fuzzbox_name + "] " + string)

def prepare_fuzzbox():
    pass

def read_log_socket(f, s):
    global lastResponse
    while True:
        data = s.recv(options.settings.buffer_size)
        
        f.write(data)
        if(data == "-=OK=-"): 
            f.write("\n")
            f.flush()
            break
        else: 
            lastResponse = data
    return lastResponse

def timestamp():
    d=datetime.now()
    return d.strftime("%Y-%m-%d %H:%M:%S:%f")

def timestamp2():
    d=datetime.now()
    return d.strftime("%Y-%m-%d-%H-%M-%S-%f")

def read_socket(s):
    options = globs.state.options

    lastResponse = ""
    status = ""
    reqScript = ""
    data = ""

    while True:
        data += s.recv(options.settings.buffer_size)
        
        if(data[-6:] == "-=OK=-"): 
            lastResponse = data[:-6]
            break

    # find status
    off = data.find("Status: ")
    if(off != -1):
        status = data[off+8:off+10]
        if(status == "SR"):
            scOff = data.find("Script: ")
            lineEnd = data[scOff+8:].find("\n")
            reqScript = data[scOff+8:scOff+8+lineEnd]

    print(timestamp())
    print("" + str(data[:-6]))
    print("")
    return (lastResponse, status, reqScript)

def read_socket_q(s):
    options = globs.state.options

    status = ""
    data = ""

    while True:
        data += s.recv(options.settings.buffer_size)
        
        if(data[-6:] == "-=OK=-"): 
            lastResponse = data[:-6]
            break

    # find status
    off = data.find("Status: ")
    if(off != -1):
        status = data[off+8:off+10]
        if(status == "SR"):
            scOff = data.find("Script: ")
            lineEnd = data[scOff+8:].find("\n")
            reqScript = data[scOff+8:scOff+8+lineEnd]

    print(timestamp())
    print("OK")
    return lastResponse

def write_socket(s, data):
    options = globs.state.options

    print(timestamp() + "> " + str(data))
    s.send(data + "-=OK=-")

def powerofff(options):
    options = globs.state.options

    try:
        pci_umount(options.slot_shared)
        pci_umount(options.slot_saved)
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
    if(options.settings.use_snapshots == False):
        rs("powerdown", options.m)
        time.sleep(options.shutdown_wait)
    else:
        rs("quit", options.m)
    options.m = None
    print("Last batch: %s" % options.tmp_disk_img)
    print("Last saved: %s" % options.saved_disk_img)

def revert(options):
    options = globs.state.options

    print("[Reverting]")
#    rs("load_ready", options.m)
    rss(options.settings.revert_scripts, options.m, options.slowdown)

def start(options):
    options = globs.state.options

    print("[%s] Starting" % timestamp())
    print options.qemu_args
    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE)
    time.sleep(3)
    options.m, _ = options.ms.accept()
    options.s, _ = options.ss.accept()

    if(options.settings.use_snapshots == True):
        revert(options)
        time.sleep(options.revert_wait)
    else:
        time.sleep(options.boot_wait)
    return m

def restart(options):
    options = globs.state.options

    powerofff(options)
    time.sleep(3)
    start(options)

def prepare_monitor(path):
    ms = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    if(os.path.exists(path)): os.remove(path)
    ms.bind(path)
    ms.listen(3)
    return ms

def prepare_serial(path):
    ms = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    if(os.path.exists(path)): os.remove(path)
    ms.bind(path)
    ms.listen(3)
    return ms

def prepare_con():
    options = globs.state.options

    ss = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#    ss.bind((options.server_ip, options.server_port))
    ss.bind(("127.0.0.1", options.fuzzbox_port))
    ss.listen(3)
    return ss

def wait_for_init(s):
    options = globs.state.options

    dt = socket.getdefaulttimeout()
    socket.setdefaulttimeout(options.init_timeout)
    dts = s.gettimeout()
    s.settimeout(options.init_timeout)
    while True:
        try:
            print("Waiting for init")
            init(options.s)
            break
        except socket.timeout:
            print("Accpet timed out, raising")
#            raise socket.timeout
            proceed1(options)
#            os.spawnv(os.P_WAIT, "/bin/ping", ["ping", options.fuzzbox_ip, "-c1"])
            continue

    print("Connected")
    socket.setdefaulttimeout(dt)
    #s.settimeout(options.fuzzbox_timeout)

    # we have cooldown, it requires long timeout. Lets test infinite
    s.settimeout(None)
    print("Socket timeout set to: %f" % options.fuzzbox_timeout)
    options.s.settimeout(options.fuzzbox_timeout)
    return options.s

def accept_con(ss):
    options = globs.state.options

    dt = socket.getdefaulttimeout()
    socket.setdefaulttimeout(80)
    dtss = ss.gettimeout()
    ss.settimeout(80)

    while True:
        try:
            print("Waiting for accept")
            options.s, addr = ss.accept()
            print("Waiting for init")
            init(options.s)
            break
        except socket.timeout:
            print("Accpet timed out, raising")
#            raise socket.timeout
            proceed1(options)
#            os.spawnv(os.P_WAIT, "/bin/ping", ["ping", options.fuzzbox_ip, "-c1"])
            continue

    print("Connected")
    socket.setdefaulttimeout(dt)
    ss.settimeout(options.fuzzbox_timeout)
    print("Socket timeout set to: %f" % options.fuzzbox_timeout)
    options.s.settimeout(options.fuzzbox_timeout)
    return options.s

def init(s):
    #banner
    # we might have some trobules here, its first read
    print("Waiting for connection...")
    read_socket(options.s)

def killLast(s):
    write_socket(s, "killLast")
    read_socket(s)

def proceed1(options):
    # executed during each fuzzbox start
    if(defined("settings.specific_preperations_1")):
        options.settings.specific_preperations_1(options)
    if(defined("settings.scripts_1")):
        rss(options.settings.scripts_1, options.m, options.slowdown)

def proceed2(options):
    # executed during each OS start
    if(defined("settings.specific_preperations_2")):
        options.settings.specific_preperations_2(options)
    if(defined("settings.scripts_2")):
        rss(options.settings.scripts_2, options.m, options.slowdown)

    s = options.s

    write_socket(s, "killExplorer")
    read_socket(s)

    write_socket(s, "startBinner")
    read_socket(s)
    return True

def proceed3(options):
    s = options.s

    write_socket(s, "spawn " + options.settings.app_path)
    read_socket(s)

    time.sleep(options.settings.start_sleep)

    if(defined("settings.specific_preperations_3")):
        options.settings.specific_preperations_3(options)
    if(defined("settings.scripts_3")):
        rss(options.settings.scripts_3, options.m, options.slowdown)

    write_socket(s, "binTest")
    read_socket(s)

    write_socket(s, "attachBinner " + options.settings.app_module)
    read_socket(s)

    write_socket(s, "setupMarkers")
    read_socket(s)

    write_socket(s, "installHandlers")
    read_socket(s)

    if(options.wait_sleep != options.settings.wait_sleep):
        write_socket(s, "setupWaitSleep {0}".format(options.wait_sleep))
        read_socket(s)

    if(options.slowdown != options.settings.slowdown):
        write_socket(s, "setupSlowdown {0}".format(options.slowdown))
        read_socket(s)

    if(defined("settings.specific_preperations_4")):
        options.settings.specific_preperations_4(options)
    if(defined("settings.scripts_4")):
        rss(options.settings.scripts_4, options.m, options.slowdown)
    return True

def proceed5(options):
    if(defined("settings.specific_preperations_5")):
        options.settings.specific_preperations_5(options)
    if(defined("settings.scripts_5")):
        rss(options.settings.scripts_5, options.m, options.slowdown)

def sig1_handler(signum, frame):
    report("Signaled")
        
def sigkill_handler(signum, frame):
    global options

    write_socket(options.s, "quit")

    powerofff(options)

    report("Killing")
    quit()

def handle_crashing_sample(dossier, sample_path, sample_file):

    print("Crash procedures")

#    os.rename(sample_path, settings.samples_saved + "/" + sample_file)
    df = open(settings.samples_saved + "/" + sample_file + ".dossier", "a")
    df.write(dossier)
    df.close()

    suOff = dossier.find("stack unwind:")
    suEnd = dossier[suOff:].find("\n\n")
    print("SU:\n%s" % dossier[suOff:suOff+suEnd])

#    write_socket(s, "cbStackUnwind")
#    read_socket(s)
#    lines = lastResponse.split("\n")

#    write_socket(s, "cbEip")
#    read_socket(s)
#    eip = lastResponse

#    write_socket(s, "cbReason")
#    read_socket(s)
#    reason = lastResponse

#    path = options.samples_binned

    return

    if(reason == "hc"):
        path += "/hc" + "/" + eip
        testdir(path)
        #hc analysis
    elif(reason == "uaf"):
        path += "/uaf" + "/" + eip
        testdir(path)
    else:
        path += "/unk" + "/" + eip
        testdir(path)

    if(testfile(path + "/dossier.txt") == False):
        f = open(path + "/dossier.txt", "a")

        write_socket(s, "cbCrashSynopsis")
        read_socket(s)
        synopsis = lastResponse

        f.write(synopsis)
        f.close()

        #dump signatures based on stack

    f = open(path + "/" + sample_file+ ".sig1", "a")
    for i in range(0, min(len(lines), 5)):
        f.write(lines[i] + "\n")
    f.close

    f = open(path + "/" + sample_file+ ".sig2", "w+")
    if(len(lines) > 5):
        for i in range(5, min(len(lines), 10)):
            f.write(lines[i] + "\n")
    f.close

    f = open(path + "/" + sample_file+ ".sig3", "w+")
    if(len(lines) > 10):
        for i in range(10, min(len(lines), 15)):
            f.write(lines[i] + "\n")
    f.close

    os.rename(sample_path, path + "/" + sample_file)
    #must be removed, so that the next run wont be affected


def settle(options):
#    write_socket(options.s, "settle " + str(options.settings.settle_sleep * options.settings.slowdown))
#    read_socket(options.s)
    pass

def register_script():
    global lastResponse
    global reqScript

    scOff = lastResponse.find("Script: ")
    lineEnd = lastResponse[scOff+8:].find("\n")
    reqScript = lastResponse[scOff+8:scOff+8+lineEnd]
    print("Registered script: %s" % reqScript)

def execute_script(options, reqScript):
    if(reqScript != ""):
        print("Detected predefined conditions, triggering reaction scripts")
        rs(reqScript, options.m, options.slowdown)
        reqScript = ""


class CrashException(Exception):
    pass

class FuzzingException(Exception):
    pass

def simple_exec(args):
    ans = Popen(args, stdout=PIPE)
    return ans.stdout.readline()[:-1]

def long_exec(args):
    ans = Popen(args, stdout=PIPE)
    return ans.stdout,read()

def create_image(path, size, label):
    simple_exec(["qemu-img", "create", "-f", "raw", path, str(size)])
    loop_dev = simple_exec(["sudo", "losetup", "-f", "--show", path])
    simple_exec(["sudo", "mkfs.ntfs", "-f", "-L", label, "-H", "0", "-S", "0", "-z", "0", "-p", "0", loop_dev])
    time.sleep(2)
    simple_exec(["sudo", "losetup", "-d", loop_dev])
    return path

def mount_via_usb(options, filee, idd):
    dev_str = write_monitor_2(options.m, "usb_add disk:%s" % filee)

def umount_via_usb(options, filee, idd):
    dev_str = write_monitor_2(options.m, "usb_del %s" % filee)

def drive_mount(options, filee, idd):
    dev_str = write_monitor_2(options.m, "drive_add 0 if=none,file=%s,format=raw,id=%s" % (filee, idd))
    if(dev_str.find("OK") < 0):
        print(dev_str)
        return
    write_monitor_2(options.m, 'device_add virtio-blk-pci,drive=%s,id=%s-virtio' % (idd, idd))

def pci_mount(options, filee):
    dev_str = write_monitor_2(options.m, "pci_add auto storage file=%s,if=virtio" % filee)
    if(dev_str.find("could not open disk image:") > -1):
        print(dev_str)
        return
    slot_off = dev_str.find("slot ") + 5
    slot = int(dev_str[slot_off])
    print("PCI dev mounted in slot: " + str(slot))
    return slot

def drive_umount(options, filee, idd):
    write_monitor_2(options.m, 'device_rem %s-virtio' % idd)

def pci_umount(options, slot):
    write_monitor_2(options.m, "pci_del %d" % slot)
#    read_monitor(options.m)

def mount_drive(options, offset=None):
    options.tmp_mountpoint = tempfile.mktemp()
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", options.tmp_mountpoint])
    print("Mounting %s" % options.tmp_mountpoint)
    if(offset == None):
        os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mount", "-o", "loop,umask=0000", options.tmp_disk_img, options.tmp_mountpoint])
    else:
        os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mount", "-o", "loop,umask=0000,offset=%s" % offset, options.tmp_disk_img, options.tmp_mountpoint])

def umount_drive(options):
    print("Umounting %s" % options.tmp_mountpoint)
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "umount", options.tmp_mountpoint])
    os.spawnv(os.P_WAIT, "/bin/rm", ["rm", "-rf", options.tmp_mountpoint])

def calc_disk_size(options):
    origin_size = os.stat(options.origin).st_size
    size_margin  = origin_size * options.samples_size_margin
    origin_size += size_margin
    disk_size = origin_size * (options.samples_count + 1) + options.settings.SERVER_SIZE
    disk_size = max(int(disk_size), options.settings.MIN_DISK_SIZE)
    return disk_size

def create_drive(options, size=None, name=None, label=None):
    if(size == None):
        size = calc_disk_size(options)
    if(size > options.settings.MAX_DISK_SIZE): raise FuzzingException
    if(name == None):
        name = tempfile.mktemp(suffix = "-samples.raw", dir=".")
    if(label == None):
        label = "secondary"
    create_image(name, size, label)
    return name

def del_mountpoint(options):
    os.spawnv(os.P_WAIT, "/bin/rm", ["rm", "-rf", options.tmp_mountpoint])
    print("Removed mountpoint")
    
def create_layout(options):
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/samples/shared"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/samples/saved"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/samples/binned"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/samples/other"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/logs"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/server"])
    # copy server files
    os.spawnv(os.P_WAIT, "/bin/cp", ["cp", "-r", "../server", options.tmp_mountpoint])
    os.spawnv(os.P_WAIT, "/bin/cp", ["cp", "-r", "../common", options.tmp_mountpoint])

def generate(options):
    samples_list = []
    create_layout(options)
    os.spawnv(os.P_WAIT, "/bin/cp", ["cp", options.origin, options.tmp_mountpoint+"/samples/shared"])

    generator_mod, generator_class = options.generator.split(".")
    mutator_mod, mutator_class = options.mutator.split(".")
    
    import_stat = "import generators.%s as genmod" % generator_mod
    exec import_stat
    assign_stat = "genclass = genmod.%s" % generator_class
    exec assign_stat

    import_stat = "import generators.%s as mutmod" % mutator_mod
    exec import_stat
    assign_stat = "mutclass = mutmod.%s" % mutator_class
    exec assign_stat

    try:
        my_generator = genclass(options.origin, options.tmp_mountpoint+"/samples/shared", mutator_=mutclass, corrector=None)
        my_generator.mutations=int(options.mutations)
        samples_list += my_generator.generate(options.samples_count)
    except Exception, e:
    
        print(e)

    print("Generated samples")
    return samples_list

def eject_cdrom(options):
    write_monitor_2(options.m, "eject ide1-cd0")

def mount_cdrom(options, path):
    write_monitor_2(options.m, "change ide1-cd0 %s" % path)

def killHost(options):
    s = options.s
    write_socket(s, "ps")
    read_socket(s)
    write_socket(s, "killHost")
    read_socket(s)
    write_socket(s, "kill dwwin.exe")
    read_socket(s)
    write_socket(s, "kill %s" % options.settings.app_module)
    read_socket(s)
    write_socket(s, "ps")
    read_socket(s)


