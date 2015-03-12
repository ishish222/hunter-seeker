#!/usr/bin/env python

from optparse import OptionParser
from csv import reader
import struct
from x64_cpu import taint_x64

def main():
    print "worky"

    op = OptionParser()
    op.add_option("-i", "--input", dest="inpath")
    
    (options, args) = op.parse_args()

    print (options, args)
    print options.inpath

    options.infile = open(options.inpath, 'r+')
    options.inreader = reader(options.infile, delimiter='@')

    for row in options.inreader:
        print row

    
    options.infile.close()
    

main()
