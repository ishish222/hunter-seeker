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

def tracer_configure_dir():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(options.sample_options.sample_file != "None"):
        write_socket(options.s, "tracer_configure_research_dir %s" % options.sample_options.research_dir);
        response, _, _ = read_socket(options.s)

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

def tracer_list_tids():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_tids");
    response, _, _ = read_socket(options.s)

    return

def tracer_print_stack():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_print_stack");
    response, _, _ = read_socket(options.s)

    return

def tracer_read_memory():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_read_memory");
    response, _, _ = read_socket(options.s)

    return

def tracer_write_memory():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_write_memory");
    response, _, _ = read_socket(options.s)

    return

def tracer_read_register():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_read_register");
    response, _, _ = read_socket(options.s)

    return

def tracer_write_register():
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_write_register");
    response, _, _ = read_socket(options.s)

    return

