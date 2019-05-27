#include <taint_x86.h>
#include <stdio.h>
#include "debug.h"
#include <iniparser.h>

#define MEM_ALLOC_DECLARED_SIZE

/*
* store / restore
*/

/* 32 bit */

void taint_x86::store_32(OFFSET off, DWORD_t v)
{
#ifdef REVERSE_ANALYSIS
    DWORD_t lost_val;
    this->restore_32(off, lost_val);
    this->a_push_lost_32(lost_val.get_DWORD());
#endif

    v.to_mem(&this->memory[off], 1);
}

void taint_x86::restore_32(OFFSET off, DWORD_t& ret)
{
    ret.from_mem(&this->memory[off], 1);
}

void taint_x86::store_32(DWORD_t off, DWORD_t v)
{
    store_32(off.get_DWORD(), v);
    return;
}

void taint_x86::restore_32(DWORD_t off, DWORD_t& ret)
{
    restore_32(off.get_DWORD(), ret);
}

/* reg */

void taint_x86::reg_store_32(OFFSET off, DWORD_t v, int tid)
{
#ifdef REVERSE_ANALYSIS
    DWORD_t lost_val;
    lost_val = this->reg_restore_32(off);
    this->a_push_lost_32(lost_val.get_DWORD());
#endif

    CONTEXT_INFO* info;
    info = this->get_tid(tid);

    info->registers[off + 0] = v[0];
    info->registers[off + 1] = v[1];
    info->registers[off + 2] = v[2];
    info->registers[off + 3] = v[3];

    return;
}

DWORD_t taint_x86::reg_restore_32(OFFSET off, int tid)
{
    CONTEXT_INFO* info;
    info = this->get_tid(tid);

    DWORD_t ret;

    ret[0] = info->registers[off + 0];
    ret[1] = info->registers[off + 1];
    ret[2] = info->registers[off + 2];
    ret[3] = info->registers[off + 3];

    return ret;
}

/* reg proxies */

void taint_x86::reg_store_32(OFFSET off, DWORD_t v)
{
    return this->reg_store_32(off, v, this->cur_tid);
}

DWORD_t taint_x86::reg_restore_32(OFFSET off)
{
    return this->reg_restore_32(off, this->cur_tid);
}

void taint_x86::reg_store_32(DWORD_t off, DWORD_t v, int tid)
{
    reg_store_32(off.get_DWORD(), v, tid);
    return;
}

DWORD_t taint_x86::reg_restore_32(DWORD_t off, int tid)
{
    return reg_restore_32(off.get_DWORD(), tid);
}

void taint_x86::reg_store_32(DWORD_t off, DWORD_t v)
{
    reg_store_32(off.get_DWORD(), v, this->cur_tid);
    return;
}

DWORD_t taint_x86::reg_restore_32(DWORD_t off)
{
    return reg_restore_32(off.get_DWORD(), this->cur_tid);
}

/* 16 bit stores & restores */

void taint_x86::store_16(OFFSET off, WORD_t v)
{
#ifdef REVERSE_ANALYSIS
    WORD_t lost_val;
    this->restore_16(off, lost_val);
    this->a_push_lost_16(lost_val.get_WORD());
#endif
    v.to_mem(&this->memory[off], 1);
}

void taint_x86::restore_16(OFFSET off, WORD_t& ret)
{
    ret.from_mem(&this->memory[off], 1);
}

void taint_x86::store_16(DWORD_t off, WORD_t v)
{
    store_16(off.get_DWORD(), v);
    return;
}

void taint_x86::restore_16(DWORD_t off, WORD_t& ret)
{
    restore_16(off.get_DWORD(), ret);
}

/* reg */

void taint_x86::reg_store_16(OFFSET off, WORD_t v, int tid)
{
#ifdef REVERSE_ANALYSIS
    WORD_t lost_val;
    lost_val = this->reg_restore_16(off);
    this->a_push_lost_16(lost_val.get_WORD());
#endif
    CONTEXT_INFO* info;
    info = this->get_tid(tid);

    info->registers[off + 0] = v[0];
    info->registers[off + 1] = v[1];

    return;
}

WORD_t taint_x86::reg_restore_16(OFFSET off, int tid)
{
    CONTEXT_INFO* info;
    info = this->get_tid(tid);

    WORD_t ret;

    ret[0] = info->registers[off + 0];
    ret[1] = info->registers[off + 1];

    return ret;
}

/* proxies */

void taint_x86::reg_store_16(OFFSET off, WORD_t v)
{
    this->reg_store_16(off, v, this->cur_tid);
}

WORD_t taint_x86::reg_restore_16(OFFSET off)
{
    this->reg_restore_16(off, this->cur_tid);
}

void taint_x86::reg_store_16(DWORD_t off, WORD_t v, int tid)
{
    reg_store_16(off.get_DWORD(), v, tid);
    return;
}

WORD_t taint_x86::reg_restore_16(DWORD_t off, int tid)
{
    return reg_restore_16(off.get_DWORD(), tid);
}

void taint_x86::reg_store_16(DWORD_t off, WORD_t v)
{
    reg_store_16(off.get_DWORD(), v, this->cur_tid);
    return;
}

WORD_t taint_x86::reg_restore_16(DWORD_t off)
{
    return reg_restore_16(off.get_DWORD(), this->cur_tid);
}

/* 8 */

void taint_x86::store_8(OFFSET off, BYTE_t v)
{
    v.to_mem(&this->memory[off]);
}

void taint_x86::restore_8(OFFSET off, BYTE_t& ret)
{
    ret.from_mem(&this->memory[off]);
}

void taint_x86::store_8(DWORD_t off, BYTE_t v)
{
    store_8(off.get_DWORD(), v);
    return;
}

void taint_x86::restore_8(DWORD_t off, BYTE_t& ret)
{
    restore_8(off.get_DWORD(), ret);
}

/* reg */

void taint_x86::reg_store_8(OFFSET off, BYTE_t v, int tid)
{
    CONTEXT_INFO* info;
    info = this->get_tid(tid);
    

    info->registers[off] = v;
    return;
}

BYTE_t taint_x86::reg_restore_8(OFFSET off, int tid)
{
    CONTEXT_INFO* info;
    info = this->get_tid(tid);

    BYTE_t ret;

    ret = info->registers[off];

    return ret;
}

void taint_x86::reg_store_8(OFFSET off, BYTE_t v)
{
    return this->reg_store_8(off, v, this->cur_tid);
}

BYTE_t taint_x86::reg_restore_8(OFFSET off)
{
    return this->reg_restore_8(off, this->cur_tid);
}

void taint_x86::reg_store_8(DWORD_t off, BYTE_t v)
{
    return this->reg_store_8(off, v, this->cur_tid);
}

void taint_x86::reg_store_8(DWORD_t off, BYTE_t v, int tid)
{
    reg_store_8(off.get_DWORD(), v, tid);
    return;
}

BYTE_t taint_x86::reg_restore_8(DWORD_t off)
{
    return this->reg_restore_8(off, this->cur_tid);
}

BYTE_t taint_x86::reg_restore_8(DWORD_t off, int tid)
{
    return reg_restore_8(off.get_DWORD(), tid);
}


/*
*   main routines
*/

int taint_x86::pre_execute_instruction(DWORD eip)
{
    this->current_prefixes = 0x0;
    // skip zeroing byte for speed

    this->cur_info = this->get_tid(this->cur_tid);
    d_print(4, "Current info: 0x%08x\n", this->cur_info->tid);
    d_print(4, "Current EIP: 0x%08x\n", this->cur_info->registers[EIP]);
    d_print(4, "Current EIP: 0x%08x\n", this->cur_info->registers[EIP+1]);
    d_print(4, "Current EIP: 0x%08x\n", this->cur_info->registers[EIP+2]);
    d_print(4, "Current EIP: 0x%08x\n", this->cur_info->registers[EIP+3]);
    
    reg_store_32(EIP, DWORD_t(eip));
    this->current_instr_length = 0x0;
    return 0x0;
}

int taint_x86::post_execute_instruction(DWORD eip)
{
    this->extended = 0x0;
    return 0x0;
}

int taint_x86::execute_instruction_at_eip(DWORD eip, DWORD tid)
{
    int ret = 0x0;

    this->cur_tid = tid;

    /* debug */
    for(int i = 0x0; i< 0x10; i++)
        if(this->my_bps[i] == eip)
        {
            printf("Breakpoint at 0x%x\n", eip);
            this->bp_hit = 0x1;
            getchar();
        }
            

    this->pre_execute_instruction(eip);

    this->current_instr_byte = &this->memory[eip];
    this->current_instr_byte->val_t = 0xff; // taint executed?
    this->current_instr_length += 1;

    // process prefixes
    while(a_is_prefix(this->current_instr_byte))
    {
        ret = (this->*(instructions_32[current_instr_byte->val]))(this->current_instr_byte);
        this->current_instr_byte = &this->memory[eip + 1];
        this->current_instr_byte->val_t = 0xff; // taint executed?
        this->current_instr_length += 1;
    }

    d_print(2, "Executing opcode: 0x%02x\n", int(current_instr_byte->val));
    if(this->extended)
    {
        d_print(3, "Extended\n");
        ret = (this->*(instructions_32_extended[current_instr_byte->val]))(current_instr_byte);
    }
    else
    {
        ret = (this->*(instructions_32[current_instr_byte->val]))(current_instr_byte);
    }

    this->post_execute_instruction(eip);

    return ret;
}

int taint_x86::execute_instruction_at_eip(DWORD eip)
{
    return this->execute_instruction_at_eip(eip, this->tids[0]);
}

CONTEXT_INFO* taint_x86::get_tid(DWORD tid)
{
    CONTEXT_INFO* ret = 0x0;
    ret = &this->ctx_info[this->tids[tid]];
    return ret;
}

int taint_x86::already_added(DWORD tid)
{
    unsigned i;
    int ret = 0x0;

    if(this->get_tid(tid)->tid == tid)
        d_print(3, "Thread 0x%08x already added\n", tid);
        ret = 1;
    return ret;
}

