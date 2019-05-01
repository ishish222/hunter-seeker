#!/usr/bin/env python

from subprocess import Popen, PIPE
from optparse import OptionParser
import importlib
import time
from sys import argv

def load_option(opt):
    global options
    global config

    ret = None

    if((opt in config) == True):
        ret = config[opt]
    if((opt in options) == True):
        ret = options[opt]
    return ret

options = {}

for i in range(0, len(argv)-2, 2):
    options[argv[i+1][1:]] = argv[i+2]

config = importlib.import_module("configs."+options["config"])
config = config.config
scriptlist = options["script"].split(",")

qemuargs = ['qemu-system-i386']
for maj_arg in ["hda", "hdb", "cdrom", "m"]:
    arg = load_option(maj_arg)
    if(arg is not None):
        qemuargs.append("-"+maj_arg)
        qemuargs.append(arg)

qemuargs += config["other_args"]

print("Script list: " + str(scriptlist))
print("Qemu args: " + str(qemuargs))

p = Popen(qemuargs, stdout=PIPE, stdin=PIPE)

print((p.stdout.readline()))
#Give qemu just a sec :)
time.sleep(3)

if("snap" in config and config["snap"] is not None):
    print(("loadvm " + config["snap"]))
    p.stdin.write("loadvm " + config["snap"] + "\n")
    #p.stdout.read()
    time.sleep(50)


print("Executing scripts")

for sc in scriptlist:
    print(("[Executing: " + sc + "]"))
    scriptmod = importlib.import_module("scripts."+sc)
    script = scriptmod.script
    print(("[ETA: " + script.eta_str + "]"))
    script.run(p)
    print(("[Executing: " + sc + " finished]"))

print("Executing scripts completed")

