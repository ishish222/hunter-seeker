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
    print("Podaj ip")
    quit()

origin_path = "../origins/acad/test.dwg"
samples_shared_path = "../samples_shared"
samples_saved = "../samples_saved"
#fuzzbox_ip = "192.168.56.101"
fuzzbox_ip = sys.argv[1]
fuzzbox_port = 12345
buffer_size = 1024
my_name = "[seeker 1]"
my_logger = logging.getLogger('MyLogger')
my_handler = logging.handlers.SysLogHandler(address = '/dev/log')
my_logger.setLevel(logging.DEBUG)
my_logger.addHandler(my_handler)

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

#setup fuzzer
my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", changer.Changer)
my_generator.mutations=1

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((fuzzbox_ip, fuzzbox_port))

#setup box

#banner
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

#assuming target class is Afx:00400000:b:00010011:00000006:0038052 / changes every spawn?
write_socket(s, "pipe SetTargetClass Afx:00400000:b:00010011:00000006:0038052")
read_socket(s)

#closing windows
write_socket(s, "pipe KillClass")
read_socket(s)

#enter test mode
write_socket(s, "testmode enter")
read_socket(s)

write_socket(s, "Z:\\original.dwg")
read_socket(s)

sample_count = 0

while(True):
    sample_path = my_generator.generate_one()
    sample_file = os.path.basename(sample_path)
    write_socket(s, "Z:\\"+str(sample_file))
    read_socket(s)
    sample_count = sample_count + 1
    os.remove(sample_path)
    if(sample_count % 100 == 0):
        print("Tested: " + str(sample_count))
        

#exit test mode
write_socket(s, "testmode exit")
read_socket(s)

write_socket(s, "quit")
read_socket(s)

s.close()

print("Finished")
