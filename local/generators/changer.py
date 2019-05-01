from .mutator import Mutator
from mmap import mmap
import random

maxAreaSize = 5

class Changer(Mutator):
    def mutate(self):
        self.pickOffset()
        fmap = mmap(self.fileno(), 0)
        fmap[self.offset] = chr(random.randint(0,255))
        fmap.close()

class AreaChanger(Mutator):
    def mutate(self):
        self.pickOffset()
        size = self.pickRandom(1, maxAreaSize)
        fmap = mmap(self.fileno(), 0)
        for i in range(0, size):
            fmap[self.offset] = chr(random.randint(0,255))
            self.offset += 1
        fmap.close()