int taint_x86::add_thread(CONTEXT_info ctx_info)
{
    DWORD already_added = 0x0;
    unsigned i;

    d_print(3, "Adding thread: 0x%08x\n", ctx_info.thread_id);

    if(!this->already_added(ctx_info.thread_id))
    {
        this->ctx_info[this->tid_count].tid = ctx_info.thread_id;
        //update lookup table
        this->tids[ctx_info.thread_id] = this->tid_count;
        this->tid_count++;

    }

    //update info
    this->cur_tid = ctx_info.thread_id;

    this->reg_store_32(EAX, ctx_info.ctx.Eax);
    this->reg_store_32(ECX, ctx_info.ctx.Ecx);
    this->reg_store_32(EDX, ctx_info.ctx.Edx);
    this->reg_store_32(EBX, ctx_info.ctx.Ebx);
    this->reg_store_32(ESI, ctx_info.ctx.Esi);
    this->reg_store_32(EDI, ctx_info.ctx.Edi);
    this->reg_store_32(ESP, ctx_info.ctx.Esp);
    this->reg_store_32(EBP, ctx_info.ctx.Ebp);
    this->reg_store_32(EFLAGS, ctx_info.ctx.EFlags);

    OFFSET addr;
    CONTEXT_INFO* info;

    for(i=0x0; i<0x6; i++)
    {
        addr = this->a_calculate_base(ctx_info.ldt[i]);
        d_print(3, "Segment 0x%02x base: 0x%08x\n", i, addr);
        this->ctx_info[this->tid_count].seg_map[i] = addr;
    }
    

    this->print_context(this->cur_tid);

    this->cur_tid = 0;
    
    return 0x0;
}

int taint_x86::del_lib(OFFSET off)
{
    int i;

    for(i=0x0; i<libs_count; i++)
    {
        if(this->libs[i].offset == off)
            this->libs[i].loaded = 0x0;
    }
    return 0x0;
}

int taint_x86::add_lib(OFFSET off, char* name)
{
    LIB_INFO new_lib;

    new_lib.offset = off;
    strcpy(new_lib.name, name);
    new_lib.loaded = 1;

    this->libs[libs_count] = new_lib;
    libs_count ++; 

    return 0x0;
}

int taint_x86::add_exception(EXCEPTION_INFO info)
{
    this->exceptions[exceptions_count] = info;
    exceptions_count++;
    return 0x0;
}

int taint_x86::del_thread(DWORD tid)
{
    d_print(3, "Removing  thread: 0x%08x\n", tid);

    return 0x0;
}

int taint_x86::load_mem_from_file(char* path)
{
    d_print(3, "Loading file: %s\n", path);
    this->dump_file = fopen(path, "rb");

    char tmp_buffer[BUFF_SIZE];
    unsigned int read_bytes;
    off_t size, read;
    struct stat st; 

    if(stat(path, &st) == 0) 
    {
        size = st.st_size;
        if(size > MEM_SIZE)
        {
            d_print(3, "mem dump too large, we support up to 0x%x Mbytes", MEM_SIZE);
            return -1;
        }
    }
    else
        d_print(3, "Error\n");

    d_print(3, "Detected size: 0x%x\n", size);
    d_print(3, "Creating taint map\n");

    // allocate space for memory from dump & shadow
    #ifdef MEM_ALLOC_DECLARED_SIZE
    this->memory = (BYTE_t*)malloc(sizeof(BYTE_t)*MEM_SIZE);
    #else
    this->memory = (BYTE_t*)malloc(sizeof(BYTE_t)*size);
    #endif

    // read dump file
    read_bytes = 0;

    unsigned int i,j;

    while(read_bytes != size)
    {
        read = fread(tmp_buffer, 1, BUFF_SIZE, this->dump_file);
    
        d_print(3, "Reading chunk: 0x%x - 0x%x\n", read_bytes, read_bytes + read);
        for(i=read_bytes, j=0; i < read_bytes + read; )
            this->memory[i++].val = tmp_buffer[j++];
        
        read_bytes += read;
    }
    d_print(3, "Finished, read total of: 0x%x\n", read_bytes);
        
    fclose(this->dump_file);

    this->mem_length = read_bytes;

    return read;
}

int taint_x86::load_instr_from_file(char* path)
{
    d_print(3, "Loading instr file: %s\n", path);
    this->instr_file = fopen(path, "rb");

    d_print(3, "Done\n");

    return 0x0;
}

int taint_x86::open_lost_file(char* path)
{
#ifdef REVERSE_ANALYSIS
    d_print(3, "Opening lost file: %s\n", path);
    this->lost_file = fopen(path, "wb");

    d_print(3, "Done\n");
#endif
    return 0x0;
}

int taint_x86::close_files()
{
#ifdef REVERSE_ANALYSIS
    fclose(this->lost_file);
#endif
    return 0x0;
}

int taint_x86::add_taint(OFFSET start, DWORD length)
{
    int i;

    // tainting
    for(i = 0x0; i< length; i++)
    {
        this->memory[start+i].val_t = 0xff;
    }

    return 0x0;
}

int taint_x86::print_bt_buffer(BYTE_t* buf, int scope)
{
    for(int i=0; i< scope; i+=sizeof(BYTE_t))
    {
        d_print(3, "0x%02x ", buf[i].val);
        if((i+1) % 0x10 == 0x0)
            d_print(3, "\n");
    }
    d_print(3, "\n");
}

int taint_x86::print_all_regs()
{
//    return this->print_bt_buffer(registers, REG_SIZE);
    return 0x0;
}

//auxilliary routines
OFFSET taint_x86::a_calculate_base(LDT_ENTRY ldt)
{
    OFFSET addr;
    addr = 0x0;

    addr = ldt.HighWord.Bytes.BaseHi;
    addr <<= 0x8;
    addr ^= ldt.HighWord.Bytes.BaseMid;
    addr <<= 0x10;
    addr ^= (ldt.BaseLow & 0x0000ffff);
    return addr;
}

BYTE_t taint_x86::a_pop_8()
{
    DWORD_t val;
    DWORD_t esp;

    // store value at stack
    esp = reg_restore_32(ESP);    
    restore_32(esp, val);

    // update ESP 
    esp += 0x4;
    reg_store_32(ESP, esp);

    // returning least significant BYTE_t
    return val[0]; 
}

WORD_t taint_x86::a_pop_16()
{
    DWORD_t val;
    WORD_t ret;
    DWORD_t esp;

    // store value at stack
    esp = reg_restore_32(ESP);    
    restore_32(esp, val);

    // update ESP 
    esp += 0x4;
    reg_store_32(ESP, esp);

    // returning least significant WORD_t
    ret.val[0] = val.val[3];
    ret.val[1] = val.val[4];
    return ret; 
}

DWORD_t taint_x86::a_pop_32()
{
    DWORD_t val;
    DWORD_t esp;

    // store value at stack
    esp = reg_restore_32(ESP);    
    //val = restore_32(esp);
    restore_32(esp, val);
    d_print(3, "esp read: 0x%x val: 0x%x\n", esp.get_DWORD(), val.get_DWORD());
    d_print(3, "here2 0x%x\n", val);

    // update ESP 
    esp += 0x4;
    reg_store_32(ESP, esp);

    return val;
}

int taint_x86::a_check_segment_override()
{
    OFFSET ret = 0x0;

    d_print(3, "Checking segment override 0x%08x\n", this->current_prefixes);

    if(this->current_prefixes & PREFIX_GS_OVERRIDE) 
    {
        ret += this->cur_info->seg_map[SEG_GS];
        d_print(3, "GS! 0x%08x\n", this->cur_info->seg_map[SEG_GS]);
    }
    if(this->current_prefixes & PREFIX_FS_OVERRIDE)
    {
        ret += this->cur_info->seg_map[SEG_FS];
        d_print(3, "FS! 0x%08x\n", this->cur_info->seg_map[SEG_FS]);
    }
    if(this->current_prefixes & PREFIX_ES_OVERRIDE)
    {
        ret += this->cur_info->seg_map[SEG_ES];
        d_print(3, "ES! 0x%08x\n", this->cur_info->seg_map[SEG_ES]);
    }
    if(this->current_prefixes & PREFIX_DS_OVERRIDE)
    {
        ret += this->cur_info->seg_map[SEG_DS];
        d_print(3, "DS! 0x%08x\n", this->cur_info->seg_map[SEG_DS]);
    }
    if(this->current_prefixes & PREFIX_CS_OVERRIDE)
    {
        ret += this->cur_info->seg_map[SEG_CS];
        d_print(3, "CS! 0x%08x\n", this->cur_info->seg_map[SEG_CS]);
    }
    if(this->current_prefixes & PREFIX_SS_OVERRIDE)
    {
        ret += this->cur_info->seg_map[SEG_SS];
        d_print(3, "SS! 0x%08x\n", this->cur_info->seg_map[SEG_SS]);
    }

    return ret;
}

int taint_x86::a_push_8(BYTE_t val)
{
    DWORD_t esp;
    DWORD_t stored;

    // update ESP 
    esp = reg_restore_32(ESP); 
    d_print(3, "before: 0x%08x\n", esp.get_DWORD());
    esp -= 0x4;
    d_print(3, "after: 0x%08x\n", esp.get_DWORD());

    stored.val[3] = val;

    // store value at stack
    d_print(3, "esp write: 0x%08x val: 0x%08x\n", esp.get_DWORD(), stored);
    store_32(esp, stored);
    reg_store_32(ESP, esp);

    return 0x0;
}

int taint_x86::a_push_16(WORD_t val)
{
    DWORD_t esp;
    DWORD_t stored;

    // update ESP 
    esp = reg_restore_32(ESP);    
    d_print(3, "before: 0x%08x\n", esp.get_DWORD());
    esp -= 0x4;
    d_print(3, "after: 0x%08x\n", esp.get_DWORD());

    stored = 0x0;
    stored.val[3] = val[0];
    stored.val[4] = val[1];

    // store value at stack
    d_print(3, "esp write: 0x%x val: 0x%x\n", esp.get_DWORD(), stored);
    store_32(esp, val);
    reg_store_32(ESP, esp);

    return 0x0;
}

int taint_x86::a_push_32(DWORD_t val)
{
    DWORD_t esp;

    // update ESP 
    esp = reg_restore_32(ESP);    
    d_print(3, "before: 0x%08x\n", esp.get_DWORD());
    esp -= 0x4;
    d_print(3, "after: 0x%08x\n", esp.get_DWORD());


    // store value at stack
    d_print(3, "esp write: 0x%x val: 0x%x\n", esp.get_DWORD(), val.get_DWORD());
    store_32(esp, val);
    reg_store_32(ESP, esp);

    return 0x0;
}

int taint_x86::a_calculate_instr_length(BYTE_t*)
{
    return 0x1;
}

int taint_x86::a_is_prefix(BYTE_t* byte)
{
    int ret = 0x0;

    switch(byte->val)
    {
        case 0xf0: ret = 0x1; break;
        case 0xf2: ret = 0x1; break;
        case 0xf3: ret = 0x1; break;
        case 0x2e: ret = 0x1; break;
        case 0x3e: ret = 0x1; break;
        case 0x26: ret = 0x1; break;
        case 0x36: ret = 0x1; break;
        case 0x64: ret = 0x1; break;
        case 0x65: ret = 0x1; break;
        case 0x66: ret = 0x1; break;
        case 0x67: ret = 0x1; break;
    }
    return ret;
}

