from mutator import Mutator
import random

class Colorizer(Mutator):
    def mutate(self):
        self.file.seek(self.offset)
        self.file.
