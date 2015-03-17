#include <stdio.h>

#define EAX 0x00
#define EBX 0x04
#define ECX 0x08
#define EDX 0x0c
#define ESI 0x10
#define EDI 0x14
#define EBP 0x18
#define ESP 0x1c
#define EFLAGS 0x20
#define EIP 0x24

//add 16 bit, 8bit offsets

#define MEM_SIZE 0x100000000
#define REG_SIZE 0x750

#define DWORD int

//typedef int (taint_x86::* instruction_routine)(char* args);

class taint_x86
{
    char memory[MEM_SIZE];
    char registers[REG_SIZE];

/*
    // 32bit
    char* eax;
    char* ecx;
    char* edx;
    char* ebx;
    char* esi;
    char* edi
    char* esp
    char* ebp
    char* eflags
    char* eip
    
    // 16bit
    char* ax;
    char* cx;
    char* dx;
    char* bx;
    char* si;
    char* di
    char* sp
    char* bp
    char* flags
    char* ip
    
    // 8bit
    char* ah;
    char* ch;
    char* dh;
    char* bh;
    char* al;
    char* cl;
    char* dl;
    char* bl;
    */
//    typedef int (taint_x86::*instruction_routine)(char* args);
    typedef int (taint_x86::*instruction_routine)(char* args);
    instruction_routine instructions_32[0x100];

    public:

    // instructions
    int r_push_eax_32(char*);
    int a_push_32(char*);

//    int store_16(DWORD, char*);
//    int restore_16(DWORD, char**);
    int store_32(DWORD, char*);
    int restore_32(DWORD, char**);

    int execute_instruction(char*);
    int print_context();

    taint_x86()
    {
        printf("Initializing taint emulator\n");
    
        this->instructions_32[0x50] = &taint_x86::r_push_eax_32;
//        this->instructions_32[0x50] = taint_x86::r_push_eax_32;
//        this->instructions_32[0x51] = r_push_ecx_32;
//        this->instructions_32[0x52] = r_push_edx_32;
//        this->instructions_32[0x53] = r_push_ebx_32;

        this->registers[ESP] = 0x1000;
    }
    ~taint_x86() {}



};