OFFSET taint_x86::a_decode_sib(BYTE_t* sib_ptr)
{
    return this->a_decode_sib_mod(sib_ptr, 0x0);
}

OFFSET taint_x86::a_decode_sib_mod(BYTE_t* sib_ptr, BYTE mod)
{
    BYTE sib_byte;
    BYTE sib_base;
    BYTE sib_idx;
    BYTE sib_ss;
    DWORD_t base, scale, index;
    BYTE_t  disp8;
    DWORD_t disp32;
    OFFSET ret;

    sib_byte = (sib_ptr)->val;
    d_print(3, "SIB byte: 0x%x\n", sib_byte);

    sib_base = (sib_byte >> SIB_BASE_OFF) & SIB_BASE_MASK;
    sib_idx  = (sib_byte >> SIB_IDX_OFF)  & SIB_IDX_MASK;
    sib_ss   = (sib_byte >> SIB_SS_OFF)   & SIB_SS_MASK;

    d_print(3, "SIB base: 0x%x\n", sib_base);
    d_print(3, "SIB idx: 0x%x\n", sib_idx);
    d_print(3, "SIB ss: 0x%x\n", sib_ss);

    // calculating: [base + scale*index ]
    // implement 16/32 switch

    // we need to take care of displacement here, so that special case with UKNOWN base can override
    d_print(3, "Displacement: ");
    switch(mod)
    {
        case 0x0:
            d_print(3, "none ");
            disp8 = 0x0;
            disp32 = 0x0;
            break;
        case 0x1:
            disp8.from_mem(sib_ptr +1);
            disp32 = 0x0;
            d_print(3, "disp8: 0x%x ", disp8.val);
            break;
        case 0x2:
            disp8 = 0x0;
            disp32.from_mem(sib_ptr +1);
            d_print(3, "disp32: 0x%x ", disp32.get_DWORD());
            break;
        case 0x3:
            d_print(3, "none ");
            break;
    }
    

    d_print(3, "Base: ");
    switch(sib_base)
    {
        case SIB_BASE_EAX: 
            base = this->reg_restore_32(EAX);
            d_print(3, "EAX, ");
            break;
        case SIB_BASE_ECX: 
            d_print(3, "ECX, ");
            base = this->reg_restore_32(ECX);
            break;
        case SIB_BASE_EDX: 
            d_print(3, "EDX, ");
            base = this->reg_restore_32(EDX);
            break;
        case SIB_BASE_EBX: 
            d_print(3, "EBX, ");
            base = this->reg_restore_32(EBX);
            break;
        case SIB_BASE_ESP: 
            d_print(3, "ESP, ");
            base = this->reg_restore_32(ESP);
            break;
        case SIB_BASE_UNKN: 
            switch(mod)
            {
                case 0x0:
                    d_print(3, "none ");
                    base = 0x0;
                    disp32.from_mem(sib_ptr +1);
                    disp8 = 0x0;
                    d_print(3, "override, disp32: 0x%x ", disp32.get_DWORD());
                    break;
                case 0x1:
                    // TODO: verify
                    d_print(3, "EBP ");
                    base = this->reg_restore_32(EBP);
                    disp8.from_mem(sib_ptr +1);
                    disp32 = 0x0;
                    d_print(3, "override, disp8: 0x%x ", disp8.val);
                    break;
                case 0x2:
                    // TODO: verify
                    d_print(3, "EBP ");
                    base = this->reg_restore_32(EBP);
                    disp32.from_mem(sib_ptr +1);
                    disp8 = 0x0;
                    d_print(3, "override, disp32: 0x%x ", disp32.get_DWORD());
                    break;
            }
            break;
        case SIB_BASE_ESI: 
            d_print(3, "ESI, ");
            base = this->reg_restore_32(ESI);
            break;
        case SIB_BASE_EDI: 
            d_print(3, "EDI, ");
            base = this->reg_restore_32(EDI);
            break;
    }

    d_print(3, "scale: ");
    switch(sib_ss)
    {
        case SIB_SCALE_1:
            scale = 1;
            d_print(3, "1, ");
            break;
        case SIB_SCALE_2:
            scale = 2;
            d_print(3, "2, ");
            break;
        case SIB_SCALE_4:
            scale = 4;
            d_print(3, "4, ");
            break;
        case SIB_SCALE_8:
            scale = 8;
            d_print(3, "8, ");
            break;
    }

    d_print(3, "index: ");
    switch(sib_idx)
    {
        case SIB_INDEX_EAX_BASED:
            index = this->reg_restore_32(EAX);
            d_print(3, "EAX\n");
            break;
        case SIB_INDEX_ECX_BASED:
            index = this->reg_restore_32(ECX);
            d_print(3, "ECX\n");
            break;
        case SIB_INDEX_EDX_BASED:
            index = this->reg_restore_32(EDX);
            d_print(3, "EDX\n");
            break;
        case SIB_INDEX_EBX_BASED:
            index = this->reg_restore_32(EBX);
            d_print(3, "EBX\n");
            break;
        case SIB_INDEX_UNKN:
            index = 0;
            d_print(3, "none\n");
            break;
        case SIB_INDEX_EBP_BASED:
            index = this->reg_restore_32(EBP);
            d_print(3, "EBP\n");
            break;
        case SIB_INDEX_ESI_BASED:
            index = this->reg_restore_32(ESI);
            d_print(3, "ESI\n");
            break;
        case SIB_INDEX_EDI_BASED:
            index = this->reg_restore_32(EDI);
            d_print(3, "EDI\n");
            break;
    }

    d_print(3, "Base: 0x%x\n", base.get_DWORD());
    d_print(3, "Scale: 0x%x\n", scale.get_DWORD());
    d_print(3, "Index: 0x%x\n", index.get_DWORD());
    d_print(3, "Disp8: 0x%x\n", disp8.val);
    d_print(3, "Disp32: 0x%x\n", disp32.get_DWORD());

    // reinterpretation for signed
    signed char* disp8p;
    DWORD disp32_reint;
    signed int* disp32p;
    disp8p = (signed char*)&(disp8.val);
    disp32_reint = disp32.get_DWORD();
    disp32p = (signed int*)&(disp32_reint);

    ret = base.get_DWORD() + *disp8p + *disp32p + (scale.get_DWORD() * index.get_DWORD());

    d_print(3, "Final value: 0x%x\n\n", ret);

    return ret; 
}


