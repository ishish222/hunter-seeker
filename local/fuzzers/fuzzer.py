#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

import sys
sys.path += ["./scripters"]
sys.path += ["../common"]

import usualparts.globs as globs
globs.init()

import codeunit
from codeunit import Instruction, Decision, GoTo
global prototypes 

class MachineError(Exception):
    pass

def stateful_routine():
    script = []
    ip = 1

    script_path = sys.argv[len(sys.argv) -1]
    print
    print "Loading script: [%s]" % script_path
    print

    script_file = open(script_path)

    # stage 1
    # import prorotypes, make a list
    script.append(None) # to fill line 0

    for line in script_file:
        line = line[:-1]

        if(line == ''):
            script.append(None)
            continue

        if(line[0] == '#'):
            script.append(None)
            continue

        if "=" in line:
            print "Found ="
            instr, ret_tab = line.split("=")
            ret_tab = ret_tab[:-1]
        else:
            instr = line
            ret_tab = None

        if "(" in instr:
            print "Found ("
            name, args = instr.split("(")
            args = args[:-1]
            if "," in args:
                args = args.split(",")
        else:
            name = instr
            args = ""

        # lookup name in prototypes
        if(name == "goto"):
            unit = GoTo(int(args, 10))
        elif(ret_tab):
            unit = Decision(name, args, ret_tab)
        else:
            unit = Instruction(name, args)
        
        # create object from class
        script.append(unit)

    print "Script length: %d" % len(script)
        
    # stage 2
    while(1):
        print ip
        if(ip < 0):
            break
        if(ip >= len(script)):
            break

        instruction = script[ip]
        if(instruction == None):
            ip += 1
            continue

        print "[%s] Currently executing: [%d] %s" % (script_path, ip, instruction.name)

        ret = instruction.execute()
        if(ret == None):
            ip += 1
        else:
            ip = ret

        print "ip after: %d" % ip

    print
    print "Finished"
    print

if __name__ == '__main__':
    stateful_routine()

