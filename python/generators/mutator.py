import sys
import os
import random
import time

class Area:
    def __init__(self):
        self.offset = 0
        self.size = 0

    def __init__(self, offset, size):
        self.offset = offset
        self.size = size

    def __lt__(self, other):
        return self.offset < other.offset


class Mutator(file):

    def __init__(self, path):
        file.__init__(self, path, "r+b")
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

    def mutate(self):
        pass 

    def calcSizeTotal(self):
        for area in self.areas:
            self.sizeTotal += area.size
#        print "Total " + str(self.sizeTotal)

    def pickVirtualOffset(self):
#        random.seed()
        r = random.randint(0, self.sizeTotal)
        print r
#        print str(0)+" "+str(self.sizeTotal)
#        print "v: "+str(r)
        return r

    def pickOffset(self):
        self.offset = 0
        self += self.pickVirtualOffset()
#       print "r: "+str(self.offset)

    def appendArea(self, area):
        self.areas.append(area)

    def removeAllAreas(self):
        del self.areas[0:len(self.areas)]

    def readSize(self, path):
        statinfo = os.stat(path)
        self.areas.append(Area(0, statinfo.st_size))
