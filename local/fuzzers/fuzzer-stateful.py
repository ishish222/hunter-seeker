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
    print()
    print("Loading machine: [%s]" % machine_file)
    print()

    import_stat = "import parts.%s as machine" % machine_file
    exec(import_stat)

    current_state = machine.Start
    default_shutdown = machine.DefaultShutdown

    while(current_state != statemachine.Exit):
        # handle possible ctrl-C
#        state = globs.state
#        if(state.shutting_down):
#            current_state = default_shutdown

        print("")
        print("=> Current state: [%s]" % current_state.name)
        if(current_state.consequence != None):
            try:
                if(hasattr(current_state, "args")):
                    current_state.executing_routine(current_state.args)
                else:
                    current_state.executing_routine()
            except statemachine.MachineError:
                print("Got state transition error")
                if(current_state.attempts < current_state.acceptable_error_count):
                    current_state.attempts = current_state.attempts +1
                    current_state.trans_error_handler() # this handler is only for preparing for repeat, fuzzing decisions should be made elsewhere
                else:
                    print("Too many state transition errors, exiting") # this means there's a possible error in machine, should notify via mail or sms
                    exit()
            except statemachine.MachineExit:
                current_state.consequence = default_shutdown
            current_state = current_state.consequence
              
        else:
            current_state = current_state.choosing_consequence()
            print("Choosing: [%s]" % current_state.name)


if __name__ == '__main__':
    stateful_routine()

