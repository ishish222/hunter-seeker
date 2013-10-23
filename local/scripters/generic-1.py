#!/usr/bin/env python

from subprocess import Popen, PIPE
from optparse import OptionParser
import importlib
import time

parser = OptionParser()
parser.add_option("-c", "--config", dest="config", help="Machine configuration FILE", metavar="FILE")
parser.add_option("-s", "--script", dest="script", help="Execute SCRIPT", metavar="SCRIPT")

(options, args) = parser.parse_args()

conf = importlib.import_module("configs."+options.config)
scriptmod = importlib.import_module("scripts."+options.script)
script = scriptmod.script

print("Loading config: " + conf.name)

qemuargs = ['qemu-system-i386']
qemuargs += conf.args
print "Qemu args: " + str(qemuargs)

p = Popen(qemuargs, stdout=PIPE, stdin=PIPE)

print(p.stdout.readline())
time.sleep(1)
print("loadvm " + conf.snap)
p.stdin.write("loadvm " + conf.snap + "\n")
#p.stdout.read()
time.sleep(40)
print "Executing keysched"

script.run(p)

print("Schedule completed")

while True:
    pass

# p = Popen(['grep', 'f'], stdout=PIPE, stdin=PIPE, stderr=STDOUT)
