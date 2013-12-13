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
Operator
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

(options, args) = parser.parse_args()

fuzzbox_name = sys.argv[1]
if(options.hda is None):
    options.hda = settings.machines[fuzzbox_name]['disk']
if(options.fuzzbox_ip is None):
    options.fuzzbox_ip = settings.machines[fuzzbox_name]['ip'] 
if(options.fuzzbox_port is None):
    options.fuzzbox_port = settings.machines[fuzzbox_name]['port'] 

#qemu settings
qemu_args =  ['qemu-system-i386']
qemu_args += ['-m', options.qemu_m]
qemu_args += ['-hda', options.machines + "/" + options.hda]
if(options.hdb is not None):
    qemu_args += ['-hdb', options.machines + "/" + options.hdb]
#qemu_args += ['-net', 'nic,model=rtl8139', '-net', 'user,restrict=n,smb=' + settings.qemu_shared_folder + ',hostfwd=tcp:127.0.0.1:' + str(options.fuzzbox_port) + '-:12345']
qemu_args += ['-net', 'nic,model=rtl8139', '-net', 'user,restrict=n,smb=' + settings.qemu_shared_folder + ',guestfwd=tcp:10.0.2.100:12345-tcp:127.0.0.1:' + str(options.fuzzbox_port)]
#qemu_args += ['-net', 'nic,model=rtl8139', '-net', 'user,restrict=n,smb=' + settings.qemu_shared_folder + ',guestfwd=tcp:127.0.0.1:' + str(options.fuzzbox_port) + '-tcp:10.0.0.1:12345']
qemu_args += ['-net', 'nic,model=rtl8139']
if(options.visible == False):
    qemu_args += ['-vnc', settings.machines[fuzzbox_name]['vnc']]
qemu_args += settings.qemu_additional

my_slowdown = float(options.slowdown)
my_logger = logging.getLogger('MyLogger')
my_handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.setLevel(logging.DEBUG)
my_logger.addHandler(my_handler)
my_timeout = 20.0

def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

def testfile(x):
    return os.path.exists(x)

testdir(settings.samples_binned)
testdir(settings.samples_binned + "/hc")
testdir(settings.samples_binned + "/uaf")
testdir(settings.samples_binned + "/unk")

def report(string):
    my_logger.info("[" + settings.log_name + ":" + fuzzbox_name + "] " + string)

def prepare_fuzzbox():
    pass

def read_log_socket(f, s):
    global lastResponse
    while True:
        data = s.recv(settings.buffer_size)
        
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
    return d.strftime("%Y-%m-%d %H:%M")

def read_socket(s):
    global lastResponse
    global status
    global reqScript
    
    lastResponse = ""
    status = ""
    reqScript = ""
    data = ""

    while True:
        data += s.recv(settings.buffer_size)
        
        if(data[-6:] == "-=OK=-"): 
            lastResponse = data[:-6]
            break

    # find status
    off = data.find("Status: ")
    if(off != -1):
        status = data[off+8:off+10]

    print(timestamp())
    print("" + str(data[:-6]))
    print("")
    return (lastResponse, status, reqScript)

def read_socket_q(s):
    global lastResponse
    global status
    global reqScript
    
    status = ""
    data = ""

    while True:
        data += s.recv(settings.buffer_size)
        
        if(data[-6:] == "-=OK=-"): 
            lastResponse = data[:-6]
            break

    # find status
    off = data.find("Status: ")
    if(off != -1):
        status = data[off+8:off+10]

    print(timestamp())
    print("OK")
    return lastResponse

def write_socket(s, data):
    print(timestamp() + "> " + str(data))
    s.send(data + "-=OK=-")

def powerofff():
    print("[Powering off]")
    global m
    rs("quit", m)
#    m.send_signal(signal.SIGKILL)
    m = None

def revert():
    print("[Reverting]")
    global m
    #rs("load_ready", m)
    rs(settings.revert_script, m, options.slowdown)

def start():
    print("[Starting]")
    global m
#    print qemu_args
    m = Popen(qemu_args, stdout=PIPE, stdin=PIPE)
    time.sleep(3)
    revert()
    proceed1()
    return m

