#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

import sys
sys.path += ["./scripters"]
sys.path += ["../common"]
import statemachine

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
        current_state.executing_routine()
        current_state = current_state.consequence


if __name__ == '__main__':
    stateful_routine()

