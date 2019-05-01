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
fuzzbox_name = sys.argv[1]
origin_path = sys.argv[2]
samples_shared_path = settings.samples_shared_path
samples_saved = settings.samples_saved
fuzzbox_ip = settings.ips[fuzzbox_name]
my_logger = logging.getLogger('MyLogger')
my_handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.setLevel(logging.DEBUG)
my_logger.addHandler(my_handler)

if(settings.visible):
    startvm = ["VBoxManage", "startvm", ""]
else:
    startvm = ["VBoxManage", "startvm", "", "--type", "headless"]

poweroff = ["VBoxManage", "controlvm", "", "poweroff"]
restorecurrent = ["VBoxManage", "snapshot", "", "restorecurrent"]
restorestart = ["VBoxManage", "snapshot", "", "restore", "[x] start"]

def sig1_handler(signum, frame):
    report("Signaled")
        
def sigkill_handler(signum, frame):
    report("Killing")
    powerofff()
    quit()

signal.signal(signal.SIGINT, sigkill_handler)

def report(string):
    my_logger.info("[" + settings.log_name + ":" + fuzzbox_name + "] " + string);

def prepare_fuzzbox():
    pass

def read_socket(s):
    data = s.recv(settings.buffer_size)
    print(("< " + str(data)))
    return data

def write_socket(s, data):
    print(("> " + str(data)))
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

def init():
    #banner
    # we might have some trobules here, its first read
    read_socket(s)

def killLast():
    write_socket(s, "killLast")
    read_socket(s)

def proceed():
    #killing explorer
    write_socket(s, "killExplorer")
    read_socket(s)

    #spawning acad
    write_socket(s, "spawn " + settings.app_path)
    read_socket(s)

    #inject to spawned
    write_socket(s, "injectLast")
    read_socket(s)
    read_socket(s)
    read_socket(s)
    read_socket(s)

    #acad hooks
    write_socket(s, "pipe installTestHook2")
    read_socket(s)
    write_socket(s, "pipe installTestHook3")
    read_socket(s)
    write_socket(s, "pipe installTestHook4")
    read_socket(s)
    write_socket(s, "pipe installTestHook5")
    read_socket(s)
    write_socket(s, "pipe installTestMod7")
    read_socket(s)

    #searching handles
    write_socket(s, "pipe FindHandles Afx:00400000:b:00010011:00000006")
    target = read_socket(s)
    target_class = target.split("-")[1]
    read_socket(s)
    print(("Setting target: " + str(target_class)))
    
    #assuming target class is Afx:00400000:b:00010011:00000006:0038052 / changes every spawn?
    write_socket(s, "pipe SetTargetClass " + str(target_class))
    read_socket(s)

    #closing windows
    write_socket(s, "pipe KillClass")
    read_socket(s)

    print("sleeping")
    time.sleep(3)

    #enter test mode
    write_socket(s, "testmode enter")
    read_socket(s)
    s.settimeout(settings.fuzzbox_timeout) 

    return True

#setup fuzzer for acad
my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", changer.Changer, corrector = "acadCorrector")
my_generator.mutations=3

#setup box
def looop():
    global s

    powerofff()
    start()
    connect()
    init()

    while True:
        try:
            ret = proceed()
            if(ret == True):
                break
            else:
                write_socket(s, "testmode exit")
                read_socket(s)
                killLast()
                continue
        except Exception as e:
            print(e)
            write_socket(s, "testmode exit")
            read_socket(s)
            killLast()
            continue
    
    
    if(settings.testing):
        #restart until passes test?
        while(True):
            write_socket(s, "Z:\\original.dwg")
            try:
                read_socket(s)
                break
            except socket.timeout:
                print("timeout/restarting")
                restart()
                connect()
                init()
                proceed()
                continue
    
    sample_count = 0
    last_time_check = time.localtime()
    
    #actual testing
    while(True):
        sample_path = my_generator.generate_one()
        sample_file = os.path.basename(sample_path)
        write_socket(s, "Z:\\"+str(sample_file))
        try:
            if(read_socket(s) == "OK"):
                pass
    #            continue
    #            command = ["rm", sample_path]
    #            os.spawnv(os.P_WAIT, "/bin/rm", command)
            else:
                 raise ErrorDetectedException
        except socket.timeout:
            print("timeout, saving & restarting")
            print("saving " + str(sample_path))
            command = ["cp", sample_path, samples_saved]
            os.spawnv(os.P_WAIT, "/bin/cp", command)
            restart()
            connect()
            init()
            proceed()
        except ErrorDetectedException:
            print("error, restarting")
            write_socket(s, "testmode exit")
            read_socket(s)
            killLast()
            proceed()
     
        sample_count = sample_count + 1
        os.remove(sample_path)
        if(sample_count % 100 == 0):
            current_time = time.localtime()
            elapsed = time.mktime(current_time) - time.mktime(last_time_check)
            report("Tested: " + str(sample_count))
            report("100 tested in " + str(elapsed) + " seconds")
            report("Last speed: " + str(10/elapsed) + " tps") 
            last_time_check = current_time
            
    s.settimeout(None)
    
    #exit test mode
    write_socket(s, "testmode exit")
    read_socket(s)
    
    write_socket(s, "quit")
    read_socket(s)
    
    s.close()
    
    print("Finished")

while True:
    try:
        looop()
    except Exception as e:
        print(e)
        time.sleep(3)
        continue
