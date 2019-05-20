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
model_name = 'model_006_compressed'

model = ML()
model.model_dir = model_path
model.load(model_name)

class Changer(object):
    def __init__(self, path):
        self.dst_path = path
        self.src_path = tempfile.mktemp()
        L = ["mv", self.dst_path, self.src_path]
        os.spawnv(os.P_WAIT, "/bin/cp", L)
        try:
#            print('Opening')
            self.dst = open(self.dst_path, 'wb+')
            self.src = open(self.src_path, 'rb')
        except Exception as e:
            print('Error changing file: {}'.format(path))
            raise e
        
        

    def prepare(self):
        self.data_gzipped_1 = self.src.read()
#        print('Ungzipping xml_1')
        self.data_xml = gzip.decompress(self.data_gzipped_1).decode('utf-8')
        
        lines = self.data_xml.split('\n')
        self.xml_header = lines[0]
        self.data_xml = '\n'.join(lines[1:])


    def change(self):
#        print('Ungzipped data head:')
#        print('{}'.format(self.data_xml[:1000]))
        tree = etree.fromstring(self.data_xml)
#        print('Encodeding')
        self.data_encoded = convert_node(tree)
#        print('Encoded data head:')
#        print('{}'.format(self.data_encoded[:1000]))
        self.offset = random.randint(1, len(self.data_encoded))
#        print('Will conserve {} bytes.'.format(self.offset))
        remaining_length = len(self.data_encoded) - self.offset +1
        generated_length = 1000
#        print('Remaining length:\n{}\n'.format(remaining_length))
#        print('Generating')
        self.data_generated = model.rnn.generate(prefix=self.data_encoded[:self.offset], max_gen_length=generated_length, return_as_list=True)[0]
#        print('Generated head:\n{}\n'.format(self.data_generated[:1000]))
        self.data_changed = self.data_encoded[:self.offset]
        self.data_changed += self.data_generated
        self.data_changed += self.data_encoded[self.offset+generated_length-1:]
#        print('Decoding')
        self.data_decoded = convert_xml(self.data_changed)
#        print('Decoded data head:')
#        print('{}'.format(self.data_decoded[:1000]))
        self.data_xml = self.xml_header
        self.data_xml += etree.tostring(self.data_decoded).decode('utf-8')

    def finalize(self):
#        print('Gzipping xml2')
        self.data_gzipped_2 = gzip.compress(self.data_xml.encode('utf-8'))
#        print('Writing to destination')
        self.dst.write(self.data_gzipped_2)
#        print('Closing')
        self.dst.close()
        self.src.close()

    def mutate(self, count):
#        print('Started generating sample')
        self.prepare()
        for i in range(0, count):
            self.change()
        self.finalize()
        print('Finished generating sample')

