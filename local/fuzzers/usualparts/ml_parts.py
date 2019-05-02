import os
import sys
sys.path += ["../../common"]
import common
import globs 
import json

from glob import glob

from textgenrnn import textgenrnn

class ML(object):
    def __init__(self):
        self.rnn = textgenrnn()
        self.input_dir = ''
        self.model_dir = ''
        self.input_list = []
        self.epochs = 1

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

    print('Loading model from: {}/{}'.format(ml.model_dir, args))
    model_path = '{}/{}'.format(ml.model_dir, args)

    weights_path = '{}_weights.hdf5'.format(model_path)
    vocab_path = '{}_vocab.json'.format(model_path)
    config_path = '{}_config.json'.format(model_path)

    ml.rnn = textgenrnn(weights_path=weights_path, vocab_path=vocab_path, config_path=config_path)

    return

def save_ml(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    print('Saving model to: {}/{}'.format(ml.model_dir, args))
    model_path = '{}/{}'.format(ml.model_dir, args)

    weights_path = '{}_weights.hdf5'.format(model_path)
    vocab_path = '{}_vocab.json'.format(model_path)
    config_path = '{}_config.json'.format(model_path)

    with open('{}_config.json'.format(model_path),'w', encoding='utf8') as outfile:
        json.dump(ml.rnn.config, outfile, ensure_ascii=False)

    with open('{}_vocab.json'.format(model_path),'w', encoding='utf8') as outfile:
        json.dump(ml.rnn.tokenizer.word_index, outfile, ensure_ascii=False)

    ml.rnn.save(weights_path)

    return

def save_samples(args=None):
    options = globs.state.options
    state = globs.state
    status = globs.state.status
    ml = globs.state.ml

    out_path = '{}/{}'.format(ml.model_dir, args)
    out = open(out_path, 'w+')

    for input_ in ml.input_list:
        out.write(input_)
        out.write('\n')

    out.close()
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

