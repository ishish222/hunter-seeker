from .mutator import Mutator
from mmap import mmap
import random
import sys
from ml import ML
from gzip import GzipFile
from tempfile import TemporaryFile
from .xml_utils import convert_xml, convert_node
from lxml import etree

maxAreaSize = 5

model_path = '/home/hs1/_SAMPLES/_LEARNING/_MODELS'
model_name = 'model_005_part'

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
        self.data = self.gzip.read().decode('utf-8')
        print(type(self.data))
        self.gzip.close()
        # save the first line
        lines = self.data.split('\n')
        self.header = lines[0]
        self.data = '\n'.join(lines[1:])

    def change(self):
        print('decompressed data head:')
        print('{}'.format(self.data[:1000]))
        tree = etree.fromstring(self.data)
        self.data2 = convert_node(tree)
        print('compressed data head:')
        print('{}'.format(self.data2[:1000]))
        self.offset = random.randint(1, len(self.data2))
        print('Will conserve {} bytes.'.format(self.offset))
        remaining_length = len(self.data2) - self.offset +1
        generated_length = 1000
        print('Remaining length:\n{}\n'.format(remaining_length))
        self.generated = model.rnn.generate(prefix=self.data2[:self.offset], max_gen_length=generated_length, return_as_list=True)[0]
        print('Generated head:\n{}\n'.format(self.generated[:1000]))
        self.data3 = self.data2[:self.offset]
        self.data3 += self.generated
        self.data3 += self.data2[self.offset+generated_length-1:]
        self.data4 = convert_xml(self.data3)
        self.generated2 = self.header
        self.generated2 += etree.tostring(self.data4).decode('utf-8')
        self.data = self.generated2

    def finalize(self):
        print('ableton_als_changer child finalize')
        self.gzip = GzipFile(fileobj=self, mode='w+')
        #self.gzip.write(self.generated)
        self.gzip.write(self.data.encode('utf-8'))
        self.gzip.close()

