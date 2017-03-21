from script import rs, rss, runscriptq, write_monitor, write_monitor_2, read_monitor
import time
from statemachine import MachineError
import common
import globs
import os
from other_parts import defined
from taint_parts import find_pid

report = common.report
write_socket = common.write_socket
read_socket = common.read_socket

def get_sample_options(args=None):
    options = globs.state.options
    parser = options.parser

    parser.add_option("", "--sample-file",dest="sample_file", help="Starting module for trace", default="none.exe")
    parser.add_option("", "--sample-process",dest="sample_process", help="Starting module for trace", default="None")
    parser.add_option("", "--research-dir",dest="research_dir", help="Starting module for trace", default="None")
    parser.add_option("", "--out-dir",    dest="out_dir", help="Starting module for trace", default="None")
    parser.add_option("", "--out-prefix", dest="out_prefix", help="Starting module for trace", default="None")
    parser.add_option("", "--st",         dest="st_string", help="Starting module for trace", default="0x0")
    parser.add_option("", "--odir",       dest="out_dir", help="Out dir for trace", default="\\\\10.0.2.4\\qemu\\")
    parser.add_option("", "--odir2",      dest="out_dir2", help="Appendix for trace output", default="last")
    parser.add_option("", "--prefix",     dest="prefix", help="Prefix for trace", default="last")
    parser.add_option("", "--logpath",    dest="log_path", help="Log path for trace", default="\\\\10.0.2.4\\qemu\\last_log.txt")
    parser.add_option("", "--limit",      dest="instr_limit", help="Instruction limit", default="0")
    parser.add_option("", "--markers",    dest="markers", help="Instruction limit", default="0")
    parser.add_option("", "--reactions",    dest="reactions", help="Instruction limit", default="0")
    parser.add_option("", "--regions",    dest="regions", help="Instruction limit", default="0")

    (options.sample_options, args) = parser.parse_args()
    options.sample_options.instr_limit = int(options.sample_options.instr_limit)

def get_additional_options(args=None):
    options = globs.state.options
    parser = options.parser

    parser.add_option("", "--smod",       dest="st_mod", help="Starting module for trace", default="0x0")
    parser.add_option("", "--emod",       dest="end_mod", help="Ending module for trace", default="kernel32.dll")
    parser.add_option("", "--saddr",      dest="st_addr", help="Starting address for trace", default="0x1b7c")
    parser.add_option("", "--eaddr",      dest="end_addr", help="Ending address for trace", default="0x52acf")
    parser.add_option("", "--odir",       dest="out_dir", help="Out dir for trace", default="\\\\10.0.2.4\\qemu\\")
    parser.add_option("", "--odir2",      dest="out_dir2", help="Appendix for trace output", default="last")
    parser.add_option("", "--prefix",     dest="prefix", help="Prefix for trace", default="last")
    parser.add_option("", "--logpath",    dest="log_path", help="Log path for trace", default="\\\\10.0.2.4\\qemu\\last_log.txt")
    parser.add_option("", "--limit",      dest="instr_limit", help="Instruction limit", default="0")
    parser.add_option("", "--descriptor", dest="descriptor", help="Instruction limit", default="default.txt")

    (options.additional_options, args) = parser.parse_args()

def spawn_internal_controller(args=None):
    options = globs.state.options

    rs("lclick", options.m)
    rs("python_spawn_internal_controller", options.m)

def spawn_internal_controller_no_thread(args=None):
    options = globs.state.options

    rs("lclick", options.m)
    rs("python_spawn_internal_controller_no_thread", options.m)

def reset_tracer_controller_status(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    state.tracers = []
    state.tracers_count = 0
    return

def start_tracer(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "spawn_tracer");
    response, _, _ = read_socket(options.s)

    # register controller on success
    state.tracers.append(response)
    state.tracers_count += 1

def start_tracer_no_log(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "spawn_tracer_no_log");
    response, _, _ = read_socket(options.s)

    # register controller on success
    state.tracers.append(response)
    state.tracers_count += 1

def start_tracer_log(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "spawn_tracer_log");
    response, _, _ = read_socket(options.s)

    # register controller on success
    state.tracers.append(response)
    state.tracers_count += 1

def start_tracer_log_remote(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "spawn_tracer_log_remote");
    response, _, _ = read_socket(options.s)

    # register controller on success
    state.tracers.append(response)
    state.tracers_count += 1

def read_prefix(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "read_prefix");
    response, _, _ = read_socket(options.s)

def stop_tracer(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "close_tracer");
    response, _, _ = read_socket(options.s)

    # register controller on success
    state.tracers.append(response)
    state.tracers_count -= 1

def more_tracers(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(state.tracers_count > 0):
        return 'Y'
    else:
        return 'N'

def which_tracer(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "which_tracer");
    response, _, _ = read_socket(options.s)

def secure_all_sections(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_secure_all_sections");
    response, _, _ = read_socket(options.s)
    return

def spawn_tracer_controller(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "start_trace_controller");
    response, _, _ = read_socket(options.s)
    return

def set_parameters(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(type(args) == list):
        args = ",".join(args)

    write_socket(options.s, "tracer_set_parameters %s" % args);
    response, _, _ = read_socket(options.s)
    return

def extract_ep(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "extract_ep %s" % args);
    response, _, _ = read_socket(options.s)
    response = response.split('\n')
    response = response[1]
    globs.state.ret = response
    print 'r'+response+'r'
    return

def stop_tracer_controller(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "stop_trace_controller");
    response, _, _ = read_socket(options.s)
    return

def trace_sample2(args=None):
    # sledzenie wykonania probki w zakresie wybranych adresow

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    additional = options.additional_options

    try:
        sample_path = globs.state.samples_list.pop()
    except Exception, e:
        print e
        globs.state.samples_exhausted = True
        return

    sample_file = os.path.basename(sample_path)
    test_path = options.settings.prepare_sample(sample_path)
    test_file = os.path.basename(test_path)

    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.settings.qemu_shared_folder+"/"+additional.out_dir2])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, options.settings.qemu_shared_folder+"/"+additional.out_dir2])

    if(options.walk_start == None):
        write_socket(options.s, "trace3 e:\\server\\a.exe e:\\samples\\shared\\%s %s %s %s %s %s %s %s %s" % (test_file, additional.st_mod, additional.st_addr, additional.end_mod, additional.end_addr, additional.out_dir+additional.out_dir2, additional.prefix, additional.log_path, additional.instr_limit))
