#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

from optparse import OptionParser
from subprocess import Popen, PIPE
import socket
import os
import logging
import logging.handlers
import time
import sys
import signal
sys.path += ["./scripters"]
sys.path += ["../common"]
import generators.generatorCorrected as generator
import settings
from script import rs, rss, runscriptq, write_monitor
from datetime import datetime
from common import *

def fuzzing_routine():
    options = get_options()
    log = open("./log-%s-%s" % (timestamp2(), options.origin), "a")
    create_sample_dirs(options)
    report("Starting fuzzer")
    print("Generic fuzzer")

    reqScript = ""
    status = "RD"

    signal.signal(signal.SIGINT, sigkill_handler)
    options.ss = prepare_con()
    start(options)
    proceed1(options)
    accept_con(options.ss)
    s = options.s

    sample_count = 0
    to_count = 0
    ma_count = 0
    last_time_check = time.localtime()

    my_generator = generator.Generator(options.origin, options.samples_shared, "."+options.extension, options.settings.mutator, corrector = None)
    my_generator.mutations=int(options.mutations)
    
    proceed2(options)

    while True:
#        try:
        if(True):
            proceed3(options)
            write_socket(s, "ps")
            read_socket(s)
            time.sleep(10)

            write_socket(s, "logStart z:\\logs\\log-%s-%s.txt" % (options.fuzzbox_name, timestamp2()))
            read_socket(s)

            write_socket(s, "checkReady")

            status = ""
            while(status != "CR"):
                (lastResponse, status, reqScript) = read_socket(s)
#                execute_script(reqScript)
                if(status == "SR"):
                    # react to SR
#                    register_script()
                    time.sleep(0.2)
                    execute_script(options, reqScript)
                    write_socket(s, "")
                    continue
                if(status == "RD"):
                    sample_path = my_generator.generate_one()
                    sample_file = os.path.basename(sample_path)
                    test_path = options.settings.prepare_sample(sample_path)
                    test_file = os.path.basename(test_path)
                    write_socket(s, "testFile " + test_file)
                    log.write("%s: " % test_file)
                    log.flush()
                    continue
                if(status == "MA" or status == "TO"):
                    # react to test end
                    proceed5(options)
                    write_socket(s, "")
                    log.write("[%s] \n" % status)
                    log.flush()

                    os.remove(sample_path)
                    if(test_path != sample_path):
                        os.remove(test_path)
                    if(status == "MA"):
                        ma_count += 1
                        to_count = 0
                    if(status == "TO"):
                        to_count += 1
                        if(to_count % 3 == 0):
                            report("3x TO, settling")
                            to_count = 0
                            settle(options)

                    # MA or TO, keep track on sample count
                    sample_count += 1
                    if(sample_count % 100 == 0):
                        current_time = time.localtime()
                        elapsed = time.mktime(current_time) - time.mktime(last_time_check)
                        report("Tested: " + str(sample_count))
                        report("100 tested in " + str(elapsed) + " seconds")
                        report("Last speed: " + str(100/elapsed) + " tps")
                        report("MA count: " + str(ma_count))
                        to_count = 0
                        ma_count = 0
                        last_time_check = current_time
                    if(sample_count % options.settings.restart_count == 0):
                        report("Tested: " + str(sample_count) + ", will restart")
                        restart(options)
                        proceed1(options)
                        accept_con(ss)
                        s = options.s
                    continue
           
            write_socket(s, "getSynopsis")
            dossier, _, _ = read_socket(s)
            handle_crashing_sample(dossier, sample_path, sample_file)
            log.write("[%s], registered, binned\n" % status)
            log.flush()
            report("CR")
            if(test_path != sample_path):
                os.remove(test_path)
            write_socket(s, "ps")
            read_socket(s)
            write_socket(s, "killHost")
            read_socket(s)
            write_socket(s, "kill dwwin.exe")
            read_socket(s)
            write_socket(s, "kill %s" % options.settings.app_module)
            read_socket(s)
            write_socket(s, "ps")
            read_socket(s)
            time.sleep(10)

#        except socket.timeout:
#            print "socket timeout, restarting"
#            report("Socket timeout after " + str(sample_count) + " samples")
#            restart(options)
#            proceed1(options)
#            accept_con(ss)
#            s = options.s
#        except Exception, e:
#            print "Unknown error, restarting"
#            print e
#            report("Unknown error after " + str(sample_count) + " samples")
#            restart(options)
#            proceed1(options)
#            accept_con(ss)
#            s = options.s

    log.close()
    s.close()
    powerofff(options)
    print("Finished")
    exit()

#main
fuzzing_routine()
