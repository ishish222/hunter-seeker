from optparse import OptionParser
import settings
import sys
import logging
import logging.handlers
import os
from datetime import datetime
from script import rs, rss, runscriptq, write_monitor
import generators.generatorCorrected as generator
import socket
from subprocess import Popen, PIPE
import time

logo = """
 __                      .__               
|  | ____________________|__| ____   ____  
|  |/ /  _ \_  __ \_  __ \  |/    \ /  _ \ 
|    <  <_> )  | \/|  | \/  |   |  (  <_> )
|__|_ \____/|__|   |__|  |__|___|  /\____/ 
     \/                          \/        
"""
print(logo)

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
    # global options can be used only in this file for reporting & signal handling
    global options

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
#    parser.add_option("-i", "--ip",             dest="fuzzbox_ip", help="Force fuzzbox ip (normally based on hda)")
#    parser.add_option("-E", "--server_ip",      dest="server_ip", help="Force server ip (normally based on hda)")
#    parser.add_option("-p", "--server_port",    dest="server_port", help="Force server port (normally based on hda)")
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
    parser.add_option("-k", "--taskset",      action="store_true", dest="use_taskset", help="Use taskset", default=settings.use_taskset)
    parser.add_option("-t", "--to-mult-factor", dest="to_mult_factor", help="Factor for calculating SO timeout based on TO (SO=TO*factor)", default=settings.to_mult_factor)
    parser.add_option("-w", "--boot-wait", dest="boot_wait", help="How long does this system boot", default=settings.boot_wait)
    parser.add_option("-W", "--shutdown-wait", dest="shutdown_wait", help="How long does this system shutdown", default=settings.shutdown_wait)


    (options, args) = parser.parse_args()

    options.fuzzbox_name = sys.argv[1]
    if(options.hda is None):
        options.hda = settings.machines[options.fuzzbox_name]['disk']
#    if(options.fuzzbox_ip is None):
#        options.fuzzbox_ip = settings.machines[options.fuzzbox_name]['ip'] 
#    if(options.fuzzbox_port is None):
#        options.fuzzbox_port = settings.machines[options.fuzzbox_name]['port'] 

#    options.fuzzbox_ip = settings.machines[options.fuzzbox_name]['ip'] 
#    options.server_ip = settings.machines[options.fuzzbox_name]['server_ip'] 
#    options.server_port = settings.machines[options.fuzzbox_name]['server_port'] 
    
#    options.tap = settings.machines[options.fuzzbox_name]['tap']
#    options.mac = settings.machines[options.fuzzbox_name]['mac']

    #qemu settings
    if(options.use_taskset is True):
        qemu_args =  ['taskset', '-c', settings.machines[options.fuzzbox_name]['taskset'], 'qemu-system-i386']
    else:
        qemu_args =  ['qemu-system-i386']
    qemu_args += ['-m', options.qemu_m]
    qemu_args += ['-drive', 'file=' + options.machines + '/' + options.hda + ',cache=none,if=virtio']
    if(options.hdb is not None):
        qemu_args += ['-drive', 'file=', './' + options.hdb, ',cache=none,if=vitrio']

    if(options.cdrom is not None):
        qemu_args += ['-cdrom', options.cdrom]

    options.ms_path = settings.machines[options.fuzzbox_name]['monitor']
    options.ss_path = settings.machines[options.fuzzbox_name]['serial']
#    qemu_args += ['-net', 'nic,model=virtio', '-net', 'tap,ifname='+options.tap+',script=no,downscript=no']
#    qemu_args += ['-net', 'nic,model=virtio', '-net', 'user,restrict=n,guestfwd=tcp:10.0.2.100:12345-tcp:127.0.0.1:' + str(options.fuzzbox_port)]
#    qemu_args += ['-net', 'nic,model=rtl8139', '-net', 'tap,ifname='+options.tap+',script=no,downscript=no']

    if(options.visible == False and options.vnc == True):
        qemu_args += ['-vnc', settings.machines[options.fuzzbox_name]['vnc']]
    qemu_args += settings.qemu_additional

    qemu_args += ['-monitor', "unix:%s" % settings.machines[options.fuzzbox_name]['monitor']]
    qemu_args += ['-serial', "unix:%s" % settings.machines[options.fuzzbox_name]['serial']]

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
    options.boot_wait = float(options.boot_wait)
    options.shutdown_wait = float(options.shutdown_wait)
    options.to_mult_factor = float(options.to_mult_factor)
    options.fuzzbox_timeout = float(options.wait_sleep*options.to_mult_factor)

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
    global options
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
#    global lastResponse
#    global status
#    global reqScript
    
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
#    global lastResponse
#    global status
#    global reqScript
    
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
    print(timestamp() + "> " + str(data))
    s.send(data + "-=OK=-")

def powerofff(options):
#    try:
#        print("Shutting down ss")
#        options.ss.shutdown(socket.SHUT_RDWR)
#        options.ss.close()
#    except Exception:
#        print("Error shutting down ss")
#        pass
    try:
        print("Shutting down s")
        options.s.shutdown(socket.SHUT_RDWR)
        options.s.close()
    except Exception:
        print("Error shutting down s")
        pass
    print("[Powering off]")
    rs("powerdown", options.m)
    options.m = None
    time.sleep(options.shutdown_wait)

def revert(options):
    print("[Reverting]")
    #rs("load_ready", m)
    rss(options.settings.revert_scripts, options.m, options.slowdown)

def start(options):
    print("[%s] Starting" % timestamp())
    print options.qemu_args
    m = Popen(options.qemu_args, stdout=PIPE, stdin=PIPE)
    time.sleep(3)
    options.m = m
#    revert(options)
#    time.sleep(60)
#    rs("open_cmd", options.m)
    return m

def restart(options):
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
    ss = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#    ss.bind((options.server_ip, options.server_port))
    ss.bind(("127.0.0.1", options.fuzzbox_port))
    ss.listen(3)
    return ss

def wait_for_init(s):
    dt = socket.getdefaulttimeout()
    socket.setdefaulttimeout(80)
    dts = s.gettimeout()
    s.settimeout(80)
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
    s.settimeout(options.fuzzbox_timeout)
    print("Socket timeout set to: %f" % options.fuzzbox_timeout)
    options.s.settimeout(options.fuzzbox_timeout)
    return options.s

def accept_con(ss):
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



