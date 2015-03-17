#include <taint_x86.h>
#include <stdio.h>

/*taint_x86::taint_x86()
{
    printf("Initializing taint emulator\n");
    
    instructions_32[0x50] = this->r_push_eax_32;
    instructions_32[0x51] = this->r_push_ecx_32;
    instructions_32[0x52] = this->r_push_edx_32;
    instructions_32[0x53] = this->r_push_ebx_32;
}


taint_x86::~taint_x86()
{
}
*/
/*int taint_x86::store_16(DWORD addr, DWORD val)
{
    //lil endian
    this->memory[addr + 0] = val & 0x00ff;
    this->memory[addr + 1] = val & 0xff00;

    return 0;
}

int taint_x86::restore_16(DWORD addr, DWORD* val)
{
    //lil endian
    DWORD a;

    a = 0;
    a += this->memory[addr + 1] << 0x00;
    a += this->memory[addr + 0] << 0x08;

    *val = a;

    return 0;
}
*/
int taint_x86::store_32(DWORD v_addr, char* val)
{
    //lil endian
/*    this->memory[addr + 0] = val & 0x000000ff;
    this->memory[addr + 1] = val & 0x0000ff00;
    this->memory[addr + 2] = val & 0x00ff0000;
    this->memory[addr + 3] = val & 0xff000000;
*/

    this->memory[v_addr + 0] = val[3];
    this->memory[v_addr + 1] = val[2];
    this->memory[v_addr + 2] = val[1];
    this->memory[v_addr + 3] = val[0];

    return 0;
}

int taint_x86::restore_32(DWORD addr, char** val)
{
    //lil endian
    /*DWORD a;

    a = 0;
    a += this->memory[addr + 3] << 0x00;
    a += this->memory[addr + 2] << 0x08;
    a += this->memory[addr + 1] << 0x10;
    a += this->memory[addr + 0] << 0x18;

    *val = a;*/
    char* val2;
    val2 = *val;

    val2[3] = this->memory[addr + 0];
    val2[2] = this->memory[addr + 1];
    val2[1] = this->memory[addr + 2];
    val2[0] = this->memory[addr + 3];

    return 0;
}

// is it necessary?
/*
DWORD taint_x86::dword(char* off)
{
    DWORD a;

    a = 0;
    a += this->registers[addr + 0] << 0x00;
    a += this->registers[addr + 1] << 0x08;
    a += this->registers[addr + 2] << 0x10;
    a += this->registers[addr + 3] << 0x18;

    *val = a;

    return a;
}
*/

int taint_x86::execute_instruction(char* instr)
{
    printf("Executing\n");
    char current_byte;
    char* args;

    printf("Executing\n");
    current_byte = instr[0];
    // check for [prefixes]
    args = instr + 1;
    printf("Executing\n");

    printf("Executing opcode: 0x%x\n", int(current_byte));

    return (this->*(instructions_32[current_byte]))(args);
}

int taint_x86::a_push_32(char* args)
{
    printf("Properly\n");
    DWORD esp;
   
    // update ESP 
    esp = DWORD(this->registers[ESP]);
    printf("ESP: 0x%x\n", esp);
    esp -= 0x4;

    // store value at stack
    store_32(esp, args);
    printf("Executing\n");
    this->registers[ESP] = esp;
    printf("Executing\n");

    return 0x0;
}

int taint_x86::r_push_eax_32(char* args)
{
    printf("Properly\n");
    return a_push_32(args);
}

int taint_x86::print_context()
{
    printf("Current context:\n\n");
    printf("EAX: 0x%08x\n", this->registers[EAX]);
    printf("ECX: 0x%08x\n", this->registers[ECX]);
    printf("EDX: 0x%08x\n", this->registers[EDX]);
    printf("EBX: 0x%08x\n", this->registers[EBX]);
    printf("ESI: 0x%08x\n", this->registers[ESI]);
    printf("EDI: 0x%08x\n", this->registers[EDI]);
    printf("EBP: 0x%08x\n", this->registers[EBP]);
    printf("ESP: 0x%08x\n", this->registers[ESP]);
    printf("EIP: 0x%08x\n", this->registers[EIP]);
    printf("\n");
}
