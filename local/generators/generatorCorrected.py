#!/usr/bin/env python

import os
import sys
import struct
import tempfile
import importlib
from mmap import mmap
import glob
import random

class GeneratorException(Exception): pass

class DirGenerator(object):
    def __init__(self, origin_path_ = "", dest_path_ = "", mutator_ = None, corrector = None, mutations_ = 3, fname = None, fext = None, real_target = None):
        self.origin_path = origin_path_
        self.dirname = os.path.basename(self.origin_path)
        self.dest_path = dest_path_
        self.mutator = mutator_
        self.mutations = mutations_
        self.corrector = corrector
        self.fname = fname
        self.fext = fext
        self.real_target = real_target

    def generate(self, amt=100):
        samples_list = []
        flist = []

        #TODO: replace with testdir?
        os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", self.dest_path])

        if(self.fext != None):
            for root, dirnames, filenames in os.walk(self.origin_path):
                for dir_ in dirnames:
                    target_pattern = "%s/%s/%s" % (self.origin_path, dir_, self.fext)
                    print(target_pattern)
                    flist += glob.glob(target_pattern)

        for i in range(0, amt):
            tname = self.dest_path
            bname = os.path.basename(tname)
#            tname = tempfile.mkdtemp(dir = self.dest_path)
#            bname = os.path.basename(tname)
#            L = ["cp", "-r", self.origin_path, tname]
#            os.spawnv(os.P_WAIT, "/bin/cp", L)

            if(self.fname != None):
                self.target_file = self.fname
            elif(self.fext != None):
                flist_len = len(flist)
                if(flist_len <1):
                    print "No candidates found"
                    raise GeneratorException
                fnum = random.randint(0, flist_len-1)
    #            self.target_file = os.path.basename(flist[fnum]) # TODO:what if deeper in directories?
                self.target_file = flist[fnum]
                self.target_file.replace(self.origin_path, "")
            else:
                print "Need to specify target file name or extension"
                raise GeneratorException
#            print "Target file: %s" % self.target_file
            #add four letters of original name
            
            self.dest_suffix = "."+self.target_file.split(".")[-1]

            fname = "%s/%s" % (tname, self.target_file)
            mutatorInstance = self.mutator(fname)
            for j in range(0, self.mutations):
                mutatorInstance.mutate()
            if(self.corrector != None):
#                print("Correcting")
                fmap = mmap(mutatorInstance.fileno(), 0)
                self.corrector(fmap)
                fmap.close()
            mutatorInstance.close()
#            samples_list.append(fname)
            if(self.real_target == None):
                samples_list.append("%s/%s" % (bname, self.target_file))
            else:
                samples_list.append("%s/%s" % (bname, self.real_target))
        return samples_list

    def generate_one(self, amt=100):
        self.generate(1) # lol?

class Generator(object):
    def __init__(self, origin_path_ = "", dest_path_ = "", dest_suffix_ = None, mutator_ = None, mutations_ = 3, corrector = None):
        self.origin_path = origin_path_
        self.dest_path = dest_path_
        if(dest_suffix_ != None):
            self.dest_suffix = dest_suffix_
        else:
            self.dest_suffix = "."+origin_path_.split(".")[-1]
        self.mutator = mutator_
        self.corrector = corrector
        self.mutations = mutations_

    def generate(self, amt=100):
        samples_list = []
        extension = os.path.basename(self.origin_path)[0:4]
        os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", self.dest_path])
        for i in range(0, amt):
            tmp, tname = tempfile.mkstemp(suffix = '-' + extension + self.dest_suffix, dir = self.dest_path)
            L = ["cp", self.origin_path, tname]
            os.spawnv(os.P_WAIT, "/bin/cp", L)
            os.close(tmp)
            mutatorInstance = self.mutator(tname)
            for j in range(0, self.mutations):
                mutatorInstance.mutate()
            if(self.corrector != None):
                print("Correcting")
                fmap = mmap(mutatorInstance.fileno(), 0)
                self.corrector(fmap)
                fmap.close()
            mutatorInstance.close()
            samples_list.append(tname)
        return samples_list

    def generate_one(self):
        self.generate(1)
