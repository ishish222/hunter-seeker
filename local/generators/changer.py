from .mutator import Mutator
from mmap import mmap
import random

maxAreaSize = 5

class Changer(Mutator):
    def change(self):
        print('changing')
        self.pickOffset()
        fmap = mmap(self.fileno(), 0)
        fmap[self.offset] = chr(random.randint(0,255))
        fmap.close()

