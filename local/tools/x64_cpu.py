#!/usr/bin/env python

import struct

class taint_x64(object):
    RAX_OFF = 0x0
    RBX_OFF = 0x8
    RCX_OFF = 0x10
    RDX_OFF = 0x18
    RSX_OFF = 0x20
    RDX_OFF = 0x28
    RBP_OFF = 0x30
    RSP_OFF = 0x38
    RFLAGS_OFF = 0x40
    RIP_OFF = 0x48

    def __init__(self, mem_size = 0x100000000):
        self.memory = bytearray(mem_size)
        self.registers = bytearray(0x750)

        self.rax = 0x0
        self.rbx = 0x0
        self.rcx = 0x0
        self.rdx = 0x0

        self.rsi = 0x0
        self.rdi = 0x0
        self.rsp = 0x0
        self.rbp = 0x0

        self.rflags = 0x0
        self.rip = 0x0

        print 'initializing taint emulator'
    
        self.r_all = %
        self.r_all['\x50'] = self.e_push
        self.r_all['\x31'] = self.e_xor
        self.r_all['\x49'] = self.e_mov

        self.r_xor = %
        self.r_xor['\xed'] = self.e_xor_ebp_ebp

        self.r_mov = %
#        self.r_mov['\xed'] = self.e_xor_ebp_ebp

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

    ### RBP
    @property
    def rbp(self):
        return struct.unpack('>q', self.registers[taint_x64.RBP_OFF:taint_x64.RBP_OFF+0x8])[0]
    @rbp.setter
    def rbp(self, val):
        self.registers[taint_x64.RBP_OFF:taint_x64.RBP_OFF+0x8] = struct.pack('>q', val)
    @property
    def ebp(self):
        return struct.unpack('>i', self.registers[taint_x64.RBP_OFF+0x4:taint_x64.RBP_OFF+0x8])[0]
    @ebp.setter
    def ebp(self, val):
        self.registers[taint_x64.RBP_OFF+0x4:taint_x64.RBP_OFF+0x8] = struct.pack('>i', val)
    @property
    def bp(self):
        return struct.unpack('>h', self.registers[taint_x64.RBP_OFF+0x6:taint_x64.RBP_OFF+0x8])[0]
    @bp.setter
    def bp(self, val):
        self.registers[taint_x64.RBP_OFF+0x6:taint_x64.RBP_OFF+0x8] = struct.pack('>h', val)

    ### RSI
    @property
    def rsi(self):
        return struct.unpack('>q', self.registers[taint_x64.RSI_OFF:taint_x64.RSI_OFF+0x8])[0]
    @rsi.setter
    def rsi(self, val):
        self.registers[taint_x64.RSI_OFF:taint_x64.RSI_OFF+0x8] = struct.pack('>q', val)
    @property
    def esi(self):
        return struct.unpack('>i', self.registers[taint_x64.RSI_OFF+0x4:taint_x64.RSI_OFF+0x8])[0]
    @esi.setter
    def esi(self, val):
        self.registers[taint_x64.RSI_OFF+0x4:taint_x64.RSI_OFF+0x8] = struct.pack('>i', val)
    @property
    def si(self):
        return struct.unpack('>h', self.registers[taint_x64.RSI_OFF+0x6:taint_x64.RSI_OFF+0x8])[0]
    @si.setter
    def si(self, val):
        self.registers[taint_x64.RSI_OFF+0x6:taint_x64.RSI_OFF+0x8] = struct.pack('>h', val)

    ### RDI
    @property
    def rdi(self):
        return struct.unpack('>q', self.registers[taint_x64.RDI_OFF:taint_x64.RDI_OFF+0x8])[0]
    @rdi.setter
    def rdi(self, val):
        self.registers[taint_x64.RDI_OFF:taint_x64.RDI_OFF+0x8] = struct.pack('>q', val)
    @property
    def edi(self):
        return struct.unpack('>i', self.registers[taint_x64.RDI_OFF+0x4:taint_x64.RDI_OFF+0x8])[0]
    @edi.setter
    def edi(self, val):
        self.registers[taint_x64.RDI_OFF+0x4:taint_x64.RDI_OFF+0x8] = struct.pack('>i', val)
    @property
    def di(self):
        return struct.unpack('>h', self.registers[taint_x64.RDI_OFF+0x6:taint_x64.RDI_OFF+0x8])[0]
    @di.setter
    def di(self, val):
        self.registers[taint_x64.RDI_OFF+0x6:taint_x64.RDI_OFF+0x8] = struct.pack('>h', val)

    ### RSP
    @property
    def rsp(self):
        return struct.unpack('>q', self.registers[taint_x64.RSP_OFF:taint_x64.RSP_OFF+0x8])[0]
    @rsp.setter
    def rsp(self, val):
        self.registers[taint_x64.RSP_OFF:taint_x64.RSP_OFF+0x8] = struct.pack('>q', val)
    @property
    def esp(self):
        return struct.unpack('>i', self.registers[taint_x64.RSP_OFF+0x4:taint_x64.RSP_OFF+0x8])[0]
    @esp.setter
    def esp(self, val):
        self.registers[taint_x64.RSP_OFF+0x4:taint_x64.RSP_OFF+0x8] = struct.pack('>i', val)
    @property
    def sp(self):
        return struct.unpack('>h', self.registers[taint_x64.RSP_OFF+0x6:taint_x64.RSP_OFF+0x8])[0]
    @sp.setter
    def sp(self, val):
        self.registers[taint_x64.RSP_OFF+0x6:taint_x64.RSP_OFF+0x8] = struct.pack('>h', val)

    ### RFLAGS
    @property
    def rflags(self):
        return struct.unpack('>q', self.registers[taint_x64.RFLAGS_OFF:taint_x64.RFLAGS_OFF+0x8])[0]
    @rflags.setter
    def rflags(self, val):
        self.registers[taint_x64.RFLAGS_OFF:taint_x64.RFLAGS_OFF+0x8] = struct.pack('>q', val)
    @property
    def eflags(self):
        return struct.unpack('>i', self.registers[taint_x64.RFLAGS_OFF+0x4:taint_x64.RFLAGS_OFF+0x8])[0]
    @eflags.setter
    def eflags(self, val):
        self.registers[taint_x64.RFLAGS_OFF+0x4:taint_x64.RFLAGS_OFF+0x8] = struct.pack('>i', val)
    @property
    def flags(self):
        return struct.unpack('>h', self.registers[taint_x64.RFLAGS_OFF+0x6:taint_x64.RFLAGS_OFF+0x8])[0]
    @flags.setter
    def flags(self, val):
        self.registers[taint_x64.RFLAGS_OFF+0x6:taint_x64.RFLAGS_OFF+0x8] = struct.pack('>h', val)

    ### RIP
    @property
    def rip(self):
        return struct.unpack('>q', self.registers[taint_x64.RIP_OFF:taint_x64.RIP_OFF+0x8])[0]
    @rip.setter
    def rip(self, val):
        self.registers[taint_x64.RIP_OFF:taint_x64.RIP_OFF+0x8] = struct.pack('>q', val)
    @property
    def eip(self):
        return struct.unpack('>i', self.registers[taint_x64.RIP_OFF+0x4:taint_x64.RIP_OFF+0x8])[0]
    @eip.setter
    def eip(self, val):
        self.registers[taint_x64.RIP_OFF+0x4:taint_x64.RIP_OFF+0x8] = struct.pack('>i', val)
    @property
    def ip(self):
        return struct.unpack('>h', self.registers[taint_x64.RIP_OFF+0x6:taint_x64.RIP_OFF+0x8])[0]
    @ip.setter
    def ip(self, val):
        self.registers[taint_x64.RIP_OFF+0x6:taint_x64.RIP_OFF+0x8] = struct.pack('>h', val)

    ### memory transfer

    def restore(address, size=0x2):
        if(size == 0x2):
            return struct.unpack('<h', self.memory[self.rsp, self.rsp + 0x2])
        if(size == 0x4):
            return struct.unpack('<i', self.memory[self.rsp, self.rsp + 0x4])
        if(size == 0x8):
            return struct.unpack('<d', self.memory[self.rsp, self.rsp + 0x8])

    def store(address, size=0x2, val)
        if(size == 0x2):
            self.memory[self.rsp, self.rsp+0x2] = struct.pack('<h', val)
        if(size == 0x4):
            self.memory[self.rsp, self.rsp+0x4] = struct.pack('<i', val)
        if(size == 0x8):
            self.memory[self.rsp, self.rsp+0x8] = struct.pack('<d', val)

    ### execution

    # push family

    def e_push(arg):
        opcode, arg = pare_instr(arg)
        # check if arg None?
        self.r_push[opcode](arg)

    def e_push_rax(arg):
        self.rsp = self.rsp - 0x8
        store(self.rsp, 0x8, self.rax)

    def e_push_rsp(arg):
        self.rsp = self.rsp - 0x8
        store(self.rsp, 0x8, self.rsp)

    # xor family

    def e_xor(arg):
        opcode, arg = pare_instr(arg)
        # check if arg None?
        self.r_xor[opcode](arg)

    def e_xor_ebp_ebp(arg = None):
        self.ebp = 0x0

    # mov family

    def e_mov(arg):
        opcode, arg = pare_instr(arg)
        # check if arg None?
        self.r_mov[opcode](arg)

    def e_mov_rdx_rsp(arg = None):
        self.rdx = self.rsp

    def e_mov_r9_rdx(arg = None):
        self.r9 = self.rdx

    # pop family

    def e_pop(arg):
        opcode, arg = pare_instr(arg)
        # check if arg None?
        self.r_pop[opcode](arg)

    def e_pop_rsi(arg = None):
        self.rsi = restore(0x8, self.rsp)
        self.rsp = self.rsp + 0x8

    # and family

    def e_and(arg):
        opcode, arg = pare_instr(arg)
        # check if arg None?
        self.r_and[opcode](arg)

    def e_and_rsp_imm(arg):
        self.rsp = self.rsp & arg

    def parse_instr(arg):
        cmd = arg[0]
        if(len(arg) > 1):
            arg = arg[1:]
        else arg = None
        return cmd, arg
        
    def execute(arg = '\x90'):
        opcode, arg = pare_instr(arg)
        self.r_all[opcode](arg)