int taint_x86::a_decode_modrm(BYTE_t* modrm_byte_ptr, modrm_ptr* r, modrm_ptr* rm, int mode_r = MODE_32, int mode_rm = MODE_32)
{
    BYTE modrm_byte;
    char modrm_mod_part;
    char modrm_rm_part;
    char modrm_reg_part;

    OFFSET reg_off;
    OFFSET tgt;
    DWORD_t temp;
    DWORD_t offset;
    BYTE_t* offset_bp;
    BYTE_t* offset_imm;
    DWORD_t off_d;
    DWORD_t disp32;
    BYTE_t  disp8;

    // for reinterpretation
    signed char* disp8p;
    DWORD disp32_reint;
    signed int* disp32p;

    // get the modrm byte
    modrm_byte = modrm_byte_ptr->val;
    //current_instr_length += 1;
    d_print(3, "Modrm byte: 0x%x\n", modrm_byte);

    // extract info
    modrm_mod_part = (modrm_byte >> MOD_OFF ) & MOD_MASK;
    modrm_rm_part =  (modrm_byte >> RM_OFF )  & RM_MASK;
    modrm_reg_part = (modrm_byte >> REG_OFF ) & REG_MASK;

    d_print(3, "Mod: \t0x%x\n", modrm_mod_part);
    d_print(3, "Rm: \t0x%x\n", modrm_rm_part);
    d_print(3, "Reg: \t0x%x\n", modrm_reg_part);
    d_print(3, "\n");

    //calculate r
    switch(modrm_reg_part)
    {
        case REG_EAX_BASED:
            r->offset = EAX;
            break;
        case REG_ECX_BASED:
            r->offset = ECX;
            break;
        case REG_EDX_BASED:
            r->offset = EDX;
            break;
        case REG_EBX_BASED:
            r->offset = EBX;
            break;
        case REG_ESP_BASED:
            r->offset = ESP;
            break;
        case REG_EBP_BASED:
            r->offset = EBP;
            break;
        case REG_ESI_BASED:
            r->offset = ESI;
            break;
        case REG_EDI_BASED:
            r->offset = EDI;
            break;
    }
    r->region = MODRM_REG;
    r->size   = MODRM_SIZE_32;

    //calculate rm
    switch(modrm_mod_part)
    {
        case MOD_MEM_REG:
            switch(modrm_rm_part)
            {
                case RM_EAX_BASED: 
                    rm->offset = this->reg_restore_32(EAX).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ECX_BASED: 
                    rm->offset = this->reg_restore_32(ECX).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDX_BASED: 
                    rm->offset = this->reg_restore_32(EDX).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBX_BASED: 
                    rm->offset = this->reg_restore_32(EBX).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_UNKN: 
                    rm->offset = this->a_decode_sib_mod(modrm_byte_ptr+1, modrm_mod_part);
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBP_BASED:
                    offset_bp  = modrm_byte_ptr + 1; 
                    off_d.from_mem(offset_bp);
                    rm->offset = off_d.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ESI_BASED: 
                    rm->offset = this->reg_restore_32(ESI).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDI_BASED: 
                    rm->offset = this->reg_restore_32(EDI).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
            }
            break;

        case MOD_MEM_REG_DISP_8:
            switch(modrm_rm_part)
            {
                case RM_EAX_BASED: 
                    rm->offset = this->reg_restore_32(EAX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.val;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ECX_BASED: 
                    rm->offset = this->reg_restore_32(ECX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.val;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDX_BASED: 
                    rm->offset = this->reg_restore_32(EDX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.val;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBX_BASED: 
                    rm->offset = this->reg_restore_32(EBX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.val;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_UNKN: 
                    rm->offset = this->a_decode_sib_mod(modrm_byte_ptr+1, modrm_mod_part);
                    // SIB decoder takes care of displacement
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBP_BASED: 
                    rm->offset = this->reg_restore_32(EBP).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1); 
                    //rm->offset += disp8.val;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ESI_BASED: 
                    rm->offset = this->reg_restore_32(ESI).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.val;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDI_BASED: 
                    rm->offset = this->reg_restore_32(EDI).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.val;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
            }
            // adjust offset with displacement 
            if(modrm_rm_part != RM_UNKN)
            {
                disp8p = (signed char*)&(disp8.val);
                rm->offset += *disp8p;
            }
            break;

        case MOD_MEM_REG_DISP_32:
            switch(modrm_rm_part)
            {
                case RM_EAX_BASED: 
                    rm->offset = this->reg_restore_32(EAX).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ECX_BASED: 
                    rm->offset = this->reg_restore_32(ECX).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += *disp32p;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDX_BASED: 
                    rm->offset = this->reg_restore_32(EDX).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBX_BASED: 
                    rm->offset = this->reg_restore_32(EBX).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_UNKN: 
                    rm->offset = this->a_decode_sib_mod(modrm_byte_ptr+1, modrm_mod_part);
                    // SIB decoder takes care of displacement
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBP_BASED: 
                    rm->offset = this->reg_restore_32(EBP).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ESI_BASED: 
                    rm->offset = this->reg_restore_32(ESI).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDI_BASED: 
                    rm->offset = this->reg_restore_32(EDI).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
            }
            // adjust offset with displacement 
            if(modrm_rm_part != RM_UNKN)
            {
                disp32_reint = disp32.get_DWORD();
                disp32p = (signed int*)&(disp32_reint);

                rm->offset += *disp32p;
            }
            break;

        case MOD_REG:
            switch(modrm_rm_part)
            {
                case REG_EAX_BASED: 
                    rm->offset = EAX;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_ECX_BASED: 
                    rm->offset = ECX;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_EDX_BASED: 
                    rm->offset = EDX;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_EBX_BASED: 
                    rm->offset = EBX;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_ESP_BASED: 
                    rm->offset = ESP;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_EBP_BASED: 
                    rm->offset = EBP;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_ESI_BASED: 
                    rm->offset = ESI;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_EDI_BASED: 
                    rm->offset = EDI;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
            }
            break;
    }
      

    // recaulculation for different modes
    
    d_print(4, "r.off: 0x%08x\n", r->offset);
    d_print(4, "r.reg: 0x%08x\n", r->region);
    d_print(4, "r.size: 0x%08x\n", r->size);
    d_print(4, "rm.off: 0x%08x\n", rm->offset);
    d_print(4, "rm.reg: 0x%08x\n", rm->region);
    d_print(4, "rm.size: 0x%08x\n", rm->size);

    d_print(4, "r decode mode: 0x%08x\n", mode_r);
    d_print(4, "rm decode mode: 0x%08x\n", mode_rm);

    if(r->region == MODRM_REG && mode_r == MODE_16)
    {
        d_print(4, "Recalculating\n");
        r->offset = this->a_offset_recalculate_16(r->offset);
        r->size = MODRM_SIZE_16;
    }
 
    if(r->region == MODRM_REG && mode_r == MODE_8)
    {
        d_print(4, "Recalculating\n");
        r->offset = this->a_offset_recalculate_8(r->offset);
        r->size = MODRM_SIZE_8;
    }

    if(rm->region == MODRM_REG && mode_rm == MODE_16)
    {
        d_print(4, "Recalculating\n");
        rm->offset = this->a_offset_recalculate_16(rm->offset);
        rm->size = MODRM_SIZE_16;
    }
 
    if(rm->region == MODRM_REG && mode_rm == MODE_8)
    {
        d_print(4, "Recalculating\n");
        rm->offset = this->a_offset_recalculate_8(rm->offset);
        rm->size = MODRM_SIZE_8;
    }

    d_print(4, "r.off: 0x%08x\n", r->offset);
    d_print(4, "r.reg: 0x%08x\n", r->region);
    d_print(4, "r.size: 0x%08x\n", r->size);
    d_print(4, "rm.off: 0x%08x\n", rm->offset);
    d_print(4, "rm.reg: 0x%08x\n", rm->region);
    d_print(4, "rm.size: 0x%08x\n", rm->size);

    // take care of seg overrides
    d_print(3, "Offset: 0x%08x\n", rm->offset);
    rm->offset += a_check_segment_override();
    d_print(3, "Offset: 0x%08x\n", rm->offset);
    return 0x0;
}

OFFSET taint_x86::a_offset_recalculate_16(OFFSET off)
{
    OFFSET off_ret;

    switch(off)
    {
        case EAX:
            off_ret = AX;
            break;
        case EBX:
            off_ret = BX;
            break;
        case ECX:
            off_ret = CX;
            break;
        case EDX:
            off_ret = DX;
            break;
        case ESI:
            off_ret = SI;
            break;
        case EDI:
            off_ret = DI;
            break;
        case EBP:
            off_ret = BP;
            break;
        case ESP:
            off_ret = SP;
            break;
    }
    return off_ret;
}

OFFSET taint_x86::a_offset_recalculate_8(OFFSET off)
{
    OFFSET off_ret;

    switch(off)
    {
        case EAX:
            off_ret = AL;
            break;
        case EBX:
            off_ret = BL;
            break;
        case ECX:
            off_ret = CL;
            break;
        case EDX:
            off_ret = DL;
            break;
        case ESI:
            off_ret = AH;
            break;
        case EDI:
            off_ret = BH;
            break;
        case EBP:
            off_ret = CH;
            break;
        case ESP:
            off_ret = DH;
            break;
    }
    return off_ret;
}

int taint_x86::a_push_lost_32(DWORD val)
{
#ifdef REVERSE_ANALYSIS
    char line[0x20];
    
    sprintf(line, "0x%08x\n", val);
    fwrite(line, strlen(line), 0x1, this->lost_file);
#endif
    return 0x0;
}

int taint_x86::a_push_lost_16(WORD val)
{
#ifdef REVERSE_ANALYSIS
    char line[0x20];
    
    sprintf(line, "0x%04x\n", val);
    fwrite(line, strlen(line), 0x1, this->lost_file);
#endif
    return 0x0;
}

int taint_x86::a_push_lost_8(BYTE val)
{
#ifdef REVERSE_ANALYSIS
    char line[0x20];
    
    sprintf(line, "0x%02x\n", val);
    fwrite(line, strlen(line), 0x1, this->lost_file);
#endif
    return 0x0;
}

DWORD taint_x86::a_pop_lost()
{

    return 0x0;
}

/*
* instructions routines
*/

// extend

int taint_x86::r_extend(BYTE_t* addr)
{
    this->extended = 0x1;

    d_print(3, "Extending\n");
    this->execute_instruction_at_eip(this->reg_restore_32(EIP).get_DWORD() + 1);

    return 0x0;
}

// inc/dec

int taint_x86::r_inc_eax_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EAX);
    temp += 1;
    this->reg_store_32(EAX, temp);
    return 0x0;
}

int taint_x86::r_inc_ecx_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(ECX);
    temp += 1;
    this->reg_store_32(ECX, temp);
    return 0x0;
}

int taint_x86::r_inc_edx_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EDX);
    temp += 1;
    this->reg_store_32(EDX, temp);
    return 0x0;
}

int taint_x86::r_inc_ebx_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EBX);
    temp += 1;
    this->reg_store_32(EBX, temp);
    return 0x0;
}

int taint_x86::r_inc_esp_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(ESP);
    temp += 1;
    this->reg_store_32(ESP, temp);
    return 0x0;
}

int taint_x86::r_inc_ebp_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EBP);
    temp += 1;
    this->reg_store_32(EBP, temp);
    return 0x0;
}

int taint_x86::r_inc_esi_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(ESI);
    temp += 1;
    this->reg_store_32(ESI, temp);
    return 0x0;
}

int taint_x86::r_inc_edi_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EDI);
    temp += 1;
    this->reg_store_32(EDI, temp);
    return 0x0;
}

int taint_x86::r_dec_eax_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EAX);
    temp -= 1;
    this->reg_store_32(EAX, temp);
    return 0x0;
}

int taint_x86::r_dec_ecx_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(ECX);
    temp -= 1;
    this->reg_store_32(ECX, temp);
    return 0x0;
}

int taint_x86::r_dec_edx_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EDX);
    temp -= 1;
    this->reg_store_32(EDX, temp);
    return 0x0;
}

int taint_x86::r_dec_ebx_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EBX);
    temp -= 1;
    this->reg_store_32(EBX, temp);
    return 0x0;
}

int taint_x86::r_dec_esp_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(ESP);
    temp -= 1;
    this->reg_store_32(ESP, temp);
    return 0x0;
}

int taint_x86::r_dec_ebp_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EBP);
    temp -= 1;
    this->reg_store_32(EBP, temp);
    return 0x0;
}

int taint_x86::r_dec_esi_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(ESI);
    temp -= 1;
    this->reg_store_32(ESI, temp);
    return 0x0;
}

int taint_x86::r_dec_edi_32(BYTE_t*)
{
    DWORD_t temp;
    temp = this->reg_restore_32(EDI);
    temp -= 1;
    this->reg_store_32(EDI, temp);
    return 0x0;
}

int taint_x86::r_noop(BYTE_t* b)
{
    return 0x0;
}

// xor

int taint_x86::r_xor_rm_r(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp_32;
    DWORD_t temp_32_2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    //[TODO: implement 16/32]

    switch(rm.region)
    {
        case MODRM_REG:
            this->restore_32(rm.offset, temp_32);
            temp_32 ^= reg_restore_32(r.offset);
            this->store_32(rm.offset, temp_32);
            break;
        case MODRM_MEM:
            this->restore_32(rm.offset, temp_32);
            temp_32 ^= reg_restore_32(r.offset);
            this->store_32(rm.offset, temp_32);
            break;
    }
    d_print(3, "Result: 0x%08x\n", temp_32.get_DWORD());
    return 0x0;
}

int taint_x86::r_xor_r_rm(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    //[TODO: implement 16/32]

    switch(rm.region)
    {
        case MODRM_REG:
            temp_32 = this->reg_restore_32(rm.offset);
            break;
        case MODRM_MEM:
            this->restore_32(rm.offset, temp_32);
            break;
    }
    temp_32 ^= reg_restore_32(r.offset);
    d_print(3, "Result: 0x%08x\n", temp_32.get_DWORD());
    this->reg_store_32(r.offset, temp_32);
    return 0x0;
}

int taint_x86::r_xor_al_imm_8(BYTE_t* b)
{

}

int taint_x86::r_xor_ax_imm_16(BYTE_t* b)
{

}

