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
    printf("storing %x at %x\n", v[3], off);
    this->memory[off + 0] = v[3];
    printf("storing %x at %x\n", v[2], off +1);
    this->memory[off + 1] = v[2];
    printf("storing %x at %x\n", v[1], off +2);
    this->memory[off + 2] = v[1];
    printf("storing %x at %x\n", v[0], off +3);
    this->memory[off + 3] = v[0];

    DWORD_t ret;
    ret[0] = this->memory[off + 3];
    printf("restoring %x from %x\n", ret[0], off +3);
    printf("restoring %x from %x\n", this->memory[off+3], off +3);

    return;
}

DWORD_t taint_x86::restore_32(OFFSET off)
{
    DWORD_t ret;

    ret[0] = this->memory[off + 3];
    printf("restoring %x from %x\n", ret[0], off +3);
    ret[1] = this->memory[off + 2];
    printf("restoring %x from %x\n", ret[1], off +2);
    ret[2] = this->memory[off + 1];
    printf("restoring %x from %x\n", ret[2], off +1);
    ret[3] = this->memory[off + 0];
    printf("restoring %x from %x\n", ret[3], off +0);

    return ret;
}

void taint_x86::store_32(DWORD_t off, DWORD_t v)
{
    store_32(off.to_DWORD(), v);
    return;
}

DWORD_t taint_x86::restore_32(DWORD_t off)
{
    return restore_32(off.to_DWORD());
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
    unsigned char current_byte;
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
    DWORD_t val;

    // store value at stack
    val = restore_32(this->esp);
    printf("esp read: 0x%x val: 0x%x\n", this->esp.to_DWORD(), val.to_DWORD());
    printf("here2 0x%x\n", val);

    // update ESP 
    this->esp += 0x4;

    return val;
}

int taint_x86::a_push_32(DWORD_t val)
{
    // update ESP 
    printf("before: 0x%08x\n", this->esp.to_DWORD());
    this->esp -= 0x4;
    printf("after: 0x%08x\n", this->esp.to_DWORD());


    // store value at stack
    printf("esp write: 0x%x val: 0x%x\n", this->esp.to_DWORD(), val.to_DWORD());
    store_32(this->esp, val);

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
    printf("val: %x\n", val);
    this->ebx = val;
    return 0x0;
}

int taint_x86::r_push_eax_32(char* args)
{
    return a_push_32(this->eax);
}

int taint_x86::decode_modrm_byte(char* args, DWORD_t* src, DWORD_t* tgt)
{
    modrm_op_ready_32 ops;

    printf("modrm byte: 0x%x\n", args[0]);

    ops = this->modrm_table_32[args[0]];
    if(ops.src != 0x0 && ops.tgt != 0x0)
    {
        printf("found match\n");
        src = ops.src;
        tgt = ops.tgt;
        printf("this->eax: 0x%p\n", &this->eax);
        printf("found src: 0x%p\n", src);
        return 0x0;
    }
        
    return 1;
}

int taint_x86::r_mov_rm_r_32(char* args)
{
    char* modrm_byte;
    DWORD_t* op1;
    DWORD_t* op2;

    modrm_byte = args;

    decode_modrm_byte(modrm_byte, op1, op2);
    printf("decoded: rm: %x, r: %x\n", op1->val, op2->val);

//    this->memory[op1->to_DWORD()] = *op2;
    printf("op1 content: %x\n", *op1);
    printf("op2 content: %x\n", *op2);
    *op1 = this->restore_32(*op2);

    return 0x0;
}

int taint_x86::r_mov_r_rm_32(char* args)
{
    char* modrm_byte;
    DWORD_t* op1;
    DWORD_t* op2;

    modrm_byte = args;

    decode_modrm_byte(modrm_byte, op1, op2);
    printf("op1 content: %x\n", op1->val);
    printf("op2 content: %x\n", op2->val);
    printf("decoded: r: %x, rm: %x\n", op1->val, op2->val);

//    *op1 = this->memory[op2->to_DWORD()];
    this->store_32(*op1, *op2);

    return 0x0;
}

int taint_x86::print_context()
{
    printf("Current context:\n\n");
    printf("EAX: 0x%08x\n", this->eax.to_DWORD());
    printf("ECX: 0x%08x\n", this->ecx.to_DWORD());
    printf("EDX: 0x%08x\n", this->edx.to_DWORD());
    printf("EBX: 0x%08x\n", this->ebx.to_DWORD());
    printf("ESI: 0x%08x\n", this->esi.to_DWORD());
    printf("EDI: 0x%08x\n", this->edi.to_DWORD());
    printf("EBP: 0x%08x\n", this->ebp.to_DWORD());
    printf("ESP: 0x%08x\n", this->esp.to_DWORD());
    printf("EIP: 0x%08x\n", this->eip.to_DWORD());
    printf("\n");
}

int taint_x86::print_t_context()
{
    printf("Current taint context:\n\n");
    printf("EAX: 0x%08x\n", this->eax.to_t_DWORD());
    printf("ECX: 0x%08x\n", this->ecx.to_t_DWORD());
    printf("EDX: 0x%08x\n", this->edx.to_t_DWORD());
    printf("EBX: 0x%08x\n", this->ebx.to_t_DWORD());
    printf("ESI: 0x%08x\n", this->esi.to_t_DWORD());
    printf("EDI: 0x%08x\n", this->edi.to_t_DWORD());
    printf("EBP: 0x%08x\n", this->ebp.to_t_DWORD());
    printf("ESP: 0x%08x\n", this->esp.to_t_DWORD());
    printf("EIP: 0x%08x\n", this->eip.to_t_DWORD());
    printf("\n");
}
