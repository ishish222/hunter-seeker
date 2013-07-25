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

origin_path = "../origins/acad/test.dwg"
samples_shared_path = "../samples_shared"
samples_saved = "../samples_saved"
fuzzbox_ip = "192.168.56.101"
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
write_socket(s, "pipe showPID")
read_socket(s)

write_socket(s, "quit")
read_socket(s)

s.close()

print("Finished")