int taint_x86::r_xor_eax_imm_32(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_sal_shl_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8.from_mem(instr_ptr+2);
            temp_8 <<= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8.from_mem(instr_ptr+2);
            temp_8 <<= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8.from_mem(instr_ptr+2);
            temp_8 >>= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8.from_mem(instr_ptr+2);
            temp_8 >>= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sar_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8.from_mem(instr_ptr+2);
            temp_8 >>= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8.from_mem(instr_ptr+2);
            temp_8 >>= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sal_shl_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t imm_8;

    d_print(3, "r_sal_shl_rm_16_32_imm_8\n");

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            d_print(3, "reg\n");
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    d_print(3, "Shift by 0x%02x\n", imm_8.val);
                    d_print(3, "Before 0x%08x\n", temp_16.get_WORD());
                    temp_16 <<= imm_8;
                    d_print(3, "Before 0x%08x\n", temp_16.get_WORD());
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    d_print(3, "Shift by 0x%02x\n", imm_8.val);
                    d_print(3, "Before 0x%08x\n", temp_32.get_DWORD());
                    temp_32 <<= imm_8;
                    d_print(3, "After 0x%08x\n", temp_32.get_DWORD());
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16 <<= imm_8;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32 <<= imm_8;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_32_imm_8(BYTE_t* instr_ptr)
{
    return 0x0;
}

int taint_x86::r_sar_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16 >>= imm_8;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32 >>= imm_8;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16 >>= imm_8;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32 >>= imm_8;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_sal_shl_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            temp_8 <<= 1;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            temp_8 <<= 1;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            temp_8 >>= 1;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            temp_8 >>= 1;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sar_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            temp_8 >>= 1;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            temp_8 >>= 1;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sal_shl_rm_16_32_1(BYTE_t* instr_ptr)
{

    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    temp_16 <<= 1;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    temp_32 <<= 1;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    temp_16 <<= 1;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    temp_32 <<= 1;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_16_32_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    temp_16 >>= 1;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    temp_32 >>= 1;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    temp_16 >>= 1;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    temp_32 >>= 1;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }
    return 0x0;
}

int taint_x86::r_sar_shr_rm_16_32_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    temp_16 >>= 1;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    temp_32 >>= 1;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    temp_16 >>= 1;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    temp_32 >>= 1;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }
    return 0x0;
}

int taint_x86::r_sal_shl_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t cl;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            cl = this->reg_restore_8(CL);
            temp_8 <<= cl;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            cl = this->reg_restore_8(CL);
            temp_8 <<= cl;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t cl;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            cl = this->reg_restore_8(CL);
            temp_8 <<= cl;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            cl = this->reg_restore_8(CL);
            temp_8 <<= cl;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sar_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t cl;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            cl = this->reg_restore_8(CL);
            temp_8 <<= cl;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            cl = this->reg_restore_8(CL);
            temp_8 <<= cl;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sal_shl_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t cl;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    cl = this->reg_restore_8(CL);
                    temp_16 <<= cl;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    cl = this->reg_restore_8(CL);
                    temp_32 <<= cl;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    cl = this->reg_restore_8(CL);
                    temp_16 <<= cl;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    cl = this->reg_restore_8(CL);
                    temp_32 <<= cl;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }
    return 0x0;
}

int taint_x86::r_shr_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t cl;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    cl = this->reg_restore_8(CL);
                    temp_16 >>= cl;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    cl = this->reg_restore_8(CL);
                    temp_32 >>= cl;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    cl = this->reg_restore_8(CL);
                    temp_16 >>= cl;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    cl = this->reg_restore_8(CL);
                    temp_32 >>= cl;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }
    return 0x0;
}

int taint_x86::r_sar_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t cl;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    cl = this->reg_restore_8(CL);
                    temp_16 >>= cl;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    cl = this->reg_restore_8(CL);
                    temp_32 >>= cl;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    cl = this->reg_restore_8(CL);
                    temp_16 >>= cl;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    cl = this->reg_restore_8(CL);
                    temp_32 >>= cl;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }
    return 0x0;
}

// sub

int taint_x86::r_sub_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 -= imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 -= imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 -= imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 -= imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_sub_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16.val[1] -= imm_8;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32.val[3] -= imm_8;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16.val[1] -= imm_8;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32.val[3] -= imm_8;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_sub_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t al;
    BYTE_t imm_8;

    al = this->reg_restore_8(AL);
    imm_8.from_mem(instr_ptr+1);
    al -= imm_8;
    this->reg_store_8(AL, al);
    
    return 0x0;
}

int taint_x86::r_sub_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t temp_16, imm_16;
    DWORD_t temp_32, imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        temp_16 = this->reg_restore_16(AX);
        imm_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", imm_16.get_WORD());
        temp_16 -= imm_16;
        this->reg_store_16(AX, temp_16);
    }
    else
    {
        temp_32 = this->reg_restore_32(EAX);
        imm_32.from_mem(instr_ptr +1);
        d_print(3, "Imm32: 0x%08x\n", imm_32.get_DWORD());
        temp_32 -= imm_32;
        d_print(3, "temp_32: 0x%08x\n", temp_32.get_DWORD());
        this->reg_store_32(EAX, temp_32);
    }

    return 0x0;
}

int taint_x86::r_sub_rm_imm8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8.from_mem(instr_ptr+2);
            temp_8 -= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8.from_mem(instr_ptr+2);
            temp_8 -= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sub_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 -= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 -= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sub_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 -= imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 -= imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 -= imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 -= imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_sub_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(r.offset);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 -= imm_8;
            this->reg_store_8(r.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, temp_8);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 -= imm_8;
            this->store_8(r.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sub_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(r.offset);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 -= imm_16;
                    this->reg_store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(r.offset);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 -= imm_32;
                    this->reg_store_32(r.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(r.offset, temp_16);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 -= imm_16;
                    this->store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(r.offset, temp_32);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 -= imm_32;
                    this->store_32(r.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}
// sbb

int taint_x86::r_sbb_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 -= imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 -= imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 -= imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 -= imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16.val[1] -= imm_8;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32.val[3] -= imm_8;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16.val[1] -= imm_8;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32.val[3] -= imm_8;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t al;
    BYTE_t imm_8;

    al = this->reg_restore_8(AL);
    imm_8.from_mem(instr_ptr+1);
    al -= imm_8;
    this->reg_store_8(AL, al);
    
    return 0x0;
}

int taint_x86::r_sbb_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t temp_16, imm_16;
    DWORD_t temp_32, imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        temp_16 = this->reg_restore_16(AX);
        imm_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", imm_16.get_WORD());
        temp_16 -= imm_16;
        this->reg_store_16(AX, temp_16);
    }
    else
    {
        temp_32 = this->reg_restore_32(EAX);
        imm_32.from_mem(instr_ptr +1);
        d_print(3, "Imm32: 0x%08x\n", imm_32.get_DWORD());
        temp_32 -= imm_32;
        d_print(3, "temp_32: 0x%08x\n", temp_32.get_DWORD());
        this->reg_store_32(EAX, temp_32);
    }

    return 0x0;
}

int taint_x86::r_sbb_rm_imm8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8.from_mem(instr_ptr+2);
            temp_8 -= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8.from_mem(instr_ptr+2);
            temp_8 -= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 -= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 -= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 -= imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 -= imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 -= imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 -= imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(r.offset);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 -= imm_8;
            this->reg_store_8(r.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, temp_8);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 -= imm_8;
            this->store_8(r.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(r.offset);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 -= imm_16;
                    this->reg_store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(r.offset);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 -= imm_32;
                    this->reg_store_32(r.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(r.offset, temp_16);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 -= imm_16;
                    this->store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(r.offset, temp_32);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 -= imm_32;
                    this->store_32(r.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

// add

int taint_x86::r_add_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 += imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 += imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 += imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 += imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_add_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16.val[1] += imm_8;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32.val[3] += imm_8;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16.val[1] += imm_8;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32.val[3] += imm_8;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_add_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t al;
    BYTE_t imm_8;

    al = this->reg_restore_8(AL);
    imm_8.from_mem(instr_ptr+1);
    al += imm_8;
    this->reg_store_8(AL, al);
    
    return 0x0;
}

int taint_x86::r_add_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t temp_16, imm_16;
    DWORD_t temp_32, imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        temp_16 = this->reg_restore_16(AX);
        imm_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", imm_16.get_WORD());
        temp_16 += imm_16;
        this->reg_store_16(AX, temp_16);
    }
    else
    {
        temp_32 = this->reg_restore_32(EAX);
        imm_32.from_mem(instr_ptr +1);
        d_print(3, "Imm32: 0x%08x\n", imm_32.get_DWORD());
        temp_32 += imm_32;
        d_print(3, "temp_32: 0x%08x\n", temp_32.get_DWORD());
        this->reg_store_32(EAX, temp_32);
    }

    return 0x0;
}

int taint_x86::r_add_rm_imm8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8.from_mem(instr_ptr+2);
            temp_8 += imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8.from_mem(instr_ptr+2);
            temp_8 += imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_add_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 += imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 += imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_add_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 += imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 += imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 += imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 += imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_add_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(r.offset);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 += imm_8;
            this->reg_store_8(r.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, temp_8);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 += imm_8;
            this->store_8(r.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_add_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(r.offset);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 += imm_16;
                    this->reg_store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(r.offset);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 += imm_32;
                    this->reg_store_32(r.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(r.offset, temp_16);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 += imm_16;
                    this->store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(r.offset, temp_32);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 += imm_32;
                    this->store_32(r.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

// mul

int taint_x86::r_mul_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t al, temp_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            al = this->reg_restore_8(AL);
            temp_8 = this->reg_restore_8(rm.offset);
            al *= temp_8;
            this->reg_store_8(AL, al);
            break;
        case (MODRM_MEM):
            this->restore_8(AL, al);
            this->restore_8(rm.offset, temp_8);
            al *= temp_8;
            this->store_8(AL, al);
            break;
    }

    return 0x0;
}

int taint_x86::r_mul_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16, ax;
    DWORD_t temp_32, eax;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    ax = this->reg_restore_16(AX);
                    temp_16 = this->reg_restore_16(rm.offset);
                    ax *= temp_16;
                    this->reg_store_16(AX, ax);
                    break;
                case MODRM_SIZE_32:
                    eax = this->reg_restore_32(EAX);
                    temp_32 = this->reg_restore_32(rm.offset);
                    eax *= temp_32;
                    this->reg_store_32(EAX, eax);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(AX, ax);
                    temp_16 = this->reg_restore_16(rm.offset);
                    ax *= temp_16;
                    this->reg_store_16(AX, ax);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(EAX, eax);
                    temp_32 = this->reg_restore_32(rm.offset);
                    eax *= temp_32;
                    this->reg_store_32(EAX, eax);
                    break;
            }
            break;
    }

    return 0x0;
}


// div

int taint_x86::r_div_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t al, temp_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            al = this->reg_restore_8(AL);
            temp_8 = this->reg_restore_8(rm.offset);
            al /= temp_8;
            this->reg_store_8(AL, al);
            break;
        case (MODRM_MEM):
            this->restore_8(AL, al);
            this->restore_8(rm.offset, temp_8);
            al /= temp_8;
            this->store_8(AL, al);
            break;
    }

    return 0x0;
}

