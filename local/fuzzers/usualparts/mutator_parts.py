import statemachine
import globs
import os
import Queue 
import generators.generatorCorrected as generatorCorrected

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
        self.original = None
        self.current_sample_name = None
        self.current_sample_base = None
        self.current_sample_path = None
        self.extension = None
        

def init_mutator(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    globs.state.mutator = Mutator()

    return

def select_changer(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    import generators.changer as changer

    globs.state.mutator.mutator = changer.Changer
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

def original(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    globs.state.mutator.original = globs.state.research_dir+ '/samples/shared/' + args
    print 'Mutator original path: %s' % globs.state.mutator.original

    return

def deploy_dir(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    
    if(args == None):
        args = ''

    globs.state.mutator.deploy_dir = globs.state.research_dir+ '/samples/shared/' + args
    print 'Mutator deploy dir: %s' % globs.state.mutator.deploy_dir

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

    if(state.mutator.generator == None):
       state.mutator.generator = generatorCorrected.DirGenerator(mutator.original, mutator.deploy_dir, mutator_ = mutator.mutator, mutations_ = mutator.mutation_count, fext = mutator.extension)

    state.mutator.generator.generate(mutator.batch_size)
    state.mutator.left_in_batch = mutator.batch_size

    return

def get_next_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    from os.path import basename, dirname

    state.mutator.current_sample_path = state.mutator.generator.samples_list.pop()    
    state.mutator.current_sample_name = basename(state.mutator.current_sample_path)
    state.mutator.current_sample_base = dirname(state.mutator.current_sample_path)
    state.mutator.left_in_batch = state.mutator.left_in_batch -1
    print 'Loaded sample: %s' % state.mutator.current_sample_name
    print 'Samples left: 0x%08x' % state.mutator.left_in_batch

    state.ret = state.mutator.current_sample_name
    print 'ret: %s' % state.ret

    return

def current_sample_name(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.ret = state.mutator.current_sample_name
    print 'ret: %s' % state.ret

    return

def current_sample_path(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    state.ret = state.mutator.current_sample_path
    print 'ret: %s' % state.ret

    return

def current_sample_drop(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    import os

    if(state.mutator.current_sample_path != None):
        os.remove(state.mutator.current_sample_path)
        print 'Sample %s removed' % state.mutator.current_sample_path

    state.mutator.current_sample_path = None
    state.mutator.current_sample_base = None
    state.mutator.current_sample_name = None

    return

def save_sample(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    from shutil import copyfile

    copyfile(state.mutator.current_sample_path, state.mutator.current_sample_base+'../saved/'+state.mutator.current_sample_name)

    return




