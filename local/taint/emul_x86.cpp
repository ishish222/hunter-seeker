#include <iostream>
#include <execinfo.h>
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <emul_x86.h>
#include "debug.h"
#include <iniparser.h>
#include "inc/pe_bliss.h"
#include <fstream>
#include <plugin.h>
#include <breakpoint.h>

#define MEM_ALLOC_DECLARED_SIZE
#define VERIFY_OOB
#define UPDATE_THREAD

#define GRAPH_OFF 11
#define CODE_BLACK  0x0
#define CODE_BLUE   0x1
#define CODE_GREEN  0x2
#define CODE_RED    0x3
#define CODE_COMMENT 0x4


#define GET_MOD(x) (x >> MOD_OFF ) & MOD_MASK
#define GET_RM(x)  (x >> RM_OFF )  & RM_MASK
#define GET_REG(x) (x >> REG_OFF ) & REG_MASK;

int taint_x86::d_print(int level, const char* format, ...)
{
    va_list argptr;

//    if(level <= DEBUG_LEVEL && this->bp_hit)
    if(level <= DEBUG_LEVEL)
    {
        va_start(argptr, format);
        vfprintf(stderr, format, argptr);
//        vfprintf(stdout, format, argptr);
        va_end(argptr);
    }

    return 0x0;
}

void taint_x86::handle_sigsegv(int signum)
{
    
    d_print(1, "\n********* SEGMENTATION FAULT *********\n\n");
    d_print(1, "Instruction no: %d\n", this->current_instr_count);
    d_print(1, "EIP: 0x%08x\n", this->current_eip);
    if(this->current_instr_byte != 0x0) d_print(1, "Opcode: 0x%02x\n", this->current_instr_byte->get_BYTE());

    void *trace[32];
    size_t size, i;
    char **strings;

    size    = backtrace(trace, 32);
    strings = backtrace_symbols(trace, size);

    d_print(1, "\nBACKTRACE:\n\n");

    for(i = 0; i < size; i++)
    {
	    d_print(1, "  %s\n", strings[i]);
    }

    d_print(1, "\n***************************************\n" );


    exit(-1);  
}

void taint_x86::handle_sigint(int signum)
{
    this->finished = 0x1;
    this->aborted = 0x1;
    d_print(1, "Eip: 0x%08x, this->end_addr: 0x%08x, limit: %d, count: %d, finishing\n", this->current_eip, this->end_addr, this->instr_limit, this->current_instr_count);
}

int taint_x86::start()
{
    if(this->plugin) this->plugin->start_callback();

    this->started = 0x1;
    return 0x0;
}

int taint_x86::add_breakpoint(BREAKPOINT bp)
{
    if(this->bpt_count >= MAX_BREAKPOINTS) return -1;

    char mode[0x50];
    strcpy(mode, "");

    if(bp.mode & BP_MODE_READ) strcat(mode, "READ|");
    if(bp.mode & BP_MODE_WRITE) strcat(mode, "WRITE|");
    if(bp.mode & BP_MODE_EXECUTE) strcat(mode, "EXECUTE|");
    
    d_print(1, "Adding breakpoint %s: tid: 0x%08x, instruction_no: %d, offset: 0x%08x, mode: %s\n", bp.name, bp.tid, bp.instruction_no, bp.offset, mode);

    this->bps[this->bpt_count].instruction_no = bp.instruction_no;
    this->bps[this->bpt_count].offset = bp.offset;
    this->bps[this->bpt_count].mode = bp.mode;
    strcpy(this->bps[this->bpt_count].name, bp.name);
    this->bps[this->bpt_count].tid= bp.tid;

    this->bpt_count++;

    return 0x0;
}

inline int taint_x86::verify_seg_sec(OFFSET off)
{
    unsigned i;
    
    for(i = 0x0; i< this->security_layer_count; i++)
    {
        if((this->security_layer[i].off <= off) && ((this->security_layer[i].off + this->security_layer[i].size) >= off))
        {
            d_print(1, "[IMPERFECTION DETECTED] Attempt of write to secured segment @ instr no: %d, eip: 0x%08x\n", this->current_instr_count, this->reg_restore_32(EIP).get_DWORD());
            return 0x1;
        }
    }
    return 0x0;
}

inline int taint_x86::verify_oob_offset(OFFSET off, OFFSET size)
{
    if(off > size)
    {
        d_print(3, "[IMPERFECTION DETECTED] Attempt of OOB read/write @ instr no: %d, eip: 0x%08x\n", this->current_instr_count, this->reg_restore_32(EIP).get_DWORD());
//        exit(1);
        return -1;
    }
    return 0x0;
}


/*
* store / restore
*/

/* 32 bit */

void taint_x86::store_32(OFFSET off, DWORD_t v)
{
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, this->mem_length) != 0x0) 
            return;

    if(this->options & OPTION_VERIFY_SEG_SEC) 
        if(verify_seg_sec(off))
            return;

    if(this->options & HANDLE_BREAKPOINTS)
    for(int i = 0x0; i< this->bpt_count; i++)
        if(((off - this->bps[i].offset) <= 0x4) && (this->bps[i].mode & BP_MODE_WRITE))
        {
            err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: WRITE to 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
            v.to_mem(&this->memory[off], 1);
            print_mem(1, off, 0x10);
            print_security_layers(1, off);

            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);

            return;
        }

    v.to_mem(&this->memory[off], 1);
}

void taint_x86::restore_32(OFFSET off, DWORD_t& ret)
{
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, this->mem_length) != 0x0) 
        {
            ret = this->invalid_dword;
            return;
        }

    if(this->options & HANDLE_BREAKPOINTS)
    for(int i = 0x0; i< this->bpt_count; i++)
        if((this->bps[i].offset == off) && (this->bps[i].mode & BP_MODE_READ))
        {
            err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: READ from 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);

            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);
        }

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
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
            return;

    CONTEXT_INFO* info;
    info = this->get_context_info(tid);

    info->registers[off + 0] = v[0];
    info->registers[off + 1] = v[1];
    info->registers[off + 2] = v[2];
    info->registers[off + 3] = v[3];

    return;
}

DWORD_t taint_x86::reg_restore_32(OFFSET off, int tid)
{
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
        {
            return this->invalid_dword;
        }

    CONTEXT_INFO* info;
    info = this->get_context_info(tid);

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
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, this->mem_length) != 0x0) 
            return;

    if(this->options & OPTION_VERIFY_SEG_SEC) 
        if(verify_seg_sec(off))
            return;

    if(this->options & HANDLE_BREAKPOINTS)
    for(int i = 0x0; i<  this->bpt_count; i++)
        if(((off - this->bps[i].offset) <= 0x2) && (this->bps[i].mode & BP_MODE_WRITE))
        {
            err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: WRITE to 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
            v.to_mem(&this->memory[off], 1);
            print_mem(1, off, 0x10);
            print_security_layers(1, off);

            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);

            return;
        }

    v.to_mem(&this->memory[off], 1);
}

void taint_x86::restore_16(OFFSET off, WORD_t& ret)
{
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, this->mem_length) != 0x0) 
        {
            ret = this->invalid_word;
            return;
        }

    if(this->options & HANDLE_BREAKPOINTS)
    for(int i = 0x0; i<  this->bpt_count; i++)
        if((this->bps[i].offset == off) && (this->bps[i].mode & BP_MODE_READ))
        {
            err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: READ from 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);

            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);

        }

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
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
            return;

    CONTEXT_INFO* info;
    info = this->get_context_info(tid);

    info->registers[off + 0] = v[0];
    info->registers[off + 1] = v[1];

    return;
}

WORD_t taint_x86::reg_restore_16(OFFSET off, int tid)
{
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
        {
            return this->invalid_word;
        }

    CONTEXT_INFO* info;
    info = this->get_context_info(tid);

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
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
            return;

if(this->options & OPTION_VERIFY_SEG_SEC) 
    if(verify_seg_sec(off))
        return;

    if(this->options & HANDLE_BREAKPOINTS)
    for(int i = 0x0; i< this->bpt_count; i++)
        if((this->bps[i].offset == off) && (this->bps[i].mode & BP_MODE_WRITE))
        {
            err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: WRITE to 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
            v.to_mem(&this->memory[off]);
            print_mem(1, off, 0x10);
            print_security_layers(1, off);

            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);

            return;
        }

    v.to_mem(&this->memory[off]);
}

void taint_x86::restore_8(OFFSET off, BYTE_t& ret)
{
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
        {
            ret = this->invalid_byte;
            return;
        }

    if(this->options & HANDLE_BREAKPOINTS)
    for(int i = 0x0; i< this->bpt_count; i++)
        if((this->bps[i].offset == off) && (this->bps[i].mode & BP_MODE_READ))
        {
            err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: READ from 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);

            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);

        }

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
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
            return;

    CONTEXT_INFO* info;
    info = this->get_context_info(tid);
    

    info->registers[off] = v;
    return;
}

BYTE_t taint_x86::reg_restore_8(OFFSET off, int tid)
{
    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
        {   
            return this->invalid_byte;
        }

    CONTEXT_INFO* info;
    info = this->get_context_info(tid);

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
    d_print(1, "[pre_execute_instruction starts]\n");
    if(this->plugin) this->plugin->pre_execute_instruction_callback(eip);

    this->cur_info = this->get_context_info(this->cur_tid);

    this->current_instr_length = 0x0;
    this->current_prefixes = 0x0;
    this->current_prefix_length = 0x0;
    this->current_eip = eip;
    this->reg_store_32(EIP, eip);
    this->current_instr_length = 0x0;
    this->current_instr_is_jump = 0x0;

    /* check if execution breakpoint has been hit */
    if(this->options & HANDLE_BREAKPOINTS)
        this->check_execution_bps();

    d_print(1, "[pre_execute_instruction ends]\n");
    return 0x0;
}


int taint_x86::post_execute_instruction(DWORD eip)
{
    d_print(1, "[post_execute_instruction starts]\n");
    if(this->plugin) this->plugin->post_execute_instruction_callback(eip);

    CONTEXT_INFO* cur_ctx;
    unsigned info_pos = this->tids[this->cur_tid];
    cur_ctx = &this->ctx_info[info_pos];

    this->extended = 0x0;

    this->last_instr_byte = this->current_instr_byte;
    this->last_instr_count = this->current_instr_count;

    // probable eip //is this graph stff or not?
    if(!this->current_instr_is_jump)
        reg_store_32(EIP, this->reg_restore_32(EIP) + this->current_instr_length);

    fflush(stdout);

    /* search propagation watchpoints */

    unsigned esp;
    esp = (UDWORD)this->reg_restore_32(ESP).get_DWORD();

    if((this->end_addr) || (this->instr_limit))
    {
        if((eip == this->end_addr) || (this->instr_limit == this->current_instr_count)) 
        {
            this->finished = 0x1;
            this->aborted = 0x1;
            d_print(1, "Eip: 0x%08x, this->end_addr: 0x%08x, limit: %d, count: %d, finishing\n", eip, this->end_addr, this->instr_limit, this->current_instr_count);
        }
    }

    /* if propagation occured, seal it */
    if(this->seal_scheduled)
    {
        this->seal_current_propagation();
        this->seal_scheduled = 0x0;
    }

    this->last_eip = this->current_eip;
    cur_ctx->last_eip = this->last_eip;

    d_print(1, "[post_execute_instruction ends]\n");
    return 0x0;
}

inline int taint_x86::execute_instruction_32(DWORD eip, DWORD tid)
{
    int ret;
    DWORD byte;

    byte = current_instr_byte->get_BYTE();

    d_print(1, "[execute_instruction_32 starts] Current instr byte: 0x%02x\n", byte);
    /* plugin instruction-specific preexecution */
    if(this->plugin)
    {
        if(this->plugin->instructions_32_start[byte])
        {
            ret = (this->plugin->*(this->plugin->instructions_32_start[byte]))(this->current_instr_byte);
        }
    }

    /* instruction preexecution */
    ret = (this->*(instructions_32[byte]))(this->current_instr_byte);

    /* plugin instruction-specific postexecution */
    if(this->plugin)
    {
        if(this->plugin->instructions_32_end[byte])
        {
            ret = (this->plugin->*(this->plugin->instructions_32_end[byte]))(this->current_instr_byte);
        }
    }

    d_print(1, "[execute_instruction_32 ends]\n");
    return ret;
}

inline int taint_x86::execute_instruction_32_extended(DWORD eip, DWORD tid)
{
    d_print(1, "[execute_instruction_32_extended starts]\n");
    int ret;
    DWORD byte;

    byte = current_instr_byte->get_BYTE();

    /* plugin preexecution */
    if(this->plugin)
    {
        if(this->plugin->instructions_32_extended_start[byte])
        {
            ret = (this->plugin->*(this->plugin->instructions_32_extended_start[byte]))(this->current_instr_byte);
        }
    }

    /* instruction preexecution */
    ret = (this->*(instructions_32_extended[byte]))(this->current_instr_byte);

    /* plugin postexecution */
    if(this->plugin)
    {
        if(this->plugin->instructions_32_extended_end[byte])
        {
            ret = (this->plugin->*(this->plugin->instructions_32_extended_end[byte]))(this->current_instr_byte);    
        }
    }

    d_print(1, "[execute_instruction_32_extended ends]\n");
    return ret;

}

int taint_x86::execute_instruction(DWORD eip, DWORD tid)
{
    d_print(1, "[execute_instruction starts]\n");
    int ret = 0x0;

    this->cur_tid = tid;

    this->current_instr_byte = &this->memory[eip];
    d_print(3, "current_instr_byte: 0x%02x\n", this->current_instr_byte->get_BYTE());

    if(this->options & OPTION_COUNT_INSTRUCTIONS)
    {
        this->current_instr_byte->set_BYTE_t(0xff); // taint executed? this should go to taint preexecution handler
    }

    //this->current_propagation->instruction = eip;
    this->current_instr_length += 1;

    // process prefixes
    while(a_is_prefix(this->current_instr_byte))
    {
        ret = this->execute_instruction_32(eip, tid);
        this->current_instr_byte->set_BYTE_t(0xff); // taint executed?
        this->current_instr_length += 1;
        this->current_prefix_length += 1;
        this->current_instr_byte = &this->memory[eip + this->current_prefix_length];
        d_print(3, "Updated prefixes: 0x%08x\n", this->current_prefixes);
    }

    d_print(3, "0x%08x: 0x%02x\n", eip, int(current_instr_byte->get_BYTE()));
    if(this->extended)
    {
        d_print(3, "Extended\n");
        ret = this->execute_instruction_32_extended(eip, tid);
    }
    else
    {
        ret = this->execute_instruction_32(eip, tid);
    }

    d_print(1, "[execute_instruction ends]\n");
    return ret;
}

int taint_x86::execute_instruction_at_eip(DWORD eip, DWORD tid)
{

    d_print(1, "==[execute_instruction_at_eip starts]\n");
    d_print(3, "[0x%08x] Inst: 0x%08x, count: %d\n", tid, eip, this->current_instr_count);
    //this->propagations[this->current_propagation_count].instruction = eip;
    //this->propagations[this->current_propagation_count].instr_count = this->current_instr_count;
    this->cur_tid = tid;
    this->pre_execute_instruction(eip);
    this->execute_instruction(eip, tid);
    this->post_execute_instruction(eip);
    d_print(1, "==[execute_instruction_at_eip ends]\n");

    return 0x0;
}


int taint_x86::execute_instruction_at_eip(DWORD eip)
{
    return this->execute_instruction_at_eip(eip, this->cur_tid);
}

int taint_x86::check_execution_bps()
{
    unsigned i;

    for(i=0x0; i<this->bpt_count; i++)
    {
        if((this->bps[i].instruction_no == this->current_instr_count) && (this->bps[i].instruction_no) && (this->bps[i].mode & BP_MODE_EXECUTE))
        {
            err_print("Breakpoint %s @%lld, 0x%08x, hit\n", this->bps[i].name, this->current_instr_count, this->current_eip);
            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);

            err_print("---\n");
        }
        else if((this->bps[i].offset == this->current_eip) && (this->bps[i].offset) && (this->bps[i].mode & BP_MODE_EXECUTE))
        {
            err_print("Breakpoint %s @%lld, 0x%08x, hit\n", this->bps[i].name, this->current_instr_count, this->current_eip);
            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);

            err_print("---\n");
        }
    }

    if(this->step_mode)
    {
        this->step_mode = 0x0;
        err_print("Step mode, @%lld, 0x%08x\n", this->current_instr_count, this->current_eip);
        if(this->plugin)
            this->plugin->breakpoint_callback(&this->bps[i]);
    }

    return 0x0;
}
CONTEXT_INFO* taint_x86::get_context_info(DWORD tid)
{
    unsigned info_pos = this->tids[tid];
    CONTEXT_INFO* ret = 0x0;
    ret = &this->ctx_info[info_pos];
    return ret;
}

int taint_x86::already_added(DWORD tid)
{
    unsigned i;
    int ret = 0x0;

    if(this->tids[tid] != -1) 
    {
        ret = 0x1;
        d_print(3, "Thread 0x%08x already added\n", tid);
    }
    else
    {
        ret = 0x0;
        d_print(3, "Thread 0x%08x NOT added\n", tid);
    }
    return ret;
}

int taint_x86::check_thread(CONTEXT_OUT ctx_out)
{
    DWORD tid;
    unsigned i;
    char graph_filename[MAX_NAME];
    char out_line[MAX_NAME];

    tid = ctx_out.thread_id;

    if(this->reg_restore_32(EAX, tid).get_DWORD() != ctx_out.ctx.Eax) goto error;
    if(this->reg_restore_32(ECX, tid).get_DWORD() != ctx_out.ctx.Ecx) goto error;
    if(this->reg_restore_32(EDX, tid).get_DWORD() != ctx_out.ctx.Edx) goto error;
    if(this->reg_restore_32(EBX, tid).get_DWORD() != ctx_out.ctx.Ebx) goto error;
    if(this->reg_restore_32(ESI, tid).get_DWORD() != ctx_out.ctx.Esi) goto error;
    if(this->reg_restore_32(EDI, tid).get_DWORD() != ctx_out.ctx.Edi) goto error;
    if(this->reg_restore_32(EBP, tid).get_DWORD() != ctx_out.ctx.Ebp) goto error;
    if(this->reg_restore_32(ESP, tid).get_DWORD() != ctx_out.ctx.Esp) goto error;
    if(this->reg_restore_32(EIP, tid).get_DWORD() != ctx_out.ctx.Eip) goto error;

    goto noerror;

    error:
        if(last_inconsistent == 0x0)
            d_print(4, "WARNING: New inconsistency detected @ %d, eip: 0x%08x\n", this->current_instr_count, this->current_eip);
        else
            d_print(4, "WARNING: inconsistency detected\n");

        d_print(4, "Is:\n");
        //this->print_context();

        d_print(4, "\nShould be:\n");
        d_print(4, "EAX: 0x%08x\n", ctx_out.ctx.Eax);
        d_print(4, "EBX: 0x%08x\n", ctx_out.ctx.Ebx);
        d_print(4, "ECX: 0x%08x\n", ctx_out.ctx.Ecx);
        d_print(4, "EDX: 0x%08x\n", ctx_out.ctx.Edx);
        d_print(4, "ESI: 0x%08x\n", ctx_out.ctx.Esi);
        d_print(4, "EDI: 0x%08x\n", ctx_out.ctx.Edi);
        d_print(4, "EBP: 0x%08x\n", ctx_out.ctx.Ebp);
        d_print(4, "ESP: 0x%08x\n", ctx_out.ctx.Esp);
        d_print(4, "EIP: 0x%08x\n", ctx_out.ctx.Eip);

        this->last_inconsistent = 0x1;

#ifdef UPDATE_THREAD
        this->mod_thread(ctx_out);
#endif

    return 0x0;

    noerror:
        this->last_inconsistent = 0x0;

    return 0x0;
}

int taint_x86::finish()
{
    if(this->plugin) this->plugin->finish_callback();

    return 0x0;    
}

int taint_x86::mod_thread(CONTEXT_OUT ctx_out)
{
    d_print(3, "Updating thread: 0x%08x\n", ctx_out.thread_id);

    if(!this->already_added(ctx_out.thread_id))
    {
        return 0x0;
    }

    DWORD_t reg;

    this->reg_store_32(EAX, ctx_out.ctx.Eax, ctx_out.thread_id);
    this->reg_store_32(ECX, ctx_out.ctx.Ecx, ctx_out.thread_id);
    this->reg_store_32(EDX, ctx_out.ctx.Edx, ctx_out.thread_id);
    this->reg_store_32(EBX, ctx_out.ctx.Ebx, ctx_out.thread_id);
    this->reg_store_32(ESI, ctx_out.ctx.Esi, ctx_out.thread_id);
    this->reg_store_32(EDI, ctx_out.ctx.Edi, ctx_out.thread_id);
    this->reg_store_32(ESP, ctx_out.ctx.Esp, ctx_out.thread_id);
    this->reg_store_32(EBP, ctx_out.ctx.Ebp, ctx_out.thread_id);
    this->reg_store_32(EFLAGS, ctx_out.ctx.EFlags, ctx_out.thread_id);

    //this->print_context(this->cur_tid);

//    this->cur_tid = 0;
    
    return 0x0;
}

int taint_x86::update_watchpoints(DWORD tid)
{
    unsigned i;
    for(i=0x0; i< this->wpt_count; i++)
        if(this->wps[i].tid == tid)
        {
            if(strstr(this->wps[i].name, "EAX") != 0x0)
            {
                this->wps[i].watched = &this->ctx_info[this->tids[this->wps[i].tid]].registers[EAX];
            }
            else if(strstr(this->wps[i].name, "EBX") != 0x0)
            {
                this->wps[i].watched = &this->ctx_info[this->tids[this->wps[i].tid]].registers[EBX];
            }
            else if(strstr(this->wps[i].name, "ECX") != 0x0)
            {
                this->wps[i].watched = &this->ctx_info[this->tids[this->wps[i].tid]].registers[ECX];
            }
            else if(strstr(this->wps[i].name, "EDX") != 0x0)
            {
                this->wps[i].watched = &this->ctx_info[this->tids[this->wps[i].tid]].registers[EDX];
            }
            else if(strstr(this->wps[i].name, "ESI") != 0x0)
            {
                this->wps[i].watched = &this->ctx_info[this->tids[this->wps[i].tid]].registers[ESI];
            }
            else if(strstr(this->wps[i].name, "EDI") != 0x0)
            {
                this->wps[i].watched = &this->ctx_info[this->tids[this->wps[i].tid]].registers[EDI];
            }
            else if(strstr(this->wps[i].name, "EBP") != 0x0)
            {
                this->wps[i].watched = &(this->ctx_info[this->tids[this->wps[i].tid]].registers[EBP]);
            }
            else if(strstr(this->wps[i].name, "ESP") != 0x0)
            {
                this->wps[i].watched = &this->ctx_info[this->tids[this->wps[i].tid]].registers[ESP];
            }
            else if(strstr(this->wps[i].name, "EIP") != 0x0)
            {
                this->wps[i].watched = &this->ctx_info[this->tids[this->wps[i].tid]].registers[EIP];
            }
            else if(strtol(this->wps[i].name, 0x0, 0x10) != 0x0)
            {
                this->wps[i].watched = &this->memory[strtol(this->wps[i].name, 0x0, 0x10)];
            }
        
        }

    return 0x0;
}

int taint_x86::add_thread(CONTEXT_OUT ctx_out)
{
    if(this->plugin) this->plugin->add_thread_callback(ctx_out);

    DWORD new_tid = ctx_out.thread_id;    

    unsigned i;
    char out_line[MAX_NAME];

    if(!this->already_added(new_tid))
    {
        d_print(3, "Adding thread: 0x%08x\n", new_tid);
        unsigned new_tid_pos = this->tid_count;
        this->tids[new_tid] = new_tid_pos;
        this->ctx_info[new_tid_pos].tid = new_tid;
        this->tid_count++;
    }

    this->update_watchpoints(new_tid);

    this->reg_store_32(EAX, ctx_out.ctx.Eax, new_tid);
    this->reg_store_32(ECX, ctx_out.ctx.Ecx, new_tid);
    this->reg_store_32(EDX, ctx_out.ctx.Edx, new_tid);
    this->reg_store_32(EBX, ctx_out.ctx.Ebx, new_tid);
    this->reg_store_32(ESI, ctx_out.ctx.Esi, new_tid);
    this->reg_store_32(EDI, ctx_out.ctx.Edi, new_tid);
    this->reg_store_32(ESP, ctx_out.ctx.Esp, new_tid);
    this->reg_store_32(EBP, ctx_out.ctx.Ebp, new_tid);
    this->reg_store_32(EFLAGS, ctx_out.ctx.EFlags, new_tid);

    OFFSET addr;
    CONTEXT_INFO* info;

    info = this->get_context_info(new_tid);

    for(i=0x0; i<0x6; i++)
    {
        addr = this->a_calculate_base(ctx_out.ldt[i]);
        d_print(3, "Segment 0x%02x base: 0x%08x\n", i, addr);
        info->seg_map[i] = addr;
    }
    

    this->cur_tid = 0; /* why? */
    return 0x0;
}

int taint_x86::apply_security(DWORD offset, DWORD size)
{
    d_print(1, "Registering security layer: 0x%08x - 0x%08x\n", offset, size);
    this->security_layer[this->security_layer_count].off = offset;
    this->security_layer[this->security_layer_count].size = size;
    this->security_layer_count++;

    return 0x0;
}

int taint_x86::register_syscall(DWORD TID, DWORD syscall_no)
{
    d_print(1, "Syscall: 0x%08x\n", syscall_no);
    return 0x0;
}

int taint_x86::apply_memory(DWORD offset, DWORD size)
{
    if(!this->started)
    {
        return 0x0;
    }

    char* buffer;
    int i;
    size_t read;

    buffer = (char*)malloc(size);
    
    d_print(2, "Position before: 0x%08x\n", ftell(this->mod_file));
    read = fread(buffer, 1, size, this->mod_file);
    d_print(2, "Position after: 0x%08x\n", ftell(this->mod_file));
    d_print(2, "Read 0x%08x bytes\n", read);

    d_print(1, "Trying to apply 0x%08x bytes @ 0x%08x, pos after: 0x%08x\n", size, offset, ftell(this->mod_file));

    if(this->options & OPTION_VERIFY_OOB)
        if(this->verify_oob_offset(offset, this->mem_length) != 0x0) 
            return 0x0;

    d_print(2, "Before:\n");
    this->print_mem(2, offset, 0x10);
    
    for(i=0x0; i<size; i++)
    {
        char found = 0x0;
        unsigned j;

        for(j = 0x0; j< 0x10; j++)
            if((this->bps[j].offset == offset+i) && (this->bps[j].mode & BP_MODE_WRITE))
                found = 1;

        if(found)
        {
            d_print(3, "Breakpoint RW: WRITE at 0x%x\n", offset);
            print_mem(3, offset + i, 0x3);

            if(this->plugin)
                this->plugin->breakpoint_callback(&this->bps[i]);

        }

            //this->propagations[this->current_propagation_count].instruction = this->last_eip;
            //this->propagations[this->current_propagation_count].instr_count = this->last_instr_count;

            this->memory[offset + i].set_BYTE(buffer[i]);
            this->memory[offset + i].set_BYTE_t(0x0);

            //this->attach_current_propagation_m_8(offset + i);

        if(found)
        {
            print_mem(3, offset + i, 0x3);
        }

    }
    
    d_print(2, "After:\n");
    this->print_mem(2, offset, 0x10);

    free(buffer);

}

int taint_x86::add_exception(EXCEPTION_INFO info)
{
    if(this->plugin) this->plugin->handle_exception_callback(info);
    
    this->exceptions[exceptions_count] = info;
    fprintf(stderr, "Exception %08x in TID %08x, instr. no: %d, eip: 0x%08x\n", info.er.ExceptionCode, info.tid, this->current_instr_count, info.er.ExceptionAddress);
    d_print(1, "Exception %08x in TID %08x, instr. no: %d, eip: 0x%08x\n", info.er.ExceptionCode, info.tid, this->current_instr_count, info.er.ExceptionAddress);
    exceptions_count++;
    
    d_print(1, "Contexts during exception:\n");
    //this->print_all_contexts();
    //this->print_all_stacks();
    d_print(1, "---\n");

    d_print(1, "Context taint during exception:\n");
    //this->print_all_t_contexts();
    //this->print_all_t_stacks();
    d_print(1, "---\n");

    // TODO: how to handle EIP?
    return 0x0;
}

int taint_x86::del_thread(DWORD tid)
{
    if(this->plugin) this->plugin->del_thread_callback(tid);

    return 0x0;
}

int taint_x86::del_thread_srsly(DWORD tid)
{
    if(this->plugin) this->plugin->del_thread_srsly_callback(tid);

    d_print(1, "Removing  thread: 0x%08x\n", tid);

    return 0x0;
}

int taint_x86::load_mem_from_file(char* path)
{
    d_print(1, "Loading file: %s\n", path);
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

    d_print(3, "\nDetected size: 0x%x\n", size);
    d_print(3, "Creating taint map\n");

    // allocate space for memory from dump & shadow


    #ifdef MEM_ALLOC_DECLARED_SIZE
    d_print(3, "Trying to allocate %llx bytes for structures\n", sizeof(BYTE_t)*MEM_SIZE);
    this->memory = (BYTE_t*)malloc(sizeof(BYTE_t)*MEM_SIZE);
    #else
    d_print(3, "Trying to allocate %llx bytes for structures\n", sizeof(BYTE_t)*size);
    this->memory = (BYTE_t*)malloc(sizeof(BYTE_t)*size);
    #endif

    if(this->memory == 0x0)
    {
        d_print(3, "Error: 0x%08x\n", errno);
        fprintf(stderr, "Failed to allocate memory\n");
        exit(0x1);
    }

    // read dump file
    read_bytes = 0;

    unsigned int i,j;

    while(read_bytes != size)
    {
        read = fread(tmp_buffer, 1, BUFF_SIZE, this->dump_file);
    
        d_print(3, "Reading chunk: 0x%x - 0x%x\n", read_bytes, read_bytes + read);
        for(i=read_bytes, j=0; i < read_bytes + read; )
            this->memory[i++].set_BYTE(tmp_buffer[j++]);
        
        read_bytes += read;
    }
    d_print(3, "Finished, read total of: 0x%x\n", read_bytes);
        
    fclose(this->dump_file);

    this->mem_length = read_bytes;

    return read;
}

int taint_x86::load_instr_from_file(char* path)
{
    if(path[strlen(path)-1] == 0xd) path[strlen(path)-1] = 0x0;
    d_print(1, "Loading instr file: %s\n", path);
    this->instr_file = fopen(path, "rb");
    if(this->instr_file == 0x0)
    {
        d_print(1, "Error opening instr file: %s, error: 0x%08x\n", path, errno);
        exit(-1);
    }

    d_print(1, "Done\n");

    return 0x0;
}

int taint_x86::open_prompt_file(char* path)
{
    d_print(1, "Opening prompt file: %s\n", path);
    this->prompt_file = fopen(path, "w");
    if(!this->prompt_file) d_print(1, "Error opening file");
    return 0x0;
}

