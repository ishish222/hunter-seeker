#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

import sys
sys.path += ["./scripters"]
sys.path += ["../common"]

import usualparts.globs as globs
globs.init()

import statemachine

class MachineError(Exception):
    pass

def stateful_routine():
    machine_file = sys.argv[len(sys.argv) -1]
    print
    print "Loading machine: [%s]" % machine_file
    print

    import_stat = "import parts.%s as machine" % machine_file
    exec import_stat

    current_state = machine.Start

    while(current_state != statemachine.Exit):
        print "=> Current state: [%s]" % current_state.name
        if(current_state.consequence != None):
            current_state.executing_routine()
        else:
            current_state.consequence = current_state.choosing_consequence()
            print "Choosing: [%s]" % current_state.consequence.name
        current_state = current_state.consequence


if __name__ == '__main__':
    stateful_routine()