int taint_x86::r_div_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16, ax;
    DWORD_t temp_32, eax;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    ax = this->reg_restore_16(AX);
                    temp_16 = this->reg_restore_16(rm.offset);
                    ax /= temp_16;
                    this->reg_store_16(AX, ax);
                    break;
                case MODRM_SIZE_32:
                    eax = this->reg_restore_32(EAX);
                    temp_32 = this->reg_restore_32(rm.offset);
                    eax /= temp_32;
                    this->reg_store_32(EAX, eax);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(AX, ax);
                    temp_16 = this->reg_restore_16(rm.offset);
                    ax /= temp_16;
                    this->reg_store_16(AX, ax);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(EAX, eax);
                    temp_32 = this->reg_restore_32(rm.offset);
                    eax /= temp_32;
                    this->reg_store_32(EAX, eax);
                    break;
            }
            break;
    }


    return 0x0;
}



// and

int taint_x86::r_and_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 &= imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 &= imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 &= imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 &= imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_and_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16.val[1] &= imm_8;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32.val[3] &= imm_8;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16.val[1] &= imm_8;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32.val[3] &= imm_8;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_and_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t al;
    BYTE_t imm_8;

    al = this->reg_restore_8(AL);
    imm_8.from_mem(instr_ptr+1);
    al &= imm_8;
    this->reg_store_8(AL, al);
    
    return 0x0;
}

int taint_x86::r_and_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t temp_16, imm_16;
    DWORD_t temp_32, imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        temp_16 = this->reg_restore_16(AX);
        imm_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", imm_16.get_WORD());
        temp_16 &= imm_16;
        this->reg_store_16(AX, temp_16);
    }
    else
    {
        temp_16 = this->reg_restore_16(AX);
        temp_32 = temp_16;
        imm_32.from_mem(instr_ptr +1);
        d_print(3, "Imm32: 0x%08x\n", imm_32.get_DWORD());
        temp_32 &= imm_32;
        d_print(3, "temp_32: 0x%08x\n", temp_32.get_DWORD());
        this->reg_store_32(EAX, temp_32);
    }

    return 0x0;
}

int taint_x86::r_and_rm_imm8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8.from_mem(instr_ptr+2);
            temp_8 &= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8.from_mem(instr_ptr+2);
            temp_8 &= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_and_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 &= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 &= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_and_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 &= imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 &= imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 &= imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 &= imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_and_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(r.offset);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 &= imm_8;
            this->reg_store_8(r.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, temp_8);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 &= imm_8;
            this->store_8(r.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_and_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(r.offset);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 &= imm_16;
                    this->reg_store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(r.offset);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 &= imm_32;
                    this->reg_store_32(r.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(r.offset, temp_16);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 &= imm_16;
                    this->store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(r.offset, temp_32);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 &= imm_32;
                    this->store_32(r.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_or_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16 |= imm_8;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32 |= imm_8;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_8.from_mem(instr_ptr+2);
                    temp_16 |= imm_8;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_8.from_mem(instr_ptr+2);
                    temp_32 |= imm_8;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_or_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8.from_mem(instr_ptr+2);
            temp_8 |= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8.from_mem(instr_ptr+2);
            temp_8 |= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_or_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 |= imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 |= imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16.from_mem(instr_ptr+2);
                    temp_16 |= imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32.from_mem(instr_ptr+2);
                    temp_32 |= imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_or_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 |= imm_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            imm_8 = this->reg_restore_8(r.offset);
            temp_8 |= imm_8;
            this->store_8(rm.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_or_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 |= imm_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 |= imm_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    imm_16 = this->reg_restore_16(r.offset);
                    temp_16 |= imm_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    imm_32 = this->reg_restore_32(r.offset);
                    temp_32 |= imm_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_or_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;
    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(r.offset);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 |= imm_8;
            this->reg_store_8(r.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, temp_8);
            imm_8 = this->reg_restore_8(rm.offset);
            temp_8 |= imm_8;
            this->store_8(r.offset, temp_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_or_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;
    WORD_t imm_16;
    DWORD_t imm_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(r.offset);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 |= imm_16;
                    this->reg_store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(r.offset);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 |= imm_32;
                    this->reg_store_32(r.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(r.offset, temp_16);
                    imm_16 = this->reg_restore_16(rm.offset);
                    temp_16 |= imm_16;
                    this->store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(r.offset, temp_32);
                    imm_32 = this->reg_restore_32(rm.offset);
                    temp_32 |= imm_32;
                    this->store_32(r.offset, temp_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_or_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t al;
    BYTE_t imm_8;

    al = this->reg_restore_8(AL);
    imm_8.from_mem(instr_ptr+1);
    al |= imm_8;
    this->reg_store_8(AL, al);
    
    return 0x0;
}

int taint_x86::r_or_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t temp_16, imm_16;
    DWORD_t temp_32, imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        temp_16 = this->reg_restore_16(AX);
        imm_16.from_mem(instr_ptr +1);
        temp_16 |= imm_16;
        this->reg_store_16(AX, temp_16);
    }
    else
    {
        temp_32 = this->reg_restore_32(AX);
        imm_32.from_mem(instr_ptr +1);
        temp_32 |= imm_32;
        this->reg_store_32(EAX, temp_32);
    }

    return 0x0;
}

// neg & not

int taint_x86::r_neg_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_32, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(r.offset);
            temp_8 = ~temp_8;
            this->reg_store_8(r.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, temp_8);
            temp_8 = ~temp_8;
            this->store_8(r.offset, temp_8);
            break;
    }
 

    return 0x0;
}

int taint_x86::r_neg_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    temp_16 = ~temp_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    temp_32 = ~temp_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    temp_16 = ~temp_16;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    temp_32 = ~temp_32;
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
    }
 
    return 0x0;
}

int taint_x86::r_not_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_32, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            temp_8 = this->reg_restore_8(rm.offset);
            temp_8 = !temp_8;
            this->reg_store_8(rm.offset, temp_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, temp_8);
            temp_8 = !temp_8;
            this->store_8(rm.offset, temp_8);
            break;
    }
 

    return 0x0;
}

int taint_x86::r_not_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t temp_16;
    DWORD_t temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    temp_16 = !temp_16;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    temp_32 = !temp_32;
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(r.offset, temp_16);
                    temp_16 = !temp_16;
                    this->store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(r.offset, temp_32);
                    temp_32 = !temp_32;
                    this->store_32(r.offset, temp_32);
                    break;
            }
            break;
    }
 
    return 0x0;
}


// pop

int taint_x86::r_pop_eax_32(BYTE_t* args)
{
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(EAX, val);
    return 0x0;
}

int taint_x86::r_pop_ecx_32(BYTE_t* args)
{
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(ECX, val);
    return 0x0;
}

int taint_x86::r_pop_edx_32(BYTE_t* args)
{
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(EDX, val);
    return 0x0;
}

int taint_x86::r_pop_ebx_32(BYTE_t* args)
{
    d_print(3, "here\n");
    DWORD_t val;
    val = a_pop_32();
    d_print(3, "val: %x\n", val);
    reg_store_32(EBX, val);
    return 0x0;
}

int taint_x86::r_pop_esp_32(BYTE_t* args)
{
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(ESP, val);
    return 0x0;
}

int taint_x86::r_pop_ebp_32(BYTE_t* args)
{
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(EBP, val);
    return 0x0;
}

int taint_x86::r_pop_esi_32(BYTE_t* args)
{
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(ESI, val);
    return 0x0;
}

int taint_x86::r_pop_edi_32(BYTE_t* args)
{
    DWORD_t val;
    val = a_pop_32();
    reg_store_32(EDI, val);
    return 0x0;
}

// push

int taint_x86::r_push_eax_32(BYTE_t* args)
{
    return a_push_32(reg_restore_32(EAX));
}

int taint_x86::r_push_ecx_32(BYTE_t* args)
{
    return a_push_32(reg_restore_32(ECX));
}

int taint_x86::r_push_edx_32(BYTE_t* args)
{
    return a_push_32(reg_restore_32(EDX));
}

int taint_x86::r_push_ebx_32(BYTE_t* args)
{
    return a_push_32(reg_restore_32(EBX));
}

int taint_x86::r_push_esp_32(BYTE_t* args)
{
    return a_push_32(reg_restore_32(ESP));
}

int taint_x86::r_push_ebp_32(BYTE_t* args)
{
    return a_push_32(reg_restore_32(EBP));
}

int taint_x86::r_push_esi_32(BYTE_t* args)
{
    return a_push_32(reg_restore_32(ESI));
}

int taint_x86::r_push_edi_32(BYTE_t* args)
{
    return a_push_32(reg_restore_32(EDI));
}

int taint_x86::r_push_imm_8(BYTE_t* args)
{
    BYTE_t val_8;
    OFFSET imm_8;

    // any other way to get offset in emulated memory?
    imm_8 = reg_restore_32(EIP).get_DWORD() +1;

    this->restore_8(imm_8, val_8);

    d_print(3, "Push value: 0x%08x\n", val_8.val);

    return a_push_8(val_8);
}

int taint_x86::r_push_imm_16_32(BYTE_t* args)
{
    WORD_t val_16;
    DWORD_t val_32;
    OFFSET imm_16_32;

    // any other way to get offset in emulated memory?
    imm_16_32 = reg_restore_32(EIP).get_DWORD() +1;

    // default is 32
    if(this->current_prefixes && PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->restore_16(imm_16_32, val_16);
        return a_push_16(val_16);
    }
    else
    {
        this->restore_32(imm_16_32, val_32);
        return a_push_32(val_32);
    }
}

int taint_x86::r_push_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp;
    BYTE_t val_8;
    WORD_t val_16;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    // TODO: sometimes still zero :<

    d_print(3, "From: 0x%08x\n", rm.offset);

    temp = this->reg_restore_32(r.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
/*
                case MODRM_SIZE_8:
                    this->reg_restore_8(rm.offset, val_8);
                    temp.val[3] = val_8;
                    break;
*/
                case MODRM_SIZE_16:
                    val_16 = this->reg_restore_16(rm.offset);
                    temp.val[2] = val_16.val[0];
                    temp.val[3] = val_16.val[1];
                    break;
                case MODRM_SIZE_32:
                    temp = this->reg_restore_32(rm.offset);
                    break;
            }
        case MODRM_MEM:
            switch(rm.size)
            {
/*
                case MODRM_SIZE_8:
                    this->restore_8(rm.offset, val_8);
                    temp.val[3] = val_8;
                    break;
*/
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val_16);
                    temp.val[2] = val_16.val[0];
                    temp.val[3] = val_16.val[1];
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp);
                    break;
            }
            break;
    }
    
    a_push_32(temp);

    return 0x0;
}

// mov