int taint_x86::open_mod_file(char* path)
{
    d_print(1, "Opening mod file: %s\n", path);
    this->mod_file = fopen(path, "rb");
    if(!this->mod_file) d_print(1, "Error opening file");
    return 0x0;
}

int taint_x86::close_files()
{
    if(this->dump_file) fclose(this->dump_file);
    if(this->instr_file) fclose(this->instr_file);
    if(this->prompt_file) fclose(this->prompt_file);
    if(this->mod_file) fclose(this->mod_file);
    return 0x0;
}

int taint_x86::print_bt_buffer(BYTE_t* buf, int scope)
{
    for(int i=0; i< scope; i+=sizeof(BYTE_t))
    {
        d_print(3, "0x%02x ", buf[i].get_BYTE());
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
    reg_propagation_cause_m_32(esp.get_DWORD());
    //val = restore_32(esp);
    restore_32(esp, val);
    d_print(3, "esp read: 0x%x val: 0x%x\n", esp.get_DWORD(), val.get_DWORD());
    d_print(3, "here2 0x%x\n", val);

    // update ESP 
    esp += 0x4;
    reg_store_32(ESP, esp);

    return val;
}

DWORD_t taint_x86::a_peek_32()
{
    DWORD_t val;
    DWORD_t esp;

    // store value at stack
    esp = reg_restore_32(ESP);    
    //val = restore_32(esp);
    restore_32(esp, val);
    d_print(3, "esp read: 0x%x val: 0x%x\n", esp.get_DWORD(), val.get_DWORD());
    d_print(3, "here2 0x%x\n", val);

    return val;
}

int taint_x86::a_check_segment_override()
{
    OFFSET ret = 0x0;

    d_print(3, "Checking segment override 0x%08x\n", this->current_prefixes);
    d_print(3, "this->cur_info: 0x%08x\n", this->cur_info);

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

    switch(byte->get_BYTE())
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

    sib_byte = (sib_ptr)->get_BYTE();
    d_print(3, "SIB byte: 0x%x\n", sib_byte);

    sib_base = (sib_byte >> SIB_BASE_OFF) & SIB_BASE_MASK;
    sib_idx  = (sib_byte >> SIB_IDX_OFF)  & SIB_IDX_MASK;
    sib_ss   = (sib_byte >> SIB_SS_OFF)   & SIB_SS_MASK;

    d_print(3, "SIB base: 0x%x\n", sib_base);
    d_print(3, "SIB idx: 0x%x\n", sib_idx);
    d_print(3, "SIB ss: 0x%x\n", sib_ss);

    // calculating: [base + scale*index ]
    // implement 16/32 switch

    d_print(3, "Current instr length: 0x%0x\n", this->current_instr_length);
    this->current_instr_length += 0x1; /* SIB byte */
    d_print(3, "Current instr length: 0x%0x\n", this->current_instr_length);

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
            //this->current_instr_length = 0x3;
            disp8.from_mem(sib_ptr +1);
            disp32 = 0x0;
            d_print(3, "disp8: 0x%x ", disp8.get_BYTE());
            break;
        case 0x2:
            //this->current_instr_length = 0x6;
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
//                    this->current_instr_length = 0x6;
                    d_print(3, "none ");
                    base = 0x0;
                    disp32.from_mem(sib_ptr +1);
                    disp8 = 0x0;
                    d_print(3, "override, disp32: 0x%x ", disp32.get_DWORD());
                    break;
                case 0x1:
//                    this->current_instr_length = 0x3;
                    d_print(3, "EBP ");
                    base = this->reg_restore_32(EBP);
                    disp8.from_mem(sib_ptr +1);
                    disp32 = 0x0;
                    d_print(3, "override, disp8: 0x%x ", disp8.get_BYTE());
                    break;
                case 0x2:
//                    this->current_instr_length = 0x6;
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
    d_print(3, "Disp8: 0x%x\n", disp8.get_BYTE());
    d_print(3, "Disp32: 0x%x\n", disp32.get_DWORD());

    // reinterpretation for signed
    signed char* disp8p;
    DWORD disp32_reint;
    signed int* disp32p;
//    disp8p = (signed char*)&(disp8.get_BYTE());
    disp8p = (signed char*)&(disp8.val);
    disp32_reint = disp32.get_DWORD();
    disp32p = (signed int*)&(disp32_reint);

    ret = base.get_DWORD() + *disp8p + *disp32p + (scale.get_DWORD() * index.get_DWORD());

    d_print(3, "Final value: 0x%x\n\n", ret);

    return ret; 
}

int taint_x86::attach_current_propagation(BYTE_t* byte)
{
    d_print(3, "Attaching current propagation\n");
    CONTEXT_INFO* info;
    info = this->get_context_info(this->cur_tid);

    PROPAGATION* current_propagation;
    current_propagation = &this->propagations[this->current_propagation_count];

    if(this->find_propagation_result(current_propagation, byte))
    {
        d_print(3, "Result already registered\n");
        return 0x0;
    }

    RESULT* cur_result;
    RESULT* new_result;

    new_result = (RESULT*)malloc(sizeof(RESULT));
    new_result->affected = byte;
    new_result->next = 0x0;

    d_print(3, "New result: 0x%08x, affected byte: 0%08x\n", new_result, new_result->affected);

    if(current_propagation->results == 0x0)
    {
        current_propagation->results = new_result;
    }
    else
    {
        cur_result = current_propagation->results;
        while(cur_result->next)
        {
            cur_result = cur_result->next;
        }
        cur_result->next = new_result;
    }
    d_print(3, "Registered result\n");

    byte->id = this->current_propagation_count;
    
    current_propagation->result_count++;

    return 0x0;
}

int taint_x86::seal_current_propagation()
{
    this->propagations[this->current_propagation_count].instruction = this->current_eip;
    this->propagations[this->current_propagation_count].instr_count = this->current_instr_count;
    this->current_propagation_count++;

    return 0x0;
}

int taint_x86::attach_current_propagation_r_8(OFFSET off)
{
    CONTEXT_INFO* info;
    info = this->get_context_info(this->cur_tid);

    if(off > REG_SIZE)
    {
        d_print(2, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }

    this->attach_current_propagation(&info->registers[off+0]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+0]);
    this->seal_scheduled = 0x1;

    return 0x0;
}

int taint_x86::attach_current_propagation_r_16(OFFSET off)
{
    CONTEXT_INFO* info;
    info = this->get_context_info(this->cur_tid);

    if(off > REG_SIZE)
    {
        d_print(2, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }

    this->attach_current_propagation(&info->registers[off+0]);
    this->attach_current_propagation(&info->registers[off+1]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+0]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+1]);
    this->seal_scheduled = 0x1;

    return 0x0;
}

int taint_x86::attach_current_propagation_r_32(OFFSET off)
{
    CONTEXT_INFO* info;
    info = this->get_context_info(this->cur_tid);

    if(off > REG_SIZE)
    {
        d_print(2, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }

    this->attach_current_propagation(&info->registers[off+0]);
    this->attach_current_propagation(&info->registers[off+1]);
    this->attach_current_propagation(&info->registers[off+2]);
    this->attach_current_propagation(&info->registers[off+3]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+0]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+1]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+2]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+3]);
    this->seal_scheduled = 0x1;

    return 0x0;
}

int taint_x86::attach_current_propagation_m_8(OFFSET off)
{
    if(off > 0x7fffffff)
    {
        d_print(2, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }
    this->attach_current_propagation(&this->memory[off+0]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+0]);
    this->seal_scheduled = 0x1;

    return 0x0;
}

int taint_x86::attach_current_propagation_m_16(OFFSET off)
{
    if(off > 0x7fffffff)
    {
        d_print(2, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }
    this->attach_current_propagation(&this->memory[off+0]);
    this->attach_current_propagation(&this->memory[off+1]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+0]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+1]);
    this->seal_scheduled = 0x1;

    return 0x0;
}

int taint_x86::attach_current_propagation_m_32(OFFSET off)
{
    if(off > 0x7fffffff)
    {
        d_print(2, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }
    this->attach_current_propagation(&this->memory[off+0]);
    this->attach_current_propagation(&this->memory[off+1]);
    this->attach_current_propagation(&this->memory[off+2]);
    this->attach_current_propagation(&this->memory[off+3]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+0]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+1]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+2]);
    d_print(3, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+3]);
    this->seal_scheduled = 0x1;

    return 0x0;
}

int taint_x86::find_propagation_result(PROPAGATION* current_propagation, BYTE_t* affected)
{
    if(current_propagation == 0x0) return 0x0;
    if(current_propagation->results == 0x0) return 0x0;

    unsigned i;
    RESULT* cur_result;
    int found = 0x0;

    for(i=0x0,cur_result=current_propagation->results; i<current_propagation->result_count; i++)
    {
        if(cur_result->affected == affected)
        {
            found = 1;
            break;
        }
        if(cur_result->next) cur_result=cur_result->next;
    }
    return found;
}

int taint_x86::find_propagation_cause(PROPAGATION* current_propagation, unsigned searched_id)
{
    if(current_propagation == 0x0) return 0x0;
    if(current_propagation->causes == 0x0) return 0x0;

    unsigned i;
    CAUSE* cur_cause;
    int found = 0x0;

    for(i=0x0,cur_cause=current_propagation->causes; i<current_propagation->cause_count; i++)
    {
        if(cur_cause->cause_id == searched_id)
        {
            found = 1;
            break;
        }
        if(cur_cause->next) cur_cause=cur_cause->next;
    }
    return found;
}

int taint_x86::reg_propagation_cause(BYTE_t* op)
{
    PROPAGATION* current_propagation;
    current_propagation = &this->propagations[this->current_propagation_count];

    unsigned byte_cause_id = op->get_BYTE_t_id();

    /* is there a real cause? */
    if(byte_cause_id == CAUSE_ID_NONE) 
    {
        d_print(3, "No prior cause\n");
        return 0x0;
    }

    /* after propagation count reset */
    if(byte_cause_id > this->current_propagation_count) 
    {
        d_print(3, "Propagation cause lost, probably due to propagation history reset\n");
        return 0x0;
    }


    d_print(3, "Registering latest propagation %d resulting in BYTE_t*: 0x%08x as a cause\n", byte_cause_id, op);

    if(this->find_propagation_cause(current_propagation, byte_cause_id))
    {
        d_print(3, "Cause already registered\n");
        return 0x0;
    }

    CAUSE* cur_elem;
    CAUSE* new_elem;

    new_elem = (CAUSE*)malloc(sizeof(CAUSE));
    if(new_elem == 0x0)
    {
        fprintf(stderr, "Error allocations memory");
        exit(1);
    }
    new_elem->cause_id = byte_cause_id;
    new_elem->next = 0x0;

    if(op->get_BYTE_t_id()) d_print(4, "Registering non-void cause\n");

    d_print(4, "Test4");
    if(current_propagation->causes == 0x0)
    {
        d_print(4, "Test4.5");
        current_propagation->causes = new_elem;
        d_print(3, "Registered cause with id: %d\n", new_elem->cause_id);
    }
    else
    {
        cur_elem = current_propagation->causes;

        while(cur_elem->next != 0x0)
            cur_elem = cur_elem->next;

        cur_elem->next = new_elem;

        d_print(3, "Registered cause with id: %d\n", new_elem->cause_id);
    }

    d_print(4, "Test5");
    current_propagation->cause_count++;
    d_print(4, "Test6");

    return 0x0;
}

int taint_x86::reg_propagation_cause_r_8(OFFSET off)
{
    CONTEXT_INFO* info;
    info = this->get_context_info(this->cur_tid);

    this->verify_oob_offset(off, REG_SIZE);

    this->reg_propagation_cause(&info->registers[off+0]);
    return 0x0;
}

int taint_x86::reg_propagation_cause_r_16(OFFSET off)
{
    CONTEXT_INFO* info;
    info = this->get_context_info(this->cur_tid);

    this->verify_oob_offset(off, REG_SIZE);

    this->reg_propagation_cause(&info->registers[off+0]);
    this->reg_propagation_cause(&info->registers[off+1]);
    return 0x0;
}

int taint_x86::reg_propagation_cause_r_32(OFFSET off)
{
    CONTEXT_INFO* info;
    info = this->get_context_info(this->cur_tid);

    this->verify_oob_offset(off, REG_SIZE);

    this->reg_propagation_cause(&info->registers[off+0]);
    this->reg_propagation_cause(&info->registers[off+1]);
    this->reg_propagation_cause(&info->registers[off+2]);
    this->reg_propagation_cause(&info->registers[off+3]);
    return 0x0;
}

int taint_x86::reg_propagation_cause_m_8(OFFSET off)
{
    this->reg_propagation_cause(&this->memory[off+0]);

    this->verify_oob_offset(off, this->mem_length);

    return 0x0;
}

int taint_x86::reg_propagation_cause_m_16(OFFSET off)
{
    this->reg_propagation_cause(&this->memory[off+0]);
    this->reg_propagation_cause(&this->memory[off+1]);

    this->verify_oob_offset(off, this->mem_length);

    return 0x0;
}

int taint_x86::reg_propagation_cause_m_32(OFFSET off)
{
    this->reg_propagation_cause(&this->memory[off+0]);
    this->reg_propagation_cause(&this->memory[off+1]);
    this->reg_propagation_cause(&this->memory[off+2]);
    this->reg_propagation_cause(&this->memory[off+3]);

    this->verify_oob_offset(off, this->mem_length);

    return 0x0;
}

int taint_x86::a_decode_modrm(BYTE_t* modrm_byte_ptr, modrm_ptr* r, modrm_ptr* rm, int mode_r = MODE_UNDEFINED, int mode_rm = MODE_UNDEFINED)
{

    /* 
        size is overriden: 
        by modrm & sib bytes
        by prefix override
        by user
    */


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
    modrm_byte = modrm_byte_ptr->get_BYTE();
    //current_instr_length += 1;
    d_print(3, "Modrm byte: 0x%x\n", modrm_byte);

    this->current_instr_length = 0x2;
    d_print(3, "Current instr length: 0x%0x\n", this->current_instr_length);

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
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EAX);

                    rm->offset = this->reg_restore_32(EAX).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ECX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ECX);

                    rm->offset = this->reg_restore_32(ECX).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EDX);

                    rm->offset = this->reg_restore_32(EDX).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EBX);

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
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EBP);

//                    this->current_instr_length += 0x1;
                    this->current_instr_length += 0x4;
                    offset_bp  = modrm_byte_ptr + 1; 
                    off_d.from_mem(offset_bp);
                    rm->offset = off_d.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ESI_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ESI);

                    rm->offset = this->reg_restore_32(ESI).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDI_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EDI);

                    rm->offset = this->reg_restore_32(EDI).get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
            }
            break;

        case MOD_MEM_REG_DISP_8:
            this->current_instr_length += 0x1;
            switch(modrm_rm_part)
            {
                case RM_EAX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EAX);

                    rm->offset = this->reg_restore_32(EAX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ECX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ECX);

                    rm->offset = this->reg_restore_32(ECX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EDX);

                    rm->offset = this->reg_restore_32(EDX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EBX);

                    rm->offset = this->reg_restore_32(EBX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_UNKN: 
                    /* SIB byte follows */
                    // SIB decoder takes care of displacement
                    rm->offset = this->a_decode_sib_mod(modrm_byte_ptr+1, modrm_mod_part);
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBP_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EBP);

                    rm->offset = this->reg_restore_32(EBP).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1); 
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ESI_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ESI);

                    rm->offset = this->reg_restore_32(ESI).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDI_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EDI);

                    rm->offset = this->reg_restore_32(EDI).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
            }
            // adjust offset with displacement 
            if(modrm_rm_part != RM_UNKN)
            {
                d_print(3, "displacement 8: 0x%02x\n", disp8.get_BYTE());
                disp8p = (signed char*)&(disp8.val);
                d_print(3, "off: 0x%08x, displacement 8: 0x%02x\n", rm->offset, *disp8p);
                rm->offset += *disp8p;
                d_print(3, "off: 0x%08x\n", rm->offset);
            }
            break;

        case MOD_MEM_REG_DISP_32:
            this->current_instr_length += 0x4;
            switch(modrm_rm_part)
            {
                case RM_EAX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EAX);

                    rm->offset = this->reg_restore_32(EAX).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ECX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ECX);

                    rm->offset = this->reg_restore_32(ECX).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += *disp32p;
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EDX);

                    rm->offset = this->reg_restore_32(EDX).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EBX);

                    rm->offset = this->reg_restore_32(EBX).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_UNKN: 
                    /* SIB decoder takes care of displacement */
                    /* SIB byte follows */
                    rm->offset = this->a_decode_sib_mod(modrm_byte_ptr+1, modrm_mod_part);
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBP_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EBP);

                    rm->offset = this->reg_restore_32(EBP).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ESI_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ESI);

                    rm->offset = this->reg_restore_32(ESI).get_DWORD();
                    disp32.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp32.get_DWORD();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDI_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EDI);

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
//            this->current_instr_length = 0x2;
            switch(modrm_rm_part)
            {
                case REG_EAX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EAX);

                    rm->offset = EAX;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_ECX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ECX);

                    rm->offset = ECX;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_EDX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EDX);

                    rm->offset = EDX;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_EBX_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EBX);

                    rm->offset = EBX;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_ESP_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ESP);

                    rm->offset = ESP;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_EBP_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EBP);

                    rm->offset = EBP;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_ESI_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(ESI);

                    rm->offset = ESI;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case REG_EDI_BASED: 
                    if(this->options & OPTION_INDEXES_PROPAGATE)
                        reg_propagation_cause_r_32(EDI);

                    rm->offset = EDI;
                    rm->region = MODRM_REG;
                    rm->size   = MODRM_SIZE_32;
                    break;
            }
            break;
    }
      
    /* check if prefix overrides modrm sizes */
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        d_print(3, "Size override\n");
        if(mode_r == MODE_UNDEFINED)
            mode_r = MODE_16;
        if(mode_rm == MODE_UNDEFINED)
            mode_rm = MODE_16;
    }

    /* recaulculation for different modes */
    
    d_print(4, "r decode mode: 0x%08x\n", mode_r);
    d_print(4, "rm decode mode: 0x%08x\n", mode_rm);

    /* isn't r region ALWAYS MODRM_REG? */

    if(mode_r == MODE_16)
    {
        d_print(3, "r.off: 0x%08x\n", r->offset);
        d_print(3, "r.reg: 0x%08x\n", r->region);
        d_print(3, "r.size: 0x%08x\n", r->size);
        d_print(3, "Triggered recalculation 16\n");
        r->offset = this->a_offset_recalculate_16(r->offset);
        r->size = MODRM_SIZE_16;
        d_print(3, "r.off: 0x%08x\n", r->offset);
        d_print(3, "r.reg: 0x%08x\n", r->region);
        d_print(3, "r.size: 0x%08x\n", r->size);
        d_print(3, "\n");
    }
 
    if(mode_r == MODE_8)
    {
        d_print(3, "r.off: 0x%08x\n", r->offset);
        d_print(3, "r.reg: 0x%08x\n", r->region);
        d_print(3, "r.size: 0x%08x\n", r->size);
        d_print(3, "Triggered recalculation 8\n");
        r->offset = this->a_offset_recalculate_8(r->offset);
        r->size = MODRM_SIZE_8;
        d_print(3, "r.off: 0x%08x\n", r->offset);
        d_print(3, "r.reg: 0x%08x\n", r->region);
        d_print(3, "r.size: 0x%08x\n", r->size);
        d_print(3, "\n");
    }

    if(mode_rm == MODE_16)
    {
        d_print(3, "rm.off: 0x%08x\n", r->offset);
        d_print(3, "rm.reg: 0x%08x\n", r->region);
        d_print(3, "rm.size: 0x%08x\n", r->size);
        if(rm->region == MODRM_REG) 
        {
            d_print(3, "Triggered recalculation 16\n");
            rm->offset = this->a_offset_recalculate_16(rm->offset);
        }
        rm->size = MODRM_SIZE_16;
        d_print(3, "rm.off: 0x%08x\n", rm->offset);
        d_print(3, "rm.reg: 0x%08x\n", rm->region);
        d_print(3, "rm.size: 0x%08x\n", rm->size);
        d_print(3, "\n");
    }
 
    if(mode_rm == MODE_8)
    {
        d_print(3, "rm.off: 0x%08x\n", rm->offset);
        d_print(3, "rm.reg: 0x%08x\n", rm->region);
        d_print(3, "rm.size: 0x%08x\n", rm->size);
        if(rm->region == MODRM_REG) 
        {
            d_print(3, "Triggered recalculation 8\n");
            rm->offset = this->a_offset_recalculate_8(rm->offset);
        }
        rm->size = MODRM_SIZE_8;
        d_print(3, "rm.off: 0x%08x\n", rm->offset);
        d_print(3, "rm.reg: 0x%08x\n", rm->region);
        d_print(3, "rm.size: 0x%08x\n", rm->size);
        d_print(3, "\n");
    }

    d_print(4, "r.off: 0x%08x\n", r->offset);
    d_print(4, "r.reg: 0x%08x\n", r->region);
    d_print(4, "r.size: 0x%08x\n", r->size);
    d_print(4, "rm.off: 0x%08x\n", rm->offset);
    d_print(4, "rm.reg: 0x%08x\n", rm->region);
    d_print(4, "rm.size: 0x%08x\n", rm->size);

    // update oper size overrides
    /*
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        r->size = MODRM_SIZE_16;
        rm->size = MODRM_SIZE_16;
    }
    */

    // take care of seg overrides
    d_print(3, "Offset: 0x%08x\n", rm->offset);
    rm->offset += a_check_segment_override();
    d_print(3, "Offset: 0x%08x\n", rm->offset);

    d_print(3, "Current instr length: 0x%0x\n", this->current_instr_length);

    /* register taint propagation for operands */
    /*
    // rm op
    switch(rm->region)
    {
        case MODRM_REG:
            switch(r->size)
            {
                case MODRM_SIZE_8:
                    this->reg_propagation_cause_r_8(rm->offset);
                    break;
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm->offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm->offset);
                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm->size)
            {
                case MODRM_SIZE_8:
                    this->reg_propagation_cause_m_8(rm->offset);
                    break;
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm->offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm->offset);
                    break;
            }
            break;

    }
*/
    return 0x0;
}

OFFSET taint_x86::a_offset_recalculate_16(OFFSET off)
{
    OFFSET off_ret;

    d_print(4, "Before recalculation 16: 0x%08x\n", off_ret);
    switch(off)
    {
        case EAX:
            off_ret = AX;
            break;
        case ECX:
            off_ret = CX;
            break;
        case EBX:
            off_ret = BX;
            break;
        case EDX:
            off_ret = DX;
            break;
        case ESP:
            off_ret = SP;
            break;
        case EBP:
            off_ret = BP;
            break;
        case ESI:
            off_ret = SI;
            break;
        case EDI:
            off_ret = DI;
            break;
    }
    d_print(4, "After recalculation 16: 0x%08x\n", off_ret);
    return off_ret;
}

OFFSET taint_x86::a_offset_recalculate_8(OFFSET off)
{
    OFFSET off_ret;

    d_print(4, "Before recalculation 8: 0x%08x\n", off);
    switch(off)
    {
        case EAX:
            off_ret = AL;
            break;
        case ECX:
            off_ret = CL;
            break;
        case EDX:
            off_ret = DL;
            break;
        case EBX:
            off_ret = BL;
            break;
        case ESP:
            off_ret = AH;
            break;
        case EBP:
            off_ret = BH;
            break;
        case ESI:
            off_ret = CH;
            break;
        case EDI:
            off_ret = DH;
            break;
    }
    d_print(4, "After recalculation 8: 0x%08x\n", off_ret);
    return off_ret;
}

int taint_x86::a_check_sign(BYTE a)
{
    if(a & 0x80)
        return 0x1;
    else 
        return 0x0;
}

int taint_x86::a_check_sign(WORD a)
{
    if(a & 0x800)
        return 0x1;
    else 
        return 0x0;
}

int taint_x86::a_check_sign(DWORD a)
{
    if(a & 0x8000)
        return 0x1;
    else 
        return 0x0;
}

int taint_x86::r_noop(BYTE_t* b)
{
    d_print(3, "Not implemented: 0x%02x\n", b->get_BYTE());
    return 0x0;
}

int taint_x86::r_jb_jc_jnae(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jae_jnb_jnc(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_je_jz(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jne_jnz(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jbe_jna(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_ja_jnbe(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_js(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jns(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jp_jpe(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jnp_jpo(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jl_jnge(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jge_jnl(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jle_jng(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jg_jnle(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_jxx(BYTE_t* b)
{
    return 0x0;
}

int taint_x86::r_noop_un(BYTE_t* b)
{
    d_print(3, "Unnecesarry: 0x%02x\n", b->get_BYTE());
    return 0x0;
}

/* unasigned begin */

/* unasigned end */

// set/clear

int taint_x86::a_check_cf()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    if(!(eflags.get_DWORD() & EFLAGS_CF))
        return 0x0;
    else
        return 0x1;
}

int taint_x86::a_check_pf()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    if(!(eflags.get_DWORD() & EFLAGS_PF))
        return 0x0;
    else
        return 0x1;
}

int taint_x86::a_check_af()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    if(!(eflags.get_DWORD() & EFLAGS_AF))
        return 0x0;
    else
        return 0x1;
}

int taint_x86::a_check_zf()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    if(!(eflags.get_DWORD() & EFLAGS_ZF))
        return 0x0;
    else
        return 0x1;
}
    
int taint_x86::a_check_sf()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    if(!(eflags.get_DWORD() & EFLAGS_SF))
        return 0x0;
    else
        return 0x1;
}

int taint_x86::a_check_if()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    if(!(eflags.get_DWORD() & EFLAGS_IF))
        return 0x0;
    else
        return 0x1;
}

int taint_x86::a_check_df()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    if(!(eflags.get_DWORD() & EFLAGS_DF))
        return 0x0;
    else
        return 0x1;
}

int taint_x86::a_check_of()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    if(!(eflags.get_DWORD() & EFLAGS_OF))
        return 0x0;
    else
        return 0x1;
}
    
    
int taint_x86::a_set_cf()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    eflags |= EFLAGS_CF;
    this->reg_store_32(EFLAGS, eflags);
    return 0x0;
}

int taint_x86::a_set_pf()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    eflags |= EFLAGS_PF;
    this->reg_store_32(EFLAGS, eflags);
    return 0x0;
}

int taint_x86::a_set_af()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    eflags |= EFLAGS_AF;
    this->reg_store_32(EFLAGS, eflags);
    return 0x0;
}

int taint_x86::a_set_zf()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    eflags |= EFLAGS_ZF;
    this->reg_store_32(EFLAGS, eflags);
    return 0x0;
}
    
int taint_x86::a_set_sf()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    eflags |= EFLAGS_SF;
    this->reg_store_32(EFLAGS, eflags);
    return 0x0;
}

int taint_x86::a_set_if()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    eflags |= EFLAGS_IF;
    this->reg_store_32(EFLAGS, eflags);
    return 0x0;
}

int taint_x86::a_set_df()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    eflags |= EFLAGS_DF;
    this->reg_store_32(EFLAGS, eflags);
    return 0x0;
}

int taint_x86::a_set_of()
{
    DWORD_t eflags;
    eflags = this->reg_restore_32(EFLAGS);
    eflags |= EFLAGS_OF;
    this->reg_store_32(EFLAGS, eflags);
    return 0x0;
}
 
int taint_x86::a_clear_cf()
{
    DWORD_t eflags;

    eflags = this->reg_restore_32(EFLAGS);
    d_print(3, "Before clearing CF: 0x%08x\n", eflags);
    eflags = eflags & EFLAGS_CLEAR_CF;
    d_print(3, "After clearing CF: 0x%08x\n", eflags);
    this->reg_store_32(EFLAGS, eflags);

    return 0x0;
}

int taint_x86::a_clear_pf()
{
    DWORD_t eflags;

    eflags = this->reg_restore_32(EFLAGS);
    d_print(3, "Before clearing PF: 0x%08x\n", eflags);
    eflags = eflags & EFLAGS_CLEAR_PF;
    d_print(3, "After clearing PF: 0x%08x\n", eflags);
    this->reg_store_32(EFLAGS, eflags);

    return 0x0;
}

int taint_x86::a_clear_af()
{
    DWORD_t eflags;

    eflags = this->reg_restore_32(EFLAGS);
    d_print(3, "Before clearing AF: 0x%08x\n", eflags);
    eflags = eflags & EFLAGS_CLEAR_AF;
    d_print(3, "After clearing AF: 0x%08x\n", eflags);
    this->reg_store_32(EFLAGS, eflags);

    return 0x0;
}

int taint_x86::a_clear_zf()
{
    DWORD_t eflags;

    eflags = this->reg_restore_32(EFLAGS);
    d_print(3, "Before clearing ZF: 0x%08x\n", eflags);
    eflags = eflags & EFLAGS_CLEAR_ZF;
    d_print(3, "After clearing ZF: 0x%08x\n", eflags);
    this->reg_store_32(EFLAGS, eflags);

    return 0x0;
}

int taint_x86::a_clear_sf()
{
    DWORD_t eflags;

    eflags = this->reg_restore_32(EFLAGS);
    d_print(3, "Before clearing SF: 0x%08x\n", eflags);
    eflags = eflags & EFLAGS_CLEAR_SF;
    d_print(3, "After clearing SF: 0x%08x\n", eflags);
    this->reg_store_32(EFLAGS, eflags);

    return 0x0;
}
    
int taint_x86::a_clear_if()
{
    DWORD_t eflags;

    eflags = this->reg_restore_32(EFLAGS);
    d_print(3, "Before clearing IF: 0x%08x\n", eflags);
    eflags = eflags & EFLAGS_CLEAR_IF;
    d_print(3, "After clearing IF: 0x%08x\n", eflags);
    this->reg_store_32(EFLAGS, eflags);

    return 0x0;
}

int taint_x86::a_clear_df()
{
    DWORD_t eflags;

    eflags = this->reg_restore_32(EFLAGS);
    d_print(3, "Before clearing DF: 0x%08x\n", eflags);
    eflags = eflags & EFLAGS_CLEAR_DF;
    d_print(3, "After clearing DF: 0x%08x\n", eflags);
    this->reg_store_32(EFLAGS, eflags);

    return 0x0;
}

int taint_x86::a_clear_of()
{
    DWORD_t eflags;

    eflags = this->reg_restore_32(EFLAGS);
    d_print(3, "Before clearing OF: 0x%08x\n", eflags);
    eflags = eflags & EFLAGS_CLEAR_OF;
    d_print(3, "After clearing OF: 0x%08x\n", eflags);
    this->reg_store_32(EFLAGS, eflags);

    return 0x0;
}

QWORD taint_x86::a_f_mul_32(DWORD_t a, DWORD_t b)
{
    QWORD res;
    UDWORD a1, b1;

    /* reduce sign */
    a1 = (UDWORD)a.get_DWORD();
    b1 = (UDWORD)b.get_DWORD();

    res = a1 * b1;

    return res;
}

