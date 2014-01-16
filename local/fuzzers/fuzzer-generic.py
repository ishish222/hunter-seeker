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
from script import rs, rss, runscriptq, write_monitor, write_monitor_2, read_monitor
from datetime import datetime
from common import *
import tempfile

signaled = False

def sig1_handler(signum, frame):
    global signaled
    report("Signaled")
    signaled = True

def fuzzing_routine():
    global signaled
    signaled = False
    options = get_options()
    log = open("./log-%s-%s-%s" % (options.fuzzbox_name, timestamp2(), options.origin), "a")
    create_sample_dirs(options)
    report("Starting fuzzer")
    print("[%s] Generic fuzzer" % timestamp())

    reqScript = ""
    status = "RD"

    signal.signal(signal.SIGINT, sigkill_handler)
#    signal.signal(signal.SIGUSR1, sig1_handler)

    #options.ss = prepare_con()
    options.ms = prepare_monitor(options.ms_path)
    options.ss = prepare_serial(options.ss_path)

    metric_res = options.metric_res

    sample_count = 0
    to_count = 0
    ma_count = 0
    last_time_check = time.localtime()

    # restart fuzzer on samples exhaustion, socket timout, etc. 
    while True:
        print("Current stats (SA/MA/TO): %d/%d/%d" % (sample_count, ma_count, to_count))
        create_drive(options)
        samples_list = generate(options)
        log.write("[%s]\n" % options.tmp_disk_img)
        log.flush()
        print("Spawning fuzz for batch: %s" % options.tmp_disk_img)
        start(options)
        print("[%s] Started" % timestamp())
        #mount_cdrom(options, options.cdrom)
        slot = pci_mount(options, options.tmp_disk_img) #hotplug should be completed during bootup
#        rs("ipconfig_set_ip_2", options.m, args=[options.fuzzbox_ip, options.server_ip])
        # it takes so fucking long
#        time.sleep(10)
#        os.spawnv(os.P_WAIT, "/bin/ping", ["ping", options.fuzzbox_ip, "-c1"])

        #time for boot
#        time.sleep(options.boot_wait)
        proceed1(options)

        try:
           wait_for_init(options.s)
        except socket.timeout:
           print("Socket timeout, restarting")
           report("Socket timeout, restarting")
           write_socket(s, "logStop")
           del_mountpoint(options)
           powerofff(options)
           if(not options.save_disks):
               os.remove(options.tmp_disk_img)
           continue
        s = options.s
#        s.settimeout(options.settings.wait_sleep * 8)


        proceed2(options)

#        try:
        if(True):

            proceed3(options)

            write_socket(s, "logStart e:\\logs\\log-%s-%s.txt" % (options.fuzzbox_name, timestamp2()))
            read_socket(s)

            if(options.profiling):
                write_socket(s, "start_profiling")

            write_socket(s, "checkReady")

#            while(not signaled):
#                pass

            status = ""
            try:
                while(status != "CR"):
#                try:
                    (lastResponse, status, reqScript) = read_socket(s)
#                execute_script(reqScript)
                    if(status == "STTO"):
                        proceed5(options)
                        continue
                    if(status == "RDTO"):
                        proceed5(options)
                    if(status == "SR"):
                    # react to SR
#                    register_script()
                        time.sleep(0.2)
                        execute_script(options, reqScript)
                        write_socket(s, "")
                        continue
                    if(status == "RD"):
                        sample_path = samples_list.pop()
                        sample_file = os.path.basename(sample_path)
                        test_path = options.settings.prepare_sample(sample_path)
                        test_file = os.path.basename(test_path)
                        write_socket(s, "testFile e:\\samples\\shared\\" + test_file)
                        options.settings.runner_0(options, [test_file])
                        log.write("%s: " % test_file)
                        log.flush()
                        continue
                    if(status == "MA" or status == "TO"):
                        # react to test end
                        proceed5(options)
                        write_socket(s, "")
                        log.write("[%s] \n" % status)
                        log.flush()
    
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
                        if(sample_count % metric_res == 0):
                            current_time = time.localtime()
                            elapsed = time.mktime(current_time) - time.mktime(last_time_check)
                            report("Tested: " + str(sample_count))
                            report(str(metric_res) + " tested in " + str(elapsed) + " seconds")
                            report("Last speed: " + str(metric_res/elapsed) + " tps")
                            report("MA count: " + str(ma_count))
                            to_count = 0
                            ma_count = 0
                            last_time_check = current_time
                            if(options.profiling):
                                write_socket(s, "dump_stats")

                        if(sample_count % options.settings.restart_count == 0):
                            report("Tested: " + str(sample_count) + ", will restart")
                            restart(options)
                            proceed1(options)
                            accept_con(ss)
                            s = options.s

                        continue
               
                write_socket(s, "getSynopsis")
                dossier, _, _ = read_socket(s)
#                handle_crashing_sample(dossier, sample_path, sample_file)
                log.write("[%s], registered, binned\n" % status)
                log.flush()
                report("CR")
    #            if(test_path != sample_path):
    #                os.remove(test_path)
#                killHost(options)
                time.sleep(10)
                raise CrashException

            except IndexError:
                print("Samples exhausted, restarting")
                report("Samples exhausted, restarting")
                write_socket(s, "logStop")
                del_mountpoint(options)
                print("Restarting")
                powerofff(options)
                if(not options.save_disks):
                    os.remove(options.tmp_disk_img)
                continue

            except socket.timeout:
                print("Socket timeout, restarting")
                report("Socket timeout, restarting")
                write_socket(s, "logStop")
                del_mountpoint(options)
                powerofff(options)
                if(not options.save_disks):
                    os.remove(options.tmp_disk_img)
                continue

            except CrashException:
                print("Got crash, restarting")
                report("Got crash, restarting")
                write_socket(s, "logStop")
                del_mountpoint(options)
                powerofff(options)
                continue

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
#import profile
#profile.run('fuzzing_routine()')
fuzzing_routine()
