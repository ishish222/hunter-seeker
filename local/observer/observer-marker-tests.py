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
from glob import glob
from shutil import copyfile


def observing_routine():
    options = get_options()
    log = open("../logs/log-%s-%s" % (timestamp(), "observer"), "w", 0)
    create_sample_dirs(options)
    report("Starting marker observer")
    print("Generic marker observer")

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

    proceed2(options)
    proceed3(options)

    for sample_path in glob(options.samples_orig + "/*." + options.extension):
        if(True):
#        try:
            # prepare sample
            sample_file = os.path.basename(sample_path)
            copyfile(sample_path, options.samples_shared + "/" + sample_file)
            sample_path = options.samples_shared + "/" + sample_file
            test_path = options.settings.prepare_sample(sample_path)
            test_file = os.path.basename(test_path)

            # start log
            write_socket(s, "logStart z:\\logs\\log-%s" % test_file)
            read_socket(s)
            log.write("%s %s" % (options.obs_command, test_file))
            write_socket(s, "%s %s" % (options.obs_command, test_file))

            # need loop
            while(status != "CR"):
                (lastResponse, status, reqScript) = read_socket(s)
                if(status == "SR"):
                    execute_script(options, reqScript)
                    write_socket(s, "")
                    continue
                if(status == "RD"):
                    break
                if(status == "MA" or status == "TO"):
                    # react to test end
                    proceed5(options)
                    write_socket(s, "")
                    log.write("%s %s succesful" % (options.obs_command, test_file))
                    log.flush()

            # handle crash
            if(status == "CR" ):
                handle_crashing_sample(sample_path, sample_file)
                report("CR")
                if(test_path != sample_path):
                    os.remove(test_path)
                write_socket(s, "killHost")
                read_socket(s)
                log.write("%s %s unsuccesful" % (options.obs_command, test_file))

            # keep track on sample count
            sample_count += 1
            if(sample_count % 10 == 0):
                current_time = time.localtime()
                elapsed = time.mktime(current_time) - time.mktime(last_time_check)
                report("Observed: " + str(sample_count))
                report("10 tested in " + str(elapsed) + " seconds")
                report("Last speed: " + str(10/elapsed) + " tps")
                last_time_check = current_time

            # stop log
            write_socket(s, "logStop")
            read_socket(s)

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

#    log.close()
#    s.close()
#    powerofff(options)
#    print("Finished")
#    exit()
    sigkill_handler(None, None)

#main
observing_routine()