QWORD taint_x86::a_f_imul_32(DWORD_t a, DWORD_t b)
{
    QWORD res;
    DWORD a1, b1;

    /* extend sign */
    a1 = (DWORD)a.get_DWORD();
    b1 = (DWORD)b.get_DWORD();

    res = a1 * b1;

    return res;
}


DWORD taint_x86::a_f_mul_16(WORD_t a, WORD_t b)
{
    DWORD res;
    UWORD a1, b1;

    /* reduce sign */
    a1 = (UWORD)a.get_WORD();
    b1 = (UWORD)b.get_WORD();

    res = a1 * b1;

    return res;
}

DWORD taint_x86::a_f_imul_16(WORD_t a, WORD_t b)
{
    DWORD res;
    WORD a1, b1;

    /* extend sign */
    a1 = (WORD)a.get_WORD();
    b1 = (WORD)b.get_WORD();

    res = a1 * b1;

    return res;
}

WORD taint_x86::a_f_mul_8(BYTE_t a, BYTE_t b)
{
    WORD res;
    BYTE a1, b1;

    /* recuded sign */
    a1 = (BYTE)a.get_BYTE();
    b1 = (BYTE)b.get_BYTE();

    res = a1*b1;

    return res;
}

WORD taint_x86::a_f_imul_8(BYTE_t a, BYTE_t b)
{
    WORD res;
    char a1, b1;

    /* extend sign */
    a1 = (char)a.get_BYTE();
    b1 = (char)b.get_BYTE();

    res = a1*b1;

    return res;
}

DWORD_t taint_x86::a_mul_32(DWORD_t a, DWORD_t b)
{
    DWORD_t res;
    UDWORD a1, b1;

    /* reduce sign */
    a1 = (UDWORD)a.get_DWORD();
    b1 = (UDWORD)b.get_DWORD();

    res.set_DWORD(a1*b1);

    return res;
}

DWORD_t taint_x86::a_imul_32(DWORD_t a, DWORD_t b)
{
    DWORD_t res;
    DWORD a1, b1;

    /* extend sign */
    a1 = (DWORD)a.get_DWORD();
    b1 = (DWORD)b.get_DWORD();

    res.set_DWORD(a1*b1);

    return res;
}

DWORD_t taint_x86::a_div_32(DWORD_t a, DWORD_t b)
{
    DWORD_t res;
    UDWORD a1, b1;

    /* reduce sign */
    a1 = (UDWORD)a.get_DWORD();
    b1 = (UDWORD)b.get_DWORD();

    res.set_DWORD(a1/b1);

    return res;
}

DWORD_t taint_x86::a_idiv_32(DWORD_t a, DWORD_t b)
{
    DWORD_t res;
    DWORD a1, b1;

    /* extend sign */
    a1 = (DWORD)a.get_DWORD();
    b1 = (DWORD)b.get_DWORD();

    res.set_DWORD(a1/b1);

    return res;
}

DWORD_t taint_x86::a_mod_32(DWORD_t a, DWORD_t b)
{
    DWORD_t res;
    UDWORD a1, b1;

    /* reduce sign */
    a1 = (UDWORD)a.get_DWORD();
    b1 = (UDWORD)b.get_DWORD();

    res.set_DWORD(a1%b1);

    return res;
}

DWORD_t taint_x86::a_imod_32(DWORD_t a, DWORD_t b)
{
    DWORD_t res;
    DWORD a1, b1;

    /* extend sign */
    a1 = (DWORD)a.get_DWORD();
    b1 = (DWORD)b.get_DWORD();

    res.set_DWORD(a1%b1);

    return res;
}

int taint_x86::a_rol_32(DWORD_t& a)
{
    DWORD val;
    char bit;

    val = a.get_DWORD();
    bit = val & 0x80000000;

    val <<= 0x1;

    if(bit)
        val | 0x00000001;
    else
        val & 0xfffffffe;
    
    a.set_DWORD(val);

    return 0x0;
}

int taint_x86::a_ror_32(DWORD_t& a)
{
    DWORD val;
    char bit;

    val = a.get_DWORD();
    bit = val & 0x00000001;

    val >>= 0x1;

    if(bit)
        val | 0x80000000;
    else
        val & 0x7fffffff;
    
    a.set_DWORD(val);

    return 0x0;
}

int taint_x86::a_rcl_32(DWORD_t& a)
{
    DWORD val;
    char bit, bit2;

    val = a.get_DWORD();
    bit = val & 0x80000000;
    bit2 = this->a_check_cf();

    val <<= 0x1;

    if(bit2)
        val | 0x00000001;
    else
        val & 0xfffffffe;
    
    if(bit)
        this->a_set_cf();
    else
        this->a_clear_cf();

    a.set_DWORD(val);

    return 0x0;
}

int taint_x86::a_rcr_32(DWORD_t& a)
{
    DWORD val;
    char bit, bit2;

    val = a.get_DWORD();
    bit = val & 0x1;
    bit2 = this->a_check_cf();

    val >>= 0x1;

    if(bit2)
        val | 0x80000000;
    else
        val & 0x7fffffff;
    
    if(bit)
        this->a_set_cf();
    else
        this->a_clear_cf();

    a.set_DWORD(val);

    return 0x0;
}


WORD_t taint_x86::a_mul_16(WORD_t a, WORD_t b)
{
    WORD_t res;
    UWORD a1, b1;

    /* reduce sign */
    a1 = (UWORD)a.get_WORD();
    b1 = (UWORD)b.get_WORD();

    res.set_WORD(a1*b1);

    return res;
}

WORD_t taint_x86::a_imul_16(WORD_t a, WORD_t b)
{
    WORD_t res;
    WORD a1, b1;

    /* extend sign */
    a1 = (WORD)a.get_WORD();
    b1 = (WORD)b.get_WORD();

    res.set_WORD(a1*b1);

    return res;
}

WORD_t taint_x86::a_div_16(WORD_t a, WORD_t b)
{
    WORD_t res;
    UWORD a1, b1;

    /* reduce sign */
    a1 = (UWORD)a.get_WORD();
    b1 = (UWORD)b.get_WORD();

    res.set_WORD(a1/b1);

    return res;
}

WORD_t taint_x86::a_idiv_16(WORD_t a, WORD_t b)
{
    WORD_t res;
    WORD a1, b1;

    /* extend sign */
    a1 = (WORD)a.get_WORD();
    b1 = (WORD)b.get_WORD();

    res.set_WORD(a1/b1);

    return res;
}

WORD_t taint_x86::a_mod_16(WORD_t a, WORD_t b)
{
    WORD_t res;
    UWORD a1, b1;

    /* reduce sign */
    a1 = (UWORD)a.get_WORD();
    b1 = (UWORD)b.get_WORD();

    res.set_WORD(a1%b1);

    return res;
}

WORD_t taint_x86::a_imod_16(WORD_t a, WORD_t b)
{
    WORD_t res;
    WORD a1, b1;

    /* extend sign */
    a1 = (WORD)a.get_WORD();
    b1 = (WORD)b.get_WORD();

    res.set_WORD(a1%b1);

    return res;
}

int taint_x86::a_rol_16(WORD_t& a)
{
    WORD val;
    char bit;

    val = a.get_WORD();
    bit = val & 0x8000;

    val <<= 0x1;

    if(bit)
        val | 0x0001;
    else
        val & 0xfffe;
    
    a.set_WORD(val);

    return 0x0;
}

int taint_x86::a_ror_16(WORD_t& a)
{
    WORD val;
    char bit;

    val = a.get_WORD();
    bit = val & 0x0001;

    val >>= 0x1;

    if(bit)
        val | 0x8000;
    else
        val & 0x7fff;
    
    a.set_WORD(val);

    return 0x0;
}

int taint_x86::a_rcl_16(WORD_t& a)
{
    WORD val;
    char bit, bit2;

    val = a.get_WORD();
    bit = val & 0x8000;
    bit2 = this->a_check_cf();

    val <<= 0x1;

    if(bit2)
        val | 0x0001;
    else
        val & 0xfffe;
    
    if(bit)
        this->a_set_cf();
    else
        this->a_clear_cf();

    a.set_WORD(val);

    return 0x0;
}

int taint_x86::a_rcr_16(WORD_t& a)
{
    WORD val;
    char bit, bit2;

    val = a.get_WORD();
    bit = val & 0x1;
    bit2 = this->a_check_cf();

    val >>= 0x1;

    if(bit2)
        val | 0x8000;
    else
        val & 0x7fff;
    
    if(bit)
        this->a_set_cf();
    else
        this->a_clear_cf();

    a.set_WORD(val);

    return 0x0;
}

BYTE_t taint_x86::a_mul_8(BYTE_t a, BYTE_t b)
{
    BYTE_t res;
    BYTE a1, b1;

    /* recuded sign */
    a1 = (BYTE)a.get_BYTE();
    b1 = (BYTE)b.get_BYTE();

    res.set_BYTE(a1*b1);

    return res;
}

BYTE_t taint_x86::a_imul_8(BYTE_t a, BYTE_t b)
{
    BYTE_t res;
    char a1, b1;

    /* extend sign */
    a1 = (char)a.get_BYTE();
    b1 = (char)b.get_BYTE();

    res.set_BYTE(a1*b1);

    return res;
}

BYTE_t taint_x86::a_div_8(BYTE_t a, BYTE_t b)
{
    BYTE_t res;
    BYTE a1, b1;

    /* reduced sign */
    a1 = (BYTE)a.get_BYTE();
    b1 = (BYTE)b.get_BYTE();

    res.set_BYTE(a1/b1);

    return res;
}

BYTE_t taint_x86::a_idiv_8(BYTE_t a, BYTE_t b)
{
    BYTE_t res;
    char a1, b1;

    /* extend sign */
    a1 = (char)a.get_BYTE();
    b1 = (char)b.get_BYTE();

    res.set_BYTE(a1/b1);

    return res;
}

BYTE_t taint_x86::a_mod_8(BYTE_t a, BYTE_t b)
{
    BYTE_t res;
    BYTE a1, b1;

    /* reduced sign */
    a1 = (BYTE)a.get_BYTE();
    b1 = (BYTE)b.get_BYTE();

    res.set_BYTE(a1%b1);

    return res;
}

BYTE_t taint_x86::a_imod_8(BYTE_t a, BYTE_t b)
{
    BYTE_t res;
    char a1, b1;

    /* extend sign */
    a1 = (char)a.get_BYTE();
    b1 = (char)b.get_BYTE();

    res.set_BYTE(a1%b1);

    return res;
}

int taint_x86::a_rol_8(BYTE_t& a)
{
    BYTE val;
    char bit;

    val = a.get_BYTE();
    bit = val & 0x80;

    val <<= 0x1;

    if(bit)
        val | 0x01;
    else
        val & 0xfe;
    
    a.set_BYTE(val);

    return 0x0;
}

int taint_x86::a_ror_8(BYTE_t& a)
{
    BYTE val;
    char bit;

    val = a.get_BYTE();
    bit = val & 0x01;

    val >>= 0x1;

    if(bit)
        val | 0x80;
    else
        val & 0x7f;
    
    a.set_BYTE(val);
    return 0x0;
}

int taint_x86::a_rcl_8(BYTE_t& a)
{
    BYTE val;
    char bit, bit2;

    val = a.get_BYTE();
    bit = val & 0x80;
    bit2 = this->a_check_cf();

    val <<= 0x1;

    if(bit2)
        val | 0x01;
    else
        val & 0xfe;
    
    if(bit)
        this->a_set_cf();
    else
        this->a_clear_cf();

    a.set_BYTE(val);

    return 0x0;
}

int taint_x86::a_rcr_8(BYTE_t& a)
{
    BYTE val;
    char bit, bit2;

    val = a.get_BYTE();
    bit = val & 0x1;
    bit2 = this->a_check_cf();

    val >>= 0x1;

    if(bit2)
        val | 0x80;
    else
        val & 0x7f;
    
    if(bit)
        this->a_set_cf();
    else
        this->a_clear_cf();

    a.set_BYTE(val);

    return 0x0;
}

DWORD_t taint_x86::a_mul_32(DWORD_t a, DWORD b)
{
    return  a_mul_32(a, DWORD_t(DWORD(b)));
}

DWORD_t taint_x86::a_imul_32(DWORD_t a, DWORD b)
{
    return  a_imul_32(a, DWORD_t(DWORD(b)));
}

DWORD_t taint_x86::a_div_32(DWORD_t a, DWORD b)
{
    return  a_div_32(a, DWORD_t(DWORD(b)));
}

DWORD_t taint_x86::a_idiv_32(DWORD_t a, DWORD b)
{
    return  a_idiv_32(a, DWORD_t(DWORD(b)));
}

DWORD_t taint_x86::a_mod_32(DWORD_t a, DWORD b)
{
    return  a_mod_32(a, DWORD_t(DWORD(b)));
}

DWORD_t taint_x86::a_imod_32(DWORD_t a, DWORD b)
{
    return a_imod_32(a, DWORD_t(DWORD(b)));
}

WORD_t taint_x86::a_mul_16(WORD_t a, WORD b)
{
    return a_mul_16(a, WORD_t(WORD(b)));
}

WORD_t taint_x86::a_imul_16(WORD_t a, WORD b)
{
    return a_imul_16(a, WORD_t(WORD(b)));
}

WORD_t taint_x86::a_div_16(WORD_t a, WORD b)
{
    return a_div_16(a, WORD_t(WORD(b)));
}

WORD_t taint_x86::a_idiv_16(WORD_t a, WORD b)
{
    return a_idiv_16(a, WORD_t(WORD(b)));
}

WORD_t taint_x86::a_mod_16(WORD_t a, WORD b)
{
    return a_mod_16(a, WORD_t(WORD(b)));
}

WORD_t taint_x86::a_imod_16(WORD_t a, WORD b)
{
    return a_imod_16(a, WORD_t(WORD(b)));
}

BYTE_t taint_x86::a_mul_8(BYTE_t a, BYTE b)
{
    return a_mul_8(a, BYTE_t(b));
}

BYTE_t taint_x86::a_imul_8(BYTE_t a, BYTE b)
{
    return a_imul_8(a, BYTE_t(b));
}

BYTE_t taint_x86::a_div_8(BYTE_t a, BYTE b)
{
    return a_div_8(a, BYTE_t(b));
}

BYTE_t taint_x86::a_idiv_8(BYTE_t a, BYTE b)
{
    return a_idiv_8(a, BYTE_t(b));
}

BYTE_t taint_x86::a_mod_8(BYTE_t a, BYTE b)
{
    return a_mod_8(a, BYTE_t(b));
}

BYTE_t taint_x86::a_imod_8(BYTE_t a, BYTE b)
{
    return a_imod_8(a, BYTE_t(b));
}


/* ordered implementation begin */

int taint_x86::r_add_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 += src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 += src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() < src_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == src_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_add_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 += src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 += src_32;
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 += src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 += src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < src_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < src_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_add_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 += src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            this->restore_8(rm.offset, src_8);
            dst_8 += src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() < src_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == src_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_add_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 += src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_r_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    src_32 = this->reg_restore_32(rm.offset);
                    dst_32 += src_32;
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, src_16);
                    dst_16 += src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_m_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    this->restore_32(rm.offset, src_32);
                    dst_32 += src_32;
                    this->reg_store_32(r.offset, dst_32);
 
                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
    }

    /* flags */
    switch(r.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < src_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < src_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_add_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t dst_8;
    BYTE_t src_8;

    this->reg_propagation_cause_r_8(AL);
    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    dst_8 = this->reg_restore_8(AL);
    src_8.from_mem(instr_ptr + this->current_instr_length);
    dst_8 += src_8;
    this->reg_store_8(AL, dst_8);
    
    this->attach_current_propagation_r_8(AL);
    
    /* flags */
    if(dst_8.get_BYTE() < src_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == src_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_add_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t dst_16, src_16;
    DWORD_t dst_32, src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 += src_16;

        this->reg_store_16(AX, dst_16);
        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_32 = this->reg_restore_32(EAX);
        src_32.from_mem(instr_ptr +1);
        d_print(3, "Imm32: 0x%08x\n", src_32.get_DWORD());
        dst_32 += src_32;
        d_print(3, "dst_32: 0x%08x\n", dst_32.get_DWORD());

        this->reg_store_32(EAX, dst_32);
        this->attach_current_propagation_r_32(EAX);
    }

    /* flags */
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
        if(dst_16.get_WORD() < src_16.get_WORD()) 
            this->a_set_cf(); 
        else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();
    else
        if(dst_32.get_DWORD() < src_32.get_DWORD()) 
            this->a_set_cf(); 
        else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();
            

    return 0x0;
}

// flags implemented ends

int taint_x86::r_or_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 |= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 |= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_or_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 |= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 |= src_32;
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 |= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 |= src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_or_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 |= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->reg_store_8(r.offset, dst_8);

            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            this->restore_8(rm.offset, src_8);
            dst_8 |= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->reg_store_8(r.offset, dst_8);

            break;
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_or_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 |= src_16;
                    this->reg_store_16(r.offset, dst_16);
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_r_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
                    src_32 = this->reg_restore_32(rm.offset);
                    dst_32 |= src_32;
                    this->reg_store_32(r.offset, dst_32);
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());
                    this->restore_16(rm.offset, src_16);
                    dst_16 |= src_16;
                    this->reg_store_16(r.offset, dst_16);
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_m_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
                    this->restore_32(rm.offset, src_32);
                    dst_32 |= src_32;
                    this->reg_store_32(r.offset, dst_32);
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_or_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t al;
    BYTE_t src_8;

    this->reg_propagation_cause_r_8(AL);
    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    al = this->reg_restore_8(AL);
    src_8.from_mem(instr_ptr + this->current_instr_length);
    al |= src_8;
    this->reg_store_8(AL, al);
    
    this->attach_current_propagation_r_8(AL);
    
    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_or_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t dst_16, src_16;
    DWORD_t dst_32, src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr +1);
        dst_16 |= src_16;
        this->reg_store_16(AX, dst_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_32 = this->reg_restore_32(EAX);
        src_32.from_mem(instr_ptr +1);
        d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
        dst_32 |= src_32;
        d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());
        this->reg_store_32(EAX, dst_32);

        this->attach_current_propagation_r_32(EAX);
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_extend(BYTE_t* addr)
{
    this->extended = 0x1;

    d_print(3, "Extending\n");
    this->execute_instruction(this->current_eip + this->current_prefix_length +1, this->cur_tid);

    return 0x0;
}

int taint_x86::r_adc_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 += (src_8 + this->a_check_cf());
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 += (src_8 + this->a_check_cf());
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() < src_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == src_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_adc_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 += (src_32 + this->a_check_cf());
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 += (src_32 + this->a_check_cf());
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < src_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < src_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }
    return 0x0;
}

int taint_x86::r_adc_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 += (src_8 + this->a_check_cf());
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            this->restore_8(rm.offset, src_8);
            dst_8 += (src_8 + this->a_check_cf());
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() < src_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == src_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_adc_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_r_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    src_32 = this->reg_restore_32(rm.offset);
                    dst_32 += (src_32 + this->a_check_cf());
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, src_16);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_m_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    this->restore_32(rm.offset, src_32);
                    dst_32 += (src_32 + this->a_check_cf());
                    this->reg_store_32(r.offset, dst_32);
 
                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
    }

    /* flags */
    switch(r.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < src_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < src_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_adc_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t dst_8;
    BYTE_t src_8;

    this->reg_propagation_cause_r_8(AL);
    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    dst_8 = this->reg_restore_8(AL);
    src_8.from_mem(instr_ptr + this->current_instr_length);
    dst_8 += (src_8 + this->a_check_cf());
    this->reg_store_8(AL, dst_8);
    
    this->attach_current_propagation_r_8(AL);
    
    /* flags */
    if(dst_8.get_BYTE() < src_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == src_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_adc_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t dst_16, src_16;
    DWORD_t dst_32, src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 += (src_16 + this->a_check_cf());
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        this->reg_store_16(AX, dst_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_32 = this->reg_restore_32(EAX);
        src_32.from_mem(instr_ptr +1);
        d_print(3, "Imm32: 0x%08x\n", src_32.get_DWORD());
        dst_32 += (src_32 + this->a_check_cf());
        d_print(3, "dst_32: 0x%08x\n", dst_32.get_DWORD());
        this->reg_store_32(EAX, dst_32);

        this->attach_current_propagation_r_32(EAX);
    }

    /* flags */
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
        if(dst_16.get_WORD() < src_16.get_WORD()) 
            this->a_set_cf(); 
        else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();
    else
        if(dst_32.get_DWORD() < src_32.get_DWORD()) 
            this->a_set_cf(); 
        else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_sbb_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8 = (this->reg_restore_8(r.offset) + this->a_check_cf());
            dst_8 -= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8 = (this->reg_restore_8(r.offset) + this->a_check_cf());
            dst_8 -= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();
        

    return 0x0;
}

int taint_x86::r_sbb_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16 = (this->reg_restore_16(r.offset) + this->a_check_cf());
                    dst_16 -= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_32 = (this->reg_restore_32(r.offset) + this->a_check_cf());
                    dst_32 -= src_32;
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16 = (this->reg_restore_16(r.offset) + this->a_check_cf());
                    dst_16 -= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_32 = (this->reg_restore_32(r.offset) + this->a_check_cf());
                    dst_32 -= src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t src_8;
    BYTE_t dst_8, old_dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            old_dst_8 = dst_8;
            src_8 = (this->reg_restore_8(rm.offset) + this->a_check_cf());
            d_print(3, "Before: 0x%08x\n", dst_8.get_BYTE());
            dst_8 -= src_8;
            d_print(3, "After: 0x%08x\n", dst_8.get_BYTE());
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            old_dst_8 = dst_8;
            this->restore_8(rm.offset, src_8);
            src_8 += this->a_check_cf();
            d_print(3, "Before: 0x%08x\n", dst_8.get_BYTE());
            dst_8 -= src_8;
            d_print(3, "After: 0x%08x\n", dst_8.get_BYTE());
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_sbb_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    src_16 = (this->reg_restore_16(rm.offset) + this->a_check_cf());
                    dst_16 -= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_r_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    old_dst_32 = dst_32;
                    src_32 = (this->reg_restore_32(rm.offset) + this->a_check_cf());
                    dst_32 -= src_32;
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    this->restore_16(rm.offset, src_16);
                    src_16 += this->a_check_cf();
                    dst_16 -= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_m_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    old_dst_32 = dst_32;
                    this->restore_32(rm.offset, src_32);
                    src_32 += this->a_check_cf();
                    dst_32 -= src_32;
                    this->reg_store_32(r.offset, dst_32);
 
                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->reg_propagation_cause_r_8(AL);
    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    dst_8 = this->reg_restore_8(AL);
    old_dst_8 = dst_8;
    src_8.from_mem(instr_ptr + this->current_instr_length);
    src_8 += this->a_check_cf();
    dst_8 -= src_8;
    this->reg_store_8(AL, dst_8);
    
    this->attach_current_propagation_r_8(AL);
    
    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_sbb_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t dst_16, old_dst_16, src_16;
    DWORD_t dst_32, old_dst_32, src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        old_dst_16 = dst_16;
        src_16.from_mem(instr_ptr +1);
        src_16 += this->a_check_cf();
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 -= src_16;

        this->attach_current_propagation_r_16(AX);
        this->reg_store_16(AX, dst_16);
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_32 = this->reg_restore_32(EAX);
        old_dst_32 = dst_32;
        src_32.from_mem(instr_ptr +1);
        src_32 += this->a_check_cf();
        d_print(3, "Imm32: 0x%08x\n", src_32.get_DWORD());
        dst_32 -= src_32;
        d_print(3, "dst_32: 0x%08x\n", dst_32.get_DWORD());

        this->attach_current_propagation_r_32(EAX);
        this->reg_store_32(EAX, dst_32);
    }

    /* flags */
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
        if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
            this->a_set_cf();
        else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();
    else
        if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
            this->a_set_cf();
        else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_and_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(r.offset);
            dst_8 &= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(r.offset);
            dst_8 &= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_and_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    d_print(3, "0x%08x\n", dst_16.get_WORD());
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 &= src_16;
                    d_print(3, "0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    d_print(3, "0x%08x\n", dst_32.get_DWORD());
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 &= src_32;
                    d_print(3, "0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    d_print(3, "0x%08x\n", dst_16.get_WORD());
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 &= src_16;
                    d_print(3, "0x%08x\n", dst_16.get_WORD());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    d_print(3, "0x%08x\n", dst_32.get_DWORD());
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 &= src_32;
                    d_print(3, "0x%08x\n", dst_32.get_DWORD());
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_and_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 &= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            this->restore_8(rm.offset, src_8);
            dst_8 &= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}


int taint_x86::r_and_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 &= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_r_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(rm.offset);
                    dst_32 &= src_32;
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    this->restore_16(rm.offset, src_16);
                    dst_16 &= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_m_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    old_dst_32 = dst_32;
                    this->restore_32(rm.offset, src_32);
                    d_print(3, "Operands: 0x%08x & 0x%08x\n", dst_32.get_DWORD(), src_32.get_DWORD());
                    dst_32 &= src_32;
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_and_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t al;
    BYTE_t src_8;

    this->reg_propagation_cause_r_8(AL);
    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    al = this->reg_restore_8(AL);
    src_8.from_mem(instr_ptr + this->current_instr_length);
    al &= src_8;
    this->reg_store_8(AL, al);
    
    this->attach_current_propagation_r_8(AL);
    
    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_and_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t dst_16, src_16;
    DWORD_t dst_32, src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr + this->current_instr_length);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 &= src_16;
        this->reg_store_16(AX, dst_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        dst_32 = dst_16;
        src_32.from_mem(instr_ptr + this->current_instr_length);
        d_print(3, "Imm32: 0x%08x\n", src_32.get_DWORD());
        dst_32 &= src_32;
        d_print(3, "dst_32: 0x%08x\n", dst_32.get_DWORD());
        this->reg_store_32(EAX, dst_32);

        this->attach_current_propagation_r_32(EAX);
    }

    /* flags */
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_es_override(BYTE_t*)
{
    this->current_prefixes |= PREFIX_ES_OVERRIDE;
    d_print(3, "ES override\n");
    return 0x0;
}

int taint_x86::r_sub_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(r.offset);
            dst_8 -= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(r.offset);
            dst_8 -= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_sub_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 -= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 -= src_32;
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 -= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 -= src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == old_dst_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_sub_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 -= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            old_dst_8 = dst_8;
            this->restore_8(rm.offset, src_8);
            dst_8 -= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_sub_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(rm.offset);
                    d_print(3, "Substracting: 0x%08x - 0x%08x\n", dst_16.get_WORD(), src_16.get_WORD());
                    dst_16 -= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_r_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(rm.offset);
                    d_print(3, "Substracting: 0x%08x - 0x%08x\n", dst_32.get_DWORD(), src_32.get_DWORD());
                    dst_32 -= src_32;
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    this->restore_16(rm.offset, src_16);
                    d_print(3, "Substracting: 0x%08x - 0x%08x\n", dst_16.get_WORD(), src_16.get_WORD());
                    dst_16 -= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_m_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    old_dst_32 = dst_32;
                    this->restore_32(rm.offset, src_32);
                    d_print(3, "Substracting: 0x%08x - 0x%08x\n", dst_32.get_DWORD(), src_32.get_DWORD());
                    dst_32 -= src_32;
                    this->reg_store_32(r.offset, dst_32);
 
                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == old_dst_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_sub_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->reg_propagation_cause_r_8(AL);
    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    dst_8 = this->reg_restore_8(AL);
    old_dst_8 = dst_8;
    src_8.from_mem(instr_ptr + this->current_instr_length);
    dst_8 -= src_8;
    this->reg_store_8(AL, dst_8);
    
    this->attach_current_propagation_r_8(AL);
    
    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_sub_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t dst_16, old_dst_16, src_16;
    DWORD_t dst_32, old_dst_32, src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        old_dst_16 = dst_16;
        src_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 -= src_16;
        this->reg_store_16(AX, dst_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_32 = this->reg_restore_32(EAX);
        old_dst_32 = dst_32;
        src_32.from_mem(instr_ptr +1);
        d_print(3, "Imm32: 0x%08x\n", src_32.get_DWORD());
        dst_32 -= src_32;
        d_print(3, "dst_32: 0x%08x\n", dst_32.get_DWORD());
        this->reg_store_32(EAX, dst_32);

        this->attach_current_propagation_r_32(EAX);
    }

    /* flags */
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
        if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
            this->a_set_cf();
        else if((dst_16.get_WORD() == old_dst_16.get_WORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();
    else
        if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
            this->a_set_cf();
        else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_cs_override(BYTE_t*)
{
    this->current_prefixes |= PREFIX_CS_OVERRIDE;
    d_print(3, "CS override\n");
    return 0x0;
}

int taint_x86::r_xor_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case MODRM_REG:
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            d_print(3, "Before 0x%08x\n", dst_8.get_BYTE());
            dst_8 ^= reg_restore_8(r.offset);
            d_print(3, "After 0x%08x\n", dst_8.get_BYTE());
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            d_print(3, "Before 0x%08x\n", dst_8.get_BYTE());
            dst_8 ^= reg_restore_8(r.offset);
            d_print(3, "After 0x%08x\n", dst_8.get_BYTE());
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }
    d_print(3, "Result: 0x%08x\n", dst_8.get_BYTE());
    
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_xor_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16, dst_16;
    DWORD_t src_32, dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 = this->reg_restore_16(rm.offset);
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= src_16;
                    d_print(3, "After 0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 = this->reg_restore_32(rm.offset);
                    d_print(3, "Before 0x%08x\n", dst_32.get_DWORD());
                    dst_32 ^= src_32;
                    d_print(3, "After 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    src_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, dst_16);
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= src_16;
                    d_print(3, "After 0x%08x\n", dst_16.get_WORD());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    src_32 = this->reg_restore_32(r.offset);
                    this->restore_32(rm.offset, dst_32);
                    d_print(3, "Before 0x%08x\n", dst_32.get_DWORD());
                    dst_32 ^= src_32;
                    d_print(3, "After 0x%08x\n", dst_32.get_DWORD());
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }
//    this->print_mem(3, rm.offset, 0x10);

    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_xor_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t src_8, dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case MODRM_REG:
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = reg_restore_8(rm.offset);

            d_print(3, "Before 0x%08x\n", dst_8.get_BYTE());
            dst_8 ^= src_8;
            d_print(3, "After 0x%08x\n", dst_8.get_BYTE());

            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_cause_r_8(r.offset);
            this->reg_propagation_cause_m_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            this->restore_8(rm.offset, src_8);

            d_print(3, "Before 0x%08x\n", dst_8.get_BYTE());
            dst_8 ^= src_8;
            d_print(3, "After 0x%08x\n", dst_8.get_BYTE());

            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
    }
    d_print(3, "Result: 0x%08x\n", dst_8.get_BYTE());
    
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_xor_r_rm(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16, dst_16;
    DWORD_t src_32, dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 = this->reg_restore_16(r.offset);
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= src_16;
                    d_print(3, "After 0x%08x\n", dst_16.get_WORD());

                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    src_32 = this->reg_restore_32(rm.offset);
                    dst_32 = this->reg_restore_32(r.offset);
                    d_print(3, "Before 0x%08x\n", dst_32.get_DWORD());
                    dst_32 ^= src_32;
                    d_print(3, "After 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);

                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, src_16);
                    dst_16 = this->reg_restore_16(r.offset);
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= src_16;
                    d_print(3, "After 0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, src_32);
                    dst_32 = this->reg_restore_32(r.offset);
                    d_print(3, "Before 0x%08x\n", dst_32.get_DWORD());
                    dst_32 ^= src_32;
                    d_print(3, "After 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);

                    break;
            }
            break;
    }

    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_xor_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t al;
    BYTE_t src_8;

    this->reg_propagation_cause_r_8(AL);
    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    al = this->reg_restore_8(AL);
    src_8.from_mem(instr_ptr + this->current_instr_length);
    al ^= src_8;
    this->reg_store_8(AL, al);

    this->attach_current_propagation_r_8(AL);
    
    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_xor_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t dst_16, src_16;
    DWORD_t dst_32, src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr + this->current_instr_length);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 ^= src_16;
        this->reg_store_16(AX, dst_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_32 = this->reg_restore_32(EAX);
        src_32.from_mem(instr_ptr + this->current_instr_length);
        d_print(3, "Imm32: 0x%08x\n", src_32.get_DWORD());
        dst_32 ^= src_32;
        d_print(3, "dst_32: 0x%08x\n", dst_32.get_DWORD());
        this->reg_store_32(EAX, dst_32);

        this->attach_current_propagation_r_32(EAX);
    }

    this->a_clear_cf();
    return 0x0;

}

