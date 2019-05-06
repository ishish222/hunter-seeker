from textgenrnn import textgenrnn
import os
import json

class ML(object):
    def __init__(self):
        self.rnn = textgenrnn()
        self.input_dir = ''
        self.model_dir = ''
        self.input_list = []
        self.epochs = 1
        return

    def load(self, args):
        print('Loading model from: {}/{}'.format(self.model_dir, args))
        model_path = '{}/{}'.format(self.model_dir, args)

        weights_path = '{}_weights.hdf5'.format(model_path)
        vocab_path = '{}_vocab.json'.format(model_path)
        config_path = '{}_config.json'.format(model_path)

        self.rnn = textgenrnn(weights_path=weights_path, vocab_path=vocab_path, config_path=config_path)
        return

    def delete(self, args):
        print('Deleting model: {}/{}'.format(self.model_dir, args))
        model_path = '{}/{}'.format(self.model_dir, args)

        weights_path = '{}_weights.hdf5'.format(model_path)
        vocab_path = '{}_vocab.json'.format(model_path)
        config_path = '{}_config.json'.format(model_path)

        os.spawnv(os.P_WAIT, "/bin/rm", ["rm", weights_path])
        os.spawnv(os.P_WAIT, "/bin/rm", ["rm", vocab_path])
        os.spawnv(os.P_WAIT, "/bin/rm", ["rm", config_path])
        return

    def save(self, args):
        print('Saving model to: {}/{}'.format(self.model_dir, args))
        model_path = '{}/{}'.format(self.model_dir, args)

        weights_path = '{}_weights.hdf5'.format(model_path)
        vocab_path = '{}_vocab.json'.format(model_path)
        config_path = '{}_config.json'.format(model_path)

        with open('{}_config.json'.format(model_path),'w', encoding='utf8') as outfile:
            json.dump(self.rnn.config, outfile, ensure_ascii=False)

        with open('{}_vocab.json'.format(model_path),'w', encoding='utf8') as outfile:
            json.dump(self.rnn.tokenizer.word_index, outfile, ensure_ascii=False)

        self.rnn.save(weights_path)
        return

    def save_samples(self, path):
        out_path = '{}/{}'.format(self.model_dir, path)
        out = open(out_path, 'w+')

        for input_ in self.input_list:
            out.write(input_)
            out.write('\n')

        out.close()
        return



