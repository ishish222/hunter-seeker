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

def spawn_responder_80(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "spawn_responder_80")
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def spawn_responder(args=80):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "spawn_responder %s" % args)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def next_response_from_file(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        write_socket(options.s, "responder_next_response_from_file %s" % globs.state.ret)
    else:
        write_socket(options.s, "responder_next_response_from_file %s" % args)

    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def next_response(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        write_socket(options.s, "responder_next_response %s" % globs.state.ret)
    else:
        write_socket(options.s, "responder_next_response %s" % args)

    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def get_dns(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "get_dns %s" % args)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return


def get_http(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "get_http %s" % args)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return


def start_log(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "logStart %s" % args)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return


def change_ip(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "change_ip %s" % args)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return

def run_cmd(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    write_socket(options.s, "run_cmd %s" % args)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response
    return


