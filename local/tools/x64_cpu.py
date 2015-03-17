#!/usr/bin/env python

import struct

#class memory_le(bytearray):
#    def __init__(self, mem_size):
#        super.__init__(mem_size)

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

        # push family
        self.r_all['\x50'] = self.e_push_eax
        self.r_all['\x51'] = self.e_push_ecx
        self.r_all['\x52'] = self.e_push_edx
        self.r_all['\x53'] = self.e_push_ebx
        self.r_all['\x54'] = self.e_push_esp
        self.r_all['\x55'] = self.e_push_ebp

        # xor family
        self.r_all['\x31'] = self.e_xor_r
        self.r_all['\x34'] = self.e_xor_ib
        self.r_all['\x49'] = self.e_mov

        # legacy routing
        self.r_xor = %
        self.r_xor['\xed'] = self.e_xor_ebp_ebp
        self.r_xor['\x31'] = self.e_xor_r
        self.r_xor['\x34'] = self.e_xor_ib

        self.r_mov = %
#        self.r_mov['\xed'] = self.e_xor_ebp_ebp

        # modrm targets for 16 addressing (rm,r pairs, eg. [ax] ax = (ax, ax))
        # if has 
        # None means there is SIB or displacement
        self.modrm16 = %

        self.modrm16[0x00] = (self.ax, self.ax)
        self.modrm16[0x01] = (self.cx, self.ax)
        self.modrm16[0x02] = (self.dx, self.ax)
        self.modrm16[0x03] = (self.bx, self.ax)
        self.modrm16[0x04] = None
        self.modrm16[0x05] = None
        self.modrm16[0x06] = (self.si, self.ax)
        self.modrm16[0x07] = (self.di, self.ax)

        self.modrm16[0x08] = (self.ax, self.cx)
        self.modrm16[0x09] = (self.cx, self.cx)
        self.modrm16[0x0a] = (self.dx, self.cx)
        self.modrm16[0x0b] = (self.bx, self.cx)
        self.modrm16[0x0c] = None
        self.modrm16[0x0d] = None
        self.modrm16[0x0e] = (self.si, self.cx)
        self.modrm16[0x0f] = (self.di, self.cx)

        self.modrm16[0x10] = (self.ax, self.dx)
        self.modrm16[0x11] = (self.cx, self.dx)
        self.modrm16[0x12] = (self.dx, self.dx)
        self.modrm16[0x13] = (self.bx, self.dx)
        self.modrm16[0x14] = None
        self.modrm16[0x15] = None
        self.modrm16[0x16] = (self.si, self.dx)
        self.modrm16[0x17] = (self.di, self.dx)

        self.modrm16[0x18] = (self.ax, self.bx)
        self.modrm16[0x19] = (self.cx, self.bx)
        self.modrm16[0x1a] = (self.dx, self.bx)
        self.modrm16[0x1b] = (self.bx, self.bx)
        self.modrm16[0x1c] = None
        self.modrm16[0x1d] = None
        self.modrm16[0x1e] = (self.si, self.bx)
        self.modrm16[0x1f] = (self.di, self.bx)

        self.modrm16[0x20] = (self.ax, self.sp)
        self.modrm16[0x21] = (self.cx, self.sp)
        self.modrm16[0x22] = (self.dx, self.sp)
        self.modrm16[0x23] = (self.bx, self.sp)
        self.modrm16[0x24] = None
        self.modrm16[0x25] = None
        self.modrm16[0x26] = (self.si, self.sp)
        self.modrm16[0x27] = (self.di, self.sp)

        self.modrm16[0x28] = (self.ax, self.bp)
        self.modrm16[0x29] = (self.cx, self.bp)
        self.modrm16[0x2a] = (self.dx, self.bp)
        self.modrm16[0x2b] = (self.bx, self.bp)
        self.modrm16[0x2c] = None
        self.modrm16[0x2d] = None
        self.modrm16[0x2e] = (self.si, self.bp)
        self.modrm16[0x2f] = (self.di, self.bp)

        self.modrm16[0x30] = (self.ax, self.si)
        self.modrm16[0x31] = (self.cx, self.si)
        self.modrm16[0x16] = (self.dx, self.si)
        self.modrm16[0x33] = (self.bx, self.si)
        self.modrm16[0x34] = None
        self.modrm16[0x35] = None
        self.modrm16[0x36] = (self.si, self.si)
        self.modrm16[0x37] = (self.di, self.si)

        self.modrm16[0x38] = (self.ax, self.di)
        self.modrm16[0x39] = (self.cx, self.di)
        self.modrm16[0x3a] = (self.dx, self.di)
        self.modrm16[0x3b] = (self.bx, self.di)
        self.modrm16[0x3c] = None
        self.modrm16[0x3d] = None
        self.modrm16[0x3e] = (self.si, self.di)
        self.modrm16[0x3f] = (self.di, self.di)

        ### 0x40-0x80 special treatment - disp8 
        self.modrm16[0x40] = (self.ax, self.ax)
        self.modrm16[0x41] = (self.cx, self.ax)
        self.modrm16[0x42] = (self.dx, self.ax)
        self.modrm16[0x43] = (self.bx, self.ax)
        self.modrm16[0x44] = None
        self.modrm16[0x45] = None
        self.modrm16[0x46] = (self.si, self.ax)
        self.modrm16[0x47] = (self.di, self.ax)

        self.modrm16[0x48] = (self.ax, self.cx)
        self.modrm16[0x49] = (self.cx, self.cx)
        self.modrm16[0x4a] = (self.dx, self.cx)
        self.modrm16[0x4b] = (self.bx, self.cx)
        self.modrm16[0x4c] = None
        self.modrm16[0x4d] = None
        self.modrm16[0x4e] = (self.si, self.cx)
        self.modrm16[0x4f] = (self.di, self.cx)

        self.modrm16[0x50] = (self.ax, self.dx)
        self.modrm16[0x51] = (self.cx, self.dx)
        self.modrm16[0x52] = (self.dx, self.dx)
        self.modrm16[0x53] = (self.bx, self.dx)
        self.modrm16[0x54] = None
        self.modrm16[0x55] = None
        self.modrm16[0x56] = (self.si, self.dx)
        self.modrm16[0x57] = (self.di, self.dx)

        self.modrm16[0x58] = (self.ax, self.bx)
        self.modrm16[0x59] = (self.cx, self.bx)
        self.modrm16[0x5a] = (self.dx, self.bx)
        self.modrm16[0x5b] = (self.bx, self.bx)
        self.modrm16[0x5c] = None
        self.modrm16[0x5d] = None
        self.modrm16[0x5e] = (self.si, self.bx)
        self.modrm16[0x5f] = (self.di, self.bx)

        self.modrm16[0x60] = (self.ax, self.sp)
        self.modrm16[0x61] = (self.cx, self.sp)
        self.modrm16[0x62] = (self.dx, self.sp)
        self.modrm16[0x63] = (self.bx, self.sp)
        self.modrm16[0x64] = None
        self.modrm16[0x65] = None
        self.modrm16[0x66] = (self.si, self.sp)
        self.modrm16[0x67] = (self.di, self.sp)

        self.modrm16[0x68] = (self.ax, self.bp)
        self.modrm16[0x69] = (self.cx, self.bp)
        self.modrm16[0x6a] = (self.dx, self.bp)
        self.modrm16[0x6b] = (self.bx, self.bp)
        self.modrm16[0x6c] = None
        self.modrm16[0x6d] = None
        self.modrm16[0x6e] = (self.si, self.bp)
        self.modrm16[0x6f] = (self.di, self.bp)

        self.modrm16[0x70] = (self.ax, self.si)
        self.modrm16[0x71] = (self.cx, self.si)
        self.modrm16[0x72] = (self.dx, self.si)
        self.modrm16[0x73] = (self.bx, self.si)
        self.modrm16[0x74] = None
        self.modrm16[0x75] = None
        self.modrm16[0x76] = (self.si, self.si)
        self.modrm16[0x77] = (self.di, self.si)

        self.modrm16[0x78] = (self.ax, self.di)
        self.modrm16[0x79] = (self.cx, self.di)
        self.modrm16[0x7a] = (self.dx, self.di)
        self.modrm16[0x7b] = (self.bx, self.di)
        self.modrm16[0x7c] = None
        self.modrm16[0x7d] = None
        self.modrm16[0x7e] = (self.si, self.di)
        self.modrm16[0x7f] = (self.di, self.di)

        ### 0x80-0xc0 special treatment - disp16
        self.modrm16[0x80] = (self.ax, self.ax)
        self.modrm16[0x81] = (self.cx, self.ax)
        self.modrm16[0x82] = (self.dx, self.ax)
        self.modrm16[0x83] = (self.bx, self.ax)
        self.modrm16[0x84] = None
        self.modrm16[0x85] = None
        self.modrm16[0x86] = (self.si, self.ax)
        self.modrm16[0x87] = (self.di, self.ax)

        self.modrm16[0x88] = (self.ax, self.cx)
        self.modrm16[0x89] = (self.cx, self.cx)
        self.modrm16[0x8a] = (self.dx, self.cx)
        self.modrm16[0x8b] = (self.bx, self.cx)
        self.modrm16[0x8c] = None
        self.modrm16[0x8d] = None
        self.modrm16[0x8e] = (self.si, self.cx)
        self.modrm16[0x8f] = (self.di, self.cx)

        self.modrm16[0x90] = (self.ax, self.dx)
        self.modrm16[0x91] = (self.cx, self.dx)
        self.modrm16[0x92] = (self.dx, self.dx)
        self.modrm16[0x93] = (self.bx, self.dx)
        self.modrm16[0x94] = None
        self.modrm16[0x95] = None
        self.modrm16[0x96] = (self.si, self.dx)
        self.modrm16[0x97] = (self.di, self.dx)

        self.modrm16[0x98] = (self.ax, self.bx)
        self.modrm16[0x99] = (self.cx, self.bx)
        self.modrm16[0x9a] = (self.dx, self.bx)
        self.modrm16[0x9b] = (self.bx, self.bx)
        self.modrm16[0x9c] = None
        self.modrm16[0x9d] = None
        self.modrm16[0x9e] = (self.si, self.bx)
        self.modrm16[0x9f] = (self.di, self.bx)

        self.modrm16[0xa0] = (self.ax, self.sp)
        self.modrm16[0xa1] = (self.cx, self.sp)
        self.modrm16[0xa2] = (self.dx, self.sp)
        self.modrm16[0xa3] = (self.bx, self.sp)
        self.modrm16[0xa4] = None
        self.modrm16[0xa5] = None
        self.modrm16[0xa6] = (self.si, self.sp)
        self.modrm16[0xa7] = (self.di, self.sp)

        self.modrm16[0xa8] = (self.ax, self.bp)
        self.modrm16[0xa9] = (self.cx, self.bp)
        self.modrm16[0xaa] = (self.dx, self.bp)
        self.modrm16[0xab] = (self.bx, self.bp)
        self.modrm16[0xac] = None
        self.modrm16[0xad] = None
        self.modrm16[0xae] = (self.si, self.bp)
        self.modrm16[0xaf] = (self.di, self.bp)

        self.modrm16[0xb0] = (self.ax, self.si)
        self.modrm16[0xb1] = (self.cx, self.si)
        self.modrm16[0xb2] = (self.dx, self.si)
        self.modrm16[0xb3] = (self.bx, self.si)
        self.modrm16[0xb4] = None
        self.modrm16[0xb5] = None
        self.modrm16[0xb6] = (self.si, self.si)
        self.modrm16[0xb7] = (self.di, self.si)

        self.modrm16[0xb8] = (self.ax, self.di)
        self.modrm16[0xb9] = (self.cx, self.di)
        self.modrm16[0xba] = (self.dx, self.di)
        self.modrm16[0xbb] = (self.bx, self.di)
        self.modrm16[0xbc] = None
        self.modrm16[0xbd] = None
        self.modrm16[0xbe] = (self.si, self.di)
        self.modrm16[0xbf] = (self.di, self.di)

        ### 0xc0-0xf0 special treatment - direct acces, no mem
        self.modrm16[0xc0] = (self.ax, self.ax)
        self.modrm16[0xc1] = (self.cx, self.ax)
        self.modrm16[0xc2] = (self.dx, self.ax)
        self.modrm16[0xc3] = (self.bx, self.ax)
        self.modrm16[0xc4] = None
        self.modrm16[0xc5] = None
        self.modrm16[0xc6] = (self.si, self.ax)
        self.modrm16[0xc7] = (self.di, self.ax)

        self.modrm16[0xc8] = (self.ax, self.cx)
        self.modrm16[0xc9] = (self.cx, self.cx)
        self.modrm16[0xca] = (self.dx, self.cx)
        self.modrm16[0xcb] = (self.bx, self.cx)
        self.modrm16[0xcc] = None
        self.modrm16[0xcd] = None
        self.modrm16[0xce] = (self.si, self.cx)
        self.modrm16[0xcf] = (self.di, self.cx)

        self.modrm16[0xd0] = (self.ax, self.dx)
        self.modrm16[0xd1] = (self.cx, self.dx)
        self.modrm16[0xd2] = (self.dx, self.dx)
        self.modrm16[0xd3] = (self.bx, self.dx)
        self.modrm16[0xd4] = None
        self.modrm16[0xd5] = None
        self.modrm16[0xd6] = (self.si, self.dx)
        self.modrm16[0xd7] = (self.di, self.dx)

        self.modrm16[0xd8] = (self.ax, self.bx)
        self.modrm16[0xd9] = (self.cx, self.bx)
        self.modrm16[0xda] = (self.dx, self.bx)
        self.modrm16[0xdb] = (self.bx, self.bx)
        self.modrm16[0xdc] = None
        self.modrm16[0xdd] = None
        self.modrm16[0xde] = (self.si, self.bx)
        self.modrm16[0xdf] = (self.di, self.bx)

        self.modrm16[0xe0] = (self.ax, self.sp)
        self.modrm16[0xe1] = (self.cx, self.sp)
        self.modrm16[0xe2] = (self.dx, self.sp)
        self.modrm16[0xe3] = (self.bx, self.sp)
        self.modrm16[0xe4] = None
        self.modrm16[0xe5] = None
        self.modrm16[0xe6] = (self.si, self.sp)
        self.modrm16[0xe7] = (self.di, self.sp)

        self.modrm16[0xe8] = (self.ax, self.bp)
        self.modrm16[0xe9] = (self.cx, self.bp)
        self.modrm16[0xea] = (self.dx, self.bp)
        self.modrm16[0xeb] = (self.bx, self.bp)
        self.modrm16[0xec] = None
        self.modrm16[0xed] = None
        self.modrm16[0xee] = (self.si, self.bp)
        self.modrm16[0xef] = (self.di, self.bp)

        self.modrm16[0xf0] = (self.ax, self.si)
        self.modrm16[0xf1] = (self.cx, self.si)
        self.modrm16[0xf2] = (self.dx, self.si)
        self.modrm16[0xf3] = (self.bx, self.si)
        self.modrm16[0xf4] = None
        self.modrm16[0xf5] = None
        self.modrm16[0xf6] = (self.si, self.si)
        self.modrm16[0xf7] = (self.di, self.si)

        self.modrm16[0xf8] = (self.ax, self.di)
        self.modrm16[0xf9] = (self.cx, self.di)
        self.modrm16[0xfa] = (self.dx, self.di)
        self.modrm16[0xfb] = (self.bx, self.di)
        self.modrm16[0xfc] = None
        self.modrm16[0xfd] = None
        self.modrm16[0xfe] = (self.si, self.di)
        self.modrm16[0xff] = (self.di, self.di)

        # modrm targets for 32 addressing (rm,r pairs, eg. [eax] eax = (eax, eax))
        # if has 
        # None means there is SIB or displacement
        self.modrm32 = %

        self.modrm32[0x00] = (self.eax, self.eax)
        self.modrm32[0x01] = (self.ecx, self.eax)
        self.modrm32[0x02] = (self.edx, self.eax)
        self.modrm32[0x03] = (self.ebx, self.eax)
        self.modrm32[0x04] = None
        self.modrm32[0x05] = None
        self.modrm32[0x06] = (self.esi, self.eax)
        self.modrm32[0x07] = (self.edi, self.eax)

        self.modrm32[0x08] = (self.eax, self.ecx)
        self.modrm32[0x09] = (self.ecx, self.ecx)
        self.modrm32[0x0a] = (self.edx, self.ecx)
        self.modrm32[0x0b] = (self.ebx, self.ecx)
        self.modrm32[0x0c] = None
        self.modrm32[0x0d] = None
        self.modrm32[0x0e] = (self.esi, self.ecx)
        self.modrm32[0x0f] = (self.edi, self.ecx)

        self.modrm32[0x10] = (self.eax, self.edx)
        self.modrm32[0x11] = (self.ecx, self.edx)
        self.modrm32[0x12] = (self.edx, self.edx)
        self.modrm32[0x13] = (self.ebx, self.edx)
        self.modrm32[0x14] = None
        self.modrm32[0x15] = None
        self.modrm32[0x16] = (self.esi, self.edx)
        self.modrm32[0x17] = (self.edi, self.edx)

        self.modrm32[0x18] = (self.eax, self.ebx)
        self.modrm32[0x19] = (self.ecx, self.ebx)
        self.modrm32[0x1a] = (self.edx, self.ebx)
        self.modrm32[0x1b] = (self.ebx, self.ebx)
        self.modrm32[0x1c] = None
        self.modrm32[0x1d] = None
        self.modrm32[0x1e] = (self.esi, self.ebx)
        self.modrm32[0x1f] = (self.edi, self.ebx)

        self.modrm32[0x20] = (self.eax, self.esp)
        self.modrm32[0x21] = (self.ecx, self.esp)
        self.modrm32[0x22] = (self.edx, self.esp)
        self.modrm32[0x23] = (self.ebx, self.esp)
        self.modrm32[0x24] = None
        self.modrm32[0x25] = None
        self.modrm32[0x26] = (self.esi, self.esp)
        self.modrm32[0x27] = (self.edi, self.esp)

        self.modrm32[0x28] = (self.eax, self.ebp)
        self.modrm32[0x29] = (self.ecx, self.ebp)
        self.modrm32[0x2a] = (self.edx, self.ebp)
        self.modrm32[0x2b] = (self.ebx, self.ebp)
        self.modrm32[0x2c] = None
        self.modrm32[0x2d] = None
        self.modrm32[0x2e] = (self.esi, self.ebp)
        self.modrm32[0x2f] = (self.edi, self.ebp)

        self.modrm32[0x30] = (self.eax, self.esi)
        self.modrm32[0x31] = (self.ecx, self.esi)
        self.modrm32[0x32] = (self.edx, self.esi)
        self.modrm32[0x33] = (self.ebx, self.esi)
        self.modrm32[0x34] = None
        self.modrm32[0x35] = None
        self.modrm32[0x36] = (self.esi, self.esi)
        self.modrm32[0x37] = (self.edi, self.esi)

        self.modrm32[0x38] = (self.eax, self.edi)
        self.modrm32[0x39] = (self.ecx, self.edi)
        self.modrm32[0x3a] = (self.edx, self.edi)
        self.modrm32[0x3b] = (self.ebx, self.edi)
        self.modrm32[0x3c] = None
        self.modrm32[0x3d] = None
        self.modrm32[0x3e] = (self.esi, self.edi)
        self.modrm32[0x3f] = (self.edi, self.edi)

        ### 0x40-0x80 special treatment - disp8 
        self.modrm32[0x40] = (self.eax, self.eax)
        self.modrm32[0x41] = (self.ecx, self.eax)
        self.modrm32[0x42] = (self.edx, self.eax)
        self.modrm32[0x43] = (self.ebx, self.eax)
        self.modrm32[0x44] = None
        self.modrm32[0x45] = None
        self.modrm32[0x46] = (self.esi, self.eax)
        self.modrm32[0x47] = (self.edi, self.eax)

        self.modrm32[0x48] = (self.eax, self.ecx)
        self.modrm32[0x49] = (self.ecx, self.ecx)
        self.modrm32[0x4a] = (self.edx, self.ecx)
        self.modrm32[0x4b] = (self.ebx, self.ecx)
        self.modrm32[0x4c] = None
        self.modrm32[0x4d] = None
        self.modrm32[0x4e] = (self.esi, self.ecx)
        self.modrm32[0x4f] = (self.edi, self.ecx)

        self.modrm32[0x50] = (self.eax, self.edx)
        self.modrm32[0x51] = (self.ecx, self.edx)
        self.modrm32[0x52] = (self.edx, self.edx)
        self.modrm32[0x53] = (self.ebx, self.edx)
        self.modrm32[0x54] = None
        self.modrm32[0x55] = None
        self.modrm32[0x56] = (self.esi, self.edx)
        self.modrm32[0x57] = (self.edi, self.edx)

        self.modrm32[0x58] = (self.eax, self.ebx)
        self.modrm32[0x59] = (self.ecx, self.ebx)
        self.modrm32[0x5a] = (self.edx, self.ebx)
        self.modrm32[0x5b] = (self.ebx, self.ebx)
        self.modrm32[0x5c] = None
        self.modrm32[0x5d] = None
        self.modrm32[0x5e] = (self.esi, self.ebx)
        self.modrm32[0x5f] = (self.edi, self.ebx)

        self.modrm32[0x60] = (self.eax, self.esp)
        self.modrm32[0x61] = (self.ecx, self.esp)
        self.modrm32[0x62] = (self.edx, self.esp)
        self.modrm32[0x63] = (self.ebx, self.esp)
        self.modrm32[0x64] = None
        self.modrm32[0x65] = None
        self.modrm32[0x66] = (self.esi, self.esp)
        self.modrm32[0x67] = (self.edi, self.esp)

        self.modrm32[0x68] = (self.eax, self.ebp)
        self.modrm32[0x69] = (self.ecx, self.ebp)
        self.modrm32[0x6a] = (self.edx, self.ebp)
        self.modrm32[0x6b] = (self.ebx, self.ebp)
        self.modrm32[0x6c] = None
        self.modrm32[0x6d] = None
        self.modrm32[0x6e] = (self.esi, self.ebp)
        self.modrm32[0x6f] = (self.edi, self.ebp)

        self.modrm32[0x70] = (self.eax, self.esi)
        self.modrm32[0x71] = (self.ecx, self.esi)
        self.modrm32[0x72] = (self.edx, self.esi)
        self.modrm32[0x73] = (self.ebx, self.esi)
        self.modrm32[0x74] = None
        self.modrm32[0x75] = None
        self.modrm32[0x76] = (self.esi, self.esi)
        self.modrm32[0x77] = (self.edi, self.esi)

        self.modrm32[0x78] = (self.eax, self.edi)
        self.modrm32[0x79] = (self.ecx, self.edi)
        self.modrm32[0x7a] = (self.edx, self.edi)
        self.modrm32[0x7b] = (self.ebx, self.edi)
        self.modrm32[0x7c] = None
        self.modrm32[0x7d] = None
        self.modrm32[0x7e] = (self.esi, self.edi)
        self.modrm32[0x7f] = (self.edi, self.edi)

        ### 0x80-0xc0 special treatment - disp32
        self.modrm32[0x80] = (self.eax, self.eax)
        self.modrm32[0x81] = (self.ecx, self.eax)
        self.modrm32[0x82] = (self.edx, self.eax)
        self.modrm32[0x83] = (self.ebx, self.eax)
        self.modrm32[0x84] = None
        self.modrm32[0x85] = None
        self.modrm32[0x86] = (self.esi, self.eax)
        self.modrm32[0x87] = (self.edi, self.eax)

        self.modrm32[0x88] = (self.eax, self.ecx)
        self.modrm32[0x89] = (self.ecx, self.ecx)
        self.modrm32[0x8a] = (self.edx, self.ecx)
        self.modrm32[0x8b] = (self.ebx, self.ecx)
        self.modrm32[0x8c] = None
        self.modrm32[0x8d] = None
        self.modrm32[0x8e] = (self.esi, self.ecx)
        self.modrm32[0x8f] = (self.edi, self.ecx)

        self.modrm32[0x90] = (self.eax, self.edx)
        self.modrm32[0x91] = (self.ecx, self.edx)
        self.modrm32[0x92] = (self.edx, self.edx)
        self.modrm32[0x93] = (self.ebx, self.edx)
        self.modrm32[0x94] = None
        self.modrm32[0x95] = None
        self.modrm32[0x96] = (self.esi, self.edx)
        self.modrm32[0x97] = (self.edi, self.edx)

        self.modrm32[0x98] = (self.eax, self.ebx)
        self.modrm32[0x99] = (self.ecx, self.ebx)
        self.modrm32[0x9a] = (self.edx, self.ebx)
        self.modrm32[0x9b] = (self.ebx, self.ebx)
        self.modrm32[0x9c] = None
        self.modrm32[0x9d] = None
        self.modrm32[0x9e] = (self.esi, self.ebx)
        self.modrm32[0x9f] = (self.edi, self.ebx)

        self.modrm32[0xa0] = (self.eax, self.esp)
        self.modrm32[0xa1] = (self.ecx, self.esp)
        self.modrm32[0xa2] = (self.edx, self.esp)
        self.modrm32[0xa3] = (self.ebx, self.esp)
        self.modrm32[0xa4] = None
        self.modrm32[0xa5] = None
        self.modrm32[0xa6] = (self.esi, self.esp)
        self.modrm32[0xa7] = (self.edi, self.esp)

        self.modrm32[0xa8] = (self.eax, self.ebp)
        self.modrm32[0xa9] = (self.ecx, self.ebp)
        self.modrm32[0xaa] = (self.edx, self.ebp)
        self.modrm32[0xab] = (self.ebx, self.ebp)
        self.modrm32[0xac] = None
        self.modrm32[0xad] = None
        self.modrm32[0xae] = (self.esi, self.ebp)
        self.modrm32[0xaf] = (self.edi, self.ebp)

        self.modrm32[0xb0] = (self.eax, self.esi)
        self.modrm32[0xb1] = (self.ecx, self.esi)
        self.modrm32[0xb2] = (self.edx, self.esi)
        self.modrm32[0xb3] = (self.ebx, self.esi)
        self.modrm32[0xb4] = None
        self.modrm32[0xb5] = None
        self.modrm32[0xb6] = (self.esi, self.esi)
        self.modrm32[0xb7] = (self.edi, self.esi)

        self.modrm32[0xb8] = (self.eax, self.edi)
        self.modrm32[0xb9] = (self.ecx, self.edi)
        self.modrm32[0xba] = (self.edx, self.edi)
        self.modrm32[0xbb] = (self.ebx, self.edi)
        self.modrm32[0xbc] = None
        self.modrm32[0xbd] = None
        self.modrm32[0xbe] = (self.esi, self.edi)
        self.modrm32[0xbf] = (self.edi, self.edi)

        ### 0xc0-0xf0 special treatment - direct acces, no mem
        self.modrm32[0xc0] = (self.eax, self.eax)
        self.modrm32[0xc1] = (self.ecx, self.eax)
        self.modrm32[0xc2] = (self.edx, self.eax)
        self.modrm32[0xc3] = (self.ebx, self.eax)
        self.modrm32[0xc4] = None
        self.modrm32[0xc5] = None
        self.modrm32[0xc6] = (self.esi, self.eax)
        self.modrm32[0xc7] = (self.edi, self.eax)

        self.modrm32[0xc8] = (self.eax, self.ecx)
        self.modrm32[0xc9] = (self.ecx, self.ecx)
        self.modrm32[0xca] = (self.edx, self.ecx)
        self.modrm32[0xcb] = (self.ebx, self.ecx)
        self.modrm32[0xcc] = None
        self.modrm32[0xcd] = None
        self.modrm32[0xce] = (self.esi, self.ecx)
        self.modrm32[0xcf] = (self.edi, self.ecx)

        self.modrm32[0xd0] = (self.eax, self.edx)
        self.modrm32[0xd1] = (self.ecx, self.edx)
        self.modrm32[0xd2] = (self.edx, self.edx)
        self.modrm32[0xd3] = (self.ebx, self.edx)
        self.modrm32[0xd4] = None
        self.modrm32[0xd5] = None
        self.modrm32[0xd6] = (self.esi, self.edx)
        self.modrm32[0xd7] = (self.edi, self.edx)

        self.modrm32[0xd8] = (self.eax, self.ebx)
        self.modrm32[0xd9] = (self.ecx, self.ebx)
        self.modrm32[0xda] = (self.edx, self.ebx)
        self.modrm32[0xdb] = (self.ebx, self.ebx)
        self.modrm32[0xdc] = None
        self.modrm32[0xdd] = None
        self.modrm32[0xde] = (self.esi, self.ebx)
        self.modrm32[0xdf] = (self.edi, self.ebx)

        self.modrm32[0xe0] = (self.eax, self.esp)
        self.modrm32[0xe1] = (self.ecx, self.esp)
        self.modrm32[0xe2] = (self.edx, self.esp)
        self.modrm32[0xe3] = (self.ebx, self.esp)
        self.modrm32[0xe4] = None
        self.modrm32[0xe5] = None
        self.modrm32[0xe6] = (self.esi, self.esp)
        self.modrm32[0xe7] = (self.edi, self.esp)

        self.modrm32[0xe8] = (self.eax, self.ebp)
        self.modrm32[0xe9] = (self.ecx, self.ebp)
        self.modrm32[0xea] = (self.edx, self.ebp)
        self.modrm32[0xeb] = (self.ebx, self.ebp)
        self.modrm32[0xec] = None
        self.modrm32[0xed] = None
        self.modrm32[0xee] = (self.esi, self.ebp)
        self.modrm32[0xef] = (self.edi, self.ebp)

        self.modrm32[0xf0] = (self.eax, self.esi)
        self.modrm32[0xf1] = (self.ecx, self.esi)
        self.modrm32[0xf2] = (self.edx, self.esi)
        self.modrm32[0xf3] = (self.ebx, self.esi)
        self.modrm32[0xf4] = None
        self.modrm32[0xf5] = None
        self.modrm32[0xf6] = (self.esi, self.esi)
        self.modrm32[0xf7] = (self.edi, self.esi)

        self.modrm32[0xf8] = (self.eax, self.edi)
        self.modrm32[0xf9] = (self.ecx, self.edi)
        self.modrm32[0xfa] = (self.edx, self.edi)
        self.modrm32[0xfb] = (self.ebx, self.edi)
        self.modrm32[0xfc] = None
        self.modrm32[0xfd] = None
        self.modrm32[0xfe] = (self.esi, self.edi)
        self.modrm32[0xff] = (self.edi, self.edi)



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

    ### decode operands

    # decoding modrm byte in 32bit addressing mode
    # returns object if register and int if memory address
    def decode_rm_r_32(arg):
        modrm_byte = arg[0]
        rm, r = self.modrm32[modrm_byte]
        else:
            if(0x00 <= arg[0] < 0x40):
                if(rm == None):
                    # SIB
                # do disp
            if(0x40 <= arg[0] < 0x80):
                if(rm == None):
                    # SIB
                # disp8
            if(0x80 <= arg[0] < 0xc0):
                if(rm == None):
                    # SIB
                # disp32
            if(0xc0 <= arg[0] < 0x100):
                if(rm == None):
                    # SIB
                # direct reg access
            return rm, r

    def decode_rm_r_16(arg):
        pass

    def decode_r_rm_32(arg):
        modrm_byte = arg[0]
        rm, r = self.modrm32[modrm_byte]
        if(rm == None):
            # SIB
        else:
            return r, rm

    def decode_rm_32(arg):
        pass

    def decode_r_32(arg):
        pass

    def decode_r_rm_16(arg):
        pass

    def decode_r_rm_16(arg):
        pass

    def decode_rm_16(arg):
        pass

    def decode_r_16(arg):
        pass

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

    # 0x31
    def e_xor_r(arg):
        src, tgt = self.decode_rm_r_32(arg[0])
        if(type(tgt) == int):
            tgt_val = restore(tgt, 0x4)
            self.store(tgt, 0x4, tgt_val ^ src)
        else:
            tgt = src ^ tgt

    # 0x34
    def e_xor_ib(arg):
        self.al = self.al ^ struct.unpack('b', arg)

    def e_xor_ebp_ebp(arg = None):
        self.ebp = 0x0

    # mov family

    def e_mov(arg):
        opcode, arg = pare_instr(arg)
        # check if arg None?
        self.r_mov[opcode](arg)

    def e_mov_r_rm_32(arg = None):
        src, tgt = self.decode_r_rm_32(arg[0])
        if(type(tgt) == int):
            self.store(tgt, 0x4, src)
        else:
            tgt = src

    def e_mov_rdx_rsp(arg = None):
        self.rdx = self.rsp

    def e_mov_r9_rdx(arg = None):
        self.r9 = self.rdx

    # push family

    # auxiliary
    def push32(arg):
        self.esp = self.esp - 0x8
        self.store(self.esp, 0x4, arg)

    # 0x50
    def e_push_eax(arg = None):
        self.push32(self.eax)

    # 0x51
    def e_push_ecx(arg = None):
        self.push32(self.ecx)

    # 0x52
    def e_push_edx(arg = None):
        self.push32(self.edx)

    # 0x53
    def e_push_ebx(arg = None):
        self.push32(self.ebx)

    # 0x54
    def e_push_esp(arg = None):
        self.push32(self.esp)

    # 0x55
    def e_push_ebp(arg = None):
        self.push32(self.ebp)

    # pop family

    def e_pop(arg = None):
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

