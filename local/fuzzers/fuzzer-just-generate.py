#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

import generators.generator as generator
#import generators.changer as changer
import generators.adder as mutator
#import generators.remover as mutator
import socket
import os
import sys

origin_path = "../origins/acad/test.dwg"
samples_shared_path = "../samples_shared"
samples_saved = "../samples_saved"
fuzzbox_ip = "192.168.56.103"
fuzzbox_port = 12345
buffer_size = 1024

def prepare_fuzzbox():
    pass

def read_socket(s):
    data = s.recv(buffer_size)
    print("< " + str(data))
    return data

def write_socket(s, data):
    print("> " + str(data))
    s.send(data)

if(len(sys.argv) < 2):
    count = 1
else:
    count = int(sys.argv[1], 10)

#my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", mutator.Changer)
#my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", mutator.AreaChanger)
my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", mutator.ByteAdder)
#my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", mutator.ShortAdder)
#my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", mutator.LongAdder)
#my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", mutator.ByteRemover)
#my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", mutator.ShortRemover)
#my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", mutator.LongRemover)
my_generator.mutations = 1
for i in range(0, count):
    sample_path = my_generator.generate_one()
