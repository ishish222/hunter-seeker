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

app_path = "C:\\Program Files\\Opera\\16.0.1196.80\\Opera.exe"

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
fuzzbox_port = 12345
buffer_size = 4096
my_name = "HS:ACAD-test"
my_logger = logging.getLogger('MyLogger')
my_handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.setLevel(logging.DEBUG)
my_logger.addHandler(my_handler)
my_timeout = 20.0

if(settings.visible):
    startvm = ["VBoxManage", "startvm", ""]
else:
    startvm = ["VBoxManage", "startvm", "", "--type", "headless"]

poweroff = ["VBoxManage", "controlvm", "", "poweroff"]
restorecurrent = ["VBoxManage", "snapshot", "", "restorecurrent"]
restorestart = ["VBoxManage", "snapshot", "", "restore", "[x] start"]

def report(string):
    my_logger.info("[" + my_name + ":" + fuzzbox_name + "] " + string);

def prepare_fuzzbox():
    pass

def read_socket(s):
    while True:
        data = s.recv(buffer_size)
        print("< " + str(data))
        if(data == "OK"): 
            break
    return data

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
            s.connect((fuzzbox_ip, fuzzbox_port))
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

def proceed():
    #killing explorer
    write_socket(s, "killExplorer")
    read_socket(s)

    #spawning acad
    write_socket(s, "spawn " + app_path)
    read_socket(s)

    #create conversation with binner

    #inject to spawned
#    write_socket(s, "injectLast")
#    read_socket(s)
#    read_socket(s)
#    read_socket(s)
#    read_socket(s)

    #acad hooks
#    write_socket(s, "pipe installTestHook2")
#    read_socket(s)
#    write_socket(s, "pipe installTestHook3")
#    read_socket(s)
#    write_socket(s, "pipe installTestHook4")
#    read_socket(s)
#    write_socket(s, "pipe installTestHook5")
#    read_socket(s)
#    write_socket(s, "pipe installTestMod7")
#    read_socket(s)

    #searching handles
#    write_socket(s, "pipe FindHandles Afx:00400000:b:00010011:00000006")
#    target = read_socket(s)
#    target_class = target.split("-")[1]
#    read_socket(s)
#    print("Setting target: " + str(target_class))
    
    #assuming target class is Afx:00400000:b:00010011:00000006:0038052 / changes every spawn?
#    write_socket(s, "pipe SetTargetClass " + str(target_class))
#    read_socket(s)

    #closing windows
#    write_socket(s, "pipe KillClass")
#    read_socket(s)

    print("sleeping")
    time.sleep(3)

    #enter test mode
    write_socket(s, "testmode enter")
    read_socket(s)
    s.settimeout(my_timeout) 

    return True

def sig1_handler(signum, frame):
    report("Signaled")
        
def sigkill_handler(signum, frame):
    report("Killing")
    powerofff()
#    revert()
    quit()
        
#setup fuzzer for acad
my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", changer.Changer)
my_generator.mutations=3

#setup box
def looop():
    global s

    powerofff()
    start()
    signal.signal(signal.SIGINT, sigkill_handler)
    connect()
    init()

    write_socket(s, "startBinner")
    read_socket(s)

    write_socket(s, "spawn " + app_path)
    read_socket(s)

    write_socket(s, "binTest")
    read_socket(s)

    write_socket(s, "ps")
    read_socket(s)

    write_socket(s, "attachBinner opera.exe")
    read_socket(s)

    while True:
        pass

    exit()

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
                print "timeout/restarting"
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
            print "timeout, saving & restarting"
            print "saving " + str(sample_path)
            command = ["cp", sample_path, samples_saved]
            os.spawnv(os.P_WAIT, "/bin/cp", command)
            restart()
            connect()
            init()
            proceed()
        except ErrorDetectedException:
            print "error, restarting"
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
