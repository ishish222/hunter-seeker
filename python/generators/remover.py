from mutator import Mutator
import random

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
        len = random.randint(2,5)
        self.seek(self.offset + len, 0)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        self.write(buffer)
        self.truncate(self.tell())

class LongRemover(Mutator):
    def mutate(self):
        self.pickOffset()
#        random.seed()
        len = random.randint(10,255)
        self.seek(self.offset + len, 0)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        self.write(buffer)
        self.truncate(self.tell())