int taint_x86::r_xor_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);

            dst_8 ^= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_m_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            this->restore_8(rm.offset, dst_8);
            src_8.from_mem(instr_ptr + this->current_instr_length);

            dst_8 ^= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }
    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_xor_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);

                    dst_16 ^= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 ^= src_32;
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 ^= src_16;
                    this->store_16(rm.offset, dst_16);
                    
                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_32(rm.offset, dst_32);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 ^= src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
    }
    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_xor_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16, val_16;
    DWORD_t dst_32, old_dst_32, val_32;
    BYTE_t src_8;
    WORD temp_16;
    DWORD temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= val_16;
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
                    dst_32 ^= val_32;
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_16(rm.offset, dst_16);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= val_16;
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_32(rm.offset, dst_32);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
                    dst_32 ^= val_32;
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }
    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_ss_override(BYTE_t*)
{
    this->current_prefixes |= PREFIX_SS_OVERRIDE;
    d_print(3, "SS override\n");
    return 0x0;
}

int taint_x86::r_cmp_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(r.offset);
            d_print(3, "Compare %02x and %02x\n", src_8.get_BYTE(), dst_8.get_BYTE());
            dst_8 -= src_8;
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(r.offset);
            d_print(3, "Compare %02x and %02x\n", src_8.get_BYTE(), dst_8.get_BYTE());
            dst_8 -= src_8;
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_cmp_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    r.offset = this->a_offset_recalculate_16(r.offset);
//                    rm.offset = this->a_offset_recalculate_16(rm.offset);
                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 -= src_16;
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 -= src_32;
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    r.offset = this->a_offset_recalculate_16(r.offset);
                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 -= src_16;
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(r.offset);
                    dst_32 -= src_32;
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_cmp_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(r.offset);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 -= src_8;
            break;
        case (MODRM_MEM):
            dst_8 = this->reg_restore_8(r.offset);
            old_dst_8 = dst_8;
            this->restore_8(rm.offset, src_8);
            dst_8 -= src_8;
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_cmp_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 -= src_16;
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(r.offset);
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(rm.offset);
                    dst_32 -= src_32;
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    this->restore_16(rm.offset, src_16);
                    dst_16 -= src_16;
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(r.offset);
                    old_dst_32 = dst_32;
                    this->restore_32(rm.offset, src_32);
                    dst_32 -= src_32;
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_cmp_al_imm_8(BYTE_t* instr_ptr)
{
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    dst_8 = this->reg_restore_8(AL);
    old_dst_8 = dst_8;
    src_8.from_mem(instr_ptr + this->current_instr_length);
    dst_8 -= src_8;
    
    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_cmp_ax_eax_imm_16_32(BYTE_t* instr_ptr)
{
    WORD_t dst_16, src_16;
    DWORD_t dst_32, src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 += src_16;
    }
    else
    {
        dst_32 = this->reg_restore_32(EAX);
        src_32.from_mem(instr_ptr +1);
        d_print(3, "Imm32: 0x%08x\n", src_32.get_DWORD());
        dst_32 += src_32;
        d_print(3, "dst_32: 0x%08x\n", dst_32.get_DWORD());
    }

    /* flags */
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
        if(dst_16.get_WORD() < src_16.get_WORD()) 
            this->a_set_cf(); 
        else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();
    else
        if(dst_32.get_DWORD() < src_32.get_DWORD()) 
            this->a_set_cf(); 
        else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
            this->a_set_cf();
        else
            this->a_clear_cf();
            
    return 0x0;
}

int taint_x86::r_ds_override(BYTE_t*)
{
    this->current_prefixes |= PREFIX_DS_OVERRIDE;
    d_print(3, "DS override\n");
    return 0x0;
}

int taint_x86::r_inc_ax_eax(BYTE_t*)
{
    DWORD_t eax;
    WORD_t ax;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        ax = this->reg_restore_16(AX);
        ax += 1;
        this->reg_store_16(AX, ax);    
    }
    else
    {
        eax = this->reg_restore_32(EAX);
        eax += 1;
        this->reg_store_32(EAX, eax);    
    }

    return 0x0;
}

int taint_x86::r_inc_cx_ecx(BYTE_t*)
{
    DWORD_t ecx;
    WORD_t cx;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        cx = this->reg_restore_16(CX);
        cx += 1;
        this->reg_store_16(CX, cx);    
    }
    else
    {
        ecx = this->reg_restore_32(ECX);
        ecx += 1;
        this->reg_store_32(ECX, ecx);    
    }

    return 0x0;
}

int taint_x86::r_inc_dx_edx(BYTE_t*)
{
    DWORD_t edx;
    WORD_t dx;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dx = this->reg_restore_16(DX);
        dx += 1;
        this->reg_store_16(DX, dx);    
    }
    else
    {
        edx = this->reg_restore_32(EDX);
        edx += 1;
        this->reg_store_32(EDX, edx);    
    }

    return 0x0;
}

int taint_x86::r_inc_bx_ebx(BYTE_t*)
{
    DWORD_t ebx;
    WORD_t bx;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        bx = this->reg_restore_16(BX);
        bx += 1;
        this->reg_store_16(BX, bx);    
    }
    else
    {
        ebx = this->reg_restore_32(EBX);
        ebx += 1;
        this->reg_store_32(EBX, ebx);    
    }

    return 0x0;
}

int taint_x86::r_inc_sp_esp(BYTE_t*)
{
    DWORD_t esp;
    WORD_t sp;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        sp = this->reg_restore_16(SP);
        sp += 1;
        this->reg_store_16(SP, sp);    
    }
    else
    {
        esp = this->reg_restore_32(ESP);
        esp += 1;
        this->reg_store_32(ESP, esp);    
    }

    return 0x0;
}

int taint_x86::r_inc_bp_ebp(BYTE_t*)
{
    DWORD_t ebp;
    WORD_t bp;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        bp = this->reg_restore_16(BP);
        bp += 1;
        this->reg_store_16(BP, bp);    
    }
    else
    {
        ebp = this->reg_restore_32(EBP);
        ebp += 1;
        this->reg_store_32(EBP, ebp);    
    }

    return 0x0;
}

int taint_x86::r_inc_si_esi(BYTE_t*)
{
    DWORD_t esi;
    WORD_t si;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        si = this->reg_restore_16(SI);
        si += 1;
        this->reg_store_16(SI, si);    
    }
    else
    {
        esi = this->reg_restore_32(ESI);
        esi += 1;
        this->reg_store_32(ESI, esi);    
    }

    return 0x0;
}

int taint_x86::r_inc_di_edi(BYTE_t*)
{
    DWORD_t edi;
    WORD_t di;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        di = this->reg_restore_16(DI);
        di += 1;
        this->reg_store_16(DI, di);    
    }
    else
    {
        edi = this->reg_restore_32(EDI);
        edi += 1;
        this->reg_store_32(EDI, edi);    
    }

    return 0x0;
}

int taint_x86::r_dec_ax_eax(BYTE_t*)
{
    DWORD_t eax;
    WORD_t ax;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        ax = this->reg_restore_16(AX);
        ax -= 1;
        this->reg_store_16(AX, ax);    
    }
    else
    {
        eax = this->reg_restore_32(EAX);
        eax -= 1;
        this->reg_store_32(EAX, eax);    
    }

    return 0x0;
}

int taint_x86::r_dec_cx_ecx(BYTE_t*)
{
    DWORD_t ecx;
    WORD_t cx;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        cx = this->reg_restore_16(CX);
        cx -= 1;
        this->reg_store_16(CX, cx);    
    }
    else
    {
        ecx = this->reg_restore_32(ECX);
        ecx -= 1;
        this->reg_store_32(ECX, ecx);    
    }

    return 0x0;
}

int taint_x86::r_dec_dx_edx(BYTE_t*)
{
    DWORD_t edx;
    WORD_t dx;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dx = this->reg_restore_16(DX);
        dx -= 1;
        this->reg_store_16(DX, dx);    
    }
    else
    {
        edx = this->reg_restore_32(EDX);
        edx -= 1;
        this->reg_store_32(EDX, edx);    
    }

    return 0x0;
}

int taint_x86::r_dec_bx_ebx(BYTE_t*)
{
    DWORD_t ebx;
    WORD_t bx;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        bx = this->reg_restore_16(BX);
        bx -= 1;
        this->reg_store_16(BX, bx);    
    }
    else
    {
        ebx = this->reg_restore_32(EBX);
        ebx -= 1;
        this->reg_store_32(EBX, ebx);    
    }

    return 0x0;
}

int taint_x86::r_dec_sp_esp(BYTE_t*)
{
    DWORD_t esp;
    WORD_t sp;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        sp = this->reg_restore_16(SP);
        sp -= 1;
        this->reg_store_16(SP, sp);    
    }
    else
    {
        esp = this->reg_restore_32(ESP);
        esp -= 1;
        this->reg_store_32(ESP, esp);    
    }

    return 0x0;
}

int taint_x86::r_dec_bp_ebp(BYTE_t*)
{
    DWORD_t ebp;
    WORD_t bp;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        bp = this->reg_restore_16(BP);
        bp -= 1;
        this->reg_store_16(BP, bp);    
    }
    else
    {
        ebp = this->reg_restore_32(EBP);
        ebp -= 1;
        this->reg_store_32(EBP, ebp);    
    }

    return 0x0;
}

int taint_x86::r_dec_si_esi(BYTE_t*)
{
    DWORD_t esi;
    WORD_t si;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        si = this->reg_restore_16(SI);
        si -= 1;
        this->reg_store_16(SI, si);    
    }
    else
    {
        esi = this->reg_restore_32(ESI);
        esi -= 1;
        this->reg_store_32(ESI, esi);    
    }

    return 0x0;
}

int taint_x86::r_dec_di_edi(BYTE_t*)
{
    DWORD_t edi;
    WORD_t di;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        di = this->reg_restore_16(DI);
        di -= 1;
        this->reg_store_16(DI, di);    
    }
    else
    {
        edi = this->reg_restore_32(EDI);
        edi -= 1;
        this->reg_store_32(EDI, edi);    
    }

    return 0x0;
}

int taint_x86::r_push_ax_eax(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        a_push_16(reg_restore_16(AX));
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        a_push_32(reg_restore_32(EAX));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_cx_ecx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(CX);
        a_push_16(reg_restore_16(CX));
    }
    else
    {
        this->reg_propagation_cause_r_32(ECX);
        a_push_32(reg_restore_32(ECX));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_dx_edx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(DX);
        a_push_16(reg_restore_16(DX));
    }
    else
    {
        this->reg_propagation_cause_r_32(EDX);
        a_push_32(reg_restore_32(EDX));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_bx_ebx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(BX);
        a_push_16(reg_restore_16(BX));
    }
    else
    {
        this->reg_propagation_cause_r_32(EBX);
        a_push_32(reg_restore_32(EBX));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_sp_esp(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(SP);
        a_push_16(reg_restore_16(SP));
    }
    else
    {
        this->reg_propagation_cause_r_32(ESP);
        a_push_32(reg_restore_32(ESP));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_bp_ebp(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(BP);
        a_push_16(reg_restore_16(BP));
    }
    else
    {
        this->reg_propagation_cause_r_32(EBP);
        a_push_32(reg_restore_32(EBP));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_si_esi(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(SI);
        a_push_16(reg_restore_16(SI));
    }
    else
    {
        this->reg_propagation_cause_r_32(ESI);
        a_push_32(reg_restore_32(ESI));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_di_edi(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(DI);
        a_push_16(reg_restore_16(DI));
    }
    else
    {
        this->reg_propagation_cause_r_32(EDI);
        a_push_32(reg_restore_32(EDI));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_pop_ax_eax(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(AX, this->a_pop_16());
        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EAX, this->a_pop_32());
        this->attach_current_propagation_r_32(EAX);
    }
    return 0x0;
}

int taint_x86::r_pop_cx_ecx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(CX, this->a_pop_16());
        this->attach_current_propagation_r_16(CX);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ECX, this->a_pop_32());
        this->attach_current_propagation_r_32(ECX);
    }
    return 0x0;
}

int taint_x86::r_pop_dx_edx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(DX, this->a_pop_16());
        this->attach_current_propagation_r_16(DX);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EDX, this->a_pop_32());
        this->attach_current_propagation_r_32(EDX);
    }
    return 0x0;
}

int taint_x86::r_pop_bx_ebx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(BX, this->a_pop_16());
        this->attach_current_propagation_r_16(BX);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EBX, this->a_pop_32());
        this->attach_current_propagation_r_32(EBX);
    }
    return 0x0;
}

int taint_x86::r_pop_sp_esp(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(SP, this->a_pop_16());
        this->attach_current_propagation_r_16(SP);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ESP, this->a_pop_32());
        this->attach_current_propagation_r_32(ESP);
    }
    return 0x0;
}

int taint_x86::r_pop_bp_ebp(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(BP, this->a_pop_16());
        this->attach_current_propagation_r_16(BP);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EBP, this->a_pop_32());
        this->attach_current_propagation_r_32(EBP);
    }
    return 0x0;
}

int taint_x86::r_pop_si_esi(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(SI, this->a_pop_16());
        this->attach_current_propagation_r_16(SI);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ESI, this->a_pop_32());
        this->attach_current_propagation_r_32(ESI);
    }
    return 0x0;
}

int taint_x86::r_pop_di_edi(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(DI, this->a_pop_16());
        this->attach_current_propagation_r_16(DI);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EDI, this->a_pop_32());
        this->attach_current_propagation_r_32(EDI);
    }
    return 0x0;
}

int taint_x86::r_pusha(BYTE_t* args)
{
    WORD_t temp_16;
    DWORD_t temp_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);
        this->reg_propagation_cause_r_16(CX);
        this->reg_propagation_cause_r_16(DX);
        this->reg_propagation_cause_r_16(BX);
        this->reg_propagation_cause_r_16(SP);
        this->reg_propagation_cause_r_16(BP);
        this->reg_propagation_cause_r_16(SI);
        this->reg_propagation_cause_r_16(DI);

        temp_16 = this->reg_restore_16(SP);

        a_push_16(reg_restore_16(AX));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_16(reg_restore_16(CX));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_16(reg_restore_16(DX));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_16(reg_restore_16(BX));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_16(temp_16);
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_16(reg_restore_16(BP));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_16(reg_restore_16(SI));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_16(reg_restore_16(DI));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);
        this->reg_propagation_cause_r_32(ECX);
        this->reg_propagation_cause_r_32(EDX);
        this->reg_propagation_cause_r_32(EBX);
        this->reg_propagation_cause_r_32(ESP);
        this->reg_propagation_cause_r_32(EBP);
        this->reg_propagation_cause_r_32(ESI);
        this->reg_propagation_cause_r_32(EDI);

        temp_32 = this->reg_restore_32(ESP);

        a_push_32(reg_restore_32(EAX));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_32(reg_restore_32(ECX));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_32(reg_restore_32(EDX));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_32(reg_restore_32(EBX));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_32(temp_32);
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_32(reg_restore_32(EBP));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_32(reg_restore_32(ESI));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
        a_push_32(reg_restore_32(EDI));
        this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());
    }
    return 0x0;
}

int taint_x86::r_popa(BYTE_t* args)
{
    WORD_t temp_16;
    DWORD_t temp_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(DI, this->a_pop_16());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(SI, this->a_pop_16());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(BP, this->a_pop_16());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        this->a_pop_16();
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(BX, this->a_pop_16());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(DX, this->a_pop_16());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(CX, this->a_pop_16());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(AX, this->a_pop_16());

        this->attach_current_propagation_r_16(DI);
        this->attach_current_propagation_r_16(SI);
        this->attach_current_propagation_r_16(BP);
        this->attach_current_propagation_r_16(SP);
        this->attach_current_propagation_r_16(BX);
        this->attach_current_propagation_r_16(DX);
        this->attach_current_propagation_r_16(CX);
        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EDI, this->a_pop_32());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ESI, this->a_pop_32());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EBP, this->a_pop_32());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        this->a_pop_32();
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EBX, this->a_pop_32());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EDX, this->a_pop_32());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ECX, this->a_pop_32());
        this->reg_propagation_cause_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EAX, this->a_pop_32());

        this->attach_current_propagation_r_32(EDI);
        this->attach_current_propagation_r_32(ESI);
        this->attach_current_propagation_r_32(EBP);
        this->attach_current_propagation_r_32(ESP);
        this->attach_current_propagation_r_32(EBX);
        this->attach_current_propagation_r_32(EDX);
        this->attach_current_propagation_r_32(ECX);
        this->attach_current_propagation_r_32(EAX);
    }
    return 0x0;
}

int taint_x86::r_pushf(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
        return a_push_16(reg_restore_16(FLAGS));
    else
        return a_push_32(reg_restore_32(EFLAGS));
}

int taint_x86::r_popf(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
        reg_store_16(FLAGS, this->a_pop_16());
    else
        reg_store_32(EFLAGS, this->a_pop_32());
    return 0x0;
}

int taint_x86::r_fs_override(BYTE_t*)
{
    this->current_prefixes |= PREFIX_FS_OVERRIDE;
    d_print(3, "FS override\n");
    return 0x0;
}

int taint_x86::r_gs_override(BYTE_t*)
{
    this->current_prefixes |= PREFIX_GS_OVERRIDE;
    d_print(3, "GS override\n");
    return 0x0;
}

int taint_x86::r_oper_size_override(BYTE_t*)
{
    this->current_prefixes |= PREFIX_OPER_SIZE_OVERRIDE;
    d_print(3, "PREFIX_OPER_SIZE_OVERRIDE\n");
    return 0x0;
}

int taint_x86::r_addr_size_override(BYTE_t*)
{
    this->current_prefixes |= PREFIX_ADDR_SIZE_OVERRIDE;
    d_print(3, "PREFIX_ADDR_SIZE_OVERRIDE\n");
    return 0x0;
}

int taint_x86::r_push_imm_16_32(BYTE_t* args)
{
    WORD_t val_16;
    DWORD_t val_32;
    OFFSET src_16_32;

    // any other way to get offset in emulated memory?
    src_16_32 = reg_restore_32(EIP).get_DWORD() +1;

    d_print(3, "Read from: 0x%08x\n", src_16_32);
    // default is 32
    if(this->current_prefixes && PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_16(src_16_32);
        this->restore_16(src_16_32, val_16);
        a_push_16(val_16);
    }
    else
    {
        this->reg_propagation_cause_m_32(src_16_32);
        this->restore_32(src_16_32, val_32);
        a_push_32(val_32);
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_imm_8(BYTE_t* args)
{
    BYTE_t val_8;
    OFFSET src_8;
    DWORD temp_32;
    DWORD_t val_32;

    // any other way to get offset in emulated memory?
    src_8 = reg_restore_32(EIP).get_DWORD() +1;

    this->reg_propagation_cause_m_8(src_8);
    this->restore_8(src_8, val_8);

    /* extend sign */
    temp_32 = (DWORD)((char)val_8.get_BYTE());
    val_32.set_DWORD(temp_32);

    d_print(3, "Push new value: 0x%08x\n", temp_32);

    a_push_32(val_32);

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

/*
int taint_x86::r_imul_r_rm_imm_16_32(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    this->a_decode_modrm(modrm_byte_ptr, &r, &rm);

    d_print(3, "Instr len: 0x%02x\n", this->current_instr_length);

    WORD_t src_16, dst_16, val_16;
    DWORD_t src_32, dst_32, val_32;
    WORD temp_16;
    DWORD temp_32;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    src_16 = this->reg_restore_16(rm.offset);
                    val_16.from_mem(addr + this->current_instr_length);

                    dst_16 = this->a_mul_16(src_16, val_16);
                    this->reg_store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    src_32 = this->reg_restore_32(rm.offset);
                    val_32.from_mem(addr + this->current_instr_length);

                    dst_32 = this->a_mul_32(src_32, val_32);
                    
                    this->reg_store_32(r.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, src_16);
                    val_16.from_mem(addr + this->current_instr_length);

                    dst_16 = this->a_mul_16(src_16, val_16);
                    this->reg_store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, src_32);
                    val_32.from_mem(addr + this->current_instr_length);

                    dst_32 = this->a_mul_32(src_32, val_32);
                    this->reg_store_32(r.offset, dst_32);
                    break;
            }
            break;
    }
    return 0x0;
}
*/

int taint_x86::r_imul_r_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16, ax, dx;
    UWORD temp_16_1, temp_16_2;
    DWORD_t src_32, eax, edx;
    UDWORD temp_32_1, temp_32_2;

    DWORD res1;
    QWORD res2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    ax = this->reg_restore_16(r.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);

                    res1 = this->a_f_imul_16(ax, src_16);
                    
                    ax.set_WORD(res1 & 0xffff);
                    res1 >>= 0x10;
                    dx.set_WORD(res1 & 0xffff);

                    this->reg_store_16(r.offset, ax);
                    // in 3-operand form result is truncated
                    // in 3-operand form result is stored in destination operand

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    eax = this->reg_restore_32(r.offset);
                    src_32.from_mem(instr_ptr + this->current_instr_length);

                    res2 = this->a_f_imul_32(eax, src_32);
                    
                    eax.set_DWORD(res2 & 0xffffffff);
                    res2 >>= 0x20;
                    edx.set_DWORD(res2 & 0xffffffff);

                    // in 3-operand form result is truncated
                    // in 3-operand form result is stored in destination operand
                    this->reg_store_32(r.offset, eax);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    ax = this->reg_restore_16(AX);
                    src_16.from_mem(instr_ptr + this->current_instr_length);

                    res1 = this->a_f_imul_16(ax, src_16);
                    
                    ax.set_WORD(res1 & 0xffff);
                    res1 >>= 0x10;
                    dx.set_WORD(res1 & 0xffff);

                    this->reg_store_16(AX, ax);
                    // in 3-operand form result is truncated
                    //this->reg_store_16(DX, dx);

                    this->attach_current_propagation_r_16(AX);
                    //this->attach_current_propagation_r_16(DX);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    eax = this->reg_restore_32(EAX);
                    src_32.from_mem(instr_ptr + this->current_instr_length);

                    res2 = this->a_f_imul_32(eax, src_32);
                    
                    eax.set_DWORD(res2 & 0xffffffff);
                    res2 >>= 0x20;
                    edx.set_DWORD(res2 & 0xffffffff);

                    this->reg_store_32(EAX, eax);
                    // in 3-operand form result is truncated
                    //this->reg_store_32(EDX, edx);

                    this->attach_current_propagation_r_32(EAX);
                    //this->attach_current_propagation_r_32(EDX);
                    break;
            }
            break;
    }


    return 0x0;
}

/*
int taint_x86::r_imul_r_rm_16_32_imm_8(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    this->a_decode_modrm(modrm_byte_ptr, &r, &rm);

    d_print(3, "Instr len: 0x%02x\n", this->current_instr_length);

    BYTE_t val_8;
    WORD_t src_16, dst_16, val_16;
    DWORD_t src_32, dst_32, val_32;
    WORD temp_16;
    DWORD temp_32;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    src_16 = this->reg_restore_16(rm.offset);
                    val_8.from_mem(addr + this->current_instr_length);
        
                    *//* extend sign *//*
                    temp_16 = (WORD)((char)val_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 = this->a_mul_16(src_16, val_16);
                    this->reg_store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    src_32 = this->reg_restore_32(rm.offset);
                    val_8.from_mem(addr + this->current_instr_length);

                    *//* extend sign *//*
                    temp_32 = (DWORD)((char)val_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 = this->a_mul_32(src_32, val_32);
                    
                    this->reg_store_32(r.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, src_16);
                    val_8.from_mem(addr + this->current_instr_length);

                    *//* extend sign *//*
                    temp_16 = (WORD)((char)val_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 = this->a_mul_16(src_16, val_16);
                    this->reg_store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, src_32);
                    val_8.from_mem(addr + this->current_instr_length);

                    *//* extend sign *//*
                    temp_32 = (DWORD)((char)val_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 = this->a_mul_32(src_32, val_32);
                    this->reg_store_32(r.offset, dst_32);
                    break;
            }
            break;
    }
    return 0x0;
}
*/

/* WARNING! in 3-operand version product is truncated */

