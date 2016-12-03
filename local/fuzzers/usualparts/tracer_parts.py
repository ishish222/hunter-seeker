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

def tracer_start_trace(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_start_trace");
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

def tracer_configure_in_dir(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_in_dir %s" % options.sample_options.research_dir);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_configure_out_dir(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_out_dir %s\\%s" % (options.sample_options.out_dir, options.sample_options.out_prefix));
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_configure_out_prefix(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_configure_out_prefix %s" % options.sample_options.out_prefix);
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

def tracer_release_thread(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = globs.state.tid

    write_socket(options.s, "tracer_release_thread 0x%08x" % args);
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

def tracer_configure_sample(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(options.sample_options.sample_file != "none.exe"):
        write_socket(options.s, "tracer_configure_sample_file %s" % options.sample_options.sample_file);
        response, _, _ = read_socket(options.s)

#    if(options.sample_options.sample_process != "none"):
#        write_socket(options.s, "tracer_configure_sample_pname %s" % options.sample_options.sample_process);
#        response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_add_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    parts = args.split(';');
    cmd = '';

    for part in parts:
        if(len(cmd) + len(part) < 0x200):
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

def tracer_read_arg_ansi(args=None):
    options = globs.state.options
    write_socket(options.s, "tracer_read_arg_ansi %s" % args);
    response, _, _ = read_socket(options.s)

def tracer_read_arg_uni(args=None):
    options = globs.state.options
    write_socket(options.s, "tracer_read_arg_uni %s" % args);
    response, _, _ = read_socket(options.s)

def tracer_read_arg(args=None):
    options = globs.state.options
    write_socket(options.s, "tracer_read_arg %s" % args);
    response, _, _ = read_socket(options.s)

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
    
    args = options.settings.builtin_reactions

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

def tracer_register_regions(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
#    args = ','.join(args)
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

def tracer_enable_reaction(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_enable_reaction %s" % args);
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    return

def tracer_enable_anchors(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(hasattr(options.settings, "builtin_reactions_anchors")):
        write_socket(options.s, "tracer_enable_reaction %s" % options.settings.builtin_reactions_anchors);
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

def tracer_set_limit(args=None):
    args = int(args, 10)

    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(options.sample_options.instr_limit > 0x0):
        write_socket(options.s, "tracer_set_limit %d" % options.sample_options.instr_limit);
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

def tracer_read_stack(args = 5):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_read_stack %d" % args);
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
    print "Last TID: 0x%08x" % globs.state.tid
    globs.state.ret = response

    return

def tracer_read_pid(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    args = globs.state.stack.pop()

    write_socket(options.s, "tracer_read_dword 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.pid = int(response[3:11], 0x10)
    print "Last PID: 0x%08x" % globs.state.pid
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

    globs.state.ep = int(response[3:11], 0x10)
    print "Last EP: 0x%08x" % globs.state.ep
    globs.state.ret = response

    return

def tracer_read_dword(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    # id source is null, use last ret
    args = globs.state.stack.pop()

    write_socket(options.s, "tracer_read_dword 0x%08x" % args);
    response, _, _ = read_socket(options.s)

    globs.state.stack.append(int(response[3:11], 0x10))
    globs.state.ret = int(response[3:11], 0x10)

    return

def tracer_write_dword(args = 0x0):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    args = int(args, 0x10)
    
    # id source is null, use last ret
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

    globs.state.stack.append(int(response[3:11], 0x10))
    globs.state.ret = response

    return

def tracer_write_register(args):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "tracer_write_register %s 0x%08x" % args);
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

