from mutator import Mutator
import random
import js.jsparser as jsparser

class nodeSwitcher(Mutator):
    def prepare(self):
        self.source = self.read()
        jsparser.all_nodes = []
        parsed = jsparser.parse(self.source)
        self.all_nodes = jsparser.all_nodes
        self.node_count = len(self.all_nodes)
        print("Nodes: %d" % self.node_count)

    def mutate(self):
        firstNode = random.randint(1, self.node_count)
        start1 = self.all_nodes[firstNode].start
        end1 = self.all_nodes[firstNode].end
        while True:
            secondNode = random.randint(1, self.node_count)
            start2 = self.all_nodes[secondNode].start
            end2 = self.all_nodes[secondNode].end
            if(start2 > end1 or end2 < start1): break

        print("1st: %s" % (self.source[start1:end1]))
        print("2nd: %s" % (self.source[start2:end2]))

#class wwwFucker(Mutator):
#    def prepare(self):
        
