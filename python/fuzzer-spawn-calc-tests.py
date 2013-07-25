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

if(len(sys.argv)) < 3:
    print("Podaj ip i nazwe boxa")
    quit()

origin_path = "../origins/acad/test.dwg"
samples_shared_path = "../samples_shared"
samples_saved = "../samples_saved"
#fuzzbox_ip = "192.168.56.101"
fuzzbox_name = sys.argv[2]
fuzzbox_ip = sys.argv[1]
fuzzbox_port = 12345
buffer_size = 1024
my_name = "[seeker 1]"
my_logger = logging.getLogger('MyLogger')
my_handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.setLevel(logging.DEBUG)
my_logger.addHandler(my_handler)
my_timeout = 10.0

#startvm = ["VBoxManage", "startvm", "", "--type", "headless"]
startvm = ["VBoxManage", "startvm", ""]
poweroff = ["VBoxManage", "controlvm", "", "poweroff"]
restorecurrent = ["VBoxManage", "snapshot", "", "restorecurrent"]
restorestart = ["VBoxManage", "snapshot", "", "restore", "[x] start"]

def report():
    my_logger.info(my_name + " " + "reporting");

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
    time.sleep(3)

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
    write_socket(s, "spawn calc.exe")
    read_socket(s)

    #inject to spawned
    write_socket(s, "injectLast")
    read_socket(s)
    read_socket(s)
    read_socket(s)
    read_socket(s)

    #enter test mode
    write_socket(s, "testmode enter")
    read_socket(s)
    s.settimeout(my_timeout) 

#setup fuzzer
my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", changer.Changer)
my_generator.mutations=1

#setup box
start()
connect()
init()
proceed()

#until passes test?

while(True):
    write_socket(s, "C:\\test.txt")
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

#actual testing

while(True):
    sample_path = my_generator.generate_one()
    sample_file = os.path.basename(sample_path)
    write_socket(s, "Z:\\"+str(sample_file))
    try:
        read_socket(s)
    except socket.timeout:
        print "timeout/restarting"
        print "saving" + str(sample_file)
        restart()
        init()
        proceed()
        continue
    sample_count = sample_count + 1
    os.remove(sample_path)
    if(sample_count % 100 == 0):
        print("Tested: " + str(sample_count))
        
s.settimeout(None)

#exit test mode
write_socket(s, "testmode exit")
read_socket(s)

write_socket(s, "quit")
read_socket(s)

s.close()

print("Finished")
