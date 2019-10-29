from optparse import OptionParser
import sys
import logging
import logging.handlers
import os
from datetime import datetime
from script import rs, rss, runscriptq, write_monitor, write_monitor_2
import socket
from subprocess import Popen, PIPE
import time
import tempfile
import globs


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
    from optparse import OptionParser
    import sys
    import globs
    import default

    class MyOptionParser(OptionParser):
        def error(self, msg):
            pass

    parser = MyOptionParser()

    # pobranie z default
    globs.state.options = default

    # przeslaniane

    parser.add_option("-M", "--machines",       dest="external_qemu_machines", help="Machines path", default=default.external_qemu_machines)
    parser.add_option("-m", "--m",              dest="external_qemu_memory_options", help="Amount of memory", default=default.external_qemu_memory_options)
    parser.add_option("-N", "--vnc",            action="store_true", dest="external_qemu_use_vnc", help="Use VNC", default=default.external_qemu_use_vnc)
    parser.add_option("-k", "--taskset",        action="store_true", dest="external_qemu_use_taskset", help="Use taskset", default=default.external_qemu_use_taskset)
    parser.add_option("-a", "--hda",            dest="hda", help="First disk")
    parser.add_option("-b", "--hdb",            dest="hdb", help="Second disk")
    parser.add_option("-c", "--cdrom",          dest="cdrom", help="CD")

    (cmdline_options, args) = parser.parse_args()

    options = globs.state.options

    globs.state.options.external_qemu_machines = cmdline_options.external_qemu_machines
    globs.state.options.external_qemu_memory_options = cmdline_options.external_qemu_memory_options
    globs.state.options.external_qemu_use_vnc = cmdline_options.external_qemu_use_vnc
    globs.state.options.external_qemu_use_taskset = cmdline_options.external_qemu_use_taskset

    globs.state.options.external_script_name = sys.argv[len(sys.argv) -2]
    globs.state.options.external_machine_name = sys.argv[len(sys.argv) -3]
    globs.state.options.external_machine = options.external_qemu_machines[options.external_machine_name]
    globs.state.out_folder = sys.argv[len(sys.argv) -1]

    globs.state.options.external_paths_final_output = globs.state.options.external_paths_final_all_output + '/' + globs.state.out_folder

def get_qemu_cmdline_unrestricted():
    options = globs.state.options
    commandline = []
    machine = options.external_machine

    if(options.external_qemu_use_taskset):
        commandline += ['taskset', '-c', machine['taskset']]
 
    commandline += [options.external_qemu_binary_path]

    commandline += ['-m', options.external_qemu_memory_options]

    drive_path = options.external_paths_machines + '/' + machine['disk']

    if(hasattr(options, 'external_qemu_drive_options')):
        drive_cmdline = 'file=' + drive_path + ',' + options.external_qemu_drive_options
    else:
        drive_cmdline = 'file=' + drive_path 
        
    commandline += ['-drive', drive_cmdline]

    # mount external folder via smb

    commandline += ['-net', 'nic,model=rtl8139', '-net', 'user,smb=%s' % options.external_paths_tmp_input_output]

    # monitor and serials

    commandline += ['-monitor', 'unix:%s' % machine['monitor']]
    commandline += ['-serial', 'unix:%s' % machine['serial']]
    commandline += ['-serial', 'unix:%s' % machine['serial'] + '-log']

    if(hasattr(options, 'external_qemu_smp')):
        commandline += ['-smp', '%s' % options.external_qemu_smp]


    if(hasattr(options, 'external_qemu_additional_options')):
        for option in options.external_qemu_additional_options:
            commandline += [option]

    if(options.external_qemu_use_vnc):
        commandline += ['-vnc', machine['vnc']]

    return commandline
    
def get_qemu_cmdline():
    options = globs.state.options
    commandline = []
    machine = options.external_machine

    if(options.external_qemu_use_taskset):
        commandline += ['taskset', '-c', machine['taskset']]
 
    commandline += [options.external_qemu_binary_path]

    commandline += ['-m', options.external_qemu_memory_options]

    drive_path = options.external_paths_machines + '/' + machine['disk']

    if(hasattr(options, 'external_qemu_drive_options')):
        drive_cmdline = 'file=' + drive_path + ',' + options.external_qemu_drive_options
    else:
        drive_cmdline = 'file=' + drive_path 
        
    commandline += ['-drive', drive_cmdline]

    # mount external folder via smb

    commandline += ['-net', 'nic,model=rtl8139', '-net', 'user,smb=%s,restrict' % options.external_paths_tmp_input_output]

    # monitor and serials

    commandline += ['-monitor', 'unix:%s' % machine['monitor']]
    commandline += ['-serial', 'unix:%s' % machine['serial']]
    commandline += ['-serial', 'unix:%s' % machine['serial'] + '-log']

    if(hasattr(options, 'external_qemu_smp')):
        commandline += ['-smp', '%s' % options.external_qemu_smp]


    if(hasattr(options, 'external_qemu_additional_options')):
        for option in options.external_qemu_additional_options:
            commandline += [option]


    if(options.external_qemu_use_vnc):
        commandline += ['-vnc', machine['vnc']]

    return commandline
    
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
        data = s.recv(options.settings.buffer_size).decode('utf-8')
        
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

    lastResponse, status, reqScript = read_socket_q(s)
    print("%s" % (lastResponse))
    return (lastResponse, status, reqScript)

