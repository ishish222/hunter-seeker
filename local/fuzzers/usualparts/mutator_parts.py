import statemachine
import globs
import os
import queue 
from datetime import datetime, timedelta

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
        self.interesting_list = []
        self.samples_interesting = 0
        self.samples_tested = 0
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
    else:
        Changer = dynamic_import('generators.{}'.format(args), 'Changer')

    globs.state.mutator.mutator = Changer

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

    print('Original path: %s' % globs.state.mutator.original_path)
    print('Dirname path: %s' % globs.state.mutator.original_dirname)
    print('Basename path: %s' % globs.state.mutator.original_basename)
    print('Original extension: %s' % globs.state.mutator.original_extension)
    print('Tested %s samples' % globs.state.mutator.samples_tested)
    print('Found %s interesting samples' % globs.state.mutator.samples_interesting)
    if(globs.state.mutator.samples_interesting > 0):
        print('===')
        print('Interesting samples:')
        for interesting_sample in globs.state.mutator.interesting_list:
            print('\t%s' % interesting_sample)
        print('===')
    print('Current sample: %s' % globs.state.mutator.current_sample_name)
    time_elapsed = datetime.now() - globs.state.mutator.start_time
    print('Time elapsed: %s' % time_elapsed)
    print('Fuzzing speed in this session: %s/sample' % (time_elapsed / globs.state.mutator.samples_tested))

    return

def original(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    from os.path import basename, dirname

    globs.state.mutator.original_path = globs.state.options.external_paths_tmp_input+ '/' +args
    globs.state.mutator.original_dirname = dirname(globs.state.mutator.original_path)
    globs.state.mutator.original_basename = basename(globs.state.mutator.original_path)
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
        tname = tempfile.mktemp(dir = mutator.deploy_dir)
        tname = '%s.%s' % (tname, mutator.original_extension)
        L = ["cp", "-r", mutator.original_path, tname]
        spawnv(os.P_WAIT, "/bin/cp", L)

        # open as mutation
        current_file = mutator.mutator(tname)
    
        # mutate
        for j in range(0, mutator.mutation_count):
            current_file.mutate()

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

def save_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    from shutil import copyfile

    copyfile(state.mutator.current_sample_path, state.mutator.current_sample_base+'/../saved/'+state.mutator.current_sample_name)

    state.mutator.interesting_list.append(state.mutator.current_sample_name)
    state.mutator.samples_interesting = state.mutator.samples_interesting +1

    return

def save_crash_data(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    crash_data_path = state.mutator.current_sample_base+'/../saved/'+state.mutator.current_sample_name+'.crash.txt'
    f = open(crash_data_path, 'w+')

    count = globs.state.stack.pop()
    count = int(count, 0x10)

    for i in range(0, count):
        f.write(globs.state.stack.pop())
        f.write('\n')

    f.close()
    return




