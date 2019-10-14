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

class Mutator(object):
    def __init__(self):
        self.mutator = None
        self.corrector = None
        self.generator = None
        self.mutation_count = 3
        self.batch_size = 100
        self.left_in_batch = 0
        self.deploy_dir = None
        self.original_path = None
        self.original_basename = None
        self.original_dirname  = None
        self.original_extension = None
        self.current_sample_name = None
        self.current_sample_base = None
        self.current_sample_path = None
        self.extension = None
        self.samples_list = []
        self.addresses = set()
        self.interesting_list = []
        self.samples_confirmed = 0
        self.samples_tested = 0
        self.timeout_count = 0
        self.start_time = datetime.now()
        

def init_mutator(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    globs.state.mutator = Mutator()

    return

def select_changer(args = None):
    from importlib import import_module

    def dynamic_import(abs_module_path, class_name):
        module_object = import_module(abs_module_path)

        target_class = getattr(module_object, class_name)

        return target_class

    options = globs.state.options
    state = globs.state
    status = globs.state.status

    if(args == None):
        from generators.changer import Changer
        print('Imported: {}.{}'.format('generators.{}'.format(args), 'Changer'))
    else:
        Changer = dynamic_import('generators.{}'.format(args), 'Changer')
        print('Imported: {}.{}'.format('generators.{}'.format(args), 'Changer'))

    globs.state.mutator.mutator = Changer
    globs.state.mutator.mutator_name = args

    return

def extension(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    globs.state.mutator.extension = args

    return

def mutation_count(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    globs.state.mutator.mutation_count = int(args, 0x10)

    return

def batch_size(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    globs.state.mutator.batch_size = int(args, 0x10)

    return

def report(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    if(len(globs.state.mutator.addresses) > 0):
        print('===')
        print('Crash addresses:')
        for address in globs.state.mutator.addresses:
            print('\t%s' % address)
        print('===')
    print('Fuzz started: \t\t\t%s' % globs.state.mutator.start_time)
    print('Now its: \t\t\t%s' % datetime.now())
    print('Selected chager: \t\t%s' % globs.state.mutator.mutator_name)
    print('Mutation count: \t\t%s' % globs.state.mutator.mutation_count)
    print('Original path: \t\t\t%s' % globs.state.mutator.original_path)
    print('Dirname path: \t\t\t%s' % globs.state.mutator.original_dirname)
    print('Basename path: \t\t\t%s' % globs.state.mutator.original_basename)
    print('Original extension: \t\t%s' % globs.state.mutator.original_extension)
    print('Tested sample: \t\t\t%s' % globs.state.mutator.samples_tested)
    print('Found interesting samples: \t%s' % globs.state.mutator.samples_confirmed)
    print('Timout encountered times: \t%s' % globs.state.mutator.timeout_count)
    print('Crash factor is \t\t%s' % (globs.state.mutator.samples_confirmed / globs.state.mutator.samples_tested))
    print('Address factor is \t\t%s' % (len(globs.state.mutator.addresses) / globs.state.mutator.samples_tested))
    print('Timout factor is \t\t%s' % (globs.state.mutator.timeout_count / globs.state.mutator.samples_tested))
    print('Current sample: \t\t%s' % globs.state.mutator.current_sample_name)
    time_elapsed = datetime.now() - globs.state.mutator.start_time
    print('Time elapsed: \t\t\t%s' % time_elapsed)
    print('Fuzzing speed in this session: \t%s/sample' % (time_elapsed / globs.state.mutator.samples_tested))

    return

def original(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    from os.path import basename, dirname

    globs.state.mutator.original_path = globs.state.options.external_paths_tmp_input+ '/' +args
    globs.state.mutator.original_dirname = dirname(globs.state.mutator.original_path)
    globs.state.mutator.original_basename = basename(globs.state.mutator.original_path)
    globs.state.mutator.original_basename_wo_extension = globs.state.mutator.original_basename.split(".")[0]
    globs.state.mutator.original_extension = globs.state.mutator.original_basename.split(".")[-1]
    print('Mutator original path: %s' % globs.state.mutator.original_path)
    print('Mutator dirname path: %s' % globs.state.mutator.original_dirname)
    print('Mutator basename path: %s' % globs.state.mutator.original_basename)
    print('Mutator original extension: %s' % globs.state.mutator.original_extension)

    return

def deploy_dir(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = ''

    globs.state.mutator.deploy_dir = globs.state.options.external_paths_tmp_input+ '/' + args
    print('Mutator deploy dir: %s' % globs.state.mutator.deploy_dir)

    return

def batch_exhausted(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(state.mutator.left_in_batch > 0):
        return "N"
    else:
        return "Y"

def generate_batch(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    mutator = state.mutator    

    from os.path import basename, dirname
    from os import spawnv
    import tempfile

    current_file = None

    for i in range(0, mutator.batch_size):
        # copy original
        tname = tempfile.mktemp(dir = mutator.deploy_dir, prefix=mutator.original_basename_wo_extension+'_')
        tname = '%s.%s' % (tname, mutator.original_extension)
        L = ["cp", "-r", mutator.original_path, tname]
        spawnv(os.P_WAIT, "/bin/cp", L)

        # open as mutation
        current_file = mutator.mutator(tname)
    
        # mutate
        current_file.mutate(mutator.mutation_count)

        if(mutator.corrector != None):
            current_file = mutator.corrector(tname)
            mutator.corrector.correct()

        state.mutator.samples_list.append(tname)

    state.mutator.left_in_batch = len(state.mutator.samples_list)

    return

def get_current_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.ret = state.mutator.current_sample_name
    print('ret: %s' % state.ret)

    return

def get_next_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    from os.path import basename, dirname

    state.mutator.current_sample_path = state.mutator.samples_list.pop()    
    state.mutator.current_sample_name = basename(state.mutator.current_sample_path)
    state.mutator.current_sample_base = dirname(state.mutator.current_sample_path)
    state.mutator.left_in_batch = state.mutator.left_in_batch -1
    state.mutator.samples_tested = state.mutator.samples_tested +1
    print('Loaded sample: %s' % state.mutator.current_sample_name)
    print('Samples left: 0x%08x' % state.mutator.left_in_batch)

    state.ret = state.mutator.current_sample_name
    print('ret: %s' % state.ret)

    return

def current_sample_name(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.ret = state.mutator.current_sample_name
    print('ret: %s' % state.ret)

    return

def current_sample_path(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.ret = state.mutator.current_sample_path
    print('ret: %s' % state.ret)

    return

def current_sample_drop(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    import os

    if(state.mutator.current_sample_path != None):
        os.remove(state.mutator.current_sample_path)
        print('Sample %s removed' % state.mutator.current_sample_path)

    state.mutator.current_sample_path = None
    state.mutator.current_sample_base = None
    state.mutator.current_sample_name = None

    return

def report_timeout(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.mutator.timeout_count = state.mutator.timeout_count +1

    return

def confirm_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    from shutil import copyfile

    input_dir = options.internal_paths_input
    input_ = input_dir+'\\'+state.mutator.current_sample_name
    #input_ = state.mutator.current_sample_path

    if(args is None):
        args = state.ret
    output_dir = options.internal_paths_output+'\\'+args

    host_output_dir = options.external_paths_tmp_output+'/'+args
    try:
        os.makedirs(host_output_dir)
    except Exception as e:
        print(e)

    output = output_dir+'\\'+state.mutator.current_sample_name

    print('Moving from {} to {}'.format(input_, output))
    cmd = 'copy {} {}'.format(input_, output)
    write_socket(options.s, "run_cmd %s" % cmd)
    response, _, _ = read_socket(options.s)

    globs.state.ret = response

    state.mutator.addresses.add(args)
    state.mutator.samples_confirmed = state.mutator.samples_confirmed +1

    return

