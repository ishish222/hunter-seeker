from script import rs, rss, runscriptq, write_monitor, write_monitor_2, read_monitor
import time
from statemachine import MachineError
import common
import globs
import os
from .other_parts import defined
from .taint_parts import find_pid

report = common.report
write_socket = common.write_socket
read_socket = common.read_socket

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def tracer_prepare_trace(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_prepare_trace");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def tracer_dump_memory(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_dump_memory");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def flush_files(args=None):
    options = globs.state.options

    write_socket(options.s, "tracer_flush_files")
    response, _, _ = read_socket(options.s)
    return

def tracer_start_strace(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_start_strace");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def tracer_start_trace(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_start_trace");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def tracer_start_trace_light(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_start_trace_light");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def tracer_start_trace_debug(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_start_trace_debug");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def tracer_stop_trace(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_stop_trace");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def trace_controller_activate_prev_tracer(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tc_activate_prev");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def trace_controller_activate_next_tracer(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tc_activate_next");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def trace_controller_current_tracer(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tc_current");
    response, _, _ = read_socket(options.s)

    response = response[12:31]

    globs.state.ret = response

    return

def tracer_configure_in_dir(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_in_dir %s" % options.internal_paths_input);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_configure_out_dir(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_out_dir %s" % (options.internal_paths_output));
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_configure_out_prefix(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_out_prefix %s" % state.out_folder);
    response, _, _ = read_socket(options.s)
    globs.state.ret = response

    return

def tracer_configure_pid_prefix(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_out_prefix %s" % globs.state.pid);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_append_out_prefix(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_append_out_prefix %s" % state.out_prefix);
    response, _, _ = read_socket(options.s)
    globs.state.ret = response

    return

def tracer_append_pid_prefix(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_append_out_prefix %s" % globs.state.pid);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_current_tid(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_current_tid");
    response, _, _ = read_socket(options.s)

    globs.state.ret = int(response[8:18], 0x10)
    return

def tracer_resize_out_buffer(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.stack.pop()

    write_socket(options.s, "tracer_resize_out_buffer %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_resize_mod_buffer(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.stack.pop()

    write_socket(options.s, "tracer_resize_mod_buffer %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_change_interval(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.stack.pop()

    write_socket(options.s, "tracer_change_interval %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_set_priority_high(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.stack.pop()

    write_socket(options.s, "tracer_set_priority_high 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_suspend_current_thread(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.tid

    write_socket(options.s, "tracer_suspend_thread 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_release_current_thread(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.tid

    write_socket(options.s, "tracer_release_thread 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_suspend_thread(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    val = globs.state.stack.pop()

    if(type(val) is not int):
        val = int(val, 0x10)

    write_socket(options.s, "tracer_suspend_thread 0x%08x" % val);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_release_thread(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    val = globs.state.stack.pop()

    if(type(val) is not int):
        val = int(val, 0x10)

    write_socket(options.s, "tracer_release_thread 0x%08x" % val);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_suspend_all(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_suspend_all");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_suspend_all_except(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.stack.pop()

    write_socket(options.s, "tracer_suspend_all_except 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_release_all(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_release_all");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_release_all_except(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.tid

    write_socket(options.s, "tracer_release_all_except 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_configure_sample_pid(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.pid

    write_socket(options.s, "tracer_configure_sample_pid 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_reopen_io(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_reopen_io");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_configure_sample(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(state.sample_name != "none.exe"):
        write_socket(options.s, "tracer_configure_sample_file %s" % state.sample_name);
        response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_get_current_config(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_get_current_config");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_add_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    parts = args.split(';');
    cmd = '';

    for part in parts:
        if(len(cmd) + len(part) < 0x100):
            cmd += ';'
            cmd += part
        else:
            write_socket(options.s, "tracer_register_reactions %s" % cmd[1:]);
            response, _, _ = read_socket(options.s)
            cmd = ''
            cmd += part

    if(len(cmd) > 0x0): 
        write_socket(options.s, "tracer_register_reactions %s" % cmd[1:]);
        response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_run_routine(args=None):
    options = globs.state.options
    write_socket(options.s, "tracer_run_routine %s" % args);
    response, _, _ = read_socket(options.s)

def read_ansi(args=None):
    options = globs.state.options
    
    if(args == None):
        addr = globs.state.ret
    else:
        addr = args

    write_socket(options.s, "tracer_read_string_ansi 0x%08x" % addr)
    response, _, _ = read_socket(options.s)
    globs.state.ret = response[3:]
    globs.state.ret = globs.state.ret.split()[0]

def read_unicode(args=None):
    options = globs.state.options
    
    if(args == None):
        addr = globs.state.ret
    else:
        addr = args

    write_socket(options.s, "tracer_read_string_unicode 0x%08x" % addr)
    response, _, _ = read_socket(options.s)
    globs.state.ret = response[3:]
    globs.state.ret = globs.state.ret.split()[0]

def set_base(args=None):
    options = globs.state.options
    addr = globs.state.stack.pop()

    if(type(addr) is not int):
        addr = int(addr, 0x10)

    write_socket(options.s, "tracer_set_base %s 0x%08x" % (args, addr));
    response, _, _ = read_socket(options.s)

def write_ansi(args=None):
    options = globs.state.options
    
    addr = globs.state.ret
    if(args == None):
        args = globs.state.stack.pop()

    write_socket(options.s, "tracer_write_string_ansi 0x%08x %s" % (addr, args));
    response, _, _ = read_socket(options.s)

def write_unicode(args=None):
    options = globs.state.options
    addr = globs.state.ret

    write_socket(options.s, "tracer_write_string_unicode 0x%08x %s" % (addr, args));
    response, _, _ = read_socket(options.s)

def tracer_read_arg_ansi(args=None):
    options = globs.state.options
    write_socket(options.s, "tracer_read_arg_ansi %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response[3:]
    globs.state.ret = globs.state.ret.split()[0]
    print('ret is: %s ' % globs.state.ret)

def tracer_read_arg_uni(args=None):
    options = globs.state.options
    write_socket(options.s, "tracer_read_arg_uni %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response[3:]
    globs.state.ret = globs.state.ret.split()[0]
    print('ret is: %s ' % globs.state.ret)

def tracer_read_arg(args=None):
    options = globs.state.options
    write_socket(options.s, "tracer_read_arg %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response[3:]
    globs.state.ret = globs.state.ret.split()[0]

def tracer_manual_st_w_self(args=None):
    options = globs.state.options
    
    if(type(globs.state.ep) == str):
        globs.state.ep = int(globs.state.ep, 0x10)

    write_socket(options.s, "tracer_register_reactions self+0x%08x,ST,0x0" % globs.state.ep);
    response, _, _ = read_socket(options.s)

def tracer_manual_st(args=None):
    options = globs.state.options
    
    if(type(globs.state.ep) == str):
        globs.state.ep = int(globs.state.ep, 0x10)

    write_socket(options.s, "tracer_register_reactions 0x%08x,ST,0x0" % globs.state.ep);
    response, _, _ = read_socket(options.s)


def tracer_manual_st_from_arg(args=None):
    options = globs.state.options
    
    ep = options.sample_options.st_string

    if(type(ep) == str):
        ep = int(ep, 0x10)

    write_socket(options.s, "tracer_register_reactions 0x%08x,ST,0x0" % ep);
    response, _, _ = read_socket(options.s)


def tracer_register_reactions_at(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    args = ','.join(args)
    loc = globs.state.stack.pop()
    if(type(loc) == int):
        loc = '0x%08x' % loc
    args = loc+','+args

    # remove new lines 
    args = args.replace('\n', '')
    if(args[-1:] == ';'):
        args = args[:-1]

    parts = args.split(';');
    cmd = '';

    for part in parts:
        if(len(cmd) + len(part) < 0x100):
            cmd += ';'
            cmd += part
        else:
            write_socket(options.s, "tracer_register_reactions %s" % cmd[1:]);
            response, _, _ = read_socket(options.s)
            cmd = ''
            cmd += ';'
            cmd += part

    if(len(cmd) > 0x1): 
        write_socket(options.s, "tracer_register_reactions %s" % cmd[1:]);
        response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_register_reactions(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    args = ','.join(args)

    # remove new lines 
    args = args.replace('\n', '')
    if(args[-1:] == ';'):
        args = args[:-1]

    parts = args.split(';');
    cmd = '';

    for part in parts:
        if(len(cmd) + len(part) < 0x100):
            cmd += ';'
            cmd += part
        else:
            write_socket(options.s, "tracer_register_reactions %s" % cmd[1:]);
            response, _, _ = read_socket(options.s)
            cmd = ''
            cmd += ';'
            cmd += part

    if(len(cmd) > 0x1): 
        write_socket(options.s, "tracer_register_reactions %s" % cmd[1:]);
        response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_register_builtin(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    args = options.internal_sys_reactions

    # remove comments after ;
    import re
    args = re.sub(r';.*\n', ';', args)
    
    # remove new lines 
    args = args.replace('\n', '')


    if(args[-1:] == ';'):
        args = args[:-1]

    parts = args.split(';');
    cmd = '';

    for part in parts:
        if(len(cmd) + len(part) < 0x100):
            cmd += ';'
            cmd += part
        else:
            write_socket(options.s, "tracer_register_reactions %s" % cmd[1:]);
            response, _, _ = read_socket(options.s)
            cmd = ''
            cmd += ';'
            cmd += part

    if(len(cmd) > 0x1): 
        write_socket(options.s, "tracer_register_reactions %s" % cmd[1:]);
        response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_handle_exceptions(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_handle_exceptions");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_read_debug_log(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_read_debug_log");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_debug_log_enable(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_log_enable");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_crash_host(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_crash_host");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_taint_last_region(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_taint_last_region");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_taint_regions(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_taint_regions");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_add_scanned_location(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        size = globs.state.stack.pop()
        addr = globs.state.stack.pop()
        args = '%s:%s' % (addr, size)
    write_socket(options.s, "tracer_add_scanned_location %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_check_region(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        size = globs.state.stack.pop()
        addr = globs.state.stack.pop()
        args = '%s:%s' % (addr, size)
#    if(options.sample_options.regions == "0"): return
    write_socket(options.s, "tracer_check_region %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_register_regions(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        size = globs.state.stack.pop()
        addr = globs.state.stack.pop()
        args = '%s:%s' % (addr, size)
#    if(options.sample_options.regions == "0"): return
    write_socket(options.s, "tracer_register_regions %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_configure_markers(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_markers %s" % (options.sample_options.markers));
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_set_st(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_set_st 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_auto_st(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_auto_st");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_activate_markers(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_activate_markers");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_configure_marker_st_end(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_marker_st %s %s" % (options.sample_options.st_mod, options.sample_options.st_addr));
    response, _, _ = read_socket(options.s)
    write_socket(options.s, "tracer_configure_marker_end %s %s" % (options.sample_options.end_mod, options.sample_options.end_addr));
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_exclusive_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_exclusive_reaction %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_deautorepeat_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_deautorepeat_reaction %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_autorepeat_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_autorepeat_reaction %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_raise_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_raise_reaction %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_lower_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_lower_reaction %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_enable_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_enable_reaction %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_disable_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_disable_reaction %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_lower_anchors(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(hasattr(options, "internal_sys_reactions_anchors")):
        args = options.internal_sys_reactions_anchors


        if(args[-1:] == ':'):
            args = args[:-1]

        parts = args.split(':');
        cmd = '';

        for part in parts:
            if(len(cmd) + len(part) < 0x100):
                cmd += ':'
                cmd += part
            else:
                write_socket(options.s, "tracer_lower_reaction %s" % cmd[1:]);
                response, _, _ = read_socket(options.s)
                cmd = ''
                cmd += ':'
                cmd += part

        if(len(cmd) > 0x1):
            write_socket(options.s, "tracer_lower_reaction %s" % cmd[1:]);
            response, _, _ = read_socket(options.s)

    return

def tracer_exclusive_anchors(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(hasattr(options, "internal_sys_reactions_anchors")):
        args = options.internal_sys_reactions_anchors


        if(args[-1:] == ':'):
            args = args[:-1]

        parts = args.split(':');
        cmd = '';

        for part in parts:
            if(len(cmd) + len(part) < 0x100):
                cmd += ':'
                cmd += part
            else:
                write_socket(options.s, "tracer_exclusive_reaction %s" % cmd[1:]);
                response, _, _ = read_socket(options.s)
                cmd = ''
                cmd += ':'
                cmd += part

        if(len(cmd) > 0x1):
            write_socket(options.s, "tracer_exclusive_reaction %s" % cmd[1:]);
            response, _, _ = read_socket(options.s)

    return

def tracer_disable_anchors(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(hasattr(options, "internal_sys_reactions_anchors")):
        args = options.internal_sys_reactions_anchors


        if(args[-1:] == ':'):
            args = args[:-1]

        parts = args.split(':');
        cmd = '';

        for part in parts:
            if(len(cmd) + len(part) < 0x100):
                cmd += ':'
                cmd += part
            else:
                write_socket(options.s, "tracer_disable_reaction %s" % cmd[1:]);
                response, _, _ = read_socket(options.s)
                cmd = ''
                cmd += ':'
                cmd += part

        if(len(cmd) > 0x1):
            write_socket(options.s, "tracer_disable_reaction %s" % cmd[1:]);
            response, _, _ = read_socket(options.s)

        globs.state.ret = response

    return

def tracer_enable_anchors(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(hasattr(options, "internal_sys_reactions_anchors")):
        args = options.internal_sys_reactions_anchors


        if(args[-1:] == ':'):
            args = args[:-1]

        parts = args.split(':');
        cmd = '';

        for part in parts:
            if(len(cmd) + len(part) < 0x100):
                cmd += ':'
                cmd += part
            else:
                write_socket(options.s, "tracer_enable_reaction %s" % cmd[1:]);
                response, _, _ = read_socket(options.s)
                cmd = ''
                cmd += ':'
                cmd += part

        if(len(cmd) > 0x1):
            write_socket(options.s, "tracer_enable_reaction %s" % cmd[1:]);
            response, _, _ = read_socket(options.s)

        globs.state.ret = response

    return

def tracer_enable_all_reactions(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_enable_all_reactions");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_disable_all_reactions(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_disable_all_reactions");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_enable_sysenter(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_enable_reaction 0x0");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_attach_sample(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_attach_sample");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_debug_sample(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_sample");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_debug_continue_decision(args):
    if(args == None):
        args = globs.DEBUG_CONTINUE
    else:
        args = int(args, 0x10)

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_continue 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    if(globs.state.ret[1:3] == "RB"):
        bp = globs.state.ret[3:].split('\n')[0]
        print(bcolors.WARNING + bcolors.BOLD + bp + bcolors.ENDC)
        print('Returning: %s' % bp)
        return bp
    else:
        print('Returning: %s' % globs.state.ret[1:3])
        return globs.state.ret[1:3]

    return

def tracer_debug_continue(args):
    if(args == None):
        args = globs.DEBUG_CONTINUE
    else:
        args = int(args, 0x10)

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_continue 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def tracer_debug_continue_time(args = globs.DEBUG_CONTINUE):
    if(args == None):
        args = globs.DEBUG_CONTINUE
    else:
        args = int(args, 0x10)

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    time = globs.state.stack.pop()
    if(type(time) != int):
        time = int(time, 10)

    write_socket(options.s, "tracer_debug_continue_time %d 0x%08x" % (time, args));
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_debug_continue_1_second(args = globs.DEBUG_CONTINUE):
    if(args == None):
        args = globs.DEBUG_CONTINUE
    else:
        args = int(args, 0x10)

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_continue_time 1000 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_debug_continue_10_seconds(args = globs.DEBUG_CONTINUE):
    if(args == None):
        args = globs.DEBUG_CONTINUE
    else:
        args = int(args, 0x10)

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_debug_continue_time 10000 %x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_set_debug_timeout(args=None):
    args = int(args, 10)

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_set_debug_timeout %d" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_set_limit(args=None):
    args = int(args, 10)

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(state.instr_limit > 0x0):
        write_socket(options.s, "tracer_set_limit %d" % state.instr_limit);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_list_markers(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_markers");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_get_exception_chance(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_get_exception_chance");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    globs.state.ret = 'Chance: ' + globs.state.ret[3:].split('\n')[0]

    return

def tracer_get_exception_code(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_get_exception_code");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    globs.state.ret = 'Code: ' + globs.state.ret[3:].split('\n')[0]

    return

def tracer_get_exception_address_str(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_get_exception_address");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    globs.state.ret = globs.state.ret[3:].split('\n')[0]

    return

def tracer_get_exception_address(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_get_exception_address");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    globs.state.ret = 'Address: ' + globs.state.ret[3:].split('\n')[0]

    return

def tracer_list_bpts(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_bpts");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_list_libs(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_libs");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_ps(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_ps");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_list_tebs(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_tebs");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_list_all_tebs(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_list_all_tebs");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_get_caller(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.stack.pop()

    if(type(args) == 'str'):
        args = int(args, 0x10)
    
    write_socket(options.s, "tracer_get_caller %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_read_stack(args = 5):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = 5
    
    write_socket(options.s, "tracer_read_stack %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_read_tid(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    args = globs.state.stack.pop()

    write_socket(options.s, "tracer_read_dword 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.tid = int(response[3:11], 0x10)
    print("Last TID: 0x%08x" % globs.state.tid)
    globs.state.ret = response

    return

def tracer_set_pid(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    if(args == None):
        args = globs.state.stack.pop()

    if(type(args) == str):
        globs.state.pid = int(args, 0x10)
    else:
        globs.state.pid = args

    print("Last PID: 0x%08x" % globs.state.pid)

    return

def tracer_read_pid(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    args = globs.state.stack.pop()

    write_socket(options.s, "tracer_read_dword 0x%08x" % args);
    response, _, _ = read_socket(options.s)
    response = response[3:11]

    globs.state.pid = int(response, 0x10)
    print("Last PID: 0x%08x" % globs.state.pid)
    globs.state.ret = response

    return

def tracer_read_ep(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    args = globs.state.stack.pop()

    write_socket(options.s, "tracer_read_dword 0x%08x" % args);
    response, _, _ = read_socket(options.s)
    response = response[3:11]

    globs.state.ep = int(response, 0x10)
    print("Last EP: 0x%08x" % globs.state.ep)
    globs.state.ret = response

    return

def tracer_print_result(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    if(args == None):
        args = globs.state.ret

    write_socket(options.s, "tracer_print %s" % args);
    response, _, _ = read_socket(options.s)

    return

def tracer_out_region(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    size = globs.state.stack.pop()
    offset = globs.state.stack.pop()

    write_socket(options.s, "tracer_out_region 0x%08x 0x%08x" % (offset, size));
    response, _, _ = read_socket(options.s)

    return

def tracer_read_region(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    size = globs.state.stack.pop()
    offset = globs.state.stack.pop()

    write_socket(options.s, "tracer_read_region 0x%08x 0x%08x" % (offset, size));
    response, _, _ = read_socket(options.s)

    return

def tracer_read_dword(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    args = globs.state.stack.pop()

    write_socket(options.s, "tracer_read_dword 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    #globs.state.stack.append(int(response[3:11], 0x10))
    globs.state.ret = int(response[3:11], 0x10)

    return

def tracer_write_dword(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    args = int(args, 0x10)
    
    # if source is null, use last ret
    args = (globs.state.stack.pop(), args)

    write_socket(options.s, "tracer_write_dword 0x%08x 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_read_register(args="EIP"):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_read_register %s" % args);
    response, _, _ = read_socket(options.s)

#   according to specification stack is intact
#    globs.state.stack.append(int(response[3:11], 0x10))
    globs.state.ret = int(response[3:11], 0x10)

    return

def tracer_write_register(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    val = globs.state.stack.pop()

    if(type(val) is not int):
        val = int(val, 0x10)

    write_socket(options.s, "tracer_write_register %s 0x%08x" % (args, val));
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_spawn(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_spawn %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return
def tracer_cooldown(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_cooldown");
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_resolve_location(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_resolve_location %s" % args);
    response, _, _ = read_socket(options.s)

#    globs.state.stack.append(int(response[3:13], 0x10))
    globs.state.ret = int(response[3:13], 0x10)
    return