#        write_socket(options.s, "trace3 e:\\server\\a.exe e:\\samples\\shared\\%s %s %s %s %s %s %s %s" % (test_file, additional.st_mod, additional.st_addr, additional.end_mod, additional.end_addr, additional.out_dir, additional.prefix, additional.log_path))
    else:
        pass
#        write_socket(options.s, "walk2 e:\\samples\\shared\\%s %d f:\\%s.mm %s" % (test_file, options.walk_level, test_file, options.walk_start))

#    options.s2, _ = options.ss2.accept()
#
    options.settings.runner_0(options, [test_file])
    options.log.write("%s: " % test_file)
    options.log.flush()
#    
    print "Waiting for instruction"
##    lastResponse = ''
##
##    while(lastResponse!='end'):
##        (lastResponse, status, reqScript) = read_socket(options.s2)
##        print lastResponse
#    buf = ''
#
#    out = options.out_path
#    fout = open(out, 'w+')
#
    while True:
        pass
#        buf = options.s2.recv(0x1000000)
#        fout.write(buf)
#
#    fout.close()            


def trace_sample2_2(args=None):
    # sledzenie wykonania probki ze wskazaniem deskryptora

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    additional = options.additional_options

    try:
        sample_path = globs.state.samples_list.pop()
    except Exception, e:
        print e
        globs.state.samples_exhausted = True
        return

    sample_file = os.path.basename(sample_path)
    test_path = options.settings.prepare_sample(sample_path)
    test_file = os.path.basename(test_path)

    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.settings.qemu_shared_folder+"/"+additional.out_dir2])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, options.settings.qemu_shared_folder+"/"+additional.out_dir2])

    if(options.walk_start == None):
        write_socket(options.s, "trace3 e:\\server\\b.exe e:\\samples\\shared\\%s e:\\samples\\shared\\%s %s %s %s %s" % (test_file, additional.descriptor, additional.out_dir+additional.out_dir2, additional.prefix, additional.log_path, additional.instr_limit))
    else:
        pass

    options.settings.runner_0(options, [test_file])
    options.log.write("%s: " % test_file)
    options.log.flush()
    
    print "Waiting for instruction"
    while True:
        pass


def configure_port(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    #write_socket(options.s, "trace e:\\samples\\shared\\%s %d f:\\%s.mm" % (test_file, options.walk_level, test_file))
    write_socket(options.s, "openserialport")

def detach_debugger(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    write_socket(options.s, "detachdebugger")

def trace_sample(args=None):
    # sledzenie wykonania probki pomiedzy wybranymi markerami

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    additional = options.additional_options

    try:
        sample_path = globs.state.samples_list.pop()
    except Exception, e:
        print e
        globs.state.samples_exhausted = True
        return

    sample_file = os.path.basename(sample_path)
    test_path = options.settings.prepare_sample(sample_path)
    test_file = os.path.basename(test_path)

    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.settings.qemu_shared_folder+"/"+additional.out_dir2])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, options.settings.qemu_shared_folder+"/"+additional.out_dir2])

    if(options.walk_start == None):
#        write_socket(options.s, "walk e:\\samples\\shared\\%s %d f:\\%s.mm" % (test_file, options.walk_level, test_file))
#        write_socket(options.s, "trace e:\\samples\\shared\\%s" % (test_file))
#        write_socket(options.s, "trace2 e:\\samples\\shared\\%s %d" % (test_file, globs.state.pid))
        write_socket(options.s, "trace4 e:\\server\\a.exe %d %s %s %s %s %s %s %s %s" % (state.pid, additional.st_mod, additional.st_addr, additional.end_mod, additional.end_addr, additional.out_dir+additional.out_dir2, additional.prefix, additional.log_path, additional.instr_limit))
