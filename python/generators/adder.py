from mutator import Mutator
import random

class ByteAdder(Mutator):
    def mutate(self):
        self.pickOffset()
        self.seek(self.offset, 0)
#        random.seed()
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        self.write(chr(random.randint(0,255)))
        self.write(buffer)

class ShortAdder(Mutator):
    def mutate(self):
        self.pickOffset()
        self.seek(self.offset, 0)
#        random.seed()
        len = random.randint(2,5)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        for i in range(0,len):
            self.write(chr(random.randint(0,255)))
        self.write(buffer)


class LongAdder(Mutator):
    def mutate(self):
        self.pickOffset()
        self.seek(self.offset, 0)
#        random.seed()
        len = random.randint(10,255)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        for i in range(0,len):
            self.write(chr(random.randint(0,255)))
        self.write(buffer)
