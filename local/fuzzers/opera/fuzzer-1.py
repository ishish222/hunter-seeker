#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

import generators.generatorCorrected as generator
import generators.changer as changer
#import correctors.correctorlib
import socket
import os
import logging
import logging.handlers
import time
import sys
import signal
import settings

class ErrorDetectedException(Exception):
    pass

if(len(sys.argv)) < 3:
    print("Podaj nazwe boxa i oryginal")
    quit()

#Configure generator
origin_path = sys.argv[2]
samples_shared_path = settings.samples_shared_path
samples_saved = settings.samples_saved
fuzzbox_name = sys.argv[1]
fuzzbox_ip = settings.ips[fuzzbox_name]
my_logger = logging.getLogger('MyLogger')
my_handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.setLevel(logging.DEBUG)
my_logger.addHandler(my_handler)
my_timeout = 20.0

BAD_ADDR_1 = 0x13518F0 # (0x400000 + 0xf518f0)

if(settings.visible):
    startvm = ["VBoxManage", "startvm", ""]
else:
    startvm = ["VBoxManage", "startvm", "", "--type", "headless"]

poweroff = ["VBoxManage", "controlvm", "", "poweroff"]
restorecurrent = ["VBoxManage", "snapshot", "", "restorecurrent"]
restorestart = ["VBoxManage", "snapshot", "", "restore", "[x] start"]

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
    my_logger.info("[" + settings.log_name + ":" + fuzzbox_name + "] " + string);

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

def read_socket(s):
    global lastResponse
    global status
    
    status = ""
    data = ""

    while True:
        data += s.recv(settings.buffer_size)
        
        if(data[-6:] == "-=OK=-"): 
            lastResponse = data[:-6]
            break

    off = data.find("Status: ")
    if(off != -1):
#        print(data[off:off+10])
        status = data[off+8:off+10]
    
    print("" + str(data[:-6]))
    print("")
    return lastResponse

#def read_socket(s):
#    data = s.recv(buffer_size)
#    print("< " + str(data))
#    return data

def write_socket(s, data):
    print("> " + str(data))
    s.send(data)

def powerofff():
    command = poweroff
    command[2] = fuzzbox_name
    ret = os.spawnv(os.P_WAIT, "/opt/VirtualBox/VBoxManage", command)

def revert():
    command = restorestart
    command[2] = fuzzbox_name
    ret = os.spawnv(os.P_WAIT, "/opt/VirtualBox/VBoxManage", command)
    time.sleep(3)

def start():
    revert()
    command = startvm
    command[2] = fuzzbox_name
    ret = os.spawnv(os.P_WAIT, "/opt/VirtualBox/VBoxManage", command)
    time.sleep(5)

def restart():
    powerofff()
    time.sleep(3)
    start()

def connect():
    global s
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    timeouts = 0
    while(True):
        try:
            s.connect((fuzzbox_ip, settings.fuzzbox_port))
            print("Connected")
            return
        except Exception:
            print("No route to host, waiting")
            timeouts += 1
            if(timeouts > 3):
                print("Unable to connect, restarting")
                restart()
                timeouts = 0
                continue
            time.sleep(2)
            continue
    # nie moze byc blocking ze wzgledu na rozbudowane odpowiedzi
#    s.setblocking(1)

def init():
    #banner
    # we might have some trobules here, its first read
    read_socket(s)
    #pass

def killLast():
    write_socket(s, "killLast")
    read_socket(s)

def proceed1():
    write_socket(s, "killExplorer")
    read_socket(s)

    write_socket(s, "startBinner")
    read_socket(s)
    
    s.settimeout(settings.fuzzbox_timeout) 

    return True

def proceed2():
    write_socket(s, "spawn " + settings.app_path)
    read_socket(s)

    time.sleep(2)

    write_socket(s, "binTest")
    read_socket(s)

    write_socket(s, "attachBinner opera.exe")
    read_socket(s)

    write_socket(s, "installHandlers")
    read_socket(s)

    write_socket(s, "installBad " + hex(BAD_ADDR_1))
    read_socket(s)

    return True

def sig1_handler(signum, frame):
    report("Signaled")
        
def sigkill_handler(signum, frame):
    report("Killing")
    powerofff()
#    revert()
    quit()
        
#setup fuzzer
my_generator = generator.Generator(origin_path, samples_shared_path, ".ogv", changer.Changer, corrector = None)
my_generator.mutations=3

def handle_crashing_sample(sample_path, sample_file):
    global s

    print("Crash procedures")

    lines = []
    eip = ""
    reason = ""

    write_socket(s, "cbStackUnwind")
    read_socket(s)
    lines = lastResponse.split("\n")

    write_socket(s, "cbEip")
    read_socket(s)
    eip = lastResponse

    write_socket(s, "cbReason")
    read_socket(s)
    reason = lastResponse

    path = settings.samples_binned

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


#setup box
def looop():
    global s
    global lastResponse

    powerofff()
    start()
    signal.signal(signal.SIGINT, sigkill_handler)
    connect()
    init()
    proceed1()

    sample_count = 0
    last_time_check = time.localtime()

    #start testing

    while True:
        try:
            proceed2()

            while(status != "CR"):
                sample_path = my_generator.generate_one()
                sample_file = os.path.basename(sample_path)
                write_socket(s, "testFile " + sample_file)
                read_socket(s)
                if(status == "BH" or status == "TO"):
                    os.remove(sample_path)

                # keep track on sample count
                sample_count += 1
                if(sample_count % 100 == 0):
                    current_time = time.localtime()
                    elapsed = time.mktime(current_time) - time.mktime(last_time_check)
                    report("Tested: " + str(sample_count))
                    report("100 tested in " + str(elapsed) + " seconds")
                    report("Last speed: " + str(10/elapsed) + " tps") 
                    last_time_check = current_time
            
            handle_crashing_sample(sample_path, sample_file)
            write_socket(s, "killHost")
            read_socket(s)

        except socket.timeout:
            print "socket timeout, restarting"
            restart()
            connect()
            init()
            proceed1()
#        except ErrorDetectedException:
#            print "error, restarting"
#            write_socket(s, "testmode exit")
#            read_socket(s)
#            killLast()
#            proceed1()
        
#    s.settimeout(None)
    
    s.close()
    print("Finished")

while True:
    try:
        looop()
    except Exception as e:
        print(e)
        time.sleep(3)
        continue