def restart():
    powerofff()
    time.sleep(3)
    start()

#moved to proceed5
def close_sample():
    global m
    runscriptq(settings.closing_plugin_name, m)

def prepare_con():
    ss = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ss.settimeout(20.0)
    ss.bind(("127.0.0.1", options.fuzzbox_port))
    ss.listen(3)
    return ss

def accept_con(ss):
    s, addr = ss.accept()
    init()
    print("Connected")
    return s

def init():
    #banner
    # we might have some trobules here, its first read
    print("Waiting for connection...")
    read_socket(s)

def killLast():
    write_socket(s, "killLast")
    read_socket(s)

def proceed1():
    # executed during each fuzzbox start
    if(defined("settings.specific_preperations_1")):
        settings.specific_preperations_1(options)
    if(defined("settings.scripts_1")):
        rss(settings.scripts_1, m, my_slowdown)
    rss(["python_server_spawn"], m, my_slowdown)

def proceed2():
    # executed during each guest system restart
    if(defined("settings.specific_preperations_2")):
        settings.specific_preperations_2(options)
    if(defined("settings.scripts_2")):
        rss(settings.scripts_2, m, my_slowdown)

    write_socket(s, "killExplorer")
    read_socket(s)

    write_socket(s, "startBinner")
    read_socket(s)
    return True

def proceed3():
    if(defined("settings.specific_preperations_3")):
        settings.specific_preperations_3(options)
    if(defined("settings.scripts_3")):
        rss(settings.scripts_3, m, my_slowdown)

    write_socket(s, "spawn " + settings.app_path)
    read_socket(s)

    time.sleep(settings.start_sleep)

    write_socket(s, "binTest")
    read_socket(s)

    write_socket(s, "attachBinner " + settings.app_module)
    read_socket(s)

    write_socket(s, "setupMarkers")
    read_socket(s)

    write_socket(s, "installHandlers")
    read_socket(s)

    if(options.slowdown != settings.slowdown):
        write_socket(s, "setupSlowdown {0}".format(options.slowdown))
        read_socket(s)

    if(defined("settings.specific_preperations_4")):
        settings.specific_preperations_4(options)
    if(defined("settings.scripts_4")):
        rss(settings.scripts_4, m, my_slowdown)
    return True

def proceed5():
    if(defined("settings.specific_preperations_5")):
        settings.specific_preperations_5(options)
    if(defined("settings.scripts_5")):
        rss(settings.scripts_5, m, my_slowdown)

def sig1_handler(signum, frame):
    report("Signaled")
        
def sigkill_handler(signum, frame):
    global s
    global ss

    powerofff()

    try:
        print("Shutting down sockets")
        s.shutdown(socket.SHUT_RDWR)
        s.close()
        ss.shutdown(socket.SHUT_RDWR)
        ss.close()
    except Exception:
        pass

    report("Killing")
    quit()

def handle_crashing_sample(sample_path, sample_file):
    global s

    cb = crash_binning()

    print("Crash procedures")

    lines = []
    eip = ""
    reason = ""

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

    write_socket(s, "getSynopsis")
    read_socket(s)
#    cb.import_string(lastResponse)
    print(lastResponse)
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
        f = open(path + "/dossier.txt", "w+")

        write_socket(s, "cbCrashSynopsis")
        read_socket(s)
        synopsis = lastResponse

        f.write(synopsis)
        f.close()

        #dump signatures based on stack

    f = open(path + "/" + sample_file+ ".sig1", "w+")
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


def settle():
    global s 

    write_socket(s, "settle " + str(settings.settle_sleep * settings.slowdown))
    read_socket(s)

def register_script():
    global lastResponse
    global reqScript

    scOff = lastResponse.find("Script: ")
    lineEnd = lastResponse[scOff+8:].find("\n")
    reqScript = lastResponse[scOff+8:scOff+8+lineEnd]
    print("Registered script: %s" % reqScript)

def execute_script(reqScript):
#    global reqScript

    if(reqScript != ""):
        print("Detected predefined conditions, triggering reaction scripts")
        rs(reqScript, m, my_slowdown)
        reqScript = ""


