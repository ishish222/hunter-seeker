#!/usr/bin/env python

from subprocess import Popen, PIPE
from optparse import OptionParser
import importlib
import time

parser = OptionParser()
parser.add_option("-c", "--config", dest="config", help="Machine configuration FILE", metavar="FILE")

(options, args) = parser.parse_args()

conf = importlib.import_module("configs."+options.config)

print("Loading config: " + conf.name)

qemuargs = ['qemu-system-i386']
qemuargs += conf.args
print qemuargs

p = Popen(qemuargs, stdout=PIPE, stdin=PIPE)

print(p.stdout.readline())
time.sleep(5)
print("loadvm " + conf.snap + "\n")
p.stdin.write("loadvm " + conf.snap + "\n")
print "done"

while True:
    pass

# p = Popen(['grep', 'f'], stdout=PIPE, stdin=PIPE, stderr=STDOUT)
