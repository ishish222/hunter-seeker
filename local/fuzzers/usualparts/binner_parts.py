import statemachine
import globs
import os
import queue 
from datetime import datetime, timedelta
import common

report = common.report
write_socket = common.write_socket
read_socket = common.read_socket


options = globs.state.options

class Binner(object):
    def __init__(self):
        state = globs.state

        self.left_in_batch = len(state.samples_list)
        self.current_sample_name = None
        self.current_sample_base = None
        self.current_sample_path = None
        self.addresses = set()
        self.samples_confirmed = 0
        self.samples_binned = 0
        self.timeout_count = 0
        self.start_time = datetime.now()
        

def init_binner(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    globs.state.binner = Binner()

    return

def load_samples(args = None):
    print('Deprecated')
    return

def report(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    if(len(globs.state.binner.addresses) > 0):
        print('===')
        print('Crash addresses:')
        for address in globs.state.binner.addresses:
            print('\t%s' % address)
        print('===')
    print('Binning started: %s' % globs.state.binner.start_time)
    print('Now its: %s' % datetime.now())
    print('Binned %s samples' % globs.state.binner.samples_binned)
    print('Confirmed %s samples' % globs.state.binner.samples_confirmed)
    print('Timout encountered %s times' % globs.state.binner.timeout_count)
    if(globs.state.binner.samples_binned > 0x0):
        print('Confirm factor is %s ' % (globs.state.binner.samples_confirmed / globs.state.binner.samples_binned))
    if(globs.state.binner.samples_confirmed > 0x0):
        print('Timout factor is %s ' % (globs.state.binner.timeout_count / globs.state.binner.samples_confirmed))
    print('Current sample: %s' % globs.state.binner.current_sample_name)
    time_elapsed = datetime.now() - globs.state.binner.start_time
    print('Time elapsed: %s' % time_elapsed)
    print('Binning speed in this session: %s/sample' % (time_elapsed / globs.state.binner.samples_binned))

    return

def batch_exhausted(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    print('Still {} samples left'.format(len(state.samples_list)))
    if(len(state.samples_list) > 0):
        return "N"
    else:
        return "Y"

def get_current_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.ret = state.binner.current_sample_name
    print('ret: %s' % state.ret)

    return

def get_next_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    from os.path import basename, dirname

    state.binner.current_sample_path = state.samples_list.pop()    
    state.binner.current_sample_name = basename(state.binner.current_sample_path)
    state.binner.current_sample_base = dirname(state.binner.current_sample_path)
    state.binner.left_in_batch = state.binner.left_in_batch -1
    state.binner.samples_binned = state.binner.samples_binned +1
    print('Loaded sample: %s' % state.binner.current_sample_name)
    print('Samples left: 0x%08x' % state.binner.left_in_batch)

    state.ret = state.binner.current_sample_name
    print('ret: %s' % state.ret)

    return

def current_sample_name(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.ret = state.binner.current_sample_name
    print('ret: %s' % state.ret)

    return

def current_sample_path(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.ret = state.binner.current_sample_path
    print('ret: %s' % state.ret)

    return

def current_sample_drop(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    import os

    if(state.binner.current_sample_path != None):
        os.remove(state.binner.current_sample_path)
        print('Sample %s removed' % state.binner.current_sample_path)

    state.binner.current_sample_path = None
    state.binner.current_sample_base = None
    state.binner.current_sample_name = None

    return

def report_timeout(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.binner.timeout_count = state.binner.timeout_count +1

    return

def unconfirm_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    from shutil import copyfile

    input_dir = options.internal_paths_input
    input_ = input_dir+'\\'+state.binner.current_sample_name

    output_dir = options.internal_paths_output+'\\unconfirmed'

    host_output_dir = options.external_paths_tmp_output+'/unconfirmed/'
    try:
        os.makedirs(host_output_dir)
    except Exception as e:
        print(e)

    output = output_dir+'\\'+state.binner.current_sample_name

    print('Moving from {} to {}'.format(input_, output))
    cmd = 'copy {} {}'.format(input_, output)
    write_socket(options.s, "run_cmd %s" % cmd)
    response, _, _ = read_socket(options.s)

    return

def confirm_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    from shutil import copyfile

    input_dir = options.internal_paths_input
    input_ = input_dir+'\\'+state.binner.current_sample_name

    if(args is None):
        args = state.ret
    output_dir = options.internal_paths_output+'\\'+args

    host_output_dir = options.external_paths_tmp_output+'/'+args
    try:
        os.makedirs(host_output_dir)
    except Exception as e:
        print(e)

    output = output_dir+'\\'+state.binner.current_sample_name

    print('Moving from {} to {}'.format(input_, output))
    cmd = 'copy {} {}'.format(input_, output)
    write_socket(options.s, "run_cmd %s" % cmd)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    state.binner.addresses.add(args)
    state.binner.samples_confirmed = state.binner.samples_confirmed +1

    return

