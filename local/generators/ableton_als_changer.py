from .mutator import Mutator
from mmap import mmap
import random
import sys
from ml import ML
from gzip import GzipFile
from tempfile import TemporaryFile

maxAreaSize = 5

model_path = '/home/hs1/_SAMPLES/_LEARNING/_MODELS'
model_name = 'model_003_final'

model = ML()
model.model_dir = model_path
model.load(model_name)

class Changer(Mutator):
    def __init__(self, path):
        Mutator.__init__(self, path)

    def prepare(self):
        print('ableton_als_changer prepare')
        print('unzipping self')
        self.pickOffset()
        self.gzip = GzipFile(fileobj=self, mode='rb')
        self.data = self.gzip.peek(self.offset).decode('utf-8')
        print(type(self.data))
        self.gzip.close()

    def change(self):
        print('ableton_als_changer child change')
        print('Will peek {} bytes.'.format(self.offset))
        print('Data:\n{}\n'.format(self.data))
        remaining_length = self.get_size() - self.offset +1
        print('Remaining length:\n{}\n'.format(remaining_length))
        self.generated = model.rnn.generate(prefix=self.data, max_gen_length=remaining_length, return_as_list=True)[0]
        print('Generated:\n{}\n'.format(self.generated))

    def finalize(self):
        print('ableton_als_changer child finalize')
        self.gzip = GzipFile(fileobj=self, mode='w')
        #self.gzip.write(self.generated)
        self.gzip.write(self.data.encode('utf-8'))
        self.gzip.close()

