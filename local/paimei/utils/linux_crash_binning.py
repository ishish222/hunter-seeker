#
# Crash Binning
# Copyright (C) 2006 Pedram Amini <pedram.amini@gmail.com>
#
# $Id: crash_binning.py 193 2007-04-05 13:30:01Z cameron $
#
# This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

'''
@author:       Pedram Amini
@license:      GNU General Public License 2.0 or later
@contact:      pedram.amini@gmail.com
@organization: www.openrce.org
'''

import sys
import zlib
import pickle

class __crash_bin_struct__(object):
    exception_module    = None
    exception_address   = 0
    write_violation     = 0
    violation_address   = 0
    violation_thread_id = 0
    context             = None
    context_dump        = None
    disasm              = None
    disasm_around       = []
    stack_unwind        = []
    seh_unwind          = []
    extra               = None


class crash_binning(object):
    '''
    @todo: Add MySQL import/export.
    '''

    bins       = {}
    last_crash = None
    pydbg      = None

    ####################################################################################################################
    def __init__ (self):
        '''
        '''

        self.bins       = {}
        self.last_crash = None
        self.pydbg      = None


    ####################################################################################################################
    def crash_synopsis (self, crash=None):
        '''
        For the supplied crash, generate and return a report containing the disassemly around the violating address,
        the ID of the offending thread, the call stack and the SEH unwind. If not crash is specified, then call through
        to last_crash_synopsis() which returns the same information for the last recorded crash.

        @see: crash_synopsis()

        @type  crash: __crash_bin_struct__
        @param crash: (Optional, def=None) Crash object to generate report on

        @rtype:  String
        @return: Crash report
        '''

        if not crash:
            return self.last_crash_synopsis()

        if crash.write_violation:
            direction = "write to"
        else:
            direction = "read from"

        synopsis = "%s:%08x %s from thread %d caused access violation\nwhen attempting to %s 0x%08x\n\n" % \
            (
                crash.exception_module,       \
                crash.exception_address,      \
                crash.disasm,                 \
                crash.violation_thread_id,    \
                direction,                    \
                crash.violation_address       \
            )

        synopsis += crash.context_dump

        synopsis += "\ndisasm around:\n"
        for (ea, inst) in crash.disasm_around:
            synopsis += "\t0x%08x %s\n" % (ea, inst)

        if len(crash.stack_unwind):
            synopsis += "\nstack unwind:\n"
            for entry in crash.stack_unwind:
                synopsis += "\t%s\n" % entry

        if len(crash.seh_unwind):
            synopsis += "\nSEH unwind:\n"
            for (addr, handler, handler_str) in crash.seh_unwind:
                synopsis +=  "\t%08x -> %s\n" % (addr, handler_str)

        return synopsis + "\n"


    ####################################################################################################################
    def export_string (self):
        last_crash = self.last_crash
        pydbg      = self.pydbg

        self.last_crash = self.pydbg = None
#        data = zlib.compress(cPickle.dumps(self, protocol=2))
        data = pickle.dumps(self, protocol=2)

        self.last_crash = last_crash
        self.pydbg      = pydbg

        return data

    ####################################################################################################################
    def export_file (self, file_name):
        '''
        Dump the entire object structure to disk.

        @see: import_file()

        @type  file_name:   String
        @param file_name:   File name to export to

        @rtype:             crash_binning
        @return:            self
        '''

        # null out what we don't serialize but save copies to restore after dumping to disk.
        last_crash = self.last_crash
        pydbg      = self.pydbg

        self.last_crash = self.pydbg = None

        fh = open(file_name, "wb+")
        fh.write(zlib.compress(pickle.dumps(self, protocol=2)))
        fh.close()

        self.last_crash = last_crash
        self.pydbg      = pydbg

        return self


    ####################################################################################################################
    def import_string (self, string):
#        tmp = cPickle.loads(zlib.decompress(string))
        tmp = pickle.loads(string)
        self.bins = tmp.bins

        return self

    ####################################################################################################################
    def import_file (self, file_name):
        '''
        Load the entire object structure from disk.

        @see: export_file()

        @type  file_name:   String
        @param file_name:   File name to import from

        @rtype:             crash_binning
        @return:            self
        '''

        fh  = open(file_name, "rb")
        tmp = pickle.loads(zlib.decompress(fh.read()))
        fh.close()

        self.bins = tmp.bins
        if(len(self.bins) > 0):
            self.last_crash = self.bins[0]

        return self


    ####################################################################################################################
    def last_crash_synopsis (self):
        '''
        For the last recorded crash, generate and return a report containing the disassemly around the violating
        address, the ID of the offending thread, the call stack and the SEH unwind.

        @see: crash_synopsis()

        @rtype:  String
        @return: Crash report
        '''

        if self.last_crash.write_violation:
            direction = "write to"
        else:
            direction = "read from"

        synopsis = "%s:%08x %s from thread %d caused access violation\nwhen attempting to %s 0x%08x\n\n" % \
            (
                self.last_crash.exception_module,       \
                self.last_crash.exception_address,      \
                self.last_crash.disasm,                 \
                self.last_crash.violation_thread_id,    \
                direction,                              \
                self.last_crash.violation_address       \
            )

        synopsis += self.last_crash.context_dump

        synopsis += "\ndisasm around:\n"
        for (ea, inst) in self.last_crash.disasm_around:
            synopsis += "\t0x%08x %s\n" % (ea, inst)

        if len(self.last_crash.stack_unwind):
            synopsis += "\nstack unwind:\n"
            for entry in self.last_crash.stack_unwind:
                synopsis += "\t%s\n" % entry

        if len(self.last_crash.seh_unwind):
            synopsis += "\nSEH unwind:\n"
            for (addr, handler, handler_str) in self.last_crash.seh_unwind:
                try:
                    disasm = self.pydbg.disasm(handler)
                except:
                    disasm = "[INVALID]"

                synopsis +=  "\t%08x -> %s %s\n" % (addr, handler_str, disasm)

        return synopsis + "\n"
