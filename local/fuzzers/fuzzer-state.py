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
from glob import glob
import machines

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

def samples_walk(options, samples_list, stats, log):
    options.s.settimeout(None)
    (lastResponse, status, reqScript) = read_socket(options.s)
    if(status == "RD"):
        sample_path = samples_list.pop()
        sample_file = os.path.basename(sample_path)
        test_path = options.settings.prepare_sample(sample_path)
        test_file = os.path.basename(test_path)
        if(options.walk_start == None):
            write_socket(options.s, "walk e:\\samples\\shared\\%s %d f:\\%s.mm" % (test_file, options.walk_level, test_file))
        else:
            write_socket(options.s, "walk2 e:\\samples\\shared\\%s %d f:\\%s.mm %s" % (test_file, options.walk_level, test_file, options.walk_start))
        options.settings.runner_0(options, [test_file])
        log.write("%s: " % test_file)
        log.flush()
        return status
    if(status == "SR"):
        time.sleep(0.2)
        execute_script(options, reqScript)
        write_socket(options.s, "")
        return status
    if(status == "PTO"):
        proceed5(options)
        return status
    if(status == "WE"):
        # react to test end
        proceed5(options)
        write_socket(options.s, "")

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

#for recurrent calls
def runsteps(options, state, steps):
    
    print "-=[%s] %s" % (steps["name"], "start")
    step = steps["start"]
    while(not step["is_final"]):
        retries = 0
#        while(step["test"](options, state) != True and step["retries"] > retries):
        while True:
            try:
                if(type(step["operation"]) == dict): 
                    runsteps(options, state, step["operation"])
                else:
                    step["operation"](options, state)
            except Exception, e:
                state.failed = True
                state.failure_reason = e
            if(step["test"](options, state)): 
                if(type(step["next_step"]) == str):
                    next_step = step["next_step"]
                else:
                    next_step = step["next_step"](options, state)
            else: 
                if(step["retries"] > retries):
                    time.sleep(step["timeout"])
                    retries += 1
                    continue
                else:
                    next_step = step["failure"]
            step = steps[next_step]
            print "-=[%s] %s" % (steps["name"], next_step)

class state_class(object):
    def __init__(self):
        self.last_step = ""
        self.failed = False
        self.initialized = False

def fuzzing_routine():
    options = get_options()
    steps = machines.fuzzer #parametrize 

    state = state_class()

    try:
        runsteps(options, state, steps)
    except Exception, e:
        print e

    exit()

fuzzing_routine()
