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
int taint_x86::store_32(DWORD off, char* val)
{
    this->memory[off + 0] = val[3];
    this->memory[off + 1] = val[2];
    this->memory[off + 2] = val[1];
    this->memory[off + 3] = val[0];

    return 0;
}

int taint_x86::restore_32(DWORD off, char* val)
{

    val[3] = this->memory[off + 0];
    val[2] = this->memory[off + 1];
    val[1] = this->memory[off + 2];
    val[0] = this->memory[off + 3];

    return 0;
}
*/
/*
DWORD taint_x86::le2dword(char* le)
{
    DWORD ret;
    char* val;
    val = (char*)&ret;

    val[3] = le[0];
    val[2] = le[1];
    val[1] = le[2];
    val[0] = le[3];

    return ret;

}
*/
void taint_x86::store_32(OFFSET off, DWORD_t v)
{
    this->memory[off + 0] = v[3];
    this->memory[off + 1] = v[2];
    this->memory[off + 2] = v[1];
    this->memory[off + 3] = v[0];
    return;
}

DWORD_t taint_x86::restore_32(OFFSET off)
{
    DWORD_t ret;

    ret[0] = this->memory[off + 3];
    ret[1] = this->memory[off + 2];
    ret[2] = this->memory[off + 1];
    ret[3] = this->memory[off + 0];
    return ret;
}

void taint_x86::store_32(DWORD_t off, DWORD_t v)
{
    store_32(off.val, v);
    return;
}

DWORD_t taint_x86::restore_32(DWORD_t off)
{
    return restore_32(off.val);
}

void taint_x86::reg_store_32(OFFSET off, DWORD_t v)
{
    this->memory[off + 0] = v[0];
    this->memory[off + 1] = v[1];
    this->memory[off + 2] = v[2];
    this->memory[off + 3] = v[3];
    
/*
    char* val;
    char* val_t;
    val = (char*)&v.val;
    val = (char*)&v.val_t;

    this->registers[off + 0].val = val[3];
    this->registers[off + 1].val = val[2];
    this->registers[off + 2].val = val[1];
    this->registers[off + 3].val = val[0];

    this->registers[off + 0].val_t = val_t[3];
    this->registers[off + 1].val_t = val_t[2];
    this->registers[off + 2].val_t = val_t[1];
    this->registers[off + 3].val_t = val_t[0];
*/
    return;
}

DWORD_t taint_x86::reg_restore_32(OFFSET off)
{
    DWORD_t ret;

    ret[0] = this->memory[off + 0];
    ret[1] = this->memory[off + 1];
    ret[2] = this->memory[off + 2];
    ret[3] = this->memory[off + 3];

/*
    char* val;
    char* val_t;
    val = (char*)&ret;
    vali_t = (char*)&ret_t;

    val[3] = this->registers[off + 0].val;
    val[2] = this->registers[off + 1].val;
    val[1] = this->registers[off + 2].val;
    val[0] = this->registers[off + 3].val;

    val_t[3] = this->registers[off + 0].val_t;
    val_t[2] = this->registers[off + 1].val_t;
    val_t[1] = this->registers[off + 2].val_t;
    val_t[0] = this->registers[off + 3].val_t;
*/
    return ret;
}
/*
void taint_x86::reg_store_32(int off, DWORD v)
{
    char* val;
    val = (char*)&v;

    this->registers[off + 0] = val[3];
    this->registers[off + 1] = val[2];
    this->registers[off + 2] = val[1];
    this->registers[off + 3] = val[0];

    return;
}

DWORD taint_x86::reg_restore_32(int off)
{
    DWORD ret;
    char* val;
    val = (char*)&ret;

    val[3] = this->registers[off + 0];
    val[2] = this->registers[off + 1];
    val[1] = this->registers[off + 2];
    val[0] = this->registers[off + 3];

    return ret;
}
*/
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

int taint_x86::print_bt_buffer(BYTE_t* buf, int scope)
{
    for(int i=0; i< scope; i+=sizeof(BYTE_t))
    {
        printf("0x%02x ", buf[i].val);
        if((i+1) % 0x10 == 0x0)
            printf("\n");
    }
    printf("\n");
}

int taint_x86::print_all_regs()
{
    return this->print_bt_buffer(registers, REG_SIZE);
}

//instructions routines

DWORD_t taint_x86::a_pop_32()
{
    DWORD_t esp;
    DWORD_t val;

    // update ESP 
    esp = reg_restore_32(ESP);

    // store value at stack
    val = restore_32(esp);

    // 
    esp += 0x4;
    //esp.val_t remains

    reg_store_32(ESP, esp);

    return val;
}

int taint_x86::a_push_32(DWORD_t val)
{
    DWORD_t esp;
   
    // update ESP 
    esp = reg_restore_32(ESP);
    esp -= 0x4;
    //esp.val_t remains

    // store value at stack
    store_32(esp, val);

    reg_store_32(ESP, esp);

    return 0x0;
}

int taint_x86::r_pop_eax_32(char* args)
{
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(EAX, val);
    return 0x0;
}

int taint_x86::r_pop_ebx_32(char* args)
{
    printf("here\n");
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(EBX, val);
    return 0x0;
}

int taint_x86::r_push_eax_32(char* args)
{
    printf("Properly\n");
    return a_push_32(reg_restore_32(EAX));
}

int taint_x86::print_context()
{
    printf("Current context:\n\n");
    printf("EAX: 0x%08x\n", this->reg_restore_32(EAX));
    printf("ECX: 0x%08x\n", this->reg_restore_32(ECX));
    printf("EDX: 0x%08x\n", this->reg_restore_32(EDX));
    printf("EBX: 0x%08x\n", this->reg_restore_32(EBX));
    printf("ESI: 0x%08x\n", this->reg_restore_32(ESI));
    printf("EDI: 0x%08x\n", this->reg_restore_32(EDI));
    printf("EBP: 0x%08x\n", this->reg_restore_32(EBP));
    printf("ESP: 0x%08x\n", this->reg_restore_32(ESP));
    printf("EIP: 0x%08x\n", this->reg_restore_32(EIP));
    printf("\n");
}
