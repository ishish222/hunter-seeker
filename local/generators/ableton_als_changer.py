from .mutator import Mutator
from mmap import mmap
import random
import sys
from ml import ML

maxAreaSize = 5

class Changer(Mutator):
    def __init__(self, path):
        Mutator.__init__(self, path)

    def prepare(self):

        print('ableton_als_changer prepare')

    def change(self):
        print('ableton_als_changer child change')
        self.pickOffset()
        print('picked offset: {}'.format(self.offset))
        fmap = mmap(self.fileno(), 0)
        #fmap[self.offset] = chr(random.randint(0,255))
        fmap[self.offset] = int(random.randint(0,255))
        fmap.close()

    def finalize(self):
        print('ableton_als_changer child finalize')

