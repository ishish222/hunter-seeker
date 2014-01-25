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

class statistics(object):
    def __init__(self, metrics):
        self.metric_res = metrics
        self.sample_count = 0
        self.to_count = 0
        self.ma_count = 0
        self.last_time_check = time.localtime()

def test_samples_batch(options, samples_list, stats, log):
    (lastResponse, status, reqScript) = read_socket(options.s)
    if(status == "PTO"):
        proceed5(options)
        return status
    if(status == "STTO"):
        proceed5(options)
        return status
    if(status == "RDTO"):
        proceed5(options)
    if(status == "SR"):
        time.sleep(0.2)
        execute_script(options, reqScript)
        write_socket(options.s, "")
        return status
    if(status == "RD"):
        sample_path = samples_list.pop()
        sample_file = os.path.basename(sample_path)
        test_path = options.settings.prepare_sample(sample_path)
        test_file = os.path.basename(test_path)
        write_socket(options.s, "testFile e:\\samples\\shared\\" + test_file)
        options.settings.runner_0(options, [test_file])
        log.write("%s: " % test_file)
        log.flush()
        return status
    if(status == "MA" or status == "TO"):
        # react to test end
        proceed5(options)
        write_socket(options.s, "")
        log.write("[%s] \n" % status)
        log.flush()

        if(status == "MA"):
            stats.ma_count += 1
            stats.to_count = 0
        if(status == "TO"):
            stats.to_count += 1
            if(stats.to_count % 3 == 0):
                report("3x TO, settling")
                stats.to_count = 0
                settle(options)

        # MA or TO, keep track on sample count
        stats.sample_count += 1
        if(stats.sample_count % stats.metric_res == 0):
            current_time = time.localtime()
            elapsed = time.mktime(current_time) - time.mktime(stats.last_time_check)
            report("Tested: " + str(stats.sample_count))
            report(str(stats.metric_res) + " tested in " + str(elapsed) + " seconds")
            report("Last speed: " + str(stats.metric_res/elapsed) + " tps")
            report("MA count: " + str(stats.ma_count))
            stats.to_count = 0
            stats.ma_count = 0
            stats.last_time_check = current_time
            if(options.profiling):
                write_socket(options.s, "dump_stats")

        if(stats.sample_count % options.settings.restart_count == 0):
            report("Tested: " + str(stats.sample_count) + ", will restart")
            restart(options)
            proceed1(options)
            accept_con(ss)
    return status

def fuzzing_routine():
    global signaled
    signaled = False
    options = get_options()
    log = open("./log-%s-%s-%s" % (options.fuzzbox_name, timestamp2(), options.origin), "a")
#    create_sample_dirs(options)
    report("Starting fuzzer")
    print("[%s] Generic fuzzer" % timestamp())

    reqScript = ""
    status = "RD"

    signal.signal(signal.SIGINT, sigkill_handler)
#    signal.signal(signal.SIGUSR1, sig1_handler)

    #options.ss = prepare_con()
    options.ms = prepare_monitor(options.ms_path)
    options.ss = prepare_serial(options.ss_path)

    stats = statistics(options.metric_res)
        
    #create saved disk
    saved_size = calc_disk_size(options) * 10
    options.saved_disk_img = create_drive(options, size=saved_size, name="%s-saved.raw" % timestamp2(), label="saved")

    # restart fuzzer on samples exhaustion, socket timout, etc. 
    while True:
        print("Current stats (SA/MA/TO): %d/%d/%d" % (stats.sample_count, stats.ma_count, stats.to_count))
        options.tmp_disk_img = create_drive(options)
        samples_list = generate(options)
        log.write("[%s]\n" % options.tmp_disk_img)
        log.flush()
        print("Spawning fuzz for batch: %s" % options.tmp_disk_img)
        start(options)
        print("[%s] Started" % timestamp())
        #mount_cdrom(options, options.cdrom)
        read_monitor(options.m)
        options.slot_shared = pci_mount(options, options.tmp_disk_img) #hotplug should be completed during bootup
        time.sleep(1)
        options.slot_saved = pci_mount(options, options.saved_disk_img) #hotplug should be completed during bootup

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

            # choose a loop

            status = ""
            try:
                while(status != "CR"):
                    loop_st = "status = %s(options, samples_list, stats, log)" % options.loop
                    exec loop_st

                write_socket(s, "getSynopsis")
                dossier, _, _ = read_socket(s)
#                handle_crashing_sample(dossier, sample_path, sample_file)
                log.write("[%s], registered, binned\n" % status)
                log.flush()
                print("CR in: %s" % options.tmp_disk_img)
                report("CR")
    #            if(test_path != sample_path):
    #                os.remove(test_path)
#                killHost(options)
#                time.sleep(10)
                raise CrashException

            except IndexError:
                print("Samples exhausted, restarting")
                report("Samples exhausted, restarting")
                write_socket(s, "logStop")
                del_mountpoint(options)
#                time.sleep(30)
                print("Restarting")
                powerofff(options)
                if(not options.save_disks):
                    os.remove(options.tmp_disk_img)
                continue

            except socket.timeout:
                # suspicious, i want that sample :)
                write_socket(s, "getSynopsis")
                print("Socket timeout, restarting")
                report("Socket timeout, restarting")
                write_socket(s, "logStop")
#                time.sleep(30)
                del_mountpoint(options)
                powerofff(options)
                if(not options.save_disks):
                    os.remove(options.tmp_disk_img)
                continue

            except CrashException:
                print("Got crash, restarting")
                report("Got crash, restarting")
#                time.sleep(30)
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
