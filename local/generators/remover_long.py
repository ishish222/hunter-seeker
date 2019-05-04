from .mutator import Mutator
import random

minShortRem = 2
maxShortRem = 5
minLongRem = 10
maxLongRem = 255


class Changer(Mutator):
    def change(self):
        self.pickOffset()
#        random.seed()
        len = random.randint(minLongRem, maxLongRem)
        self.seek(self.offset + len, 0)
        buffer = self.read(-1)
        self.seek(self.offset, 0)
        self.write(buffer)
        self.truncate(self.tell())
