#!/usr/bin/env python

import os
import sys
import struct
import tempfile
import correctors.correctorlib as correctorlib
from mmap import mmap

class Generator(object):
    def __init__(self, origin_path_ = "", dest_path_ = "", dest_suffix_ = ".sample", mutator_ = None, mutations_ = 3):
        self.origin_path = origin_path_
        self.dest_path = dest_path_
        self.dest_suffix = dest_suffix_
        self.mutator = mutator_
        self.mutations = mutations_

    def generate(self, amt=100):
        os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", self.dest_path])
        for i in range(0, amt):
            tmp, tname = tempfile.mkstemp(suffix = self.dest_suffix, dir = self.dest_path)
            L = ["cp", self.origin_path, tname]
            os.spawnv(os.P_WAIT, "/bin/cp", L)
            os.close(tmp)
            mutatorInstance = self.mutator(tname)
            for j in range(0, self.mutations):
                mutatorInstance.mutate()
            print("Correcting")
            fmap = mmap(mutatorInstance.fileno(), 0)
            correctorlib.correct_all(fmap)
            fmap.close()
            mutatorInstance.close()

    def generate_one(self):
        os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", self.dest_path])
        tmp, tname = tempfile.mkstemp(suffix = self.dest_suffix, dir = self.dest_path)
        L = ["cp", self.origin_path, tname]
        os.spawnv(os.P_WAIT, "/bin/cp", L)
        os.close(tmp)
        mutatorInstance = self.mutator(tname)
        for j in range(0, self.mutations):
            mutatorInstance.mutate()
        print("Correcting")
        fmap = mmap(mutatorInstance.fileno(), 0)
        correctorlib.correct_all(fmap)
        fmap.close()
        return tname
