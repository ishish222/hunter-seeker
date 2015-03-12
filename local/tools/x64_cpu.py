#!/usr/bin/env python

import struct

class taint_x64(object):
    RAX_OFF = 0x0
    RBX_OFF = 0x8
    RCX_OFF = 0x10
    RDX_OFF = 0x18
    def __init__(self, mem_size = 0x100000000):
        self.memory = bytearray(mem_size)
        self.registers = bytearray(0x750)
        self.rax = 0x0
        self.rbx = 0x0
        self.rcx = 0x0
        self.rdx = 0x0
    ### RAX
    @property
    def rax(self):
        return struct.unpack('>q', self.registers[taint_x64.RAX_OFF:taint_x64.RAX_OFF+0x8])[0]
    @rax.setter
    def rax(self, val):
        self.registers[taint_x64.RAX_OFF:taint_x64.RAX_OFF+0x8] = struct.pack('>q', val)
    @property
    def eax(self):
        return struct.unpack('>i', self.registers[taint_x64.RAX_OFF+0x4:taint_x64.RAX_OFF+0x8])[0]
    @eax.setter
    def eax(self, val):
        self.registers[taint_x64.RAX_OFF+0x4:taint_x64.RAX_OFF+0x8] = struct.pack('>i', val)
    @property
    def ax(self):
        return struct.unpack('>h', self.registers[taint_x64.RAX_OFF+0x6:taint_x64.RAX_OFF+0x8])[0]
    @ax.setter
    def ax(self, val):
        self.registers[taint_x64.RAX_OFF+0x6:taint_x64.RAX_OFF+0x8] = struct.pack('>h', val)
    @property
    def ah(self):
        return ord(struct.unpack('>c', self.registers[taint_x64.RAX_OFF+0x6:taint_x64.RAX_OFF+0x7])[0])
    @ah.setter
    def ah(self, val):
        self.registers[taint_x64.RAX_OFF+0x6:taint_x64.RAX_OFF+0x7] = struct.pack('>c', val)
    @property
    def al(self):
        return ord(struct.unpack('>c', self.registers[taint_x64.RAX_OFF+0x7:taint_x64.RAX_OFF+0x8])[0])
    @al.setter
    def al(self, val):
        self.registers[taint_x64.RAX_OFF+0x7:taint_x64.RAX_OFF+0x8] = struct.pack('>c', val)
    ### RBX
    @property
    def rbx(self):
        return struct.unpack('>q', self.registers[taint_x64.RBX_OFF:taint_x64.RBX_OFF+0x8])[0]
    @rax.setter
    def rbx(self, val):
        self.registers[taint_x64.RBX_OFF:taint_x64.RBX_OFF+0x8] = struct.pack('>q', val)
    @property
    def ebx(self):
        return struct.unpack('>i', self.registers[taint_x64.RBX_OFF+0x4:taint_x64.RBX_OFF+0x8])[0]
    @eax.setter
    def ebx(self, val):
        self.registers[taint_x64.RBX_OFF+0x4:taint_x64.RBX_OFF+0x8] = struct.pack('>i', val)
    @property
    def bx(self):
        return struct.unpack('>h', self.registers[taint_x64.RBX_OFF+0x6:taint_x64.RBX_OFF+0x8])[0]
    @bx.setter
    def bx(self, val):
        self.registers[taint_x64.RBX_OFF+0x6:taint_x64.RBX_OFF+0x8] = struct.pack('>h', val)
    @property
    def bh(self):
        return ord(struct.unpack('>c', self.registers[taint_x64.RBX_OFF+0x6:taint_x64.RBX_OFF+0x7])[0])
    @bh.setter
    def bh(self, val):
        self.registers[taint_x64.RBX_OFF+0x6:taint_x64.RBX_OFF+0x7] = struct.pack('>c', val)
    @property
    def bl(self):
        return ord(struct.unpack('>c', self.registers[taint_x64.RBX_OFF+0x7:taint_x64.RBX_OFF+0x8])[0])
    @bl.setter
    def bl(self, val):
        self.registers[taint_x64.RBX_OFF+0x7:taint_x64.RBX_OFF+0x8] = struct.pack('>c', val)


    ### RCX
    @property
    def rcx(self):
        return struct.unpack('>q', self.registers[taint_x64.RCX_OFF:taint_x64.RCX_OFF+0x8])[0]
    @rcx.setter
    def rcx(self, val):
        self.registers[taint_x64.RCX_OFF:taint_x64.RCX_OFF+0x8] = struct.pack('>q', val)
    @property
    def ecx(self):
        return struct.unpack('>i', self.registers[taint_x64.RCX_OFF+0x4:taint_x64.RCX_OFF+0x8])[0]
    @ecx.setter
    def ecx(self, val):
        self.registers[taint_x64.RCX_OFF+0x4:taint_x64.RCX_OFF+0x8] = struct.pack('>i', val)
    @property
    def cx(self):
        return struct.unpack('>h', self.registers[taint_x64.RCX_OFF+0x6:taint_x64.RCX_OFF+0x8])[0]
    @cx.setter
    def cx(self, val):
        self.registers[taint_x64.RCX_OFF+0x6:taint_x64.RCX_OFF+0x8] = struct.pack('>h', val)
    @property
    def ch(self):
        return ord(struct.unpack('>c', self.registers[taint_x64.RCX_OFF+0x6:taint_x64.RCX_OFF+0x7])[0])
    @ch.setter
    def ch(self, val):
        self.registers[taint_x64.RCX_OFF+0x6:taint_x64.RCX_OFF+0x7] = struct.pack('>c', val)
    @property
    def cl(self):
        return ord(struct.unpack('>c', self.registers[taint_x64.RCX_OFF+0x7:taint_x64.RCX_OFF+0x8])[0])
    @cl.setter
    def cl(self, val):
        self.registers[taint_x64.RCX_OFF+0x7:taint_x64.RCX_OFF+0x8] = struct.pack('>c', val)

    ### RDX
    @property
    def rdx(self):
        return struct.unpack('>q', self.registers[taint_x64.RDX_OFF:taint_x64.RDX_OFF+0x8])[0]
    @rdx.setter
    def rdx(self, val):
        self.registers[taint_x64.RDX_OFF:taint_x64.RDX_OFF+0x8] = struct.pack('>q', val)
    @property
    def edx(self):
        return struct.unpack('>i', self.registers[taint_x64.RDX_OFF+0x4:taint_x64.RDX_OFF+0x8])[0]
    @edx.setter
    def edx(self, val):
        self.registers[taint_x64.RDX_OFF+0x4:taint_x64.RDX_OFF+0x8] = struct.pack('>i', val)
    @property
    def dx(self):
        return struct.unpack('>h', self.registers[taint_x64.RDX_OFF+0x6:taint_x64.RDX_OFF+0x8])[0]
    @dx.setter
    def dx(self, val):
        self.registers[taint_x64.RDX_OFF+0x6:taint_x64.RDX_OFF+0x8] = struct.pack('>h', val)
    @property
    def dh(self):
        return ord(struct.unpack('>c', self.registers[taint_x64.RDX_OFF+0x6:taint_x64.RDX_OFF+0x7])[0])
    @dh.setter
    def dh(self, val):
        self.registers[taint_x64.RDX_OFF+0x6:taint_x64.RDX_OFF+0x7] = struct.pack('>c', val)
    @property
    def dl(self):
        return ord(struct.unpack('>c', self.registers[taint_x64.RDX_OFF+0x7:taint_x64.RDX_OFF+0x8])[0])
    @dl.setter
    def dl(self, val):
        self.registers[taint_x64.RDX_OFF+0x7:taint_x64.RDX_OFF+0x8] = struct.pack('>c', val)


