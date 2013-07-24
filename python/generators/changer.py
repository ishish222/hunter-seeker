from mutator import Mutator
from mmap import mmap
import random

class Changer(Mutator):
    def mutate(self):
        self.pickOffset()
        map = mmap(self.fileno(), 0)
        map[self.offset] = chr(random.randint(0,255))
        map.close()