int taint_x86::r_imul_r_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16, src_16_2;
    UWORD temp_16_1, temp_16_2;
    DWORD_t src_32, src_32_2;
    UDWORD temp_32_1, temp_32_2;

    DWORD res1;
    QWORD res2;

    BYTE_t imm_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    src_16 = imm_8;
    src_32 = imm_8;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);

                    src_16_2 = this->reg_restore_16(rm.offset);

                    d_print(3, "0x%08x * 0x%08x = ", src_16.get_WORD(), src_16_2.get_WORD());
                    res1 = this->a_f_imul_16(src_16_2, src_16);
                    
                    d_print(3, "0x%08llx\n", res1);
                    src_16.set_WORD(res1 & 0xffff);
                    d_print(3, "0x%08llx\n", src_16.get_WORD());

                    this->reg_store_16(r.offset, src_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    src_32_2 = this->reg_restore_32(rm.offset);

                    d_print(3, "0x%08x * 0x%08x = ", src_32.get_DWORD(), src_32_2.get_DWORD());
                    res2 = this->a_f_imul_32(src_32_2, src_32);
                    
                    d_print(3, "0x%08llx\n", res2);
                    src_32.set_DWORD(res2 & 0xffffffff);
                    d_print(3, "0x%08x\n", src_32.get_DWORD());

                    this->reg_store_32(r.offset, src_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);

                    this->restore_16(rm.offset, src_16_2);

                    d_print(3, "0x%08x * 0x%08x = ", src_16.get_WORD(), src_16_2.get_WORD());
                    res1 = this->a_f_imul_16(src_16_2, src_16);
                    
                    d_print(3, "0x%08llx\n", res1);
                    src_16.set_WORD(res1 & 0xffff);
                    d_print(3, "0x%08llx\n", src_16.get_WORD());

                    this->reg_store_16(r.offset, src_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, src_32_2);

                    d_print(3, "0x%08x & 0x%08x = ", src_32.get_DWORD(), src_32_2.get_DWORD());
                    res2 = this->a_f_imul_32(src_32_2, src_32);
                    
                    d_print(3, "0x%08llx\n", res2);
                    src_32.set_DWORD(res2 & 0xffffffff);
                    d_print(3, "0x%08x\n", src_32.get_DWORD());

                    this->reg_store_32(r.offset, src_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_decode_execute_80(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_add_rm_imm_8(addr);
            break;
        case ECX: //0x1
            return this->r_or_rm_imm_8(addr);
            break;
        case EDX: //0x2
            return this->r_adc_rm_imm_8(addr);
            break;
        case EBX: //0x3
            return this->r_sbb_rm_imm8(addr);
            break;
        case ESP: //0x4
            return this->r_and_rm_imm8(addr);
            break;
        case EBP: //0x5 
            return this->r_sub_rm_imm8(addr);
            break;
        case ESI: //0x6 
            return this->r_xor_rm_imm_8(addr);
            break;
        case EDI: //0x7 
            return this->r_cmp_rm_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_81(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_add_rm_imm_16_32(addr);
            break;
        case ECX: //0x1
            return this->r_or_rm_imm_16_32(addr);
            break;
        case EDX: //0x2
            return this->r_adc_rm_imm_16_32(addr);
            break;
        case EBX: //0x3
            return this->r_sbb_rm_imm_16_32(addr);
            break;
        case ESP: //0x4
            return this->r_and_rm_imm_16_32(addr);
            break;
        case EBP: //0x5
            return this->r_sub_rm_imm_16_32(addr);
            break;
        case ESI: //0x6 
            return this->r_xor_rm_imm_16_32(addr);
            break;
        case EDI: //0x7 
            return this->r_cmp_rm_imm_16_32(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_83(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_add_rm_16_32_imm_8(addr);
            break;
        case ECX: //0x1
            return this->r_or_rm_16_32_imm_8(addr);
            break;
        case EDX: //0x2
            return this->r_adc_rm_16_32_imm_8(addr);
            break;
        case EBX: //0x3
            return this->r_sbb_rm_16_32_imm_8(addr);
            break;
        case ESP: //0x4
            return r_and_rm_16_32_imm_8(addr);
            break;
        case EBP: //0x5
            return this->r_sub_rm_16_32_imm_8(addr);
            break;
        case ESI: //0x6 
            return this->r_xor_rm_16_32_imm_8(addr);
            break;
        case EDI: //0x7 
            return this->r_cmp_rm_16_32_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_xchg_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;

    BYTE_t dst_8, dst_8_2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case MODRM_REG:
            dst_8 = this->reg_restore_8(r.offset);
            dst_8_2 = this->reg_restore_8(rm.offset);
            this->reg_store_8(rm.offset, dst_8);
            this->reg_store_8(r.offset, dst_8_2);
            break;
        case MODRM_MEM:
            dst_8 = this->reg_restore_8(r.offset);
            this->restore_8(rm.offset, dst_8_2);
            this->reg_store_8(r.offset, dst_8_2);
            this->store_8(rm.offset, dst_8);
            break;
    }
    return 0x0;
}

int taint_x86::r_xchg_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;

    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    r.offset = this->a_offset_recalculate_16(r.offset);
//                    rm.offset = this->a_offset_recalculate_16(rm.offset);
                    dst_16 = this->reg_restore_16(r.offset);
                    dst_16_2 = this->reg_restore_16(rm.offset);
                    this->reg_store_16(rm.offset, dst_16);
                    this->reg_store_16(r.offset, dst_16_2);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(r.offset);
                    dst_32_2 = this->reg_restore_32(rm.offset);
                    this->reg_store_32(rm.offset, dst_32);
                    this->reg_store_32(r.offset, dst_32_2);
                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    r.offset = this->a_offset_recalculate_16(r.offset);
                    dst_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, dst_16_2);
                    this->reg_store_16(r.offset, dst_16_2);
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(r.offset);
                    this->restore_32(rm.offset, dst_32_2);
                    this->reg_store_32(r.offset, dst_32_2);
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_mov_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);
    temp = this->reg_restore_8(r.offset);

    this->reg_propagation_cause_r_8(r.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            d_print(3, "Writing 0x%08x @ 0x%08x\n", temp.get_BYTE(), rm.offset);
            this->reg_store_8(rm.offset, temp);    
            this->attach_current_propagation_r_8(rm.offset);
            break;
        case MODRM_MEM:
            d_print(3, "Writing 0x%08x @ 0x%08x\n", temp.get_BYTE(), rm.offset);
            this->store_8(rm.offset, temp);    
            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    return 0x0;
}

int taint_x86::r_mov_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp_32;
    WORD_t temp_16;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    d_print(3, "Got offset: 0x%08x\n", r.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            if(rm.size == MODRM_SIZE_16)
            {
                this->reg_propagation_cause_r_16(r.offset);

                temp_16 = this->reg_restore_16(r.offset);
                d_print(3, "Read 16: 0x%08x from reg: %02x\n", temp_16.get_WORD(), r.offset);
                this->reg_store_16(rm.offset, temp_16);

                this->attach_current_propagation_r_16(rm.offset);
            }
            else
            {
                this->reg_propagation_cause_r_32(r.offset);

                temp_32 = this->reg_restore_32(r.offset);
                d_print(3, "Read 32: 0x%08x\n", temp_32.get_DWORD());
                this->reg_store_32(rm.offset, temp_32);

                this->attach_current_propagation_r_32(rm.offset);
            }
            break;
        case MODRM_MEM:
            if(rm.size == MODRM_SIZE_16)
            {
                this->reg_propagation_cause_r_16(r.offset);

                temp_16 = this->reg_restore_16(r.offset);
                d_print(3, "Read 16: 0x%08x from reg: %02x\n", temp_16.get_WORD(), r.offset);
                this->store_16(rm.offset, temp_16);

                this->attach_current_propagation_m_16(rm.offset);
            }
            else
            {
                this->reg_propagation_cause_r_32(r.offset);

                temp_32 = this->reg_restore_32(r.offset);
                d_print(3, "Read 32: 0x%08x\n", temp_32.get_DWORD());
                this->store_32(rm.offset, temp_32);

                this->attach_current_propagation_m_32(rm.offset);
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_mov_r_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t temp;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case MODRM_REG:
            this->reg_propagation_cause_r_8(rm.offset);
            temp = this->reg_restore_8(rm.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_cause_m_8(rm.offset);
            this->restore_8(rm.offset, temp);
            break;
    }

    d_print(3, "Read: 0x%08x\n", temp.get_BYTE());

/*
    d_print(3, "To recalculate offset: 0x%08x\n", r.offset);
    r.offset = this->a_offset_recalculate_8(r.offset);
    d_print(3, "Recalculated offset: 0x%08x\n", r.offset);
*/
    this->reg_store_8(r.offset, temp);

    this->attach_current_propagation_r_8(r.offset);

    return 0x0;
}

int taint_x86::r_mov_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp_32;
    WORD_t temp_16;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    d_print(3, "Got offset: 0x%08x\n", rm.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            if(rm.size == MODRM_SIZE_16)
            {
                this->reg_propagation_cause_r_16(rm.offset);

                temp_16 = this->reg_restore_16(rm.offset);
                d_print(3, "Read: 0x%08x\n", temp_16.get_WORD());
                this->reg_store_16(r.offset, temp_16);
                
                this->attach_current_propagation_r_16(r.offset);
            }
            else
            {
                this->reg_propagation_cause_r_32(rm.offset);

                temp_32 = this->reg_restore_32(rm.offset);
                d_print(3, "Read: 0x%08x\n", temp_32.get_DWORD());
                this->reg_store_32(r.offset, temp_32);
                
                this->attach_current_propagation_r_32(r.offset);
            }
            break;
        case MODRM_MEM:
            if(rm.size == MODRM_SIZE_16)
            {
                this->reg_propagation_cause_m_16(rm.offset);

                this->restore_16(rm.offset, temp_16);
                d_print(3, "Read: 0x%08x\n", temp_16.get_WORD());
                this->reg_store_16(r.offset, temp_16);
                
                this->attach_current_propagation_r_16(r.offset);
            }
            else
            {
                this->reg_propagation_cause_m_32(rm.offset);

                this->restore_32(rm.offset, temp_32);
                d_print(3, "Read: 0x%08x\n", temp_32.get_DWORD());
                this->reg_store_32(r.offset, temp_32);
                
                this->attach_current_propagation_r_32(r.offset);
            }
            break;
    }


    return 0x0;
}

int taint_x86::r_lea(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;

    WORD_t dst_16;
    DWORD_t dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.size)
    {
        case MODRM_SIZE_16:
            this->reg_propagation_cause_m_16(0x0); /* how to solve this? */
            dst_16 = rm.offset;
            this->reg_store_16(r.offset, dst_16);
            this->attach_current_propagation_r_16(r.offset);
            break;
        case MODRM_SIZE_32:
            this->reg_propagation_cause_m_32(0x0);
            dst_32 = rm.offset;
            this->reg_store_32(r.offset, dst_32);
            this->attach_current_propagation_r_32(r.offset);
            break;
    }

    return 0x0;
}

int taint_x86::r_xchg_ax_eax_ax_eax(BYTE_t*)
{
    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(AX);
        dst_16_2 = this->reg_restore_16(AX);
        this->reg_store_16(AX, dst_16);
        this->reg_store_16(AX, dst_16_2);
    }
    else
    {
        dst_32 = this->reg_restore_32(EAX);
        dst_32_2 = this->reg_restore_32(EAX);
        this->reg_store_32(EAX, dst_32);
        this->reg_store_32(EAX, dst_32_2);
    }
    return 0x0;
}

int taint_x86::r_xchg_ax_eax_cx_ecx(BYTE_t*)
{
    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(AX);
        dst_16_2 = this->reg_restore_16(CX);
        this->reg_store_16(CX, dst_16);
        this->reg_store_16(AX, dst_16_2);
    }
    else
    {
        dst_32 = this->reg_restore_32(ECX);
        dst_32_2 = this->reg_restore_32(EAX);
        this->reg_store_32(EAX, dst_32);
        this->reg_store_32(ECX, dst_32_2);
    }
    return 0x0;
}

int taint_x86::r_xchg_ax_eax_dx_edx(BYTE_t*)
{
    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(DX);
        dst_16_2 = this->reg_restore_16(AX);
        this->reg_store_16(AX, dst_16);
        this->reg_store_16(DX, dst_16_2);
    }
    else
    {
        dst_32 = this->reg_restore_32(EDX);
        dst_32_2 = this->reg_restore_32(EAX);
        this->reg_store_32(EAX, dst_32);
        this->reg_store_32(EDX, dst_32_2);
    }
    return 0x0;
}

int taint_x86::r_xchg_ax_eax_bx_ebx(BYTE_t*)
{
    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(BX);
        dst_16_2 = this->reg_restore_16(AX);
        this->reg_store_16(AX, dst_16);
        this->reg_store_16(BX, dst_16_2);
    }
    else
    {
        dst_32 = this->reg_restore_32(EBX);
        dst_32_2 = this->reg_restore_32(EAX);
        this->reg_store_32(EAX, dst_32);
        this->reg_store_32(EBX, dst_32_2);
    }
    return 0x0;
}

int taint_x86::r_xchg_ax_eax_sp_esp(BYTE_t*)
{
    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(SP);
        dst_16_2 = this->reg_restore_16(AX);
        this->reg_store_16(AX, dst_16);
        this->reg_store_16(SP, dst_16_2);
    }
    else
    {
        dst_32 = this->reg_restore_32(ESP);
        dst_32_2 = this->reg_restore_32(EAX);
        this->reg_store_32(EAX, dst_32);
        this->reg_store_32(ESP, dst_32_2);
    }
    return 0x0;
}

int taint_x86::r_xchg_ax_eax_bp_ebp(BYTE_t*)
{
    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(BP);
        dst_16_2 = this->reg_restore_16(AX);
        this->reg_store_16(AX, dst_16);
        this->reg_store_16(BP, dst_16_2);
    }
    else
    {
        dst_32 = this->reg_restore_32(EBP);
        dst_32_2 = this->reg_restore_32(EAX);
        this->reg_store_32(EAX, dst_32);
        this->reg_store_32(EBP, dst_32_2);
    }
    return 0x0;
}

int taint_x86::r_xchg_ax_eax_si_esi(BYTE_t*)
{
    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(SI);
        dst_16_2 = this->reg_restore_16(AX);
        this->reg_store_16(AX, dst_16);
        this->reg_store_16(SI, dst_16_2);
    }
    else
    {
        dst_32 = this->reg_restore_32(ESI);
        dst_32_2 = this->reg_restore_32(EAX);
        this->reg_store_32(EAX, dst_32);
        this->reg_store_32(ESI, dst_32_2);
    }
    return 0x0;
}

int taint_x86::r_xchg_ax_eax_di_edi(BYTE_t*)
{
    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        dst_16 = this->reg_restore_16(DI);
        dst_16_2 = this->reg_restore_16(AX);
        this->reg_store_16(AX, dst_16);
        this->reg_store_16(DI, dst_16_2);
    }
    else
    {
        dst_32 = this->reg_restore_32(EDI);
        dst_32_2 = this->reg_restore_32(EAX);
        this->reg_store_32(EAX, dst_32);
        this->reg_store_32(EDI, dst_32_2);
    }
    return 0x0;
}

int taint_x86::r_cdq(BYTE_t*)
{
    WORD_t src_16;
    WORD temp_16;
    DWORD_t src_32;
    DWORD temp_32;
    QWORD temp_64;

    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        src_16 = this->reg_restore_16(AX);
        
        /* extend sign */
        temp_32 = (DWORD)(src_16.get_WORD());

        d_print(3, "Got val: 0x%08x\n", temp_32);

        dst_16.set_WORD((WORD)(temp_32 & 0xffff));   
        this->reg_store_16(AX, dst_16);

        temp_32 >>= 0x10;

        dst_16.set_WORD((WORD)(temp_32 & 0xffff));   
        this->reg_store_16(DX, dst_16);
    }
    else
    {
        src_32 = this->reg_restore_32(EAX);
        d_print(3, "Got val: 0x%08x\n", src_32.get_DWORD());
        
        /* extend sign */
        temp_64 = (QWORD)(src_32.get_DWORD());

        d_print(3, "Got val: 0x%016xl\n", temp_64);

        dst_32.set_DWORD((DWORD)(temp_64 & 0xffffffff));   
        this->reg_store_32(EAX, dst_32);

        temp_64 >>= 0x20;

        dst_32.set_DWORD((DWORD)(temp_64 & 0xffffffff));   
        this->reg_store_32(EDX, dst_32);
    }
    return 0x0;
}

int taint_x86::r_sahf(BYTE_t* instr_ptr)
{
    /* TODO */
    return 0x0;
}

int taint_x86::r_lahf(BYTE_t* instr_ptr)
{
    /* TODO */
    return 0x0;
}

int taint_x86::r_mov_al_moffset_8(BYTE_t* instr_ptr)
{
    DWORD_t off;
    BYTE_t dst_8;

    off.from_mem(instr_ptr +1);
    off += this->a_check_segment_override();

    this->reg_propagation_cause_m_8(off.get_DWORD());

    d_print(3, "Reading from: 0x%x\n", off.get_DWORD());

    this->restore_8(off, dst_8);
    this->reg_store_8(AL, dst_8);

    this->attach_current_propagation_r_8(AL);

    return 0x0;
}

int taint_x86::r_mov_ax_eax_moffset_16_32(BYTE_t* instr_ptr)
{
    DWORD_t off;
    DWORD_t dst_32;
    WORD_t dst_16;

    off.from_mem(instr_ptr +1);
    off += this->a_check_segment_override();

    d_print(3, "Reading from: 0x%x\n", off.get_DWORD());

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->restore_16(off, dst_16);
        this->reg_propagation_cause_m_16(off.get_DWORD());

        this->reg_store_16(AX, dst_16);
        this->attach_current_propagation_r_16(AX);

    }
    else
    {
        this->restore_32(off, dst_32);
        this->reg_propagation_cause_m_32(off.get_DWORD());

        d_print(3, "Get val: 0x%08x\n", dst_32.get_DWORD());
        this->reg_store_32(EAX, dst_32);
        this->attach_current_propagation_r_16(AX);

    }
    return 0x0;
}

int taint_x86::r_mov_moffset_8_al(BYTE_t* instr_ptr)
{
    DWORD_t off;
    BYTE_t dst_8;

    off.from_mem(instr_ptr +1);
    off += this->a_check_segment_override();

    d_print(3, "Reading from: 0x%x\n", off.get_DWORD());

    this->reg_propagation_cause_r_8(AL);

    dst_8 = this->reg_restore_8(AL);
    this->store_8(off, dst_8);

    this->attach_current_propagation_m_8(off.get_DWORD());

    return 0x0;
}

int taint_x86::r_mov_moffset_16_32_ax_eax(BYTE_t* instr_ptr)
{
    DWORD_t off;
    DWORD_t dst_32;
    WORD_t dst_16;

    off.from_mem(instr_ptr +1);
    off += this->a_check_segment_override();

    d_print(3, "Writing to: 0x%x\n", off.get_DWORD());

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_r_16(AX);

        dst_16 = this->reg_restore_16(AX);
        this->store_16(off, dst_16);

        this->attach_current_propagation_m_16(off.get_DWORD());
    }
    else
    {
        this->reg_propagation_cause_r_32(EAX);

        dst_32 = this->reg_restore_32(EAX);
        d_print(3, "Get val: 0x%08x\n", dst_32.get_DWORD());
        this->store_32(off, dst_32);

        this->attach_current_propagation_m_32(off.get_DWORD());
    }
    return 0x0;
}

int taint_x86::r_movs_moffset_8(BYTE_t* instr_ptr)
{
    DWORD_t off_src, off_dst;
    BYTE_t val_8;
    DWORD_t ecx;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    off_src = this->reg_restore_32(ESI);
    off_dst = this->reg_restore_32(EDI);

    this->reg_propagation_cause_m_8(off_src.get_DWORD());

    this->restore_8(off_src.get_DWORD(), val_8);
    this->store_8(off_dst.get_DWORD(), val_8);

    this->attach_current_propagation_m_8(off_dst.get_DWORD());

    if(this->a_check_df())
    {
        off_src -=0x1;
        off_dst -=0x1;
    }
    else
    {
        off_src +=0x1;
        off_dst +=0x1;
    }
    
    return 0x0;
}

int taint_x86::r_movs_moffset_16_32(BYTE_t* instr_ptr)
{
    DWORD_t off_src, off_dst;
    DWORD_t val_32;
    WORD_t val_16;
    DWORD_t ecx;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    off_src = this->reg_restore_32(ESI);
    off_dst = this->reg_restore_32(EDI);

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_cause_m_16(off_src.get_DWORD());

        this->restore_16(off_src.get_DWORD(), val_16);
        d_print(3, "Val: 0x%04x\n", val_16.get_WORD());
        this->store_16(off_dst.get_DWORD(), val_16);

        this->attach_current_propagation_m_16(off_dst.get_DWORD());

        if(this->a_check_df())
        {
            off_src -=0x2;
            off_dst -=0x2;
        }
        else
        {
            off_src +=0x2;
            off_dst +=0x2;
        }
    }
    else
    {
        this->reg_propagation_cause_m_32(off_src.get_DWORD());

        this->restore_32(off_src.get_DWORD(), val_32);
        d_print(3, "Val: 0x%04x\n", val_32.get_DWORD());
        this->store_32(off_dst.get_DWORD(), val_32);

        this->attach_current_propagation_m_32(off_dst.get_DWORD());

        if(this->a_check_df())
        {
            off_src -=0x4;
            off_dst -=0x4;
        }
        else
        {
            off_src +=0x4;
            off_dst +=0x4;
        }
    }

    this->reg_store_32(ESI, off_src);
    this->reg_store_32(EDI, off_dst);

    return 0x0;
}

int taint_x86::r_cmps_m_8(BYTE_t*)
{
    /* decrease ECX */
    DWORD_t ecx;
    DWORD_t esi;
    DWORD_t edi;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    esi = this->reg_restore_32(ESI);
    edi = this->reg_restore_32(EDI);

    if(this->a_check_df())
    {
        esi = esi -1;
        edi = edi -1;
    }
    else
    {
        esi = esi +1;
        edi = edi +1;
    }
    this->reg_store_32(ESI, esi);
    this->reg_store_32(EDI, edi);

    /* ignore flags for now */

    return 0x0;
}

int taint_x86::r_cmps_m_16_32(BYTE_t*)
{
    /* decrease ECX */
    DWORD_t ecx;
    DWORD_t esi;
    DWORD_t edi;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    esi = this->reg_restore_32(ESI);
    edi = this->reg_restore_32(EDI);

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        if(this->a_check_df())
        {
            esi = esi -0x2;
            edi = edi -0x2;
        }
        else
        {
            esi = esi +0x2;
            edi = edi +0x2;
        }

        this->reg_store_32(ESI, esi);
        this->reg_store_32(EDI, edi);
    }
    else
    {
        if(this->a_check_df())
        {
            esi = esi -0x4;
            edi = edi -0x4;
        }
        else
        {
            esi = esi +0x4;
            edi = edi +0x4;
        }

        this->reg_store_32(ESI, esi);
        this->reg_store_32(EDI, edi);
    }

    /* ignore flags for now */

    return 0x0;
}


int taint_x86::r_stosd_8(BYTE_t*)
{
    /* decrease ECX */
    DWORD_t ecx;
    DWORD_t edi;

    BYTE_t al;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    /* store */
    d_print(3, "stos8\n");
    edi = this->reg_restore_32(EDI);

    al = this->reg_restore_8(AL);
    this->store_8(edi.get_DWORD(), al);

    if(this->a_check_df())
        edi = edi -1;
    else
        edi = edi +1;
    this->reg_store_32(EDI, edi);

    return 0x0;
}

int taint_x86::r_stosd_16_32(BYTE_t*)
{
    /* decrease ECX */
    DWORD_t ecx;
    DWORD_t edi;

    DWORD_t eax;
    WORD_t ax;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    /* store */
    edi = this->reg_restore_32(EDI);

    d_print(3, "Stos: 0x%08x\n", this->current_prefixes);
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        d_print(3, "stos16\n");
        ax = this->reg_restore_16(AX);
        this->store_16(edi.get_DWORD(), ax);
    
        if(this->a_check_df())
            edi = edi -0x2;
        else
            edi = edi +0x2;

        this->reg_store_32(EDI, edi);
    }
    else
    {
        d_print(3, "stos32\n");
        eax = this->reg_restore_32(EAX);
        this->store_32(edi.get_DWORD(), eax);
    
        if(this->a_check_df())
            edi = edi -0x4;
        else
            edi = edi +0x4;

        this->reg_store_32(EDI, edi);
    }

    return 0x0;
}

int taint_x86::r_lods_8(BYTE_t*)
{
    /* decrease ECX */
    DWORD_t ecx;
    DWORD_t esi;

    BYTE_t al;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    /* restore */
    d_print(3, "lods8\n");
    esi = this->reg_restore_32(ESI);

    this->restore_8(esi.get_DWORD(), al);
    this->reg_store_8(AL, al);

    if(this->a_check_df())
        esi = esi -1;
    else
        esi = esi +1;

    this->reg_store_32(ESI, esi);

    return 0x0;
}

int taint_x86::r_lods_16_32(BYTE_t*)
{
    /* decrease ECX */
    DWORD_t ecx;
    DWORD_t esi;

    DWORD_t eax;
    WORD_t ax;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    /* store */
    esi = this->reg_restore_32(ESI);

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        d_print(3, "lods16\n");
        this->restore_16(esi.get_DWORD(), ax);
        this->reg_store_16(AX, ax);
    
        if(this->a_check_df())
            esi = esi -0x2;
        else
            esi = esi +0x2;
            
        this->reg_store_32(ESI, esi);
    }
    else
    {
        d_print(3, "lods32\n");
        this->restore_32(esi.get_DWORD(), eax);
        this->reg_store_32(EAX, eax);

        if(this->a_check_df())
            esi = esi -0x4;
        else
            esi = esi +0x4;

        this->reg_store_32(ESI, esi);
    }


    return 0x0;
}

int taint_x86::r_scas_m_8(BYTE_t*)
{
    /* decrease ECX */
    DWORD_t ecx;
    DWORD_t edi;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    edi = this->reg_restore_32(EDI);

    if(this->a_check_df())
        edi = edi -1;
    else
        edi = edi +1;

    this->reg_store_32(EDI, edi);

    return 0x0;
}

int taint_x86::r_scas_m_16_32(BYTE_t*)
{
    /* decrease ECX */
    DWORD_t ecx;
    DWORD_t edi;

    /* rep & repne causes ecx decrease */
    if((this->current_prefixes & PREFIX_REP) || (this->current_prefixes & PREFIX_REPNE))
    {
        ecx = this->reg_restore_32(ECX);
        ecx = ecx-1;
        this->reg_store_32(ECX, ecx);
    }

    edi = this->reg_restore_32(EDI);

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        if(this->a_check_df())
            edi = edi -0x2;
        else
            edi = edi +0x2;
            
        this->reg_store_32(EDI, edi);
    }
    else
    {
        if(this->a_check_df())
            edi = edi -0x4;
        else
            edi = edi +0x4;

        this->reg_store_32(EDI, edi);
    }

    return 0x0;
}

int taint_x86::r_mov_al_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t src_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    this->reg_propagation_cause_m_8(offset);

    src_8.from_mem(&this->memory[offset]);

    this->reg_store_8(AL, src_8);

    this->attach_current_propagation_r_8(AL);

    return 0x0;
}

int taint_x86::r_mov_cl_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t src_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    this->reg_propagation_cause_m_8(offset);

    src_8.from_mem(&this->memory[offset]);

    this->reg_store_8(CL, src_8);

    this->attach_current_propagation_r_8(CL);

    return 0x0;
}

int taint_x86::r_mov_dl_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t src_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    this->reg_propagation_cause_m_8(offset);

    src_8.from_mem(&this->memory[offset]);

    this->reg_store_8(DL, src_8);

    this->attach_current_propagation_r_8(DL);

    return 0x0;
}

int taint_x86::r_mov_bl_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t src_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    this->reg_propagation_cause_m_8(offset);

    src_8.from_mem(&this->memory[offset]);

    this->reg_store_8(BL, src_8);

    this->attach_current_propagation_r_8(BL);

    return 0x0;
}

int taint_x86::r_mov_ah_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t src_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    this->reg_propagation_cause_m_8(offset);

    src_8.from_mem(&this->memory[offset]);

    this->reg_store_8(AH, src_8);

    this->attach_current_propagation_r_8(AH);

    return 0x0;
}

int taint_x86::r_mov_bh_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t src_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    this->reg_propagation_cause_m_8(offset);

    src_8.from_mem(&this->memory[offset]);

    this->reg_store_8(BH, src_8);

    this->attach_current_propagation_r_8(BL);

    return 0x0;
}

int taint_x86::r_mov_ch_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t src_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    this->reg_propagation_cause_m_8(offset);

    src_8.from_mem(&this->memory[offset]);

    this->reg_store_8(CH, src_8);

    this->attach_current_propagation_r_8(CL);

    return 0x0;
}

int taint_x86::r_mov_dh_imm8(BYTE_t* instr_ptr)
{
    OFFSET offset;
    BYTE_t src_8;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    this->reg_propagation_cause_m_8(offset);

    src_8.from_mem(&this->memory[offset]);

    this->reg_store_8(DH, src_8);

    this->attach_current_propagation_r_8(DL);

    return 0x0;
}

int taint_x86::r_mov_eax_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t src_16;
    DWORD_t src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(AX, src_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_32(offset);

        src_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EAX, src_32);
    }

    return 0x0;
}

int taint_x86::r_mov_ecx_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t src_16;
    DWORD_t src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(CX, src_16);

        this->attach_current_propagation_r_16(CX);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_32(offset);

        src_32.from_mem(&this->memory[offset]);
        this->reg_store_32(ECX, src_32);

        this->attach_current_propagation_r_32(ECX);
    }

    return 0x0;
}

int taint_x86::r_mov_edx_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t src_16;
    DWORD_t src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(DX, src_16);

        this->attach_current_propagation_r_16(DX);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        src_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EDX, src_32);

        this->attach_current_propagation_r_32(EDX);
    }

    return 0x0;
}

int taint_x86::r_mov_ebx_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t src_16;
    DWORD_t src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(BX, src_16);

        this->attach_current_propagation_r_16(BX);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_32(offset);

        src_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EBX, src_32);

        this->attach_current_propagation_r_32(EBX);
    }

    return 0x0;
}

int taint_x86::r_mov_esp_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t src_16;
    DWORD_t src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(SP, src_16);

        this->attach_current_propagation_r_16(SP);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_32(offset);

        src_32.from_mem(&this->memory[offset]);
        this->reg_store_32(ESP, src_32);

        this->attach_current_propagation_r_32(ESP);
    }

    return 0x0;
}

int taint_x86::r_mov_ebp_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t src_16;
    DWORD_t src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(BP, src_16);

        this->attach_current_propagation_r_16(BP);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_32(offset);

        src_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EBP, src_32);

        this->attach_current_propagation_r_32(EBP);
    }

    return 0x0;
}

int taint_x86::r_mov_esi_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t src_16;
    DWORD_t src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(SI, src_16);

        this->attach_current_propagation_r_16(SI);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_32(offset);

        src_32.from_mem(&this->memory[offset]);
        this->reg_store_32(ESI, src_32);

        this->attach_current_propagation_r_32(ESI);
    }

    return 0x0;
}

int taint_x86::r_mov_edi_imm_16_32(BYTE_t* instr_ptr)
{
    OFFSET offset;
    WORD_t src_16;
    DWORD_t src_32;

    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(DI, src_16);

        this->attach_current_propagation_r_16(DI);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_cause_m_32(offset);

        src_32.from_mem(&this->memory[offset]);
        this->reg_store_32(EDI, src_32);

        this->attach_current_propagation_r_32(EDI);
    }

    return 0x0;
}


