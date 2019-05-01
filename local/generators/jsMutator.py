from .mutator import Mutator
import random
from . import js.jsparser as jsparser

class nodeSwitcher(Mutator):
    def prepare(self):
        pass

    def mutate(self):
        self.seek(0)
        self.source = self.read()
#        print(self.source)
        jsparser.all_nodes = []
        try:
            jsparser.parse(self.source)
        except Exception:
#            print("Syntax error, I'm unable to introduce more mutations, leaving sample as it is")
            return
        self.all_nodes = jsparser.all_nodes
        self.node_count = len(self.all_nodes)
#        print("Nodes: %d" % self.node_count)
        firstNode = random.randint(1, self.node_count-1)
        start1 = self.all_nodes[firstNode].start
        end1 = self.all_nodes[firstNode].end
        while True:
            secondNode = random.randint(1, self.node_count-1)
            start2 = self.all_nodes[secondNode].start
            end2 = self.all_nodes[secondNode].end
            if(start2 > end1 or end2 < start1): break

#        print("1st: %s" % (self.source[start1:end1]))
#        print("2nd: %s" % (self.source[start2:end2]))

        if(start2 > end1):
            s1 = start1
            s2 = start2
            e1 = end1
            e2 = end2
        else:
            s1 = start2
            s2 = start1
            e1 = end2
            e2 = end1
            
        self.newsource = self.source[:s1]
        self.newsource += self.source[s2:e2]
        self.newsource += self.source[e1:s2]
        self.newsource += self.source[s1:e1]
        self.newsource += self.source[e2:]

        self.seek(0)
        self.write(self.newsource)  #new source has the same size
        self.truncate()
#        self.seek(0)
#        print(self.read())


#class wwwFucker(Mutator):
#    def prepare(self):
        
