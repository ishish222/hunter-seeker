from .mutator import Mutator
import random

minShortAdd = 2
maxShortAdd = 5
minLongAdd = 10
maxLongAdd = 255

class Changer(Mutator):
    def change(self):
        self.pickOffset()
        self.seek(self.offset, 0)
#        random.seed()
        len = random.randint(minLongAdd, maxLongAdd)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        for i in range(0,len):
            self.write(chr(random.randint(0,255)))
        self.write(buffer)
