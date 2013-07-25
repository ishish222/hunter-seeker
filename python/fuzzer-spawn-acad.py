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

if(len(sys.argv)) < 2:
    print("Podaj nazwe boxa")
    quit()

ips = { 
'xpsp2-1': '192.168.56.110'
}

origin_path = "../origins/acad/test.dwg"
samples_shared_path = "../samples_shared"
samples_saved = "../samples_saved"
fuzzbox_name = sys.argv[1]
fuzzbox_ip = ips[fuzzbox_name]
fuzzbox_port = 12345
buffer_size = 1024
my_name = "[HS]"
my_logger = logging.getLogger('MyLogger')
my_handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.setLevel(logging.DEBUG)
my_logger.addHandler(my_handler)
my_timeout = 20.0

#startvm = ["VBoxManage", "startvm", "", "--type", "headless"]
startvm = ["VBoxManage", "startvm", ""]
poweroff = ["VBoxManage", "controlvm", "", "poweroff"]
restorecurrent = ["VBoxManage", "snapshot", "", "restorecurrent"]
restorestart = ["VBoxManage", "snapshot", "", "restore", "[x] start"]

def report(string):
    my_logger.info(my_name + " " + string);

def prepare_fuzzbox():
    pass

def read_socket(s):
    data = s.recv(buffer_size)
    print("< " + str(data))
    return data

def write_socket(s, data):
    print("> " + str(data))
    s.send(data)

def start():
    command = startvm
    command[2] = fuzzbox_name
    ret = os.spawnv(os.P_WAIT, "/opt/VirtualBox/VBoxManage", command)
    time.sleep(5)

def restart():
    command = poweroff
    command[2] = fuzzbox_name
    ret = os.spawnv(os.P_WAIT, "/opt/VirtualBox/VBoxManage", command)
    time.sleep(3)

    command = restorestart
    command[2] = fuzzbox_name
    ret = os.spawnv(os.P_WAIT, "/opt/VirtualBox/VBoxManage", command)
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

    #enter test mode
    write_socket(s, "testmode enter")
    read_socket(s)
    s.settimeout(my_timeout) 

#setup fuzzer for acad
my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", changer.Changer)
my_generator.mutations=1

#setup box
start()
connect()
init()
proceed()

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
        read_socket(s)
    except socket.timeout:
        print "timeout/restarting"
        print "saving " + str(sample_path)
        command = ["cp", sample_path, samples_saved]
        os.spawnv(os.P_WAIT, "/bin/cp", command)
        restart()
        connect()
        init()
        proceed()
        continue
    sample_count = sample_count + 1
    os.remove(sample_path)
    if(sample_count % 100 == 0):
        current_time = time.localtime()
        elapsed = time.mktime(current_time) - time.mktime(last_time_check)
        report("Tested: " + str(sample_count))
        report("100 tested in " + str(elapsed) + " seconds")
        report("Last speed: " + str(100/elapsed) + " tps") 
        
s.settimeout(None)

#exit test mode
write_socket(s, "testmode exit")
read_socket(s)

write_socket(s, "quit")
read_socket(s)

s.close()

print("Finished")
