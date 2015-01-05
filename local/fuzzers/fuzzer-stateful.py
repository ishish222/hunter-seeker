#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

import sys
import statemachine

def stateful_routine():
    machine_file = sys.argv[len(sys.argv) -1]
    print machine_file

    import_stat = "import parts.%s as machine" % machine_file
    exec import_stat

    current_state = machine.Start

    while(current_state != statemachine.Exit):
        current_state.executing_routine()
        current_state = current_state.consequence

    print "zaladowanie obiektow stanow"
    print "zaladowanie zadanej mapy stanow"
    print "wykonanie zadanej mapy stanow"
    print ""


if __name__ == '__main__':
    stateful_routine()