#        write_socket(options.s, "trace4 e:\\server\\a.exe %d %s %s %s %s %s %s %s" % (state.pid, additional.st_mod, additional.st_addr, additional.end_mod, additional.end_addr, additional.out_dir, additional.prefix, additional.log_path))

    else:
        pass
#        write_socket(options.s, "walk2 e:\\samples\\shared\\%s %d f:\\%s.mm %s" % (test_file, options.walk_level, test_file, options.walk_start))

#    options.s2, _ = options.ss2.accept()
#
    options.settings.runner_0(options, [test_file])
    options.log.write("%s: " % test_file)
    options.log.flush()
#    
    print "Waiting for instruction"

def test_sample(args=None):
    # test single sample
    
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # resetowanie stanu
    globs.state.status = ""
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

def open_sample(args=None):
    # test single sample
    
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # resetowanie stanu
    globs.state.status = ""
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
    write_socket(options.s, "openFile e:\\samples\\shared\\" + test_file)

    options.settings.runner_0(options, [test_file])
    options.log.write("%s: " % test_file)
    options.log.flush()

def perform_pre_test(args=None):
    options = globs.state.options

    common.proceed4(options)

def perform_after_test(args=None):
    options = globs.state.options

    # zamknij probke
    common.proceed5(options)
    write_socket(options.s, "")

def log_result(args=None):
    options = globs.state.options

    # wpis w logu
    options.log.write("[%s] \n" % globs.state.status)
    options.log.flush()

def update_stats(args=None):
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


def read_output(args=None):
    options = globs.state.options
    state = globs.state

    if(state.samples_exhausted):
        return

    try:
        (lastResponse, status, reqScript) = read_socket(options.s)
        globs.state.status = status
        globs.state.lastResponse = lastResponse
        globs.state.reqScript = reqScript

    except Exception, e:
        globs.state.timeout = True
        raise MachineError

def read_last_sample(args=None):
    return read_output()

def execute_script(args=None):
    options = globs.state.options
    common.execute_script(options, globs.state.reqScript)
    write_socket(options.s, "")

def handle_crash(args=None):
    #crash is handled internally by binner, maybe we should change it
    print("Got crash, restarting")
    report("Got crash, restarting")

def handle_samples_exhaustion(args=None):
    options = globs.state.options

    print("Samples exhaustion")
    report("Samples exhaustion")
    if(not options.save_disks):
        os.remove(options.tmp_disk_img)

    
def run_ret(args=None):
    options = globs.state.options
    rss("ret", options.m, options.slowdown)

def load_ep(args = None):
    globs.state.ret = globs.state.ep

def set_glob_pattern(args = None):
    print "Setting glob pattern to %s" % args
    options = globs.state.options
    options.glob_pattern = args

def set_sample_file(args = None):
    print "Setting sample file to %s" % args
    options = globs.state.options
    options.sample_options.sample_file = args

def set_research_dir(args = None):
    print "Setting research dir to %s" % args
    options = globs.state.options
    options.sample_options.research_dir = args

def set_out_dir(args = None):
    print "Setting out dir to %s" % args
    options = globs.state.options
    options.sample_options.out_dir = args

def check_dir(directory):
    import os
    try:
        os.stat(directory)
    except:
        os.mkdir(directory)

def check_host_dir(args = None):
    options = globs.state.options

    # here check for existence of all necessary dirs. If absent, create them
    
    check_dir(options.settings.qemu_shared_folder + '/' + options.sample_options.out_prefix)
    check_dir(options.settings.log_dir)
    check_dir(options.settings.saved_dir)
    check_dir(options.settings.qemu_drives_dir)

def save_first_ep(args = None):
    options = globs.state.options
    globs.state.ep = options.sample_options.st_string

def save_offset(args = None):
    if(args == None):
        globs.state.offset = globs.state.ret
    else:
        globs.state.offset = args

def save_size(args = None):
    if(args == None):
        globs.state.size = globs.state.ret
    else:
        globs.state.size = args

def save_ep(args = None):
    if(args == None):
        globs.state.ep = globs.state.ret
    else:
        globs.state.ep = args

def write_last_suspension(args = None):
    globs.state.last_suspension = globs.state.ret


def read_last_suspension(args = None):
    globs.state.ret = globs.state.last_suspension

def decision(args=None):
    options = globs.state.options

    print globs.state.ret[1:3]
    print globs.state.ret

    if(globs.state.ret[1:3] == "RB"):
        bp = globs.state.ret[3:].split('\n')[0]
        return bp
    else:
        return globs.state.ret[1:3]


def interrupt(args=None):
    options = globs.state.options
    
    write_socket(options.s, "interrupt");
    response, _, _ = read_socket(options.s)

    return

def dump_file(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    if(args == None):
        args = globs.state.ret
    
    write_socket(options.s, "run_cmd copy %s \\\\10.0.2.4\\qemu\\dump" % args)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return
