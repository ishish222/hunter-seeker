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

from os import listdir
ready = False

class ErrorDetectedException(Exception):
    pass

if(len(sys.argv)) < 2:
    print("Podaj nazwe boxa")
    quit()

#Configure generator
origin_path = "../origins/acad/test.dwg"
samples_shared_path = "../samples_shared"
samples_saved = "../samples_saved"
samples_suspected = "../samples_suspected"
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
pause = ["VBoxManage", "controlvm", "", "pause"]
resume = ["VBoxManage", "controlvm", "", "resume"]
restorecurrent = ["VBoxManage", "snapshot", "", "restorecurrent"]
restorestart = ["VBoxManage", "snapshot", "", "restore", "[x] start"]

def report(string):
    my_logger.info("[" + my_name + ":" + fuzzbox_name + "] " + string);

def prepare_fuzzbox():
    pass

def read_socket(s):
    data = s.recv(buffer_size)
    print(("< " + str(data)))
    return data

def write_socket(s, data):
    print(("> " + str(data)))
    s.send(data)

def powerofff():
    command = poweroff
    command[2] = fuzzbox_name
    ret = os.spawnv(os.P_WAIT, "/opt/VirtualBox/VBoxManage", command)

def pausee():
    command = pause
    command[2] = fuzzbox_name
    ret = os.spawnv(os.P_WAIT, "/opt/VirtualBox/VBoxManage", command)

def resumee():
    command = resume
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
    while(True):
        try:
            s.connect((fuzzbox_ip, fuzzbox_port))
            print("Connected")
            break
        except Exception:
            print("No route to host, waiting")
            time.sleep(2)
            continue

def init():
    #banner
    # we might have some trobules here, its first read
    read_socket(s)


def proceed():
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
    s.settimeout(my_timeout) 

def sig1_handler(signum, frame):
    report("Verifying")
    resumee()
    for filee in os.listdir(samples_saved):
        if(filee[len(filee)-3:] != "dwg"):
            continue
        write_socket(s, "Y:\\"+str(filee))
        try:
            if(read_socket(s) == "OK"):
                # not interesting
                command = ["rm", samples_saved+"/"+filee]
                os.spawnv(os.P_WAIT, "/bin/rm", command)
                continue
            else:
                 raise ErrorDetectedException
        except socket.timeout:
            report("Found suspect: " + filee + ", saving to: " + samples_suspected + "/" + filee)
            command = ["mv", samples_saved+"/"+filee, samples_suspected]
            os.spawnv(os.P_WAIT, "/bin/mv", command)
            restart()
            connect()
            init()
            proceed()
            continue
        except ErrorDetectedException:
            # not interesting
            command = ["rm", samples_saved+"/"+filee]
            os.spawnv(os.P_WAIT, "/bin/rm", command)
            restart()
            connect()
            init()
            proceed()
            continue
    pausee()
        
def sigkill_handler(signum, frame):
    report("Killing")
    powerofff()
    os.unlink(pidfile)
    quit()
        
#setup box
start()
connect()
init()
proceed()
pausee()

signal.signal(signal.SIGUSR1, sig1_handler)
signal.signal(signal.SIGINT, sigkill_handler)

#Drop pidfile
pid = str(os.getpid())
pidfile = "/tmp/hs-2nd.pid"

if os.path.isfile(pidfile):
    report("Removing old pidfile, hope that's fine")
    os.unlink(pidfile)

file(pidfile, 'w').write(pid)

ready = True

report("Second loop ready")

while(True):
    pass

#exit test mode
write_socket(s, "testmode exit")
read_socket(s)

write_socket(s, "quit")
read_socket(s)

s.close()

print("Finished")
