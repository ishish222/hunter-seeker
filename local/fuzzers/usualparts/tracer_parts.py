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

def trace_controller_activate_prev_tracer():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tc_avtivate_prev");
    response, _, _ = read_socket(options.s)

    return

def trace_controller_activate_next_tracer():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tc_activate_next");
    response, _, _ = read_socket(options.s)

    return

def tracer_configure_in_dir():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(options.sample_options.sample_file != "None"):
        write_socket(options.s, "tracer_configure_in_dir %s" % options.sample_options.research_dir);
        response, _, _ = read_socket(options.s)
    else:
        print "Error, no research dir"
        exit(1)
    return

def tracer_configure_out_dir():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(options.sample_options.sample_file != "None"):
        write_socket(options.s, "tracer_configure_out_dir %s" % options.sample_options.out_dir);
        response, _, _ = read_socket(options.s)
    else:
        print "Error, no research dir"
        exit(1)
    return

def tracer_configure_out_prefix():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(options.sample_options.sample_file != "None"):
        write_socket(options.s, "tracer_configure_out_prefix %s" % options.sample_options.out_prefix);
        response, _, _ = read_socket(options.s)
    else:
        print "Error, no research dir"
        exit(1)
    return

def tracer_configure_sample():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(options.sample_options.sample_file != "none.exe"):
        write_socket(options.s, "tracer_configure_sample_file %s" % options.sample_options.sample_file);
        response, _, _ = read_socket(options.s)

    if(options.sample_options.sample_process != "None"):
        write_socket(options.s, "tracer_configure_sample_pname %s" % options.sample_options.sample_process);
        response, _, _ = read_socket(options.s)

    return

def tracer_configure_markers():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_markers %s" % (options.sample_options.markers));
    response, _, _ = read_socket(options.s)

    return

def tracer_activate_markers():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_activate_markers");
    response, _, _ = read_socket(options.s)

    return

def tracer_configure_marker_st_end():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_marker_st %s %s" % (options.sample_options.st_mod, options.sample_options.st_addr));
    response, _, _ = read_socket(options.s)
    write_socket(options.s, "tracer_configure_marker_end %s %s" % (options.sample_options.end_mod, options.sample_options.end_addr));
    response, _, _ = read_socket(options.s)

    return

def tracer_debug_sample():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_sample");
    response, _, _ = read_socket(options.s)

    return

def tracer_debug_continue():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_continue");
    response, _, _ = read_socket(options.s)

    return

def tracer_debug_continue_1_second():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_continue_time 1000");
    response, _, _ = read_socket(options.s)

    return

def tracer_debug_continue_10_seconds():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_continue_time 10000");
    response, _, _ = read_socket(options.s)

    return

def tracer_list_markers():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_markers");
    response, _, _ = read_socket(options.s)

    return

def tracer_list_bpts():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_bpts");
    response, _, _ = read_socket(options.s)

    return

def tracer_list_libs():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_libs");
    response, _, _ = read_socket(options.s)

    return

def tracer_list_tebs():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_tebs");
    response, _, _ = read_socket(options.s)

    return

def tracer_read_stack(args = 5):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_read_stack %d" % args);
    response, _, _ = read_socket(options.s)

    return

def tracer_read_dword(args=0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    if(args == None):
        args = int(globs.state.ret[3:11], 0x10)

    write_socket(options.s, "tracer_read_dword 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_write_dword(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    # id source is null, use last ret
    if(args[0] == None):
        args = (int(globs.state.ret[3:11], 0x10), args[1])

    write_socket(options.s, "tracer_write_dword 0x%08x 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    return

def tracer_read_register(args="EIP"):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_read_register %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_write_register(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_write_register %s 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    return

