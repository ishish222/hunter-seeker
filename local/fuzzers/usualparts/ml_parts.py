import os
import sys
sys.path += ["./ml_module"]
from ml import ML
import globs 
import json

from glob import glob

def init_ml(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    globs.state.ml = ML()
    globs.state.ml.rnn.reset()

    return

def load_ml(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    ml.load(args)
    return

def delete_ml(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    ml.delete(args)
    return

def save_ml(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    ml.save(args)

    return
    
def save_samples(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    ml.save_samples(args)
    return

def check_more(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    if(len(ml.input_list) > 0):
        return "Y"
    else:
        return "N"

def set_epochs(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    if(type(args) != 'int'):
        args = int(args)

    ml.epochs = args
    return

def set_input_dir(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    ml.input_dir = args

    return

def set_model_dir(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    ml.model_dir = args

    return

def set_input_filename(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    input_path = '{}/{}'.format(ml.input_dir, args)
    ml.input_list.append(input_path)

    return

def set_input_glob(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    pattern = '{}/{}'.format(ml.input_dir, args)

    print('Input glob pattern: {}'.format(pattern))

    input_list = glob(pattern)
    for sample in input_list:
        print('Adding {} to list'.format(sample))
        ml.input_list.append(sample)

    return

def train_model(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    while(len(ml.input_list) > 0):
        input_ = ml.input_list.pop()
        print('Training model on: {}'.format(input_))
        ml.rnn.train_from_largetext_file(input_, new_model=False, num_epochs=ml.epochs)

    print('Training finished')

    return

def train_model_step(args = None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    input_ = ml.input_list.pop()
    print('Training model on: {}'.format(input_))
    ml.rnn.train_from_largetext_file(input_, new_model=False, num_epochs=ml.epochs)

    print('Training step finished')

    return

