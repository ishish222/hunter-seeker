import sys
import os
import random
import time
import io

class Area:
    def __init__(self):
        self.offset = 0
        self.size = 0

    def __init__(self, offset, size):
        self.offset = offset
        self.size = size
        self.prepared = False
        self.finalized = False

    def __lt__(self, other):
        return self.offset < other.offset


class Mutator(io.FileIO):

    def __init__(self, path):
        io.FileIO.__init__(self, path, "r+b")
        random.seed(time.time())
        self.areasCount = 0
        self.offset = 0
        self.sizeTotal = 0
        self.areas = list()
        self.selectorPath = "selector"
        if(os.path.isfile("selector")):
            self.readSelector()
        else:
            self.readSize(path)
        self.calcSizeTotal()

    def get_size(self):
        cur_pos = self.tell()
        self.seek(0,2)
        size = self.tell()
        self.seek(cur_pos,0)
        return size

    def prepare(self):
        pass

    def finalize(self):
        pass

    def sortAreas(self):
        self.areas.sort()

    def printAreas(self):
        for area in self.areas:
#           print area.offset
#           print area.size
            pass

    def readSelector(self):
        self.selector = open(self.selectorPath)
        for line in self.selector:
            offset, size = line.split(" ")
            self.areas.append(Area(int(offset), int(size)))
        self.selector.close()

        self.sortAreas()
#        self.printAreas()
#        self.calcSizeTotal()

    def setSelectorPath(self, path):
        self.selectorPath = path

    def __iadd__(self, off):
        for area in self.areas:
            if(off < area.size):
                self.offset = area.offset + off
                break
            else:
                off -= area.size
        return self

    def getOffset(self):
        return self.offset

    def change(self):
        print('NOT IMPLEMENTED!!!')
        pass

    def mutate(self, mutation_count=1):
        self.prepare()

        for j in range(0, mutation_count):
            self.change()

        self.finalize()
        return

    def calcSizeTotal(self):
        for area in self.areas:
            self.sizeTotal += area.size

    def pickVirtualOffset(self):
        r = self.pickRandom(0, self.sizeTotal)
        return r

    def pickOffset(self):
        self.offset = 0
        self.offset += self.pickVirtualOffset()

    def pickRandom(self, from_, to_):
        r = random.randint(from_, to_)
        return r

    def appendArea(self, area):
        self.areas.append(area)

    def removeAllAreas(self):
        del self.areas[0:len(self.areas)]

    def readSize(self, path):
        statinfo = os.stat(path)
        self.areas.append(Area(0, statinfo.st_size))