def read_socket_q(s):
    options = globs.state.options

    lastResponse = ""
    status = ""
    reqScript = ""
    data = ""

    while True:
        data += s.recv(1).decode('utf-8')
        
        if(data[-6:] == "-=OK=-"): 
            lastResponse = data[:-6]
            break

    # this is ugly, might cause trouble
    if(lastResponse[1] == 'R'):
        status = lastResponse[1:3]
    else:
        status = 'RI'

    return (lastResponse, status, reqScript)

def write_socket(s, data):
    options = globs.state.options

#    print(timestamp() + "> " + str(data))
    print(("> " + str(data)))
    s.send((data + "-=OK=-").encode('utf-8'))

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
    print(("Last batch: %s" % options.tmp_disk_img))
    print(("Last saved: %s" % options.saved_disk_img))

def revert(options):
    options = globs.state.options

    print("[Reverting]")
#    rs("load_ready", options.m)
    rss(options.settings.revert_scripts, options.m, options.slowdown)

def start(options):
    options = globs.state.options

    print(("[%s] Starting" % timestamp()))
    print(options.qemu_args)
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

def prepare_pipe(path):
    print('Preparing pipe: %s' % path)
    ms = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    if(os.path.exists(path)): os.remove(path)
    ms.bind(path)
    ms.listen(3)
    return ms

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
    print(("Socket timeout set to: %f" % options.fuzzbox_timeout))
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
    print(("Socket timeout set to: %f" % options.fuzzbox_timeout))
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
    return True

def proceed4(options):
    if(defined("settings.specific_preperations_4")):
        options.settings.specific_preperations_4(options)
    if(defined("settings.scripts_4")):
        rss(options.settings.scripts_4, options.m, options.slowdown)

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
    print(("SU:\n%s" % dossier[suOff:suOff+suEnd]))

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
    print(("Registered script: %s" % reqScript))

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

def drive_umount(options, filee, idd):
    write_monitor_2(options.m, 'device_rem %s-virtio' % idd)

def pci_mount(options, filee):
    dev_str = write_monitor_2(options.m, "pci_add auto storage file=%s,if=virtio" % filee)
    if(dev_str.find("could not open disk image:") > -1):
        print(dev_str)
        return
    slot_off = dev_str.find("slot ") + 5
    slot = int(dev_str[slot_off])
    print(("PCI dev mounted in slot: " + str(slot)))
    return slot

def pci_mount_wo_virtio(options, filee):
    dev_str = write_monitor_2(options.m, "pci_add auto storage file=%s,if=none" % filee)
    if(dev_str.find("could not open disk image:") > -1):
        print(dev_str)
        return
    slot_off = dev_str.find("slot ") + 5
    slot = int(dev_str[slot_off])
    print(("PCI dev mounted in slot: " + str(slot)))
    return slot

def pci_umount(options, slot):
    write_monitor_2(options.m, "pci_del %d" % slot)
#    read_monitor(options.m)

def mount_drive(options, offset=None):
    print(("Mounting %s" % options.tmp_mountpoint))
    if(offset == None):
        os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mount", "-o", "loop,umask=0000", options.tmp_disk_img, options.tmp_mountpoint])
    else:
        os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mount", "-o", "loop,umask=0000,offset=%s" % offset, options.tmp_disk_img, options.tmp_mountpoint])

def umount_drive(options):
    print(("Umounting %s" % options.tmp_mountpoint))
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "umount", options.tmp_mountpoint])

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

def create_mountpoint(options):
    options.tmp_mountpoint = tempfile.mktemp()
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", options.tmp_mountpoint])
    print(("Created mountpoint: %s" % options.tmp_mountpoint))

def del_mountpoint(options):
    os.spawnv(os.P_WAIT, "/bin/rm", ["rm", "-rf", options.tmp_mountpoint])
    print(("Removed mountpoint: %s" % options.tmp_mountpoint))
    
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
    os.spawnv(os.P_WAIT, "/bin/cp", ["cp", options.origin, '%s/%s' % (options.tmp_mountpoint, options.settings.samples_shared_subdir)])

    generator_mod, generator_class = options.generator.split(".")
    mutator_mod, mutator_class = options.mutator.split(".")
    
    import_stat = "import generators.%s as genmod" % generator_mod
    exec(import_stat)
    assign_stat = "genclass = genmod.%s" % generator_class
    exec(assign_stat)

    import_stat = "import generators.%s as mutmod" % mutator_mod
    exec(import_stat)
    assign_stat = "mutclass = mutmod.%s" % mutator_class
    exec(assign_stat)

    try:
        my_generator = genclass(options.origin, options.tmp_mountpoint+"/samples/shared", mutator_=mutclass, corrector=None)
        my_generator.mutations=int(options.mutations)
        samples_list += my_generator.generate(options.samples_count)
    except Exception as e:
    
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


