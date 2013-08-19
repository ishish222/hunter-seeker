#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

import generators.generator as generator
import generators.changer as changer
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

if(len(sys.argv)) < 2:
    print("Podaj nazwe boxa")
    quit()

#Configure generator
origin_path = "../origins/acad/test.dwg"
samples_shared_path = "../samples_shared"
samples_saved = "../samples_saved"
fuzzbox_name = sys.argv[1]
fuzzbox_ip = settings.ips[fuzzbox_name]
fuzzbox_port = 12345
buffer_size = 1024
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
    data = s.recv(buffer_size)
    print("< " + str(data))
    return data

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
    write_socket(s, "spawn C:\\Program Files\\AutoCAD 2010\\acad.exe")
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

    #searching handles
    write_socket(s, "pipe FindHandles Afx:00400000:b:00010011:00000006")
    target = read_socket(s)
    target_class = target.split("-")[1]
    read_socket(s)
    print("Setting target: " + str(target_class))
    
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
    s.settimeout(my_timeout) 

    return True

def sig1_handler(signum, frame):
    report("Signaled")
        
def sigkill_handler(signum, frame):
    report("Killing")
    powerofff()
    quit()
        
#setup fuzzer for acad
my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", changer.Changer)
my_generator.mutations=3

#setup box
start()
connect()
init()

signal.signal(signal.SIGINT, sigkill_handler)

sample_count = 0
last_time_check = time.localtime()

#write_socket(s, "pipe installTestHook3")
#read_socket(s)

write_socket(s, "dn4off")
read_socket(s)

#actual binning
write_socket(s, "invoke w:\\paimei\\crash_binner.py")
read_socket(s)

while True:
    pass

write_socket(s, "quit")
read_socket(s)

s.close()

print("Finished")
