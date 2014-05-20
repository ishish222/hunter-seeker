import qemu_parts
import disk_fs_parts
import binner_parts
import other_parts
import mechanisms as m
import sys
sys.path += ["../../common"]
import common
import os
import time

read_socket = common.read_socket
write_socket = common.write_socket
report = common.report

def dummy_start(options, state):
    print("You have to select appropriate mechanism")

dummy = {
    "name" : "Dummy",
    "start" : m.step(
        dummy_start, 
        next_step = "stop",
        ),
    "stop" : m.step(
        is_final = True
        )
}

