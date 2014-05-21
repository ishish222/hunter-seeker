#!/usr/bin/env python

import os
import sys
import struct
#import log.log as log
import tempfile
import glob
import random

#log_info = log.log_info
#log_debug = log.log_debug
#log_error = log.log_error

class GeneratorException(Exception): pass

class DirGenerator(object):
    def __init__(self, origin_path_ = "", fname = None, fext = None, dest_path_ = "", mutator_ = None, mutations_ = 3):
        self.origin_path = origin_path_
        self.dirname = os.path.basename(self.origin_path)
        self.dest_path = dest_path_
        if(fname != None):
            self.target_file = fname
        elif(fext != None):
            target_pattern = "%s/%s" % (self.origin_path, fext)
            flist = glob.glob(target_pattern)
            flist_len = len(flist)
            fnum = random.randint(0, flist_len-1)
#            self.target_file = os.path.basename(flist[fnum]) # TODO:what if deeper in directories?
            self.target_file = flist[fnum]
            self.target_file.replace(self.origin_path, "")
        else:
            print "Need to specify target file name or extension"
            raise GeneratorException
        print "Target file: %s" % self.target_file
        self.dest_suffix = "."+self.target_file.split(".")[-1]
        self.mutator = mutator_
        self.mutations = mutations_

    def generate(self, amt=100):
        #TODO: replace with testdir?
        os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", self.dest_path])
        for i in range(0, amt):
            print "."
            tmp, tname = tempfile.mkdtemp(dir = self.dest_path)
            L = ["cp", "-r", self.origin_path, tname]
            os.spawnv(os.P_WAIT, "/bin/cp", L)
            os.close(tmp)
            fname = "%s/%s" % (tname, self.target_file)
            mutatorInstance = self.mutator(fname)
            for j in range(0, self.mutations):
                mutatorInstance.mutate()
            mutatorInstance.close()

    def generate_one(self, amt=100):
        self.generate(1) # lol?

class Generator(object):
    def __init__(self, origin_path_ = "", dest_path_ = "", dest_suffix_ = None, mutator_ = None, mutations_ = 3):
        self.origin_path = origin_path_
        self.dest_path = dest_path_
        if(dest_suffix_ != None):
            self.dest_suffix = dest_suffix_
        else:
            self.dest_suffix = "."+origin_path_.split(".")[-1]
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
            mutatorInstance.close()

    def generate_one(self):
        os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", self.dest_path])
        tmp, tname = tempfile.mkstemp(suffix = self.dest_suffix, dir = self.dest_path)
        L = ["cp", self.origin_path, tname]
        os.spawnv(os.P_WAIT, "/bin/cp", L)
        os.close(tmp)
        for j in range(0, self.mutations):
            self.mutator(tname).mutate()
        return tname
