from .mutator import Mutator
import random

minShortRem = 2
maxShortRem = 5
minLongRem = 10
maxLongRem = 255


class ByteRemover(Mutator):
    def mutate(self):
        self.pickOffset()
        self.seek(self.offset+1, 0)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        self.write(buffer)
        self.truncate(self.tell())

class ShortRemover(Mutator):
    def mutate(self):
        self.pickOffset()
#        random.seed()
        len = random.randint(minShortRem, maxShortRem)
        self.seek(self.offset + len, 0)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        self.write(buffer)
        self.truncate(self.tell())

class LongRemover(Mutator):
    def mutate(self):
        self.pickOffset()
#        random.seed()
        len = random.randint(minLongRem, maxLongRem)
        self.seek(self.offset + len, 0)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        self.write(buffer)
        self.truncate(self.tell())
