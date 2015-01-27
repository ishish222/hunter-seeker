import time
from statemachine import MachineError
import common
import globs
import os
from other_parts import defined

report = common.report
write_socket = common.write_socket
read_socket = common.read_socket

def walk_sample():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    try:
        sample_path = globs.state.samples_list.pop()
    except Exception, e:
        print e
        globs.state.samples_exhausted = True
        return

    sample_file = os.path.basename(sample_path)
    test_path = options.settings.prepare_sample(sample_path)
    test_file = os.path.basename(test_path)

    if(options.walk_start == None):
        write_socket(options.s, "walk e:\\samples\\shared\\%s %d f:\\%s.mm" % (test_file, options.walk_level, test_file))
    else:
        write_socket(options.s, "walk2 e:\\samples\\shared\\%s %d f:\\%s.mm %s" % (test_file, options.walk_level, test_file, options.walk_start))

    options.settings.runner_0(options, [test_file])
    options.log.write("%s: " % test_file)
    options.log.flush()


def test_sample():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

#    for s in globs.state.samples_list:
#        print s
    #this should be on run end
#    print("Current stats (SA/MA/TO): %d/%d/%d" % (stats.sample_count, stats.ma_count, stats.to_count))

    try:
        sample_path = globs.state.samples_list.pop()
    except Exception, e:
        print e
        globs.state.samples_exhausted = True
        return

    sample_file = os.path.basename(sample_path)
    test_path = options.settings.prepare_sample(sample_path)
    test_file = os.path.basename(test_path)
    write_socket(options.s, "testFile e:\\samples\\shared\\" + test_file)

    options.settings.runner_0(options, [test_file])
    options.log.write("%s: " % test_file)
    options.log.flush()
    #test is under way
    #options.log.write("[%s] \n" % status)
    #options.log.flush()


def perform_pre_test():
    options = globs.state.options

    common.proceed4(options)

def perform_after_test():
    options = globs.state.options

    common.proceed5(options)
    write_socket(options.s, "")

def update_stats():
    options = globs.state.options
    status = globs.state.status
    stats = globs.state.stats

    if(status == "MA"):
        globs.state.stats.ma_count += 1
        globs.state.stats.to_count = 0
    if(status == "TO"):
        globs.state.stats.to_count += 1
        if(globs.state.stats.to_count % 3 == 0):
            report("3x TO, settling")
            globs.state.stats.to_count = 0
            common.settle(options)
    globs.state.stats.sample_count += 1

    # print stats if reached metric deadline
    if(stats.sample_count % stats.metric_res == 0):
        current_time = time.localtime()
        elapsed = time.mktime(current_time) - time.mktime(stats.last_time_check)
        report("Tested: " + str(stats.sample_count))
        report(str(stats.metric_res) + " tested in " + str(elapsed) + " seconds")
        report("Last speed: " + str(stats.metric_res/elapsed) + " tps")
        report("MA count: " + str(stats.ma_count))
        globs.state.stats.to_count = 0
        globs.state.stats.ma_count = 0
        globs.state.stats.last_time_check = current_time
        if(options.profiling):
            write_socket(options.s, "dump_stats")


def read_output():
    options = globs.state.options
    state = globs.state

    if(state.samples_exhausted):
        return

    print "waiting for output"

    try:
        (lastResponse, status, reqScript) = read_socket(options.s)
        globs.state.status = status
        globs.state.lastResponse = lastResponse
        globs.state.reqScript = reqScript
    except Exception, e:
        raise MachineError
        globs.state.timeout = True

def read_last_sample():
    return read_output()

def execute_script():
    options = globs.state.options
    common.execute_script(options, globs.state.reqScript)
    write_socket(options.s, "")

def handle_crash():
    #crash is handled internally by binner, maybe we should change it
    print("Got crash, restarting")
    report("Got crash, restarting")

def handle_samples_exhaustion():
    options = globs.state.options

    print("Samples exhaustion")
    report("Samples exhaustion")
    if(not options.save_disks):
        os.remove(options.tmp_disk_img)

    