int taint_x86::r_decode_execute_c0(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_rol_rm_imm_8(addr);
            break;
        case ECX: //0x1
            return this->r_ror_rm_imm_8(addr);
            break;
        case EDX: //0x2
            return this->r_rcl_rm_imm_8(addr);
            break;
        case EBX: //0x3
            return this->r_rcr_rm_imm_8(addr);
            break;
        case ESP: //0x4
            return this->r_sal_shl_rm_imm_8(addr);
            break;
        case EBP: //0x5 
            return this->r_shr_rm_imm_8(addr);
            break;
        case ESI: //0x6 
            break;
        case EDI: //0x7 
            return this->r_sar_rm_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_c1(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_rol_rm_16_32_imm_8(addr);
            break;
        case ECX: //0x1
            return this->r_ror_rm_16_32_imm_8(addr);
            break;
        case EDX: //0x2
            return this->r_rcl_rm_16_32_imm_8(addr);
            break;
        case EBX: //0x3
            return this->r_rcr_rm_16_32_imm_8(addr);
            break;
        case ESP: //0x4
            return this->r_sal_shl_rm_16_32_imm_8(addr);
            break;
        case EBP: //0x5
            return this->r_shr_rm_32_imm_8(addr);
            break;
        case ESI: //0x6 
            break;
        case EDI: //0x7 
            return this->r_sar_rm_16_32_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_retn(BYTE_t*)
{
    OFFSET offset;
    WORD_t src_16;
    unsigned i, count;

    offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;

    src_16.from_mem(&this->memory[offset]);

    d_print(3, "offset: 0x%08x, read val: 0x%08x\n", offset, src_16.get_WORD());

    count = src_16.get_WORD();
    count /= 0x4;

    if(count > 0x20)
    {
        d_print(1, "Error, infinite retn\n");
        return -1;
    }

    for(i=0x0; i<count; i++)
        this->a_pop_32();
    
    DWORD_t ret;
    ret = this->a_pop_32();
    d_print(3, "Will return to: 0x%08x\n", ret.get_DWORD());

    this->reg_store_32(EIP, ret);
    attach_current_propagation_r_32(EIP);

    this->current_instr_is_jump = 0x1;

    return 0x0;

}

int taint_x86::r_ret(BYTE_t*)
{
    DWORD_t ret;
    ret = this->a_pop_32();
    d_print(3, "Will return to: 0x%08x\n", ret.get_DWORD());

    this->reg_store_32(EIP, ret);
    attach_current_propagation_r_32(EIP);

    this->current_instr_is_jump = 0x1;


    return 0x0;
}

int taint_x86::r_decode_execute_c6(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_mov_rm_imm_8(addr);
            break;
        case ECX: //0x1
            break;
        case EDX: //0x2
            break;
        case EBX: //0x3
            break;
        case ESP: //0x4
//            return this->r_sal_shl_rm_16_32_imm_8(addr);
            break;
        case EBP: //0x5
//            return this->r_shr_rm_32_imm_8(addr);
            break;
        case ESI: //0x6 
            break;
        case EDI: //0x7 
//            return this->r_sar_rm_16_32_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_c7(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_mov_rm_imm_16_32(addr);
            break;
        case ECX: //0x1
            break;
        case EDX: //0x2
            break;
        case EBX: //0x3
            break;
        case ESP: //0x4
            break;
        case EBP: //0x5
            break;
        case ESI: //0x6
            break;
        case EDI: //0x7 
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_leave(BYTE_t* addr)
{
    DWORD_t esp, ebp;

    esp = this->reg_restore_32(EBP);
    this->reg_store_32(ESP, esp);

    d_print(3, "ESP: 0x%08x\n", this->reg_restore_32(ESP).get_DWORD());
    ebp = this->a_pop_32();
    d_print(3, "ESP: 0x%08x\n", this->reg_restore_32(ESP).get_DWORD());

    this->reg_store_32(EBP, ebp);

    return 0x0;
}

int taint_x86::r_decode_execute_d0(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_rol_rm_8_1(addr);
            break;
        case ECX: //0x1
            return this->r_ror_rm_8_1(addr);
            break;
        case EDX: //0x2
            return this->r_rcl_rm_8_1(addr);
            break;
        case EBX: //0x3
            return this->r_rcr_rm_8_1(addr);
            break;
        case ESP: //0x4
            return this->r_sal_shl_rm_8_1(addr);
            break;
        case EBP: //0x5
            return this->r_shr_rm_8_1(addr);
            break;
        case ESI: //0x6
            break;
        case EDI: //0x7 
            return this->r_sar_rm_8_1(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_d1(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_rol_rm_16_32_1(addr);
            break;
        case ECX: //0x1
            return this->r_ror_rm_16_32_1(addr);
            break;
        case EDX: //0x2
            return this->r_rcl_rm_16_32_1(addr);
            break;
        case EBX: //0x3
            return this->r_rcr_rm_16_32_1(addr);
            break;
        case ESP: //0x4
            return this->r_sal_shl_rm_16_32_1(addr); 
            break;
        case EBP: //0x5
            return this->r_shr_rm_16_32_1(addr);
            break;
        case ESI: //0x6
            break;
        case EDI: //0x7 
            return this->r_sar_rm_16_32_1(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_d2(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_rol_rm_8_cl(addr);
            break;
        case ECX: //0x1
            return this->r_ror_rm_8_cl(addr);
            break;
        case EDX: //0x2
            return this->r_rcl_rm_8_cl(addr);
            break;
        case EBX: //0x3
            return this->r_rcr_rm_8_cl(addr);
            break;
        case ESP: //0x4
            return this->r_sal_shl_rm_8_cl(addr);
            break;
        case EBP: //0x5
            return this->r_shr_rm_8_cl(addr);
            break;
        case ESI: //0x6
            break;
        case EDI: //0x7 
            return this->r_sar_rm_8_cl(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_d3(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_rol_rm_16_32_cl(addr);
            break;
        case ECX: //0x1
            return this->r_ror_rm_16_32_cl(addr);
            break;
        case EDX: //0x2
            return this->r_rcl_rm_16_32_cl(addr);
            break;
        case EBX: //0x3
            return this->r_rcr_rm_16_32_cl(addr);
            break;
        case ESP: //0x4
            return this->r_sal_shl_rm_16_32_cl(addr); 
            break;
        case EBP: //0x5
            return this->r_shr_rm_16_32_cl(addr);
            break;
        case ESI: //0x6
            break;
        case EDI: //0x7 
            return this->r_sar_rm_16_32_cl(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}


int taint_x86::r_call_rel(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    DWORD_t target, target_2;
    DWORD disp32_reint, *disp32p;

    ret_addr = this->reg_restore_32(EIP);
    ret_addr += this->current_instr_length;
    
    target.from_mem(instr_ptr + this->current_instr_length);
    ret_addr += 0x4;

    disp32_reint = target.get_DWORD();
    disp32p = (signed int*)&(disp32_reint);


    target_2 = ret_addr + *disp32p; //signed displacement & operand size
//    this->handle_call(this->cur_info, target_2.get_DWORD(), ret_addr.get_DWORD());
    a_push_32(ret_addr);
    
    this->reg_store_32(EIP, target_2);
    this->current_instr_is_jump = 0x1;
    return 0x0;
}

int taint_x86::r_jmp_rel_8(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    BYTE_t target;
    DWORD_t target_2;
    char disp8_reint, *disp8p;

    CONTEXT_INFO* cur_ctx;
    cur_ctx = &this->ctx_info[this->tids[this->cur_tid]];

    ret_addr = this->reg_restore_32(EIP);
    ret_addr += this->current_instr_length;
    ret_addr += 0x1;

    target.from_mem(instr_ptr + this->current_instr_length);
    disp8_reint = target.get_BYTE();
    disp8p = (char*)&(disp8_reint);
    target_2 = ret_addr + *disp8p; //signed displacement & operand size

    d_print(3, "ret_addr: 0x%08x, target: 0x%08x, target2: 0x%08x\n", ret_addr.get_DWORD(), target.get_BYTE(), target_2.get_DWORD());

    return 0x0;
}

int taint_x86::r_jmp_rel_16_32(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    DWORD_t target;
    char out_line[MAX_NAME];
    CONTEXT_INFO* cur_ctx;
    cur_ctx = &this->ctx_info[this->tids[this->cur_tid]];

    target.from_mem(instr_ptr + this->current_instr_length);
    ret_addr = this->reg_restore_32(EIP);
    ret_addr += 0x5;
    target += ret_addr;

    a_push_32(ret_addr);
    
    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());
    this->reg_store_32(EIP, target);
    this->current_instr_is_jump = 0x1;

    return 0x0;
}

int taint_x86::r_jmp_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    src_16 = this->reg_restore_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    src_32 = this->reg_restore_32(rm.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->restore_16(rm.offset, src_16);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->restore_32(rm.offset, src_32);
                    break;
            }
            break;
    }

    DWORD_t ret_addr;
    DWORD_t target;
    target = 0x0 + src_16.get_WORD() + src_32.get_DWORD();

    CONTEXT_INFO* cur_ctx;
    cur_ctx = &this->ctx_info[this->tids[this->cur_tid]];

//    ret_addr = this->reg_restore_32(EIP);
//    ret_addr += 0x5;

//    a_push_32(ret_addr);
    
//    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());
    this->reg_store_32(EIP, target);

    this->attach_current_propagation_r_32(EIP);
    this->current_instr_is_jump = 0x1;

    return 0x0;
}

int taint_x86::r_lock(BYTE_t*)
{
    this->current_prefixes |= PREFIX_LOCK;
    return 0x0;
}

int taint_x86::r_repne(BYTE_t*)
{
    this->current_prefixes |= PREFIX_REPNE;
    return 0x0;
}

int taint_x86::r_rep(BYTE_t*)
{
    this->current_prefixes |= PREFIX_REP;
    return 0x0;
}

int taint_x86::r_decode_execute_f6(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_noop_un(addr); // test
            break;
        case EDX: //0x2
            return this->r_not_rm_8(addr);
            break;
        case EBX: //0x3
            return this->r_neg_rm_8(addr);
            break;
        case ESP: //0x4
            return this->r_mul_rm_8(addr);
            break;
        case EBP: //0x5
            return this->r_imul_rm_8(addr);
            break;
        case ESI: //0x6
            return this->r_div_rm_8(addr);
            break;
        case EDI: //0x7 
            return this->r_idiv_rm_8(addr);
            break;

    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_decode_execute_f7(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0
            return this->r_noop_un(addr); // test
            break;
        case EDX: //0x2
            return this->r_not_rm_16_32(addr);
            break;
        case EBX: //0x3 
            return this->r_neg_rm_16_32(addr);
            break;
        case ESP: //0x4
            return this->r_mul_rm_16_32(addr);
            break;
        case EBP: //0x5
            return this->r_imul_rm_16_32(addr);
            break;
        case ESI: //0x6 
            return this->r_div_rm_16_32(addr);
            break;
        case EDI: //0x7 
            return this->r_idiv_rm_16_32(addr);
            break;

    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_clc(BYTE_t* b)
{
    return this->a_clear_cf();
}

int taint_x86::r_stc(BYTE_t* b)
{
    return this->a_set_cf();
}

int taint_x86::r_cli(BYTE_t* b)
{
    return this->a_clear_if();
}

int taint_x86::r_sti(BYTE_t* b)
{
    return this->a_set_if();
}

int taint_x86::r_cld(BYTE_t* b)
{
    return this->a_clear_df();
}

int taint_x86::r_std(BYTE_t* b)
{
    return this->a_set_df();
}

int taint_x86::r_decode_execute_fe(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0 
            return this->r_inc_rm_8(addr);
            break;
        case ECX: //0x1 
            return this->r_dec_rm_8(addr);
            break;
        case EDX: //0x2 
            break;
        case EBX: //0x3 
            break;
        case ESI: //0x6 
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}


int taint_x86::r_decode_execute_ff(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    d_print(1, "r_decode_execute_ff decoded offset: 0x%08x\n", r.offset);
    switch(r.offset)
    {
        case EAX: //0x0 
            return this->r_inc_rm_16_32(addr);
            break;
        case ECX: //0x1 
            return this->r_dec_rm_16_32(addr);
            break;
        case EDX: //0x2 
            return this->r_call_abs_near(addr);
            break;
        case EBX: //0x3 
            return this->r_call_abs_far(addr);
            break;
        case ESP: //0x4
            return this->r_jmp_rm_16_32(addr);
            break;
        case ESI: //0x6 
            return this->r_push_rm_16_32(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_cmpxchg_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8, tmp_8;
    BYTE_t al;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    al = this->reg_restore_8(AL);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            src_8 = this->reg_restore_8(r.offset);
            if(al.get_BYTE() == dst_8.get_BYTE())
            {
                d_print(3, "equal\n");
                this->reg_store_8(rm.offset, src_8);
            }
            else
            {
                d_print(3, "not equal\n");
                this->reg_store_8(AL, dst_8);
                this->store_8(rm.offset, src_8);
            }
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            src_8 = this->reg_restore_8(r.offset);
            if(al.get_BYTE() == dst_8.get_BYTE())
            {
                d_print(3, "equal\n");
                this->store_8(rm.offset, src_8);
            }
            else
            {
                d_print(3, "not equal\n");
                this->reg_store_8(AL, dst_8);
                this->store_8(rm.offset, src_8);
            }
            break;
    }

    /* flags */
    /* TODO: fix
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();
    */

    return 0x0;
}

int taint_x86::r_cmpxchg_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16, ax;
    DWORD_t src_32, eax;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    d_print(3, "0x%08x\n", this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE);

    switch(rm.region)
    {
        case (MODRM_REG):
            if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
            {
                    ax = this->reg_restore_16(AX);
                    dst_16 = this->reg_restore_16(rm.offset);
                    d_print(3, "Read 16: 0x%08x\n", dst_16.get_WORD());
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    if(ax.get_WORD() == dst_16.get_WORD())
                    {
                        d_print(3, "equal\n");
                        this->reg_store_16(rm.offset, src_16);
                    }
                    else
                    {
                        d_print(3, "not equal\n");
                        this->reg_store_16(AX, dst_16);
                        this->reg_store_16(rm.offset, src_16);
                    }
            }
            else
            {
                    eax = this->reg_restore_32(EAX);
                    dst_32 = this->reg_restore_32(rm.offset);
                    d_print(3, "Read 32: 0x%08x\n", dst_32.get_DWORD());
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(r.offset);
                    if(eax.get_DWORD() == dst_32.get_DWORD())
                    {
                        d_print(3, "equal\n");
                        this->reg_store_32(rm.offset, src_32);
                    }
                    else
                    {
                        d_print(3, "not equal\n");
                        this->reg_store_32(EAX, dst_32);
                        this->reg_store_32(rm.offset, src_32);
                    }
            }
            break;
        case (MODRM_MEM):
            if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
            {
                    this->restore_16(AX, ax);
                    this->restore_16(rm.offset, dst_16);
                    d_print(3, "Read 16: 0x%08x\n", dst_16.get_WORD());
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    if(ax.get_WORD() == dst_16.get_WORD())
                    {
                        d_print(3, "equal\n");
                        this->store_16(rm.offset, src_16);
                    }
                    else
                    {
                        d_print(3, "not equal\n");
                        this->reg_store_16(AX, dst_16);
                        this->store_16(rm.offset, src_16);
                    }
            }
            else
            {
                    this->restore_32(EAX, eax);
                    this->restore_32(rm.offset, dst_32);
                    d_print(3, "Read 32: 0x%08x\n", dst_32.get_DWORD());
                    old_dst_32 = dst_32;
                    src_32 = this->reg_restore_32(r.offset);
                    d_print(3, "Src: 0x%08x\n", src_32.get_DWORD());
                    if(eax.get_DWORD() == dst_32.get_DWORD())
                    {
                        d_print(3, "equal\n");
                        this->store_32(rm.offset, src_32);
                    }
                    else
                    {
                        d_print(3, "not equal\n");
                        this->reg_store_32(EAX, dst_32);
                        this->store_32(rm.offset, src_32);
                    }
            }
            break;
    }

    /* flags */
    /* TODO: fix
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }
*/
    return 0x0;
}

int taint_x86::r_btr_rm_r_16_32(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    WORD_t src_16, dst_16;
    WORD mask_16;
    DWORD mask_32;
    DWORD_t src_32, dst_32;
    BYTE result = 0x0;
    BYTE bit = 0x1;
    WORD_t count_16;
    DWORD_t count_32;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    mask_16 = 0x1;
    mask_32 = 0x1;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    src_16 = this->reg_restore_16(rm.offset);
                    
                    count_16 = this->reg_restore_16(r.offset);
                    count_16 %= 0x10;
                    mask_16 <<= count_16.get_WORD();

                    if(src_16.get_WORD() & mask_16 != 0x0)
                        this->a_set_cf();
                    else
                        this->a_clear_cf();
                    
                    mask_16 = ~mask_16;
                    dst_16.set_WORD(src_16.get_WORD() & mask_16);

                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    src_32 = this->reg_restore_32(rm.offset);
                    
                    count_32 = this->reg_restore_32(r.offset);
                    count_32 %= 0x20;
                    mask_32 <<= count_32.get_DWORD();

                    if(src_32.get_DWORD() & mask_32 != 0x0)
                        this->a_set_cf();
                    else
                        this->a_clear_cf();
                    
                    mask_32 = ~mask_32;
                    dst_32.set_DWORD(src_32.get_DWORD() & mask_32);

                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, src_16);
                    
                    count_16 = this->reg_restore_16(r.offset);
                    count_16 %= 0x10;
                    mask_16 <<= count_16.get_WORD();

                    if(src_16.get_WORD() & mask_16 != 0x0)
                        this->a_set_cf();
                    else
                        this->a_clear_cf();
                    
                    mask_16 = ~mask_16;
                    dst_16.set_WORD(src_16.get_WORD() & mask_16);

                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, src_32);
                    
                    count_32 = this->reg_restore_32(r.offset);
                    count_32 %= 0x20;
                    mask_32 <<= count_32.get_DWORD();

                    if(src_32.get_DWORD() & mask_32 != 0x0)
                        this->a_set_cf();
                    else
                        this->a_clear_cf();
                    
                    mask_32 = ~mask_32;
                    dst_32.set_DWORD(src_32.get_DWORD() & mask_32);

                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_movzx_r_16_32_rm_8(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;
    DWORD_t dst_32;
    WORD_t dst_16;
    BYTE_t dst_8;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_8); //TODO: verify

    dst_16 = 0; 
    dst_32 = 0; 

    switch(rm.region)
    {
        case MODRM_REG:
            this->reg_propagation_cause_r_8(rm.offset);
            dst_8 = this->reg_restore_8(rm.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_cause_m_8(rm.offset);
            this->restore_8(rm.offset, dst_8);
            break;
    }

    d_print(3, "Read byte: 0x%02x\n", dst_8.get_BYTE());

    switch(r.size)
    {
        case MODRM_SIZE_16:
            dst_16 += dst_8;
            dst_16.val[0] = 0x0;
            this->reg_store_16(r.offset, dst_16);
            this->attach_current_propagation_r_16(r.offset);
            break;
        case MODRM_SIZE_32:
            dst_32 += dst_8;
            dst_32.val[0] = 0x0;
            dst_32.val[1] = 0x0;
            this->reg_store_32(r.offset, dst_32);
            this->attach_current_propagation_r_32(r.offset);
            break;
    }

    return 0x0;
}

int taint_x86::r_movzx_r_32_rm_16(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;
    DWORD_t dst_32;
    WORD_t dst_16;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_16);

    dst_16 = 0; 
    dst_32 = 0; 

    switch(rm.region)
    {
        case MODRM_REG:
            dst_16 = this->reg_restore_16(rm.offset);
            this->reg_propagation_cause_r_16(rm.offset);
            break;
        case MODRM_MEM:
            this->restore_16(rm.offset, dst_16);
            this->reg_propagation_cause_m_16(rm.offset);
            break;
    }

    d_print(3, "Got src value: 0x%08x\n", (UDWORD)dst_16.get_WORD());
    dst_32 += dst_16;
    dst_32.val[0] = 0x0;
    dst_32.val[1] = 0x0;
    d_print(3, "Got dst value: 0x%08x\n", dst_32.get_DWORD());
    
    this->reg_store_32(r.offset, dst_32);
    this->attach_current_propagation_r_32(r.offset);
    d_print(3, "Offset: 0x%08x, val: 0x%08x\n", r.offset, dst_32.get_DWORD());

    return 0x0;
}

int taint_x86::r_decode_execute_0fba(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0 
            break;
        case ECX: //0x1 
            break;
        case EDX: //0x2 
            break;
        case EBX: //0x3 
            break;
        case ESP: //0x4
            break;
        case ESI: //0x6 
            return this->r_btr_rm_16_32_imm_8(addr);
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}

int taint_x86::r_btr_rm_16_32_imm_8(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    WORD_t src_16, dst_16;
    WORD mask_16;
    DWORD mask_32;
    DWORD_t src_32, dst_32;
    BYTE result = 0x0;
    BYTE bit = 0x1;
    BYTE_t count;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    count.from_mem(addr + this->current_instr_length);
    mask_16 = 0x1;
    mask_32 = 0x1;
    mask_32 <<= count.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    src_16 = this->reg_restore_16(rm.offset);
                    
                    count.set_BYTE(0x10);
                    mask_16 <<= count.get_BYTE();

                    if(src_16.get_WORD() & mask_16 != 0x0)
                        this->a_set_cf();
                    else
                        this->a_clear_cf();
                    
                    mask_16 = ~mask_16;
                    dst_16.set_WORD(src_16.get_WORD() & mask_16);

                    d_print(3, "Writing: 0x%08x\n", dst_16.get_WORD());

                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    src_32 = this->reg_restore_32(rm.offset);
                    
                    count.set_BYTE(0x20);
                    mask_32 <<= count.get_BYTE();

                    if(src_32.get_DWORD() & mask_32 != 0x0)
                        this->a_set_cf();
                    else
                        this->a_clear_cf();
                    
                    mask_32 = ~mask_32;
                    dst_32.set_DWORD(src_32.get_DWORD() & mask_32);

                    this->reg_store_32(rm.offset, dst_32);

                    d_print(3, "Writing: 0x%08x\n", dst_32.get_DWORD());

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, src_16);
                    
                    count.set_BYTE(0x10);
                    mask_16 <<= count.get_BYTE();

                    if(src_16.get_WORD() & mask_16 != 0x0)
                        this->a_set_cf();
                    else
                        this->a_clear_cf();
                    
                    mask_16 = ~mask_16;
                    dst_16.set_WORD(src_16.get_WORD() & mask_16);

                    d_print(3, "Writing: 0x%08x\n", dst_16.get_WORD());

                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, src_32);
                    
                    count.set_BYTE(0x20);
                    mask_32 <<= count.get_BYTE();

                    if(src_32.get_DWORD() & mask_32 != 0x0)
                        this->a_set_cf();
                    else
                        this->a_clear_cf();
                    
                    mask_32 = ~mask_32;
                    dst_32.set_DWORD(src_32.get_DWORD() & mask_32);

                    this->store_32(rm.offset, dst_32);
                    
                    d_print(3, "Writing: 0x%08x\n", dst_32.get_DWORD());

                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_bsf_r_rm_16_32(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    WORD_t src_16, dst_16;
    DWORD_t src_32, dst_32;
    BYTE result = 0x0;
    BYTE bit = 0x1;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    src_16 = this->reg_restore_16(rm.offset);
                    if(src_16.get_WORD() == 0x0)
                    {
                        this->a_set_zf(); 
                        break;
                    }
                    this->a_clear_zf(); 
                    while(!(src_16.get_WORD() && (WORD)0x1))
                        src_16 >> 1;
                    dst_16 = result;
                    this->reg_store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    src_32 = this->reg_restore_32(rm.offset);
                    if(src_32.get_DWORD() == 0x0)
                    {
                        this->a_set_zf(); 
                        break;
                    }
                    this->a_clear_zf(); 
                    while(!(src_32.get_DWORD() && (DWORD)0x1))
                        src_32 >> 1;
                    dst_32 = result;
                    this->reg_store_32(r.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, src_16);
                    if(src_16.get_WORD() == 0x0)
                    {
                        this->a_set_zf(); 
                        break;
                    }
                    this->a_clear_zf(); 
                    while(!(src_16.get_WORD() && (WORD)0x1))
                        src_16 >> 1;
                    dst_16 = result;
                    this->store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, src_32);
                    if(src_32.get_DWORD() == 0x0)
                    {
                        this->a_set_zf(); 
                        break;
                    }
                    this->a_clear_zf(); 
                    while(!(src_32.get_DWORD() && (DWORD)0x1))
                        src_32 >> 1;
                    dst_32 = result;
                    this->store_32(r.offset, dst_32);
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < src_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < src_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }
    
    return 0x0;
}

int taint_x86::r_movsx_r_16_32_rm_8(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;
    DWORD_t dst_32, val_32;
    WORD_t dst_16, val_16;
    BYTE_t dst_8;

    DWORD temp_32;
    WORD temp_16;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_8);

    dst_16 = 0; 
    dst_32 = 0; 

    switch(rm.region)
    {
        case MODRM_REG:
            this->reg_propagation_cause_r_8(rm.offset);
            dst_8 = this->reg_restore_8(rm.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_cause_m_8(rm.offset);
            this->restore_8(rm.offset, dst_8);
            break;
    }

    d_print(3, "Read val: 0x%02x\n", dst_8.get_BYTE());

    switch(r.size)
    {
        case MODRM_SIZE_16:
            /* extend sign */ 
            temp_16 = (DWORD)((char)dst_8.get_BYTE());
            val_16.set_WORD(temp_16);
   
            dst_16 = val_16;

            r.offset = this->a_offset_recalculate_16(r.offset);
            this->reg_store_16(r.offset, dst_16);
            this->attach_current_propagation_r_16(r.offset);
            break;
        case MODRM_SIZE_32:
            /* extend sign */ 
            temp_32 = (DWORD)((char)dst_8.get_BYTE());
            val_32.set_DWORD(temp_32);
   
            dst_32 = val_32;

            this->reg_store_32(r.offset, dst_32);
            this->attach_current_propagation_r_32(r.offset);
            break;
    }

    return 0x0;
}

int taint_x86::r_movsx_r_32_rm_16(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;
    DWORD_t dst_32;
    WORD_t dst_16;

    modrm_byte_ptr = addr +1;
    a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_16);

    dst_16 = 0; 
    dst_32 = 0; 

    switch(rm.region)
    {
        case MODRM_REG:
            dst_16 = this->reg_restore_16(rm.offset);
            d_print(3, "Offset: 0x%08x got src value: 0x%08x\n", rm.offset, this->reg_restore_16(rm.offset).get_WORD());
            this->reg_propagation_cause_r_16(rm.offset);
            break;
        case MODRM_MEM:
            this->restore_16(rm.offset, dst_16);
            this->reg_propagation_cause_m_16(rm.offset);
            break;
    }

    d_print(3, "Got src value: 0x%08x\n", dst_16.get_WORD());
    dst_32 += dst_16;

    if(this->a_check_sign(dst_16.get_WORD()))
    {
         dst_32.val[0] = 0xff;
         dst_32.val[1] = 0xff;
    }
    
    this->reg_store_32(r.offset, dst_32);
    this->attach_current_propagation_r_32(r.offset);
    d_print(3, "Offset: 0x%08x, val: 0x%08x\n", r.offset, dst_32.get_DWORD());

    return 0x0;
}

int taint_x86::r_xadd_rm_r_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;

    BYTE_t dst_8, dst_8_2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case MODRM_REG:
            dst_8 = this->reg_restore_8(r.offset);
            dst_8_2 = this->reg_restore_8(rm.offset);
            this->reg_store_8(rm.offset, dst_8+dst_8_2);
            this->reg_store_8(r.offset, dst_8_2);
            break;
        case MODRM_MEM:
            dst_8 = this->reg_restore_8(r.offset);
            this->restore_8(rm.offset, dst_8_2);
            this->reg_store_8(r.offset, dst_8_2);
            this->store_8(rm.offset, dst_8+dst_8_2);
            break;
    }
    return 0x0;
}

int taint_x86::r_xadd_rm_r_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;

    WORD_t dst_16, dst_16_2;
    DWORD_t dst_32, dst_32_2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(r.offset);
                    dst_16_2 = this->reg_restore_16(rm.offset);
                    this->reg_store_16(rm.offset, dst_16 + dst_16_2);
                    this->reg_store_16(r.offset, dst_16_2);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(r.offset);
                    dst_32_2 = this->reg_restore_32(rm.offset);
                    this->reg_store_32(rm.offset, dst_32 + dst_32_2);
                    this->reg_store_32(r.offset, dst_32_2);
                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, dst_16_2);
                    this->reg_store_16(r.offset, dst_16_2);
                    this->store_16(rm.offset, dst_16+dst_16_2);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(r.offset);
                    this->restore_32(rm.offset, dst_32_2);
                    this->reg_store_32(r.offset, dst_32_2);
                    this->store_32(rm.offset, dst_32 + dst_32_2);
                    break;
            }
            break;
    }

    return 0x0;
}


int taint_x86::r_imul_r_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 *= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_r_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    src_32 = this->reg_restore_32(rm.offset);
                    dst_32 *= src_32;
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(r.offset);
                    this->reg_propagation_cause_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, src_16);
                    dst_16 *= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(r.offset);
                    this->reg_propagation_cause_m_32(rm.offset);

                    dst_32 = this->reg_restore_32(r.offset);
                    this->restore_32(rm.offset, src_32);
                    dst_32 *= src_32;
                    this->reg_store_32(r.offset, dst_32);

                    this->attach_current_propagation_r_32(r.offset);
                    break;
            }
            break;
    }

    /* flags */
    switch(r.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < src_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < src_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;

    return 0x0;
}


int taint_x86::r_sysenter(BYTE_t*)
{
    this->a_pop_32();
    return 0x0;
}

int taint_x86::r_shld_r_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    WORD_t src_16;
    DWORD_t dst_32;
    DWORD_t src_32;

    WORD mask_16;
    DWORD mask_32;

    WORD temp_16_1, temp_16_2;
    DWORD temp_32_1, temp_32_2;
    
    BYTE_t count;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    count.from_mem(instr_ptr + this->current_instr_length);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_16 = this->reg_restore_16(rm.offset);
            src_16 = this->reg_restore_16(r.offset);

            mask_16 = 0xffff;
            mask_16 << 0x10 - count.get_BYTE();

            temp_16_1 = src_16.get_WORD();
            temp_16_1 &= mask_16;
            temp_16_1 >> 0x10 - count.get_BYTE();

            temp_16_2 = dst_16.get_WORD();
            temp_16_2 << count.get_BYTE();
            temp_16_2 |= temp_16_1;

            dst_16.set_WORD(temp_16_2);

            this->reg_store_16(rm.offset, dst_16);
            break;
        case (MODRM_MEM):
            this->restore_32(rm.offset, dst_32);
            src_32 = this->reg_restore_32(r.offset);

            mask_32 = 0xffffffff;
            mask_32 << 0x20 - count.get_BYTE();

            temp_32_1 = src_32.get_DWORD();
            temp_32_1 &= mask_32;
            temp_32_1 >> 0x20 - count.get_BYTE();

            temp_32_2 = dst_32.get_DWORD();
            temp_32_2 << count.get_BYTE();
            temp_32_2 |= temp_32_1;

            dst_32.set_DWORD(temp_32_2);

            this->store_32(rm.offset, dst_32);
            break;
    }

    return 0x0;
}

int taint_x86::r_shld_r_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    WORD_t src_16;
    DWORD_t dst_32;
    DWORD_t src_32;

    WORD mask_16;
    DWORD mask_32;

    WORD temp_16_1, temp_16_2;
    DWORD temp_32_1, temp_32_2;
    
    BYTE_t count;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    count = reg_restore_8(CL);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_16 = this->reg_restore_16(rm.offset);
            src_16 = this->reg_restore_16(r.offset);

            mask_16 = 0xffff;
            mask_16 << 0x10 - count.get_BYTE();

            temp_16_1 = src_16.get_WORD();
            temp_16_1 &= mask_16;
            temp_16_1 >> 0x10 - count.get_BYTE();

            temp_16_2 = dst_16.get_WORD();
            temp_16_2 << count.get_BYTE();
            temp_16_2 |= temp_16_1;

            dst_16.set_WORD(temp_16_2);

            this->reg_store_16(rm.offset, dst_16);
            break;
        case (MODRM_MEM):
            this->restore_32(rm.offset, dst_32);
            src_32 = this->reg_restore_32(r.offset);

            mask_32 = 0xffffffff;
            mask_32 << 0x20 - count.get_BYTE();

            temp_32_1 = src_32.get_DWORD();
            temp_32_1 &= mask_32;
            temp_32_1 >> 0x20 - count.get_BYTE();

            temp_32_2 = dst_32.get_DWORD();
            temp_32_2 << count.get_BYTE();
            temp_32_2 |= temp_32_1;

            dst_32.set_DWORD(temp_32_2);

            this->store_32(rm.offset, dst_32);
            break;
    }

    return 0x0;
}

int taint_x86::r_shrd_r_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    WORD_t src_16;
    DWORD_t dst_32;
    DWORD_t src_32;

    WORD mask_16;
    DWORD mask_32;

    WORD temp_16_1, temp_16_2;
    DWORD temp_32_1, temp_32_2;
    
    BYTE_t count;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    count.from_mem(instr_ptr + this->current_instr_length);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_16 = this->reg_restore_16(rm.offset);
            src_16 = this->reg_restore_16(r.offset);

            mask_16 = 0xffff;
            mask_16 >> 0x10 - count.get_BYTE();

            temp_16_1 = src_16.get_WORD();
            temp_16_1 &= mask_16;
            temp_16_1 << 0x10 - count.get_BYTE();

            temp_16_2 = dst_16.get_WORD();
            temp_16_2 >> count.get_BYTE();
            temp_16_2 |= temp_16_1;

            dst_16.set_WORD(temp_16_2);

            this->reg_store_16(rm.offset, dst_16);
            break;
        case (MODRM_MEM):
            this->restore_32(rm.offset, dst_32);
            src_32 = this->reg_restore_32(r.offset);

            mask_32 = 0xffffffff;
            mask_32 >> 0x20 - count.get_BYTE();

            temp_32_1 = src_32.get_DWORD();
            temp_32_1 &= mask_32;
            temp_32_1 << 0x20 - count.get_BYTE();

            temp_32_2 = dst_32.get_DWORD();
            temp_32_2 >> count.get_BYTE();
            temp_32_2 |= temp_32_1;

            dst_32.set_DWORD(temp_32_2);

            this->store_32(rm.offset, dst_32);
            break;
    }

    return 0x0;
}

int taint_x86::r_shrd_r_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    WORD_t src_16;
    DWORD_t dst_32;
    DWORD_t src_32;

    WORD mask_16;
    DWORD mask_32;

    WORD temp_16_1, temp_16_2;
    DWORD temp_32_1, temp_32_2;
    
    BYTE_t count;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    count = reg_restore_8(CL);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_16 = this->reg_restore_16(rm.offset);
            src_16 = this->reg_restore_16(r.offset);

            mask_16 = 0xffff;
            mask_16 >> 0x10 - count.get_BYTE();

            temp_16_1 = src_16.get_WORD();
            temp_16_1 &= mask_16;
            temp_16_1 << 0x10 - count.get_BYTE();

            temp_16_2 = dst_16.get_WORD();
            temp_16_2 >> count.get_BYTE();
            temp_16_2 |= temp_16_1;

            dst_16.set_WORD(temp_16_2);

            this->reg_store_16(rm.offset, dst_16);
            break;
        case (MODRM_MEM):
            this->restore_32(rm.offset, dst_32);
            src_32 = this->reg_restore_32(r.offset);

            mask_32 = 0xffffffff;
            mask_32 >> 0x20 - count.get_BYTE();

            temp_32_1 = src_32.get_DWORD();
            temp_32_1 &= mask_32;
            temp_32_1 << 0x20 - count.get_BYTE();

            temp_32_2 = dst_32.get_DWORD();
            temp_32_2 >> count.get_BYTE();
            temp_32_2 |= temp_32_1;

            dst_32.set_DWORD(temp_32_2);

            this->store_32(rm.offset, dst_32);
            break;
    }

    return 0x0;
}


