from .mutator import Mutator
from mmap import mmap
import random
import sys
from ml import ML
import gzip
from tempfile import TemporaryFile
from .xml_utils import convert_xml, convert_node
from lxml import etree
import tempfile
import os

maxAreaSize = 5

model_path = '/home/hs1/_SAMPLES/_LEARNING/_MODELS'
model_name = 'xar_raw_001'

model = ML()
model.model_dir = model_path
model.load(model_name)

class Changer(object):
    def __init__(self, path):
        self.dst_path = path
        self.src_path = tempfile.mktemp()
        L = ["cp", self.dst_path, self.src_path]
        os.spawnv(os.P_WAIT, "/bin/cp", L)
        try:
            self.dst = open(self.dst_path, 'wb+')
            self.src = open(self.src_path, 'rb')
        except Exception as e:
            print('Error changing file: {}'.format(path))
            raise e

    def prepare(self):
        self.data_read = self.src.read()

    def change(self):
        self.offset = random.randint(1, len(self.data_read))
        remaining_length = len(self.data_read) - self.offset +1
        generated_length = 100
        self.data_generated = model.rnn.generate(prefix=self.data_read[:self.offset], max_gen_length=generated_length, return_as_list=True)[0]
        self.data_changed = self.data_read[:self.offset]
        self.data_changed += self.data_generated
        self.data_changed += self.data_read[self.offset+generated_length-1:]

    def finalize(self):
        self.dst.write(self.data_changed)
        self.dst.close()
        self.src.close()

    def mutate(self, count):
        self.prepare()
        for i in range(0, count):
            self.change()
        self.finalize()
        print('Finished generating sample')