int taint_x86::r_mov_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp_32, imm_32;
    WORD_t temp_16, imm_16;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    d_print(3, "Target: 0x%08x\n", rm.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    imm_16.from_mem(instr_ptr +3); 
                    this->reg_store_16(rm.offset, imm_16);
                    break;
                case MODRM_SIZE_32:
                    imm_32.from_mem(instr_ptr +3); 
                    this->reg_store_32(rm.offset, imm_32);
                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    imm_16.from_mem(instr_ptr +3); 
                    this->store_16(rm.offset, imm_16);
                    break;
                case MODRM_SIZE_32:
                    imm_32.from_mem(instr_ptr +3); 
                    this->store_32(rm.offset, imm_32);
                    break;
            }
            break;
    }



    return 0x0;
}

int taint_x86::r_mov_rm_r_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    temp = this->reg_restore_32(r.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            this->reg_store_32(rm.offset, temp);    
            break;
        case MODRM_MEM:
            this->store_32(rm.offset, temp);    
            break;
    }

    return 0x0;
}

int taint_x86::r_mov_r_rm_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    d_print(3, "Got offset: 0x%08x\n", rm.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            temp = this->reg_restore_32(rm.offset);
            break;
        case MODRM_MEM:
            this->restore_32(rm.offset, temp);
            break;
    }

    d_print(3, "Read: 0x%08x\n", temp.get_DWORD());

    this->reg_store_32(r.offset, temp);
    return 0x0;
}

int taint_x86::r_mov_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    temp = this->reg_restore_8(r.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            this->reg_store_8(rm.offset, temp);    
            break;
        case MODRM_MEM:
            this->store_8(rm.offset, temp);    
            break;
    }

    return 0x0;
}

int taint_x86::r_mov_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case MODRM_REG:
            temp = this->reg_restore_8(rm.offset);
            break;
        case MODRM_MEM:
            this->restore_8(rm.offset, temp);
            break;
    }

    this->reg_store_8(r.offset, temp);
    return 0x0;
}

int taint_x86::r_mov_ax_eax_moffset_16_32(BYTE_t* instr_ptr)
{
    DWORD_t off;
    DWORD_t temp_32;
    WORD_t temp_16;

    off.from_mem(instr_ptr +1);

    d_print(3, "Reading from: 0x%x\n", off.get_DWORD());

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->restore_16(off, temp_16);
        this->reg_store_16(AX, temp_16);
        return 0x0;
    }
    else
    {
        this->restore_32(off, temp_32);
        d_print(3, "Get val: 0x%08x\n", temp_32.get_DWORD());
        this->reg_store_32(EAX, temp_32);
        return 0x0;
    }
}

int taint_x86::r_mov_al_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t imm_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
    imm_8.from_mem(&this->memory[offset]);

    this->reg_store_8(AL, imm_8);

    return 0x0;
}

int taint_x86::r_mov_bl_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t imm_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
    imm_8.from_mem(&this->memory[offset]);

    this->reg_store_8(AL, imm_8);

    return 0x0;
}

int taint_x86::r_mov_cl_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t imm_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
    imm_8.from_mem(&this->memory[offset]);

    this->reg_store_8(AL, imm_8);

    return 0x0;
}

int taint_x86::r_mov_dl_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t imm_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
    imm_8.from_mem(&this->memory[offset]);

    this->reg_store_8(AL, imm_8);

    return 0x0;
}

int taint_x86::r_mov_ah_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t imm_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
    imm_8.from_mem(&this->memory[offset]);

    this->reg_store_8(AH, imm_8);

    return 0x0;
}

int taint_x86::r_mov_bh_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t imm_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
    imm_8.from_mem(&this->memory[offset]);

    this->reg_store_8(BH, imm_8);

    return 0x0;
}

int taint_x86::r_mov_ch_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t imm_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
    imm_8.from_mem(&this->memory[offset]);

    this->reg_store_8(CH, imm_8);

    return 0x0;
}

int taint_x86::r_mov_dh_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t imm_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
    imm_8.from_mem(&this->memory[offset]);

    this->reg_store_8(DH, imm_8);

    return 0x0;
}

int taint_x86::r_mov_eax_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t imm_16;
    DWORD_t imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_16.from_mem(&this->memory[offset]);
        this->reg_store_16(AX, imm_16);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EAX, imm_32);
    }

    return 0x0;
}

int taint_x86::r_mov_ecx_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t imm_16;
    DWORD_t imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_16.from_mem(&this->memory[offset]);
        this->reg_store_16(CX, imm_16);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_32.from_mem(&this->memory[offset]);
        this->reg_store_32(ECX, imm_32);
    }

    return 0x0;
}

int taint_x86::r_mov_edx_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t imm_16;
    DWORD_t imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_16.from_mem(&this->memory[offset]);
        this->reg_store_16(DX, imm_16);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EDX, imm_32);
    }

    return 0x0;
}

int taint_x86::r_mov_ebx_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t imm_16;
    DWORD_t imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_16.from_mem(&this->memory[offset]);
        this->reg_store_16(BX, imm_16);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EBX, imm_32);
    }

    return 0x0;
}

int taint_x86::r_mov_esp_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t imm_16;
    DWORD_t imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_16.from_mem(&this->memory[offset]);
        this->reg_store_16(SP, imm_16);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_32.from_mem(&this->memory[offset]);
        this->reg_store_32(ESP, imm_32);
    }

    return 0x0;
}

int taint_x86::r_mov_ebp_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t imm_16;
    DWORD_t imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_16.from_mem(&this->memory[offset]);
        this->reg_store_16(BP, imm_16);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EBP, imm_32);
    }

    return 0x0;
}

int taint_x86::r_mov_esi_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t imm_16;
    DWORD_t imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_16.from_mem(&this->memory[offset]);
        this->reg_store_16(SI, imm_16);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_32.from_mem(&this->memory[offset]);
        this->reg_store_32(ESI, imm_32);
    }

    return 0x0;
}

int taint_x86::r_mov_edi_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t imm_16;
    DWORD_t imm_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_16.from_mem(&this->memory[offset]);
        this->reg_store_16(DI, imm_16);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        imm_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EDI, imm_32);
    }

    return 0x0;
}


// add, sub, mul, div

//deprecated
/*
int taint_x86::r_sub_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    OFFSET offset;
    DWORD_t temp_32;
    BYTE_t imm_8;
    WORD_t temp_16;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(rm.offset);
                    offset = this->reg_restore_32(EIP).get_DWORD() + 0x2;
                    imm_8.from_mem(&this->memory[offset]);
                    temp_16 -= imm_8;
                    this->reg_store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(rm.offset);
                    offset = this->reg_restore_32(EIP).get_DWORD() + 0x2;
                    d_print(3, "Will read from: 0x%08x\n", offset);
                    imm_8.from_mem(&this->memory[offset]);
                    d_print(3, "Before: 0x%08x\n", temp_32.get_DWORD());
                    d_print(3, "Imm8: 0x%08x\n", imm_8.val);
                    temp_32 -= imm_8;
                    d_print(3, "After: 0x%08x\n", temp_32.get_DWORD());
                    this->reg_store_32(rm.offset, temp_32);
                    break;
            }
            break;

        case MODRM_MEM:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, temp_16);
                    offset = this->reg_restore_32(EIP).get_DWORD() + 0x2;
                    imm_8.from_mem(&this->memory[offset]);
                    temp_16 -= imm_8;
                    this->store_16(rm.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp_32);
                    offset = this->reg_restore_32(EIP).get_DWORD() + 0x2;
                    d_print(3, "Will read from: 0x%08x\n", offset);
                    imm_8.from_mem(&this->memory[offset]);
                    d_print(3, "Before: 0x%08x\n", temp_32.get_DWORD());
                    d_print(3, "Imm8: 0x%08x\n", imm_8.val);
                    temp_32 -= imm_8;
                    d_print(3, "After: 0x%08x\n", temp_32.get_DWORD());
                    this->store_32(rm.offset, temp_32);
                    break;
            }
            break;
        }

    return 0x0;
}


// TODO: review this, this is bad
int taint_x86::r_sub_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    OFFSET offset;
    DWORD_t temp_32;
    DWORD_t temp_32_2;
    WORD_t temp_16;
    WORD_t temp_16_2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(r.offset);
                    temp_16 -= this->reg_restore_16(rm.offset);
                    this->reg_store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(r.offset);
                    temp_32 -= this->reg_restore_32(rm.offset);
                    this->reg_store_32(r.offset, temp_32);
                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    temp_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, temp_16_2);
                    temp_16 -= temp_16_2;
                    this->reg_store_16(r.offset, temp_16);
                    break;
                case MODRM_SIZE_32:
                    temp_32 = this->reg_restore_32(r.offset);
                    this->restore_32(rm.offset, temp_32_2);
                    temp_32 -= temp_32_2;
                    this->reg_store_32(r.offset, temp_32);
                    break;
            }
            break;
    }
    return 0x0;
}
*/
int taint_x86::r_lea(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;

    WORD_t temp_16;
    DWORD_t temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.size)
    {
        case MODRM_SIZE_16:
            temp_16 = rm.offset;
            this->reg_store_16(r.offset, temp_16);
            break;
        case MODRM_SIZE_32:
            temp_32 = rm.offset;
            this->reg_store_32(r.offset, temp_32);
            break;
    }

    return 0x0;
}

// calls

int taint_x86::r_retn(BYTE_t*)
{
    OFFSET offset;
    WORD_t imm_16;
    unsigned i, count;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    imm_16.from_mem(&this->memory[offset]);

    count = imm_16.get_WORD();
    count /= 0x4;

    for(i=0x0; i<count; i++)
        this->a_pop_32();
    
    this->a_pop_32();

    return 0x0;
}

int taint_x86::r_ret(BYTE_t*)
{
    this->a_pop_32();
    return 0x0;
}

// calls

int taint_x86::r_call_rel(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;

    ret_addr = this->reg_restore_32(EIP);
    ret_addr += 0x5;

    a_push_32(ret_addr);
    
    return 0x0;
}

int taint_x86::r_call_abs_near(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    modrm_ptr rm, r;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    ret_addr = this->reg_restore_32(EIP);

    if(rm.region == MODRM_REG)
        ret_addr += 0x2;
    else
        ret_addr += 0x6;

    a_push_32(ret_addr);
    
    return 0x0;
}

int taint_x86::r_call_abs_far(BYTE_t*)
{
    DWORD_t ret_addr;

    ret_addr = this->reg_restore_32(EIP);
    ret_addr += 0x5;

    a_push_32(ret_addr);
    
    return 0x0;
}


// prefixes

int taint_x86::r_lock(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_LOCK;
    return 0x0;
}

int taint_x86::r_repne(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_REPNE;
    return 0x0;
}

int taint_x86::r_rep(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_REP;
    return 0x0;
}

int taint_x86::r_cs_override(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_CS_OVERRIDE;
    printf("CS override\n");
    return 0x0;
}

int taint_x86::r_ss_override(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_SS_OVERRIDE;
    printf("SS override\n");
    return 0x0;
}