/* ordered implementation ends */

/* decoded implementation */

// 80

int taint_x86::r_add_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 += src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_m_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 += src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() < old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();
        
    return 0x0;
}

int taint_x86::r_or_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 |= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_m_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            this->restore_8(rm.offset, dst_8);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 |= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }
    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_adc_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 += (src_8 + this->a_check_cf());
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_m_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 += (src_8 + this->a_check_cf());
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() < old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();
        
    return 0x0;
}

int taint_x86::r_sbb_rm_imm8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            src_8 += this->a_check_cf();
            dst_8 -= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_m_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            src_8 += this->a_check_cf();
            dst_8 -= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();
        
    return 0x0;
}

int taint_x86::r_and_rm_imm8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 &= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_m_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            this->restore_8(rm.offset, dst_8);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 &= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }
    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_sub_rm_imm8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 -= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_m_8(rm.offset);
            this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 -= src_8;
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();
        
    return 0x0;
}

int taint_x86::r_cmp_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 -= src_8;
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 -= src_8;
            break;
    }

    /* flags */
    if(dst_8.get_BYTE() > old_dst_8.get_BYTE()) 
        this->a_set_cf(); 
    else if((dst_8.get_BYTE() == old_dst_8.get_BYTE()) && (this->a_check_cf()))
        this->a_set_cf();
    else
        this->a_clear_cf();
        
    return 0x0;
}

// 81

int taint_x86::r_add_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 += src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 += src_32;
                    this->reg_store_32(rm.offset, dst_32);
                    
                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 += src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 += src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < src_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < src_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_or_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 |= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 |= src_32;
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 |= src_16;
                    this->store_16(rm.offset, dst_16);
                    
                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_32(rm.offset, dst_32);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 |= src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
    }
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_adc_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 += (src_32 + this->a_check_cf());
                    this->reg_store_32(rm.offset, dst_32);
                    
                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 += (src_32 + this->a_check_cf());
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < src_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < src_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    src_16 += this->a_check_cf();
                    dst_16 -= src_16;
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    src_32 += this->a_check_cf();
                    dst_32 -= src_32;
                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    src_16 += this->a_check_cf();
                    dst_16 -= src_16;
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    src_32 += this->a_check_cf();
                    dst_32 -= src_32;
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}
int taint_x86::r_and_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 &= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 &= src_32;
                    this->reg_store_32(rm.offset, dst_32);
                    
                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 &= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 &= src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }
    this->a_clear_cf();

    return 0x0;
}

int taint_x86::r_sub_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 -= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 -= src_32;
                    this->reg_store_32(rm.offset, dst_32);
                    
                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 -= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 -= src_32;
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_cmp_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    WORD_t src_16;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 -= src_16;
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 -= src_32;
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 -= src_16;
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_32.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 -= src_32;
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == src_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == src_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

// 83 

int taint_x86::r_add_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16, val_16;
    DWORD_t dst_32, old_dst_32, val_32;
    BYTE_t src_8;
    WORD temp_16;
    DWORD temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    //this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 += val_16;
                    this->reg_store_16(rm.offset, dst_16);
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    //this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 += val_32;
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    //this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 += val_16;
                    this->store_16(rm.offset, dst_16);
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    //this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 += val_32;
                    this->store_32(rm.offset, dst_32);
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < old_dst_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == old_dst_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < old_dst_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == old_dst_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_or_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16, val_16;
    DWORD_t dst_32, old_dst_32, val_32;
    BYTE_t src_8;
    WORD temp_16;
    DWORD temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());
                    dst_16 |= val_16;
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
                    dst_32 |= val_32;
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_16(rm.offset, dst_16);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());
                    dst_16 |= val_16;
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_32(rm.offset, dst_32);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
                    dst_32 |= val_32;
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());
                    this->store_32(rm.offset, dst_32);

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }
    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_adc_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16, val_16;
    DWORD_t dst_32, old_dst_32, val_32;
    BYTE_t src_8;
    WORD temp_16;
    DWORD temp_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 += (val_16 + this->a_check_cf());
                    this->reg_store_16(rm.offset, dst_16);
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 += (val_32 + this->a_check_cf());
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);

                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(rm.offset);
                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 += (val_16 + this->a_check_cf());
                    this->store_16(rm.offset, dst_16);
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(rm.offset);
                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    d_print(3, "Before: 0x%08x\n", dst_32.get_DWORD());

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 += (val_32 + this->a_check_cf());
                    this->store_32(rm.offset, dst_32);
                    d_print(3, "After: 0x%08x\n", dst_32.get_DWORD());

                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() < old_dst_16.get_WORD()) 
                this->a_set_cf(); 
            else if((dst_16.get_WORD() == old_dst_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() < old_dst_32.get_DWORD()) 
                this->a_set_cf(); 
            else if((dst_32.get_DWORD() == old_dst_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_sbb_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16, val_16;
    WORD temp_16;
    DWORD_t dst_32, old_dst_32, val_32;
    DWORD temp_32;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    
                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    val_16 += this->a_check_cf();
                    dst_16 -= val_16;
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    
                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    val_32 += this->a_check_cf();
                    dst_32 -= val_32;

                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    
                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    val_16 += this->a_check_cf();
                    dst_16-= val_16;
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    
                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    val_32 += this->a_check_cf();
                    dst_32-= val_32;

                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == old_dst_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == old_dst_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_and_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16;
    DWORD_t dst_32, old_dst_32;
    BYTE_t src_8;

    WORD temp_16;
    WORD_t src_16;
    DWORD temp_32;
    DWORD_t src_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    this->reg_propagation_cause_r_16(rm.offset);
//                    this->reg_propagation_cause_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    
                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    src_16.set_WORD(temp_16);
                    dst_16 &= src_16;

                    d_print(3, "Anding with: 0x%08x\n", src_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);

//                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
//                    this->reg_propagation_cause_r_32(rm.offset);
//                    this->reg_propagation_cause_r_32(r.offset);

                    dst_32 = this->reg_restore_32(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    
                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    d_print(3, "Extended: 0x%08x\n", temp_32);
                    src_32.set_DWORD(temp_32);
                    dst_32 &= src_32;

                    d_print(3, "Anding with: 0x%08x\n", src_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);

//                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    this->reg_propagation_cause_m_16(rm.offset);
//                    this->reg_propagation_cause_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    src_16.set_WORD(temp_16);
                    dst_16 &= src_16;
                    d_print(3, "Anding with: 0x%08x\n", src_16.get_WORD());

                    this->store_16(rm.offset, dst_16);

//                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
//                    this->reg_propagation_cause_m_32(rm.offset);
//                    this->reg_propagation_cause_r_32(r.offset);

                    this->restore_32(rm.offset, dst_32);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    d_print(3, "Extended: 0x%08x\n", temp_32);
                    src_32.set_DWORD(temp_32);
                    dst_32 &= src_32;
                    d_print(3, "Anding with: 0x%08x\n", src_32.get_DWORD());

                    this->store_32(rm.offset, dst_32);

//                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }
    this->a_clear_cf();
    return 0x0;
}

int taint_x86::r_sub_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16, val_16;
    DWORD_t dst_32, old_dst_32, val_32;
    BYTE_t src_8;
    DWORD temp_32;
    WORD temp_16;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    this->reg_propagation_cause_r_16(rm.offset);
//                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 -= val_16;
                    this->reg_store_16(rm.offset, dst_16);
 
//                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
//                    this->reg_propagation_cause_r_32(rm.offset);
//                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 -= val_32;
                    this->reg_store_32(rm.offset, dst_32);

//                    this->attach_current_propagation_r_32(rm.offset);

                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    this->reg_propagation_cause_r_16(rm.offset);
//                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 -= val_16;
                    this->store_16(rm.offset, dst_16);

//                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
//                    this->reg_propagation_cause_r_32(rm.offset);
//                    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 -= val_32;
                    this->store_32(rm.offset, dst_32);

//                    this->attach_current_propagation_m_32(rm.offset);

                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == old_dst_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == old_dst_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

int taint_x86::r_cmp_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16, old_dst_16, val_16;
    DWORD_t dst_32, old_dst_32, val_32;
    BYTE_t src_8;
    DWORD temp_32;
    WORD temp_16;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 -= val_16;
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    old_dst_32 = dst_32;
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 -= val_32;
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_16 = (WORD)((char)src_8.get_BYTE());
                    val_16.set_WORD(temp_16);

                    dst_16 -= val_16;
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    old_dst_32 = dst_32;
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    /* extend sign */
                    temp_32 = (DWORD)((char)src_8.get_BYTE());
                    val_32.set_DWORD(temp_32);

                    dst_32 -= val_32;
                    break;
            }
            break;
    }

    /* flags */
    switch(rm.size)
    {
        case MODRM_SIZE_16:
            if(dst_16.get_WORD() > old_dst_16.get_WORD()) 
                this->a_set_cf();
            else if((dst_16.get_WORD() == old_dst_16.get_WORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
        case MODRM_SIZE_32:
            if(dst_32.get_DWORD() > old_dst_32.get_DWORD()) 
                this->a_set_cf();
            else if((dst_32.get_DWORD() == old_dst_32.get_DWORD()) && (this->a_check_cf()))
                this->a_set_cf();
            else
                this->a_clear_cf();
            break;
    }

    return 0x0;
}

// c0

int taint_x86::r_sal_shl_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            //dst_8 <<= src_8;

            for(i=0x0; i< src_8.get_BYTE(); i++)
                dst_8 = this->a_mul_8(dst_8, BYTE_t(2));

            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            //dst_8 <<= src_8;

            for(i=0x0; i< src_8.get_BYTE(); i++)
                dst_8 = this->a_mul_8(dst_8, BYTE_t(2));

            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            //dst_8 >>= src_8;
            for(i=0x0; i<src_8.get_BYTE(); i++)
                dst_8 = this->a_div_8(dst_8, 0x2);

            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            //dst_8 >>= src_8;
            for(i=0x0; i<src_8.get_BYTE(); i++)
                dst_8 = this->a_div_8(dst_8, 0x2);

            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sar_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t src_8;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            //dst_8 >>= src_8;
            for(i=0x0; i<src_8.get_BYTE(); i++)
                dst_8 = this->a_idiv_8(dst_8, 0x2);

            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            //dst_8 >>= src_8;
            for(i=0x0; i<src_8.get_BYTE(); i++)
                dst_8 = this->a_idiv_8(dst_8, 0x2);

            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

// c1

int taint_x86::r_sal_shl_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    BYTE_t src_8;
    BYTE i;

    d_print(3, "r_sal_shl_rm_16_32_src_8\n");

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            d_print(3, "reg\n");
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    d_print(3, "Shift by 0x%02x\n", src_8.get_BYTE());
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());

                    for(i=0x0; i<src_8.get_BYTE(); i++)
                        dst_16 = this->a_mul_16(dst_16, 0x2);

                    //dst_16 <<= src_8;
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    d_print(3, "Shift by 0x%02x\n", src_8.get_BYTE());
                    d_print(3, "Before 0x%08x\n", dst_32.get_DWORD());

                    for(i=0x0; i<src_8.get_BYTE(); i++)
                        dst_32 = this->a_mul_32(dst_32, 0x2);

                    //dst_32 <<= src_8;
                    d_print(3, "After 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    for(i=0x0; i<src_8.get_BYTE(); i++)
                        dst_16 = this->a_mul_16(dst_16, 0x2);

                    //dst_16 <<= src_8;
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    src_8.from_mem(instr_ptr + this->current_instr_length);

                    for(i=0x0; i<src_8.get_BYTE(); i++)
                        dst_32 = this->a_mul_32(dst_32, 0x2);

                    //dst_32 <<= src_8;
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    BYTE_t src_8;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    src_8.from_mem(instr_ptr + this->current_instr_length);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_32 = this->reg_restore_32(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            //dst_32 >>= src_8;
            for(i=0x0; i<src_8.get_BYTE(); i++)
                dst_32 = this->a_div_32(dst_32, 0x2);

            this->reg_store_32(rm.offset, dst_32);
            break;
        case (MODRM_MEM):
            this->restore_32(rm.offset, dst_32);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            //dst_32 >>= src_8;
            for(i=0x0; i<src_8.get_BYTE(); i++)
                dst_32 = this->a_div_32(dst_32, 0x2);

            this->store_32(rm.offset, dst_32);
            break;
    }
    return 0x0;
}

int taint_x86::r_sar_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 >>= src_8;
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 >>= src_8;
                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 >>= src_8;
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    src_8.from_mem(instr_ptr + this->current_instr_length);
                    dst_32 >>= src_8;
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    return 0x0;
}

// c6

int taint_x86::r_mov_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8, old_dst_8;
    BYTE_t src_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    this->reg_propagation_cause_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
    src_8.from_mem(instr_ptr + this->current_instr_length);
    dst_8 += src_8;

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_store_8(rm.offset, dst_8);
            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->store_8(rm.offset, dst_8);
            this->attach_current_propagation_m_8(rm.offset);
            break;
    }

    return 0;
}

// c7

int taint_x86::r_mov_rm_imm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t dst_32, src_32;
    WORD_t dst_16, src_16;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    d_print(3, "Target: 0x%08x\n", rm.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
                    src_16.from_mem(instr_ptr +this->current_instr_length); 
                    this->reg_store_16(rm.offset, src_16);
                    this->attach_current_propagation_r_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
                    src_32.from_mem(instr_ptr +this->current_instr_length); 
                    this->reg_store_32(rm.offset, src_32);
                    this->attach_current_propagation_r_32(rm.offset);
                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
                    src_16.from_mem(instr_ptr +this->current_instr_length); 
                    this->store_16(rm.offset, src_16);
                    this->attach_current_propagation_m_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
                    src_32.from_mem(instr_ptr +this->current_instr_length); 
                    this->store_32(rm.offset, src_32);
                    this->attach_current_propagation_m_32(rm.offset);
                    break;
            }
            break;
    }

    return 0x0;
}

// d0

int taint_x86::r_sal_shl_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            //dst_8 <<= 1;
            dst_8 = this->a_mul_8(dst_8, 0x2);
            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            //dst_8 <<= 1;
            dst_8 = this->a_mul_8(dst_8, 0x2);
            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            //dst_8 >>= 1;
            dst_8 = this->a_div_8(dst_8, 0x2);
            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            //dst_8 >>= 1;
            dst_8 = this->a_div_8(dst_8, 0x2);
            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sar_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            //dst_8 >>= 1;
            dst_8 = this->a_idiv_8(dst_8, 0x2);
            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            //dst_8 >>= 1;
            dst_8 = this->a_idiv_8(dst_8, 0x2);
            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

// d1

int taint_x86::r_sal_shl_rm_16_32_1(BYTE_t* instr_ptr)
{

    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    //dst_16 <<= 1;

                    dst_16 = this->a_mul_16(dst_16, 0x2);

                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    //dst_32 <<= 1;

                    dst_32 = this->a_mul_32(dst_32, 0x2);

                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    //dst_16 <<= 1;

                    dst_16 = this->a_mul_16(dst_16, 0x2);

                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    //dst_32 <<= 1;

                    dst_32 = this->a_mul_32(dst_32, 0x2);

                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_16_32_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    //dst_16 >>= 1;
                    dst_16 = this->a_div_16(dst_16, 0x2);
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    //dst_32 >>= 1;
                    dst_32 = this->a_div_32(dst_32, 0x2);
                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    //dst_16 >>= 1;
                    dst_16 = this->a_div_16(dst_16, 0x2);
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    //dst_32 >>= 1;
                    dst_32 = this->a_div_32(dst_32, 0x2);
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }
    return 0x0;
}

int taint_x86::r_sar_rm_16_32_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    //dst_16 >>= 1;
                    dst_16 = this->a_idiv_16(dst_16, 0x2);
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    //dst_32 >>= 1;
                    dst_32 = this->a_idiv_32(dst_32, 0x2);
                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    //dst_16 >>= 1;
                    dst_16 = this->a_idiv_16(dst_16, 0x2);
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    //dst_32 >>= 1;
                    dst_32 = this->a_idiv_32(dst_32, 0x2);
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }
    return 0x0;
}

// d2

int taint_x86::r_sal_shl_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t cl;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            cl = this->reg_restore_8(CL);
            //dst_8 <<= cl;

            for(i=0x0; i< cl.get_BYTE(); i++)
                dst_8 = this->a_mul_8(dst_8, BYTE_t(2));

            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            cl = this->reg_restore_8(CL);
            //dst_8 <<= cl;

            for(i=0x0; i< cl.get_BYTE(); i++)
                dst_8 = this->a_mul_8(dst_8, BYTE_t(2));

            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_shr_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t cl;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            cl = this->reg_restore_8(CL);
            //dst_8 <<= cl;
            for(i=0x0; i<cl.get_BYTE(); i++)
                dst_8 = this->a_div_8(dst_8, 0x2);

            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            cl = this->reg_restore_8(CL);
            //dst_8 <<= cl;
            for(i=0x0; i<cl.get_BYTE(); i++)
                dst_8 = this->a_div_8(dst_8, 0x2);

            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

int taint_x86::r_sar_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;
    BYTE_t cl;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            cl = this->reg_restore_8(CL);
            //dst_8 <<= cl;
            for(i=0x0; i<cl.get_BYTE(); i++)
                dst_8 = this->a_idiv_8(dst_8, 0x2);

            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            cl = this->reg_restore_8(CL);
            //dst_8 <<= cl;
            for(i=0x0; i<cl.get_BYTE(); i++)
                dst_8 = this->a_idiv_8(dst_8, 0x2);

            this->store_8(rm.offset, dst_8);
            break;
    }

    return 0x0;
}

// d3

int taint_x86::r_sal_shl_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    BYTE_t cl;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    cl = this->reg_restore_8(CL);
                    //dst_16 <<= cl;

                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_16 = this->a_mul_16(dst_16, 0x2);

                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    cl = this->reg_restore_8(CL);
                    //dst_32 <<= cl;

                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_32 = this->a_mul_32(dst_32, 0x2);

                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    cl = this->reg_restore_8(CL);
                    //dst_16 <<= cl;

                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_16 = this->a_mul_16(dst_16, 0x2);

                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    cl = this->reg_restore_8(CL);
                    //dst_32 <<= cl;

                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_32 = this->a_mul_32(dst_32, 0x2);

                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }
    return 0x0;
}

int taint_x86::r_shr_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    BYTE_t cl;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    cl = this->reg_restore_8(CL);
                    //dst_16 >>= cl;
                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_16 = this->a_div_16(dst_16, 0x2);
                    
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    cl = this->reg_restore_8(CL);
                    //dst_32 >>= cl;
                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_32 = this->a_div_32(dst_32, 0x2);

                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    cl = this->reg_restore_8(CL);
                    //dst_16 >>= cl;
                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_16 = this->a_div_16(dst_16, 0x2);

                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    cl = this->reg_restore_8(CL);
                    //dst_32 >>= cl;
                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_32 = this->a_div_32(dst_32, 0x2);

                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }
    return 0x0;
}

int taint_x86::r_sar_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;
    BYTE_t cl;
    BYTE i;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    cl = this->reg_restore_8(CL);
                    //dst_16 >>= cl;
                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_16 = this->a_idiv_16(dst_16, 0x2);

                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    cl = this->reg_restore_8(CL);
                    //dst_32 >>= cl;
                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_32 = this->a_idiv_32(dst_32, 0x2);

                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    cl = this->reg_restore_8(CL);
                    //dst_16 >>= cl;
                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_16 = this->a_idiv_16(dst_16, 0x2);

                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    cl = this->reg_restore_8(CL);
                    //dst_32 >>= cl;
                    for(i=0x0; i<cl.get_BYTE(); i++)
                        dst_32 = this->a_idiv_32(dst_32, 0x2);

                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }
    return 0x0;
}

int taint_x86::r_rcl_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);
            this->a_rcl_8(val);
            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);
            this->a_rcl_8(val);
            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_rcr_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);
            this->a_rcr_8(val);
            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);
            this->a_rcr_8(val);
            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_rol_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);
            this->a_rol_8(val);
            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);
            this->a_rol_8(val);
            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_ror_rm_8_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);
            this->a_ror_8(val);
            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);
            this->a_ror_8(val);
            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_rcl_rm_16_32_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    this->a_rcl_16(val16);
                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    this->a_rcl_32(val32);
                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    this->a_rcl_16(val16);
                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    this->a_rcl_32(val32);
                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_rcr_rm_16_32_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    this->a_rcr_16(val16);
                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    this->a_rcr_32(val32);
                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    this->a_rcr_16(val16);
                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    this->a_rcr_32(val32);
                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }


    return 0x0;
}

int taint_x86::r_rol_rm_16_32_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    this->a_rol_16(val16);
                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    this->a_rol_32(val32);
                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    this->a_rol_16(val16);
                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    this->a_rol_32(val32);
                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }


    return 0x0;
}

int taint_x86::r_ror_rm_16_32_1(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    this->a_ror_16(val16);
                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    this->a_ror_32(val32);
                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    this->a_ror_16(val16);
                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    this->a_ror_32(val32);
                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }


    return 0x0;
}

int taint_x86::r_rol_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;
    BYTE count;
    unsigned i;

    count = this->reg_restore_8(CL).get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);

            for(i = 0x0; i<count; i++)
                this->a_rol_8(val);

            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);

            for(i = 0x0; i<count; i++)
                this->a_rol_8(val);

            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_ror_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;
    BYTE count;
    unsigned i;

    count = this->reg_restore_8(CL).get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);

            for(i = 0x0; i<count; i++)
                this->a_ror_8(val);

            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);

            for(i = 0x0; i<count; i++)
                this->a_ror_8(val);

            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_rcl_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;
    BYTE count;
    unsigned i;

    count = this->reg_restore_8(CL).get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);

            for(i = 0x0; i<count; i++)
                this->a_rcl_8(val);

            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);

            for(i = 0x0; i<count; i++)
                this->a_rcl_8(val);

            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_rcr_rm_8_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;
    BYTE count;
    unsigned i;

    count = this->reg_restore_8(CL).get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);

            for(i = 0x0; i<count; i++)
                this->a_rcr_8(val);

            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);

            for(i = 0x0; i<count; i++)
                this->a_rcr_8(val);

            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_rcl_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;
    BYTE count;
    unsigned i;

    count = this->reg_restore_8(CL).get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcl_16(val16);

                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcl_16(val16);

                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcl_16(val16);

                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcl_16(val16);

                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_rcr_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;
    BYTE count;
    unsigned i;

    count = this->reg_restore_8(CL).get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcr_16(val16);

                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcr_16(val16);

                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcr_16(val16);

                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcr_16(val16);

                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_rol_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;
    BYTE count;
    unsigned i;

    count = this->reg_restore_8(CL).get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rol_16(val16);

                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rol_16(val16);

                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rol_16(val16);

                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rol_16(val16);

                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_ror_rm_16_32_cl(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;
    BYTE count;
    unsigned i;

    count = this->reg_restore_8(CL).get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_ror_16(val16);

                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_ror_16(val16);

                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_ror_16(val16);

                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_ror_16(val16);

                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_rcl_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;
    BYTE_t imm_8;
    BYTE count;
    unsigned i;

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    count = imm_8.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);

            for(i = 0x0; i<count; i++)
                this->a_rcl_8(val);

            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);

            for(i = 0x0; i<count; i++)
                this->a_rcl_8(val);

            this->store_8(rm.offset, val);
            break;
    }


    return 0x0;
}

int taint_x86::r_rcr_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;
    BYTE_t imm_8;
    BYTE count;
    unsigned i;

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    count = imm_8.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);

            for(i = 0x0; i<count; i++)
                this->a_rcr_8(val);

            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);

            for(i = 0x0; i<count; i++)
                this->a_rcr_8(val);

            this->store_8(rm.offset, val);
            break;
    }


    return 0x0;
}

int taint_x86::r_rol_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;
    BYTE_t imm_8;
    BYTE count;
    unsigned i;

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    count = imm_8.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);

            for(i = 0x0; i<count; i++)
                this->a_rol_8(val);

            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);

            for(i = 0x0; i<count; i++)
                this->a_rol_8(val);

            this->store_8(rm.offset, val);
            break;
    }


    return 0x0;
}

int taint_x86::r_ror_rm_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    BYTE_t val;
    BYTE_t imm_8;
    BYTE count;
    unsigned i;

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    count = imm_8.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            val = this->reg_restore_8(rm.offset);

            for(i = 0x0; i<count; i++)
                this->a_ror_8(val);

            this->reg_store_8(rm.offset, val);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, val);

            for(i = 0x0; i<count; i++)
                this->a_ror_8(val);

            this->store_8(rm.offset, val);
            break;
    }

    return 0x0;
}

int taint_x86::r_rcl_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;
    BYTE_t imm_8;
    BYTE count;
    unsigned i;

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    count = imm_8.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcl_16(val16);

                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcl_16(val16);

                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcl_16(val16);

                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcl_16(val16);

                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_rcr_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;
    BYTE_t imm_8;
    BYTE count;
    unsigned i;

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    count = imm_8.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcr_16(val16);

                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcr_16(val16);

                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcr_16(val16);

                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rcr_16(val16);

                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }


    return 0x0;
}

int taint_x86::r_rol_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;
    BYTE_t imm_8;
    BYTE count;
    unsigned i;

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    count = imm_8.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rol_16(val16);

                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rol_16(val16);

                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rol_16(val16);

                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_rol_16(val16);

                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }

    return 0x0;
}

int taint_x86::r_ror_rm_16_32_imm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    this->a_decode_modrm(instr_ptr +1, &r, &rm);
    WORD_t val16;
    DWORD_t val32;
    BYTE_t imm_8;
    BYTE count;
    unsigned i;

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    count = imm_8.get_BYTE();

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    val16 = this->reg_restore_16(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_ror_16(val16);

                    this->reg_store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    val32 = this->reg_restore_32(rm.offset);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_ror_16(val16);

                    this->reg_store_32(rm.offset, val32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val16);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_ror_16(val16);

                    this->store_16(rm.offset, val16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, val32);
                    
                    for(i = 0x0; i< count; i++)
                        this->a_ror_16(val16);

                    this->store_32(rm.offset, val32);
                    break;
            }
            break;
    }


    return 0x0;
}



// f6

int taint_x86::r_not_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_32, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(rm.offset);
            d_print(3, "Before: 0x%08x\n", dst_8.get_BYTE());
            dst_8.set_BYTE(~dst_8.get_BYTE());
            d_print(3, "After: 0x%08x\n", dst_8.get_BYTE());
            this->reg_store_8(rm.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(rm.offset, dst_8);
            d_print(3, "Before: 0x%08x\n", dst_8.get_BYTE());
            dst_8.set_BYTE(~dst_8.get_BYTE());
            d_print(3, "After: 0x%08x\n", dst_8.get_BYTE());
            this->store_8(rm.offset, dst_8);
            break;
    }
 

    return 0x0;
}

int taint_x86::r_neg_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_32, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(r.offset);
            dst_8.set_BYTE(-dst_8.get_BYTE());
            this->reg_store_8(r.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, dst_8);
            dst_8.set_BYTE(-dst_8.get_BYTE());
            this->store_8(r.offset, dst_8);
            break;
    }
 

    return 0x0;
}

int taint_x86::r_mul_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t al, dst_8;
    WORD_t ax;
    WORD res;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(AL);
            this->reg_propagation_cause_r_8(rm.offset);

            al = this->reg_restore_8(AL);
            dst_8 = this->reg_restore_8(rm.offset);

            res = this->a_f_mul_8(al, dst_8);
            ax.set_WORD(res);

            this->reg_store_16(AX, ax);

            this->attach_current_propagation_r_16(AX);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(AL);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(AL, al);
            this->restore_8(rm.offset, dst_8);

            res = this->a_f_mul_8(al, dst_8);
            ax.set_WORD(res);

            this->store_16(AX, ax);

            this->attach_current_propagation_r_16(AX);
            break;
    }

    return 0x0;
}

int taint_x86::r_imul_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t al, dst_8;
    WORD_t ax;
    WORD res;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            this->reg_propagation_cause_r_8(AL);
            this->reg_propagation_cause_r_8(rm.offset);

            al = this->reg_restore_8(AL);
            dst_8 = this->reg_restore_8(rm.offset);

            res = this->a_f_imul_8(al, dst_8);
            ax.set_WORD(res);

            this->reg_store_16(AX, ax);

            this->attach_current_propagation_r_16(AX);
            break;
        case (MODRM_MEM):
            this->reg_propagation_cause_r_8(AL);
            this->reg_propagation_cause_m_8(rm.offset);

            this->restore_8(AL, al);
            this->restore_8(rm.offset, dst_8);

            res = this->a_f_imul_8(al, dst_8);
            ax.set_WORD(res);

            this->store_16(AX, ax);

            this->attach_current_propagation_r_16(AX);
            break;
    }

    return 0x0;
}

// f7

int taint_x86::r_not_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    d_print(3, "Before not: 0x%08x\n", dst_16.get_WORD());
                    dst_16.set_WORD(~dst_16.get_WORD());
                    d_print(3, "After not: 0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    d_print(3, "Before not: 0x%08x\n", dst_32.get_DWORD());
                    dst_32.set_DWORD(~dst_32.get_DWORD());
                    d_print(3, "After not: 0x%08x\n", dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(r.offset, dst_16);
                    d_print(3, "Before not: 0x%08x\n", dst_16.get_WORD());
                    dst_16.set_WORD(~dst_16.get_WORD());
                    d_print(3, "After not: 0x%08x\n", dst_16.get_WORD());
                    this->store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(r.offset, dst_32);
                    d_print(3, "Before not: 0x%08x\n", dst_32.get_DWORD());
                    dst_32.set_DWORD(~dst_32.get_DWORD());
                    d_print(3, "After not: 0x%08x\n", dst_32.get_DWORD());
                    this->store_32(r.offset, dst_32);
                    break;
            }
            break;
    }
 
    return 0x0;
}

int taint_x86::r_neg_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(rm.offset);
                    dst_16.set_WORD(-dst_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(rm.offset);
                    dst_32.set_DWORD(-dst_32.get_DWORD());
                    this->reg_store_32(rm.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    dst_16.set_WORD(-dst_16.get_WORD());
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    dst_32.set_DWORD(-dst_32.get_DWORD());
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }
 
    return 0x0;
}

