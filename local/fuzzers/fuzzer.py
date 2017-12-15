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

import signal

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    LIGHTBLUE = '\033[42m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class MachineError(Exception):
    pass

def sigkill_handler(signum, frame):
    raise MachineError
    return True

def preparse_script(script_path):
    global global_script
    global global_labels

    print
    print bcolors.OKBLUE + "Preparsing script: [%s]" % script_path + bcolors.ENDC
    print

    script_file = open(script_path)

    gathering = 0
    longline = ''

    global_script.append(None) # to fill line 0

    for line in script_file:
        # clear line from tabs
        line = line[:-1]
        line = line.replace('\t', '')
        line = line.strip()

        print("Parsing: %s" % line)

        if(line == ''):
            global_script.append(None)
            continue

        if(line[0] == '#'):
            global_script.append(None)
            continue

        # read until parantheses closure
        if('(' in line):
            if(')' not in line):
                gathering = 1
                longline += line
                continue

        if(gathering):
            longline += line
            if(')' in line):
                line = longline.replace('\n', '')
                line = longline.replace('\t', '')
                line = longline.replace(' ', '')
                gathering = 0
                longline = ''
            else:
                continue

        # create token for function
        if(line[-1:] == ':'):
            line = line[:-1]
            global_labels[line] = len(global_script)
            global_script.append(None)
            continue

        if "=" in line:
            instr, ret_tab = line.split("=")
            ret_tab = ret_tab[1:-1]
        else:
            instr = line
            ret_tab = None

#        print instr

        if "(" in instr:
            name, args = instr.split("(")
            args = args[:-1]
            if "," in args:
                args = args.split(",")
        else:
            name = instr
            args = ""

        if(name == "goto"):
            unit = GoTo(args)
        elif(name == "Include"):
            preparse_script(args)
            continue
        elif(name == "Version"):
            continue
        elif(name == "Call"):
            continue
        elif(name == "Return"):
            continue
        elif(ret_tab):
            unit = Decision(name, args, ret_tab)
        else:
            unit = Instruction(name, args)

        # create object from class
        global_script.append(unit)

    return True

def perform(script_path):
    global global_script
    global global_labels

    global_script = []
    global_labels = {}

    global_stack = []

    preparse_script(script_path)

    ip = 1

    while(1):
        if(ip < 0):
            break
        if(ip >= len(global_script)):
            break

        instruction = global_script[ip]
        if(instruction == None):
            ip += 1
            continue

        try:
            #print "[%s] (%s)" % (instruction.name, script_path)
            print bcolors.LIGHTBLUE + "===[%s]" % (instruction.name) + bcolors.ENDC
            if(instruction.name == 'Call'):
                global_stack.insert(0, ip+1)
                ret = instruction.args
            if(instruction.name == 'Return'):
                ret = global_stack.pop()
            else:
                ret = instruction.execute()
        except MachineError:
            print("Exception:", sys.exc_info()[0])
            if(hasattr(globs, 'first_chance')):
                if(globs.first_chance == 1):
                    print 'First chance handler finished'
                    print 'Exiting'
                    exit(0)
            else:
                print 'First chance handler'
                globs.first_chance = 1

            if(globs.exc_label != None):
                ret = globs.exc_label
                print 'ret = %s' % ret

        if(ret == None):
            ip += 1
        else:
            try:
                ip = int(ret, 10)
            except ValueError:
                try:
                    ip = labels[ret]
                except KeyError:
                    globs.first_chance = 0x0
                    print 'Key error: %s' % ip
                    raise MachineError

        print

    return True


def stateful_routine(script_path):
    script = []
    labels = {}
    ip = 1

    signal.signal(signal.SIGINT, sigkill_handler)
    signal.signal(signal.SIGUSR1, sigkill_handler)
    signal.signal(signal.SIGHUP, sigkill_handler)

    print
    print bcolors.OKBLUE + "Loading script: [%s]" % script_path + bcolors.ENDC
    print

    script_file = open(script_path)

    # stage 1
    # import prorotypes, make a list

    gathering = 0
    longline = ''

    script.append(None) # to fill line 0

    for line in script_file:
        line = line[:-1]
        line = line.replace('\t', '')
        line = line.strip()

        if(line == ''):
            script.append(None)
            continue

        if(line[0] == '#'):
            script.append(None)
            continue

        # read until parantheses closure
        if('(' in line):
            if(')' not in line):
                gathering = 1
                longline += line
                continue            

        if(gathering):
            longline += line
            if(')' in line):
                line = longline.replace('\n', '')
                line = longline.replace('\t', '')
                line = longline.replace(' ', '')
                gathering = 0
                longline = ''
            else:
                continue

        if(line[-1:] == ':'):
            line = line[:-1]
            labels[line] = len(script)
            script.append(None)
            continue

        if "=" in line:
            instr, ret_tab = line.split("=")
            ret_tab = ret_tab[1:-1]
        else:
            instr = line
            ret_tab = None

#        print instr

        if "(" in instr:
            name, args = instr.split("(")
            args = args[:-1]
            if "," in args:
                args = args.split(",")
        else:
            name = instr
            args = ""

        if(name == "goto"):
            unit = GoTo(args)
        elif(ret_tab):
            unit = Decision(name, args, ret_tab)
        else:
            unit = Instruction(name, args)
        
        # create object from class
        script.append(unit)

#    print "Script length: %d" % len(script)
#    print "Script:"
#    for line in script:
#        if line is None:
#            continue
#        if(hasattr(line, 'args')):
#            print '%s - %s' % (line.name, line.args)
#        else:
#            print '%s' % (line.name)
        
    # stage 2
    while(1):
        if(ip < 0):
            break
        if(ip >= len(script)):
            break

        instruction = script[ip]
        if(instruction == None):
            ip += 1
            continue

        try:
            #print "[%s] (%s)" % (instruction.name, script_path)
            print bcolors.LIGHTBLUE + "===[%s]" % (instruction.name) + bcolors.ENDC
            if(instruction.name == 'Execute'):
                stateful_routine(instruction.args)
                ret = None
            else:
                ret = instruction.execute()
        except MachineError:
            print("Exception:", sys.exc_info()[0])
            if(hasattr(globs, 'first_chance')):
                if(globs.first_chance == 1):
                    print 'First chance handler finished'
                    print 'Exiting'
                    exit(0)
            else:
                print 'First chance handler'
                globs.first_chance = 1

            if(globs.exc_label != None):
                ret = globs.exc_label
                print 'ret = %s' % ret

        if(ret == None):
            ip += 1
        else:
            try:
                ip = int(ret, 10)
            except ValueError:
                try:
                    ip = labels[ret]
                except KeyError:
                    globs.first_chance = 0x0
                    print 'Key error: %s' % ip
                    raise MachineError
    
        print

    print "Finished: %s" % script_path

def check_version(script_path):
    script_file = open(script_path)
    for line in script_file:
        if('Version(2)' in line):
            return 2
    return 0

if __name__ == '__main__':
    if(check_version(sys.argv[len(sys.argv) -1]) == 2):
        perform(sys.argv[len(sys.argv) -1])
    else:
        stateful_routine(sys.argv[len(sys.argv) -1])