int taint_x86::r_ds_override(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_DS_OVERRIDE;
    printf("DS override\n");
    return 0x0;
}

int taint_x86::r_es_override(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_ES_OVERRIDE;
    printf("ES override\n");
    return 0x0;
}

int taint_x86::r_fs_override(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_FS_OVERRIDE;
    printf("FS override\n");
    return 0x0;
}

int taint_x86::r_gs_override(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_GS_OVERRIDE;
    printf("GS override\n");
    return 0x0;
}

int taint_x86::r_oper_size_override(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_OPER_SIZE_OVERRIDE;
    return 0x0;
}

int taint_x86::r_addr_size_override(BYTE_t*)
{
    this->current_prefixes ^= PREFIX_ADDR_SIZE_OVERRIDE;
    return 0x0;
}

// need decoding

int taint_x86::r_decode_execute_80(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case EBX:
            return this->r_sbb_rm_imm8(addr);
            break;
        case ECX:
            return this->r_or_rm_imm_8(addr);
            break;
        case ESP:
            return this->r_and_rm_imm8(addr);
            break;
        case EBP: 
            return this->r_sub_rm_imm8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_81(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case EBX:
            return this->r_sbb_r_rm_16_32(addr);
            break;
        case ECX:
            return this->r_or_rm_imm_16_32(addr);
            break;
        case ESP:
            return r_and_rm_imm_16_32(addr);
            break;
        case EBP: // reg code for 0x5
            return this->r_sub_r_rm_16_32(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_83(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case EBX:
            return this->r_sbb_rm_16_32_imm_8(addr);
            break;
        case ECX:
            return this->r_or_rm_16_32_imm_8(addr);
            break;
        case ESP:
            return r_and_rm_16_32_imm_8(addr);
            break;
        case EBP:
            return this->r_sub_rm_16_32_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_c0(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESP:
            //return this->r_sal_rm_imm_8(addr);
            //return this->r_shl_rm_imm_8(addr);
            return this->r_sal_shl_rm_imm_8(addr);
            break;
        case EBP:
            return this->r_shr_rm_imm_8(addr);
            break;
        case EDI: 
            return this->r_sar_rm_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_c1(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESP:
            //return this->r_sal_rm_16_imm_8(addr);
            //return this->r_shl_rm_16_imm_8(addr);
            //return this->r_sal_rm_32_imm_8(addr);
            //return this->r_shl_rm_32_imm_8(addr);
            return this->r_sal_shl_rm_16_32_imm_8(addr);
            break;
        case EBP:
            return this->r_shr_rm_32_imm_8(addr);
            break;
        case EDI: 
            //return this->r_sar_rm_16_imm_8(addr);
            //return this->r_sar_rm_32_imm_8(addr);
            return this->r_sar_rm_16_32_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_c7(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case EAX:
            return this->r_mov_rm_imm_16_32(addr);
            break;
        case EBP:
            break;
        case EDI: 
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_d0(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESP:
            //return this->r_sal_rm_8_1(addr);
            //return this->r_shl_rm_8_1(addr);
            return this->r_sal_shl_rm_8_1(addr);
            break;
        case EBP:
            return this->r_shr_rm_8_1(addr);
            break;
        case EDI: 
            return this->r_sar_rm_8_1(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_d1(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESP:
            //return this->r_sal_rm_16_1(addr);
            //return this->r_sal_rm_32_1(addr); //the same?
            //return this->r_shl_rm_32_1(addr); 
            return this->r_sal_shl_rm_16_32_1(addr); 
            break;
        case EBP:
            //return this->r_shr_rm_16_1(addr)
            //return this->r_shr_rm_32_1(addr)
            return this->r_shr_rm_16_32_1(addr);
            break;
        case EDI: 
            //return this->r_sar_rm_16_1(addr);
            //return this->r_sar_rm_32_1(addr);
            //return this->r_shr_rm_16_1(addr);
            return this->r_sar_shr_rm_16_32_1(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_d2(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESP:
            //return this->r_sal_rm_8_cl(addr);
            //return this->r_shl_rm_8_cl(addr);
            return this->r_sal_shl_rm_8_cl(addr);
            break;
        case EBP:
            return this->r_shr_rm_8_cl(addr);
            break;
        case EDI: 
            return this->r_sar_rm_8_cl(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_d3(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESP:
            //return this->r_sal_rm_16_cl(addr);
            //return this->r_shl_rm_16_cl(addr);
            //return this->r_sal_rm_32_cl(addr); 
            //return this->r_shl_rm_32_cl(addr); 
            return this->r_sal_shl_rm_16_32_cl(addr); 
            break;
        case EBP:
            //return this->r_shr_rm_16_cl(addr)
            //return this->r_shr_rm_32_cl(addr)
            return this->r_shr_rm_16_32_cl(addr);
            break;
        case EDI: 
            //return this->r_sar_rm_16_cl(addr);
            //return this->r_sar_rm_32_cl(addr);
            return this->r_sar_rm_16_32_cl(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}


int taint_x86::r_decode_execute_f6(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESI: // reg code for 0x6
            break;
        case EDX: // reg code 2
            return this->r_not_rm_8(addr);
            break;
        case EBX: // reg code 3
            return this->r_neg_rm_8(addr);
            break;

    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_f7(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESI: // reg code for 0x6
            break;
        case EDX: // reg code 2
            return this->r_not_rm_16_32(addr);
            break;
        case EBX: // reg code 3
            return this->r_neg_rm_16_32(addr);
            break;

    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_ff(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(r.offset)
    {
        case ESI: // reg code for 0x6
            return r_push_rm_16_32(addr);
            break;
        case EDX: // reg code 2
            return this->r_call_abs_near(addr);
            break;
        case EBX: // reg code 3
            return this->r_call_abs_far(addr);
            break;

    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->val, r.offset);

    return 0x0;
}

// extended functions

int taint_x86::r_movzx_r_16_32_rm_8(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;
    DWORD_t temp_32;
    WORD_t temp_16;
    BYTE_t temp_8;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_8);

    temp_16 = 0; 
    temp_32 = 0; 

    switch(rm.region)
    {
        case MODRM_REG:
            temp_8 = this->reg_restore_8(rm.offset);
            break;
        case MODRM_MEM:
            this->restore_8(rm.offset, temp_8);
            break;
    }

    switch(r.size)
    {
        case MODRM_SIZE_16:
            temp_16 += temp_8;
            this->reg_store_16(r.offset, temp_16);
            break;
        case MODRM_SIZE_32:
            temp_32 += temp_8;
            this->reg_store_32(r.offset, temp_32);
            break;
    }

    return 0x0;
}

int taint_x86::r_movzx_r_32_rm_16(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;
    DWORD_t temp_32;
    WORD_t temp_16;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_16);

    temp_16 = 0; 
    temp_32 = 0; 

    switch(rm.region)
    {
        case MODRM_REG:
            temp_16 = this->reg_restore_16(rm.offset);
            d_print(3, "Offset: 0x%08x got src value: 0x%08x\n", rm.offset, this->reg_restore_16(rm.offset).get_WORD());
            break;
        case MODRM_MEM:
            this->restore_16(rm.offset, temp_16);
            break;
    }

    d_print(3, "Got src value: 0x%08x\n", temp_16.get_WORD());
    temp_32 += temp_16;
    
    this->reg_store_32(r.offset, temp_32);
    d_print(3, "Offset: 0x%08x, val: 0x%08x\n", r.offset, temp_32.get_DWORD());

    return 0x0;
}

// rest of main functions

int taint_x86::print_mem(OFFSET start, DWORD len)
{
    for(OFFSET i=start; i<start+len; i++)
    {
        d_print(3, "0x%x: 0x%x\n", i, this->memory[i].val);
    }
    d_print(3, "\n");
}

int taint_x86::print_t_mem(OFFSET start, DWORD len)
{
    for(OFFSET i=start; i<start+len; i++)
    {
        d_print(3, "0x%x: 0x%x\n", i, this->memory[i].val_t);
    }
    d_print(3, "\n");
}


int taint_x86::print_stack(DWORD len)
{
    OFFSET start;
    OFFSET addr;
    DWORD_t temp;
    len *= 0x4;

    start = this->reg_restore_32(ESP).get_DWORD();

    for(OFFSET i=start; i<start+len; i+=0x4)
    {
        temp.from_mem(&this->memory[i]);
        d_print(3, "0x%08x: 0x%08x\n", i, temp.get_DWORD());
    }
    d_print(3, "\n");
}

int taint_x86::print_t_stack(DWORD len)
{
    OFFSET start;
    OFFSET addr;
    DWORD_t temp;
    len *= 0x4;

    start = this->reg_restore_32(ESP).get_DWORD();

    for(OFFSET i=start; i<start+len; i+=0x4)
    {
        temp.from_mem(&this->memory[i]);
        d_print(3, "0x%08x: 0x%08x\n", i, temp.get_DWORD());
    }
    d_print(3, "\n");
}

int taint_x86::print_context()
{
    return this->print_context(this->cur_tid);
}

int taint_x86::print_t_context()
{
    return this->print_t_context(this->cur_tid);
}

int taint_x86::print_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        d_print(3, "No such context: 0x%08x\n", tid);
        return 0x0;
    }
    d_print(3, "WARNING!!! EIP is one instruction late!\n");
    d_print(3, "Context 0x%08x:\n", tid);
    d_print(3, "EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_DWORD());
    d_print(3, "EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_DWORD());
    d_print(3, "ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_DWORD());
    d_print(3, "EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_DWORD());
    d_print(3, "ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_DWORD());
    d_print(3, "EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_DWORD());
    d_print(3, "EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_DWORD());
    d_print(3, "ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_DWORD());
    d_print(3, "EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_DWORD());
    d_print(3, "\n");
}

int taint_x86::print_t_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        d_print(3, "No such context: 0x%08x\n", tid);
        return 0x0;
    }
    d_print(3, "Taint context 0x%08x:\n", tid);
    d_print(3, "EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_t_DWORD());
    d_print(3, "EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_t_DWORD());
    d_print(3, "ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_t_DWORD());
    d_print(3, "EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_t_DWORD());
    d_print(3, "ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_t_DWORD());
    d_print(3, "EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_t_DWORD());
    d_print(3, "EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_t_DWORD());
    d_print(3, "ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_t_DWORD(), tid);
    d_print(3, "EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_t_DWORD(), tid);
    d_print(3, "\n");
}

int taint_x86::print_all_contexts()
{
    unsigned i;
    for(i=0; i<this->tid_count; i++)
        this->print_context(this->ctx_info[i].tid);
    return 0x0;
}

int taint_x86::print_all_t_contexts()
{
    unsigned i;
    for(i=0; i<this->tid_count; i++)
        this->print_t_context(this->ctx_info[i].tid);
    return 0x0;
}