int taint_x86::r_div_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t al, ah, src_8;
    UWORD ax;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            ax = this->reg_restore_16(AX).get_WORD();
            src_8 = this->reg_restore_8(rm.offset);
            if(src_8.get_BYTE() == 0x0)
            {
                d_print(1, "Division by zero detected.\n");
                return 0x1;
            }
            ah = ax % src_8.get_BYTE();
            al = ax / src_8.get_BYTE();

            this->reg_store_8(AL, al);
            this->reg_store_8(AH, ah);
            break;
        case (MODRM_MEM):
            ax = this->reg_restore_16(AX).get_WORD();
            this->restore_8(rm.offset, src_8);
            if(src_8.get_BYTE() == 0x0)
            {
                d_print(1, "Division by zero detected.\n");
                return 0x1;
            }

            ah = ax % src_8.get_BYTE();
            al = ax / src_8.get_BYTE();

            this->reg_store_8(AL, al);
            this->reg_store_8(AH, ah);
            break;
    }

    return 0x0;
}

int taint_x86::r_idiv_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t al, ah, src_8;
    WORD ax;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            ax = this->reg_restore_16(AX).get_WORD();
            src_8 = this->reg_restore_8(rm.offset);
            if(src_8.get_BYTE() == 0x0)
            {
                d_print(1, "Division by zero detected.\n");
                return 0x1;
            }
            ah = ax % (char)src_8.get_BYTE();
            al = ax / (char)src_8.get_BYTE();

            this->reg_store_8(AL, al);
            this->reg_store_8(AH, ah);
            break;
        case (MODRM_MEM):
            ax = this->reg_restore_16(AX).get_WORD();
            this->restore_8(rm.offset, src_8);
            if(src_8.get_BYTE() == 0x0)
            {
                d_print(1, "Division by zero detected.\n");
                return 0x1;
            }

            ah = ax % (char)src_8.get_BYTE();
            al = ax / (char)src_8.get_BYTE();

            this->reg_store_8(AL, al);
            this->reg_store_8(AH, ah);
            break;
    }

    return 0x0;
}

// fe

int taint_x86::r_inc_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(r.offset);
            dst_8 += 1;
            this->reg_store_8(r.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, dst_8);
            dst_8 += 1;
            this->store_8(r.offset, dst_8);
            break;
    }

    /* flags */
    /* cf not affected */

    return 0x0;
}

int taint_x86::r_dec_rm_8(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    BYTE_t dst_8;

    this->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_8, MODE_8);

    switch(rm.region)
    {
        case (MODRM_REG):
            dst_8 = this->reg_restore_8(r.offset);
            dst_8 -= 1;
            this->reg_store_8(r.offset, dst_8);
            break;
        case (MODRM_MEM):
            this->restore_8(r.offset, dst_8);
            dst_8 -= 1;
            this->store_8(r.offset, dst_8);
            break;
    }

    /* flags */
    /* cf not affected */

    return 0x0;
}

// ff

int taint_x86::r_inc_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(r.offset);
                    dst_16 += 1;
                    this->reg_store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(r.offset);
                    dst_32 += 1;
                    this->reg_store_32(r.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    dst_16 += 1;
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    dst_32 += 1;
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    /* flags */
    /* cf not affected */

    return 0x0;
}

int taint_x86::r_dec_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t dst_16;
    DWORD_t dst_32;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    dst_16 = this->reg_restore_16(r.offset);
                    dst_16 -= 1;
                    this->reg_store_16(r.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    dst_32 = this->reg_restore_32(r.offset);
                    dst_32 -= 1;
                    this->reg_store_32(r.offset, dst_32);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, dst_16);
                    dst_16 -= 1;
                    this->store_16(rm.offset, dst_16);
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, dst_32);
                    dst_32 -= 1;
                    this->store_32(rm.offset, dst_32);
                    break;
            }
            break;
    }

    /* flags */
    /* cf not affected */

    return 0x0;
}

int taint_x86::r_call_abs_near(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    DWORD_t target;
    WORD_t target_16;
    modrm_ptr rm, r;
    char out_line[MAX_NAME];

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    ret_addr = this->reg_restore_32(EIP);

    if(rm.region == MODRM_REG)
    {
            target = this->reg_restore_32(rm.offset);
    }
    else
    {
            this->restore_32(rm.offset, target);
    }

    ret_addr += this->current_instr_length;
    d_print(3, "Adding 0x%02x to ret\n", this->current_instr_length);

    a_push_32(ret_addr);
    
    this->reg_store_32(EIP, target);
    this->current_instr_is_jump = 0x1;

    return 0x0;
}

int taint_x86::r_call_abs_far(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    DWORD_t target;
    char out_line[MAX_NAME];

    target.from_mem(instr_ptr + this->current_instr_length);
    ret_addr = this->reg_restore_32(EIP);
    ret_addr += 0x5;

    a_push_32(ret_addr);
    
    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());
    this->reg_store_32(EIP, target);
    this->current_instr_is_jump = 0x1;

    return 0x0;
}

int taint_x86::r_push_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    DWORD_t temp;
    WORD_t val_16;

    temp = 0;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    // TODO: sometimes still zero :<

    d_print(3, "From: 0x%08x\n", rm.offset);

    switch(rm.region)
    {
        case MODRM_REG:
            d_print(3, "Push from reg\n");
            switch(rm.size)
            {
                case MODRM_SIZE_16:
//                    rm.offset = this->a_offset_recalculate_16(rm.offset);
                    val_16 = this->reg_restore_16(rm.offset);
                    temp += val_16;
                    break;
                case MODRM_SIZE_32:
                    temp = this->reg_restore_32(rm.offset);
                    break;
            }
        case MODRM_MEM:
            d_print(3, "Push from mem\n");
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->restore_16(rm.offset, val_16);
                    temp += val_16;
                    break;
                case MODRM_SIZE_32:
                    this->restore_32(rm.offset, temp);
                    break;
            }
            break;
    }
    
//    d_print(3, "Mem @ 0x%08x\n", rm.offset);
//    print_mem(3, rm.offset, 0x10);

    d_print(3, "Pushing: 0x%08x\n", temp.get_DWORD());
    a_push_32(temp);
    
    return 0x0;
}

int taint_x86::r_mul_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16, ax, dx;
    UWORD temp_16_1, temp_16_2;
    DWORD_t src_32, eax, edx;
    UDWORD temp_32_1, temp_32_2;

    DWORD res1;
    QWORD res2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(AX);
                    this->reg_propagation_cause_r_16(rm.offset);

                    ax = this->reg_restore_16(AX);
                    src_16 = this->reg_restore_16(rm.offset);

                    res1 = this->a_f_mul_16(ax, src_16);
                    
                    ax.set_WORD(res1 & 0xffff);
                    res1 >>= 0x10;
                    dx.set_WORD(res1 & 0xffff);

                    this->reg_store_16(AX, ax);
                    this->reg_store_16(DX, dx);

                    this->attach_current_propagation_r_16(AX);
                    this->attach_current_propagation_r_16(DX);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(AX);
                    this->reg_propagation_cause_r_32(rm.offset);

                    eax = this->reg_restore_32(EAX);
                    src_32 = this->reg_restore_32(rm.offset);

                    res2 = this->a_f_mul_32(eax, src_32);
                    
                    eax.set_DWORD(res2 & 0xffffffff);
                    res2 >>= 0x20;
                    edx.set_DWORD(res2 & 0xffffffff);

                    this->reg_store_32(EAX, eax);
                    this->reg_store_32(EDX, edx);

                    this->attach_current_propagation_r_32(AX);
                    this->attach_current_propagation_r_32(DX);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(AX);
                    this->reg_propagation_cause_m_16(rm.offset);

                    ax = this->reg_restore_16(AX);
                    this->restore_16(rm.offset, src_16);

                    res1 = this->a_f_mul_16(ax, src_16);
                    
                    ax.set_WORD(res1 & 0xffff);
                    res1 >>= 0x10;
                    dx.set_WORD(res1 & 0xffff);

                    this->reg_store_16(AX, ax);
                    this->reg_store_16(DX, dx);

                    this->attach_current_propagation_r_16(AX);
                    this->attach_current_propagation_r_16(DX);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(AX);
                    this->reg_propagation_cause_m_32(rm.offset);

                    eax = this->reg_restore_32(EAX);
                    this->restore_32(rm.offset, src_32);

                    res2 = this->a_f_mul_32(eax, src_32);
                    
                    eax.set_DWORD(res2 & 0xffffffff);
                    res2 >>= 0x20;
                    edx.set_DWORD(res2 & 0xffffffff);

                    this->reg_store_32(EAX, eax);
                    this->reg_store_32(EDX, edx);

                    this->attach_current_propagation_r_32(AX);
                    this->attach_current_propagation_r_32(DX);
                    break;
            }
            break;
    }


    return 0x0;
}

int taint_x86::r_imul_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16, ax, dx;
    UWORD temp_16_1, temp_16_2;
    DWORD_t src_32, eax, edx;
    UDWORD temp_32_1, temp_32_2;

    DWORD res1;
    QWORD res2;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(AX);
                    this->reg_propagation_cause_r_16(rm.offset);

                    ax = this->reg_restore_16(AX);
                    src_16 = this->reg_restore_16(rm.offset);

                    res1 = this->a_f_imul_16(ax, src_16);
                    
                    ax.set_WORD(res1 & 0xffff);
                    res1 >>= 0x10;
                    dx.set_WORD(res1 & 0xffff);

                    this->reg_store_16(AX, ax);
                    this->reg_store_16(DX, dx);

                    this->attach_current_propagation_r_16(AX);
                    this->attach_current_propagation_r_16(DX);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(AX);
                    this->reg_propagation_cause_r_32(rm.offset);

                    eax = this->reg_restore_32(EAX);
                    src_32 = this->reg_restore_32(rm.offset);

                    res2 = this->a_f_imul_32(eax, src_32);
                    
                    eax.set_DWORD(res2 & 0xffffffff);
                    res2 >>= 0x20;
                    edx.set_DWORD(res2 & 0xffffffff);

                    this->reg_store_32(EAX, eax);
                    this->reg_store_32(EDX, edx);

                    this->attach_current_propagation_r_32(AX);
                    this->attach_current_propagation_r_32(DX);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_cause_r_16(AX);
                    this->reg_propagation_cause_m_16(rm.offset);

                    ax = this->reg_restore_16(AX);
                    this->restore_16(rm.offset, src_16);

                    res1 = this->a_f_imul_16(ax, src_16);
                    
                    ax.set_WORD(res1 & 0xffff);
                    res1 >>= 0x10;
                    dx.set_WORD(res1 & 0xffff);

                    this->reg_store_16(AX, ax);
                    this->reg_store_16(DX, dx);

                    this->attach_current_propagation_r_16(AX);
                    this->attach_current_propagation_r_16(DX);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_cause_r_32(AX);
                    this->reg_propagation_cause_m_32(rm.offset);

                    eax = this->reg_restore_32(EAX);
                    this->restore_32(rm.offset, src_32);

                    res2 = this->a_f_imul_32(eax, src_32);
                    
                    eax.set_DWORD(res2 & 0xffffffff);
                    res2 >>= 0x20;
                    edx.set_DWORD(res2 & 0xffffffff);

                    this->reg_store_32(EAX, eax);
                    this->reg_store_32(EDX, edx);

                    this->attach_current_propagation_r_32(AX);
                    this->attach_current_propagation_r_32(DX);
                    break;
            }
            break;
    }


    return 0x0;
}

int taint_x86::r_div_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16, ax, dx;
    UWORD temp_16_1, temp_16_2;
    DWORD_t src_32, eax, edx;
    UDWORD temp_32_1, temp_32_2;
    UDWORD axdx;
    UQWORD eaxedx;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    ax = this->reg_restore_16(AX);
                    dx = this->reg_restore_16(DX);
                    src_16 = this->reg_restore_16(rm.offset);

                    if(src_16.get_WORD() == 0x0) 
                    {
                        d_print(1, "Division by zero detected\n");
                        return 0x1;
                    }
                    axdx = dx.get_WORD();
                    axdx <<= 0x10;
                    axdx += ax.get_WORD();

                    dx = axdx % (UWORD)src_16.get_WORD();
                    ax = axdx / (UWORD)src_16.get_WORD();
                    this->reg_store_16(AX, ax);
                    this->reg_store_16(DX, dx);
                    break;
                case MODRM_SIZE_32:
                    eax = this->reg_restore_32(EAX);
                    edx = this->reg_restore_32(EDX);
                    src_32 = this->reg_restore_32(rm.offset);

                    if(src_32.get_DWORD() == 0x0) 
                    {
                        d_print(1, "Division by zero detected\n");
                        return 0x1;
                    }

                    eaxedx = edx.get_DWORD();
                    eaxedx <<=0x20;
                    eaxedx += eax.get_DWORD();

                    edx = (UDWORD)(eaxedx % (UDWORD)src_32.get_DWORD());
                    eax = (UDWORD)(eaxedx / (UDWORD)src_32.get_DWORD());
                    this->reg_store_32(EAX, eax);
                    this->reg_store_32(EDX, edx);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    ax = this->reg_restore_16(AX);
                    this->restore_16(rm.offset, src_16);

                    if(src_16.get_WORD() == 0x0) 
                    {
                        d_print(1, "Division by zero detected\n");
                        return 0x1;
                    }
                    axdx = dx.get_WORD();
                    axdx <<= 0x10;
                    axdx += ax.get_WORD();

                    dx = axdx % (UWORD)src_16.get_WORD();
                    ax = axdx / (UWORD)src_16.get_WORD();
                    this->reg_store_16(AX, ax);
                    this->reg_store_16(DX, dx);
                    break;
                case MODRM_SIZE_32:
                    eax = this->reg_restore_32(EAX);
                    this->restore_32(rm.offset, src_32);

                    if(src_32.get_DWORD() == 0x0) 
                    {
                        d_print(1, "Division by zero detected\n");
                        return 0x1;
                    }

                    eaxedx = edx.get_DWORD();
                    eaxedx <<=0x20;
                    eaxedx += eax.get_DWORD();

                    edx = (UDWORD)(eaxedx % (UDWORD)src_32.get_DWORD());
                    eax = (UDWORD)(eaxedx / (UDWORD)src_32.get_DWORD());
                    this->reg_store_32(EAX, eax);
                    this->reg_store_32(EDX, edx);
                    break;
            }
            break;
    }


    return 0x0;
}

int taint_x86::r_idiv_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16, ax, dx;
    DWORD_t src_32, eax, edx;

    this->a_decode_modrm(instr_ptr +1, &r, &rm);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    ax = this->reg_restore_16(AX);
                    src_16 = this->reg_restore_16(rm.offset);
                    if(src_16.get_WORD() == 0x0) 
                    {
                        d_print(1, "Division by zero detected\n");
                        return 0x1;
                    }
                    dx = this->a_mod_16(dx, src_16);
                    ax = this->a_idiv_16(ax, src_16);
                    this->reg_store_16(AX, ax);
                    this->reg_store_16(DX, dx);
                    break;
                case MODRM_SIZE_32:
                    eax = this->reg_restore_32(EAX);
                    src_32 = this->reg_restore_32(rm.offset);
                    if(src_32.get_DWORD() == 0x0) 
                    {
                        d_print(1, "Division by zero detected\n");
                        return 0x1;
                    }
                    edx = this->a_mod_32(edx, src_32);
                    eax = this->a_idiv_32(eax, src_32);
                    this->reg_store_32(EAX, eax);
                    this->reg_store_32(EDX, edx);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    ax = this->reg_restore_16(AX);
                    this->restore_16(rm.offset, src_16);
                    if(src_16.get_WORD() == 0x0) 
                    {
                        d_print(1, "Division by zero detected\n");
                        return 0x1;
                    }
                    dx = this->a_mod_16(dx, src_16);
                    ax = this->a_idiv_16(ax, src_16);
                    this->reg_store_16(AX, ax);
                    this->reg_store_16(DX, dx);
                    break;
                case MODRM_SIZE_32:
                    eax = this->reg_restore_32(EAX);
                    this->restore_32(rm.offset, src_32);
                    if(src_32.get_DWORD() == 0x0) 
                    {
                        d_print(1, "Division by zero detected\n");
                        return 0x1;
                    }
                    edx = this->a_mod_32(edx, src_32);
                    eax = this->a_idiv_32(eax, src_32);
                    this->reg_store_32(EAX, eax);
                    this->reg_store_32(EDX, edx);
                    break;
            }
            break;
    }


    return 0x0;
}

/* decoded implementation ends */

// rest of main functions


int taint_x86::print_security_layers(int level, OFFSET off)
{
    unsigned i;
   
    d_print(level, "Security layers:\n");

    for(i = 0x0; i< this->security_layer_count; i++)
    {
        d_print(level, "0x%x +0x%x\n", this->security_layer[i].off, this->security_layer[i].size);
        d_print(level, "0x%x +0x%x\n", this->security_layer[i].off, this->security_layer[i].size);
        if(off >= this->security_layer[i].off)
        {
            d_print(1, "Offset 0x%08x above 0x%08x\n", off, this->security_layer[i].off);
        }
        if(off <= (this->security_layer[i].off + this->security_layer[i].size))
        {
            d_print(1, "Offset 0x%08x below 0x%08x\n", off, (this->security_layer[i].off + this->security_layer[i].size));
        }
        if((this->security_layer[i].off <= off) && ((this->security_layer[i].off + this->security_layer[i].size) >= off))
        {
            d_print(1, "Offset 0x%08x in range\n", off);
        }
    }

    return 0x0;
}


int taint_x86::print_security_layers(int level)
{
    unsigned i;
   
    d_print(level, "Security layers:\n");

    for(i = 0x0; i< this->security_layer_count; i++)
    {
        d_print(level, "0x%x +0x%x\n", this->security_layer[i].off, this->security_layer[i].size);
    }

    return 0x0;
}


int taint_x86::print_mem(int level, OFFSET start, DWORD len)
{
    for(OFFSET i=start; i<start+len; i++)
    {
        d_print(level, "0x%x: 0x%x\n", i, this->memory[i].get_BYTE());
    }
    d_print(level, "\n");
}

int taint_x86::print_t_mem(int level, OFFSET start, DWORD len)
{
    for(OFFSET i=start; i<start+len; i++)
    {
        d_print(level, "0x%x: 0x%x\n", i, this->memory[i].get_BYTE_t());
    }
    d_print(level, "\n");
}


int taint_x86::print_err_mem(OFFSET start, DWORD len)
{
    char line_temp[MAX_NAME];
    char line1[MAX_NAME];
    char line2[MAX_NAME];
    char cur_byte;

    line_temp[0]=0x0;
    line1[0]=0x0;
    line2[0]=0x0;

    err_print("Memory @ 0x%08x:\n", start);
    for(OFFSET i=0x0; i<len; i++)
    {
        if(((i) % 0x10) == 0x0)
        {
            err_print("%s\t%s\n", line1, line2);
            line1[0]=0x0;
            line2[0]=0x0;
            sprintf(line_temp, "0x%08x: ", start+i);
            strcat(line1, line_temp);
        }
        cur_byte = this->memory[start+i].get_BYTE();        
        sprintf(line_temp, "%02x ", cur_byte & 0xff);
        strcat(line1, line_temp);
        if(cur_byte <0x20) cur_byte = 0x2e;
        if(cur_byte >0x80) cur_byte = 0x2e;

        sprintf(line_temp, "%c", cur_byte);
        strcat(line2, line_temp);
    }
    err_print("%s\t%s\n", line1, line2);
    err_print("\n");
}

int taint_x86::print_err_t_mem(OFFSET start, DWORD len)
{
    err_print("Memory TAINT @ 0x%08x:\n", start);
    for(OFFSET i=start; i<start+len; i++)
    {
        err_print("0x%x: 0x%x\n", i, this->memory[i].get_BYTE_t());
    }
    err_print("\n");
}


int taint_x86::print_err_stack(DWORD tid, DWORD len)
{
    OFFSET start;
    OFFSET addr;
    DWORD_t temp;
    len *= 0x4;

    start = this->reg_restore_32(ESP, tid).get_DWORD();

    err_print("Stack in TID 0x%08x:\n", tid);
    for(OFFSET i=start; i<start+len; i+=0x4)
    {
        temp.from_mem(&this->memory[i]);
        err_print("0x%08x: 0x%08x\n", i, temp.get_DWORD());
    }
    err_print("\n");
}

int taint_x86::print_err_t_stack(DWORD tid, DWORD len)
{
    OFFSET start;
    OFFSET addr;
    DWORD_t temp;
    len *= 0x4;

    start = this->reg_restore_32(ESP, tid).get_DWORD();

    err_print("Stack TAINT in TID 0x%08x:\n", tid);
    for(OFFSET i=start; i<start+len; i+=0x4)
    {
        temp.from_mem(&this->memory[i]);
        err_print("0x%08x: 0x%08x\n", i, temp.get_DWORD_t());
    }
    err_print("\n");
}

int taint_x86::print_stack(DWORD tid, int level, DWORD len)
{
    OFFSET start;
    OFFSET addr;
    DWORD_t temp;
    len *= 0x4;

    start = this->reg_restore_32(ESP, tid).get_DWORD();

    d_print(level, "Stack in TID 0x%08x:\n", tid);
    for(OFFSET i=start; i<start+len; i+=0x4)
    {
        temp.from_mem(&this->memory[i]);
        d_print(level, "0x%08x: 0x%08x\n", i, temp.get_DWORD());
    }
    d_print(level, "\n");
}

int taint_x86::print_t_stack(DWORD tid, int level, DWORD len)
{
    OFFSET start;
    OFFSET addr;
    DWORD_t temp;
    len *= 0x4;

    start = this->reg_restore_32(ESP, tid).get_DWORD();

    d_print(level, "Context 0x%08x:\n", tid);
    for(OFFSET i=start; i<start+len; i+=0x4)
    {
        temp.from_mem(&this->memory[i]);
        d_print(level, "0x%08x: 0x%08x\n", i, temp.get_DWORD_t());
    }
    d_print(level, "\n");
}

int taint_x86::print_stack(int level, DWORD len)
{
    return this->print_stack(this->cur_tid, level, len);
}

int taint_x86::print_t_stack(int level, DWORD len)
{
    return this->print_t_stack(this->cur_tid, level, len);
}

int taint_x86::print_context()
{
    return this->print_context(this->cur_tid);
}

int taint_x86::print_t_context()
{
    return this->print_t_context(this->cur_tid);
}

int taint_x86::print_err_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        err_print("No such context: 0x%08x\n", tid);
        return 0x0;
    }
    err_print("\nWARNING!!! EIP is one instruction late!\n");
    err_print("Context for TID: 0x%08x:\n", tid);
    err_print("EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_DWORD());
    err_print("EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_DWORD());
    err_print("ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_DWORD());
    err_print("EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_DWORD());
    err_print("ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_DWORD());
    err_print("EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_DWORD());
    err_print("EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_DWORD());
    err_print("ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_DWORD());
    err_print("EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_DWORD());
    err_print("EFL: 0x%08x\n", this->reg_restore_32(EFLAGS, tid).get_DWORD());
    err_print("\n");

    DWORD eflags;
    eflags = this->reg_restore_32(EFLAGS).get_DWORD();
    if(eflags & EFLAGS_CF) err_print("CF\n");
    if(eflags & EFLAGS_PF) err_print("PF\n");
    if(eflags & EFLAGS_AF) err_print("AF\n");
    if(eflags & EFLAGS_ZF) err_print("ZF\n");
    if(eflags & EFLAGS_SF) err_print("SF\n");
    if(eflags & EFLAGS_IF) err_print("IF\n");
    if(eflags & EFLAGS_DF) err_print("DF\n");
    if(eflags & EFLAGS_OF) err_print("OF\n");
    err_print("\n");
}

int taint_x86::print_err_t_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        err_print("No such context: 0x%08x\n", tid);
        return 0x0;
    }
    err_print("Context TAINT for TID: 0x%08x:\n", tid);
    err_print("EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_DWORD_t());
    err_print("EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_DWORD_t());
    err_print("ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_DWORD_t());
    err_print("EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_DWORD_t());
    err_print("ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_DWORD_t());
    err_print("EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_DWORD_t());
    err_print("EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_DWORD_t());
    err_print("ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_DWORD_t());
    err_print("EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_DWORD_t());
    err_print("EFL: 0x%08x\n", this->reg_restore_32(EFLAGS, tid).get_DWORD_t());
    err_print("\n");

    DWORD eflags;
    eflags = this->reg_restore_32(EFLAGS).get_DWORD_t();
    if(eflags & EFLAGS_CF) err_print("CF\n");
    if(eflags & EFLAGS_PF) err_print("PF\n");
    if(eflags & EFLAGS_AF) err_print("AF\n");
    if(eflags & EFLAGS_ZF) err_print("ZF\n");
    if(eflags & EFLAGS_SF) err_print("SF\n");
    if(eflags & EFLAGS_IF) err_print("IF\n");
    if(eflags & EFLAGS_DF) err_print("DF\n");
    if(eflags & EFLAGS_OF) err_print("OF\n");
    err_print("\n");
}

int taint_x86::print_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        d_print(1, "No such context: 0x%08x\n", tid);
        return 0x0;
    }
    d_print(1, "\nWARNING!!! EIP is one instruction late!\n");
    d_print(1, "Context for TID: 0x%08x:\n", tid);
    d_print(1, "EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_DWORD());
    d_print(1, "EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_DWORD());
    d_print(1, "ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_DWORD());
    d_print(1, "EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_DWORD());
    d_print(1, "ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_DWORD());
    d_print(1, "EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_DWORD());
    d_print(1, "EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_DWORD());
    d_print(1, "ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_DWORD());
    d_print(1, "EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_DWORD());
    d_print(1, "EFL: 0x%08x\n", this->reg_restore_32(EFLAGS, tid).get_DWORD());
    d_print(1, "\n");

    DWORD eflags;
    eflags = this->reg_restore_32(EFLAGS).get_DWORD();
    if(eflags & EFLAGS_CF) d_print(1, "CF\n");
    if(eflags & EFLAGS_PF) d_print(1, "PF\n");
    if(eflags & EFLAGS_AF) d_print(1, "AF\n");
    if(eflags & EFLAGS_ZF) d_print(1, "ZF\n");
    if(eflags & EFLAGS_SF) d_print(1, "SF\n");
    if(eflags & EFLAGS_IF) d_print(1, "IF\n");
    if(eflags & EFLAGS_DF) d_print(1, "DF\n");
    if(eflags & EFLAGS_OF) d_print(1, "OF\n");
    d_print(1, "\n");
}

int taint_x86::print_t_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        d_print(1, "No such context: 0x%08x\n", tid);
        return 0x0;
    }
    d_print(1, "Taint context 0x%08x:\n", tid);
    d_print(1, "EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_DWORD_t());
    d_print(1, "EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_DWORD_t());
    d_print(1, "ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_DWORD_t());
    d_print(1, "EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_DWORD_t());
    d_print(1, "ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_DWORD_t());
    d_print(1, "EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_DWORD_t());
    d_print(1, "EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_DWORD_t());
    d_print(1, "ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_DWORD_t());
    d_print(1, "EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_DWORD_t());
    d_print(1, "EFL: 0x%08x\n", this->reg_restore_32(EFLAGS, tid).get_DWORD_t());
    d_print(1, "\n");

    DWORD eflags;
    eflags = this->reg_restore_32(EFLAGS).get_DWORD_t();
    if(eflags & EFLAGS_CF) d_print(1, "CF\n");
    if(eflags & EFLAGS_PF) d_print(1, "PF\n");
    if(eflags & EFLAGS_AF) d_print(1, "AF\n");
    if(eflags & EFLAGS_ZF) d_print(1, "ZF\n");
    if(eflags & EFLAGS_SF) d_print(1, "SF\n");
    if(eflags & EFLAGS_IF) d_print(1, "IF\n");
    if(eflags & EFLAGS_DF) d_print(1, "DF\n");
    if(eflags & EFLAGS_OF) d_print(1, "OF\n");
    d_print(1, "\n");
}

int taint_x86::verify_t_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        d_print(3, "No such context: 0x%08x\n", tid);
        return 0x0;
    }
    if(this->reg_restore_32(EAX, tid).get_DWORD_t() == 0x0
        && this->reg_restore_32(EBX, tid).get_DWORD_t() == 0x0
        && this->reg_restore_32(ECX, tid).get_DWORD_t() == 0x0
        && this->reg_restore_32(EDX, tid).get_DWORD_t() == 0x0
        && this->reg_restore_32(ESI, tid).get_DWORD_t() == 0x0
        && this->reg_restore_32(EDI, tid).get_DWORD_t() == 0x0
        && this->reg_restore_32(EBP, tid).get_DWORD_t() == 0x0
        && this->reg_restore_32(ESP, tid).get_DWORD_t() == 0x0
        && this->reg_restore_32(EIP, tid).get_DWORD_t() == 0x0)
        d_print(3, "Context not flawed\n");
    else
    {
        d_print(3, "Context flawed 0x%08x:\n", tid);
        d_print(3, "EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_DWORD_t());
        d_print(3, "EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_DWORD_t());
        d_print(3, "ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_DWORD_t());
        d_print(3, "EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_DWORD_t());
        d_print(3, "ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_DWORD_t());
        d_print(3, "EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_DWORD_t());
        d_print(3, "EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_DWORD_t());
        d_print(3, "ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_DWORD_t());
        d_print(3, "EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_DWORD_t());
        d_print(3, "EFL: 0x%08x\n", this->reg_restore_32(EFLAGS, tid).get_DWORD_t());
    }
    d_print(3, "\n");

    DWORD eflags;
    eflags = this->reg_restore_32(EFLAGS).get_DWORD_t();
    if(eflags & EFLAGS_CF) d_print(3, "CF\n");
    if(eflags & EFLAGS_PF) d_print(3, "PF\n");
    if(eflags & EFLAGS_AF) d_print(3, "AF\n");
    if(eflags & EFLAGS_ZF) d_print(3, "ZF\n");
    if(eflags & EFLAGS_SF) d_print(3, "SF\n");
    if(eflags & EFLAGS_IF) d_print(3, "IF\n");
    if(eflags & EFLAGS_DF) d_print(3, "DF\n");
    if(eflags & EFLAGS_OF) d_print(3, "OF\n");
    d_print(3, "\n");
}

int taint_x86::print_all_stacks()
{
    unsigned i;
    for(i=0; i<this->tid_count; i++)
        this->print_stack(this->ctx_info[i].tid, 1, 0x10);
    return 0x0;
}

int taint_x86::print_all_t_stacks()
{
    unsigned i;
    for(i=0; i<this->tid_count; i++)
        this->print_t_stack(this->ctx_info[i].tid, 1, 0x10);
    return 0x0;
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

int taint_x86::print_err_all_stacks()
{
    unsigned i;
    for(i=0; i<this->tid_count; i++)
        this->print_err_stack(this->ctx_info[i].tid, 0x30);
    return 0x0;
}

int taint_x86::print_err_all_t_stacks()
{
    unsigned i;
    for(i=0; i<this->tid_count; i++)
        this->print_err_t_stack(this->ctx_info[i].tid, 0x30);
    return 0x0;
}

int taint_x86::print_err_all_contexts()
{
    unsigned i;
    for(i=0; i<this->tid_count; i++)
        this->print_err_context(this->ctx_info[i].tid);
    return 0x0;
}

int taint_x86::print_err_all_t_contexts()
{
    unsigned i;
    for(i=0; i<this->tid_count; i++)
        this->print_err_t_context(this->ctx_info[i].tid);
    return 0x0;
}

