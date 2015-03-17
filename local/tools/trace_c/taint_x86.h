#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EAX     0x00
#define EBX     0x04
#define ECX     0x08
#define EDX     0x0c
#define ESI     0x10
#define EDI     0x14
#define EBP     0x18
#define ESP     0x1c
#define EFLAGS  0x20
#define EIP     0x24

//add 16 bit, 8bit offsets

#define MEM_SIZE 0x100000000
#define REG_SIZE 0x750

#define DWORD int

//typedef int (taint_x86::* instruction_routine)(char* args);

class taint_x86
{
//    char memory[MEM_SIZE];
    char* memory;
    char registers[REG_SIZE];

//    typedef int (taint_x86::*instruction_routine)(char* args);
    typedef int (taint_x86::*instruction_routine)(char* args);
    instruction_routine instructions_32[0x100];

    public:

    // instructions
    int r_pop_eax_32(char*);
    int r_pop_ebx_32(char*);
    int r_push_eax_32(char*);

    // auxilliary
    int a_push_32(DWORD);
    DWORD a_pop_32();

//    int store_16(DWORD, char*);
//    int restore_16(DWORD, char**);
    DWORD le2dword(char*);

    void store_32(DWORD, DWORD);
    DWORD restore_32(DWORD);

    void reg_store_32(DWORD, DWORD);
    DWORD reg_restore_32(DWORD);

    int execute_instruction(char*);
    int print_context();
    int print_buffer(char*, DWORD);
    int print_all_regs();

    taint_x86()
    {
        printf("Initializing taint emulator\n");
    
        this->instructions_32[0x50] = &taint_x86::r_push_eax_32;
        this->instructions_32[0x58] = &taint_x86::r_pop_eax_32;
        this->instructions_32[0x5a] = &taint_x86::r_pop_ebx_32;
//        this->instructions_32[0x50] = taint_x86::r_push_eax_32;
//        this->instructions_32[0x51] = r_push_ecx_32;
//        this->instructions_32[0x52] = r_push_edx_32;
//        this->instructions_32[0x53] = r_push_ebx_32;

        //this->reg_store_32(EAX, "\x11\x22\x33\x44");
//        this->reg_store_32(ESP, "\x00\x00\x11\x00");
        this->memory = (char*)malloc(MEM_SIZE);

        this->reg_store_32(EAX, 0x44332211);
        this->reg_store_32(EBX, 0x55332211);
        this->reg_store_32(ESP, 0x00001100);

    }

    ~taint_x86() {
        free(this->memory);
    }

};


