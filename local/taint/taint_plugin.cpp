#include <taint_plugin.h>
#include <emul_x86.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <debug.h>
#include <plugin.h>
#include <utils.h>

#include <limits.h>

/* przerobic tak by pokazywalo ze z pluginu
int taint_plugin::d_print(int level, const char* format, ...)
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
*/
int taint_plugin::pre_execute_instruction_callback(DWORD eip)
{
    return 0x0;
}

int taint_plugin::post_execute_instruction_callback(DWORD eip)
{
    return 0x0;
}

int taint_plugin::breakpoint_callback(BREAKPOINT* bp)
{
    d_print(1, "taint_plugin::breakpoint_callback\n");
    d_print(1, "taint Breakpoint %s @%lld, 0x%08x, hit\n", bp->name, this->taint_eng->current_instr_count, this->taint_eng->current_eip);
    this->prompt_taint();

    return 0x0;
}

int taint_plugin::start_callback()
{
    return 0x0;
}

int taint_plugin::finish_callback()
{
    return 0x0;    
}

int taint_plugin::add_thread_callback(CONTEXT_OUT ctx_out)
{
    return 0x0;
}

int taint_plugin::del_thread_callback(DWORD tid)
{
    return 0x0;
}

int taint_plugin::del_thread_srsly_callback(DWORD tid)
{
    return 0x0;
}

int taint_plugin::handle_exception_callback(EXCEPTION_INFO info)
{
    return 0x0;
}

int taint_plugin::write_history(FILE* f)
{
    unsigned i;
    /*
    for(i=0x0; i<this->taint_eng->current_propagation_count; i++) 
    {
        
    }
    */
    return 0x0;
}

int taint_plugin::print_taint_history(BYTE_t* target, unsigned branches)
{
    unsigned taint_id;
    taint_id = target->get_BYTE_t_id();

    if(taint_id == 0x0) return 0x0;
    else return this->print_taint_history(taint_id, branches);
}

#include <stdarg.h>

int taint_plugin::d_print_prompt(int level, const char* format, ...)
{
    va_list argptr;

    va_start(argptr, format);
    if(this->taint_eng->prompt_file)
        vfprintf(this->taint_eng->prompt_file, format, argptr);
    va_end(argptr);

    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);
    return 0x0;
}

int taint_plugin::print_taint_history(unsigned id, unsigned branches)
{
    if(id == 0x0) return 0x0;

    unsigned i,j;
    unsigned count;

    PROPAGATION_ELEM* cur_elem;
    
    /* identation */
    if(this->out_tab > MAX_OUT_TAB)
    {
        //d_print(1, "...\n");
        //d_err_print("...\n");
        return 0x0;
    }

    this->out_tab++;

//    d_print(3, "1\n");
    cur_elem = this->taint_eng->propagations[id].causes;
    if(cur_elem)
    {
        d_print(4, "Propagations elem count: %d\n", this->taint_eng->propagations[id].elem_count);
       
        if(branches == 0x0) count = this->taint_eng->propagations[id].elem_count;
        else count = branches;

        unsigned last_instr = 0x0;
 
        for(i=0x0; i<count;i++)
        {
            if(this->taint_eng->propagations[cur_elem->cause_id].instr_count != last_instr)
            {
                d_print(4, "%d -> %d\n", id, cur_elem->cause_id);
                this->print_taint_history(cur_elem->cause_id, branches);
                last_instr = this->taint_eng->propagations[cur_elem->cause_id].instr_count;
            }
            if(cur_elem->next)
            {
                d_print(4, "Traversing: 0x%08x -> 0x%08x\n");
                cur_elem = cur_elem->next;
            }
            else break;
        } 
    }

    for(j=0x0; j<this->out_tab; j++)
    {
        d_print_prompt(1, " ");
    }
#ifdef USE_DISTORM
    char buf[0x20];
    int k;
    for(k=0x0; k<0x20; k++)
        buf[k] = this->taint_eng->memory[this->taint_eng->propagations[id].instruction +k].get_BYTE();

    /* decoding */

    unsigned decoded;
    _DecodeResult res;
 _DecodedInst decodedInstructions[0x1];
    
    res = distorm_decode(this->taint_eng->propagations[id].instruction, (const unsigned char*)buf, 0x20, Decode32Bits, decodedInstructions, 0x1, &decoded);

    d_print_prompt(1, "%d: (%d)0x%08x: %s%s%s\n", id, this->taint_eng->propagations[id].instr_count, this->taint_eng->propagations[id].instruction, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
#else
    d_print_prompt(1, "Taint id: %d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d\n", id, this->taint_eng->propagations[id].instruction, this->taint_eng->memory[this->taint_eng->propagations[id].instruction].get_BYTE(), this->taint_eng->propagations[id].instr_count);
#endif
    this->out_tab--;
    return 0x0;
}

int taint_plugin::print_taint_ops(unsigned id)
{
    TAINTED* cur_op;

    cur_op = this->taint_eng->propagations[id].first_op;
    if(!cur_op) return 0x0;

    do
    {
        d_print(3, "ops");
        if(cur_op->next) cur_op = cur_op->next;
    }
    while(cur_op->next);

    return 0x0;
}

int taint_plugin::print_taint_history(BYTE_t* target, OFFSET instr_count, unsigned branches)
{
    PROPAGATION* cur_propagation;
    unsigned taint_id;
    taint_id = target->get_BYTE_t_id();

    unsigned i, j;
    DWORD found;

    if(taint_id == 0x0) return 0x0;

    found = 0x0;

    /* search for proper propagation */  
    for(i = this->taint_eng->current_propagation_count; i>0x0; i--)
    {
        cur_propagation = &this->taint_eng->propagations[i];
    //    d_err_print("Veryfing propagation: %lld\n", i);
        for(j=0x0; j<0x4; j++)
        {
            if(cur_propagation->result[j] == 0x0)
                break;
            if((cur_propagation->instr_count <= instr_count) && (cur_propagation->result[j] == target)) 
            {
                found = 1;
                break;
            }
        }
        if(found) break;
    }
    taint_id = i;

    if(taint_id == 0x0) return 0x0;
    else return this->print_taint_history(taint_id, branches);
}

int taint_plugin::parse_trace_string(char* string, TRACE_WATCHPOINT* twp)
{
    DWORD ctx_index;
    
    d_print(1, "Parsing: %s\n", string);

    twp->interactive = 0x0;
    twp->branches = 0x1;

    char* current;

    while(1)
    {
        if(string[0] == 'i') 
        {
            twp->interactive = 1;
            string ++;
            continue;
        }
        else if(string[0] == 'f') 
        {
            twp->branches = 0;
            string ++;
            continue;
        }
        else break;
    }

    current = strtok(string, ",");
    if(!current)
    {
        d_print(1, "Error parsing breakpoint: %s\n", string);
        return -1;
    }
    twp->offset = strtol(current, 0x0, 10);
    d_print(1, "Instr count: %lld\n", twp->offset);

    current = strtok(0x0, ",");
    if(!current)
    {
        d_print(1, "Error parsing breakpoint: %s\n", string);
        return -1;
    }
    twp->mem_offset = strtol(current, 0x0, 0x10);
    d_print(1, "Offset: 0x%08x\n", twp->mem_offset);

    current = strtok(0x0, ",");
    if(!current)
    {
        d_print(1, "Error parsing breakpoint: %s\n", string);
        return -1;
    }
    twp->tid = strtol(current, 0x0, 0x10);
    d_print(1, "TID: 0x%08x\n", twp->tid);

    ctx_index = this->taint_eng->tids[twp->tid];

    current = strtok(0x0, "+");
    if(!current)
    {
        d_print(1, "Error parsing breakpoint: %s\n", string);
        return -1;
    }
    strcpy(twp->name, current);
    d_print(1, "%s\n", twp->name);


    return 0x0;
}

int taint_plugin::query_history(TRACE_WATCHPOINT twp)
{
    d_print_prompt(1, "Printing history for instruction count: %lld\n", twp.offset);
    return this->print_taint_history(twp.watched, twp.offset, twp.branches);
}
int taint_plugin::prompt_taint()
{
    TRACE_WATCHPOINT twp;
    char command[MAX_NAME];

    while(1)
    {
        d_print_prompt(1, "\n---\n");
        d_print_prompt(1, "Enter taint query and press [ENTER]\n> ");
        scanf("%s", command);
        if(strcmp(command, "q") == 0x0) break;
        if(this->taint_eng->prompt_file) fprintf(this->taint_eng->prompt_file, "%s\n", command);
        if(command[0] == 'd')
        {
            this->dump_cmd(command);
            continue;
        }

        if(this->parse_trace_string(command, &twp) != 0x0) continue;
        this->trace_watchpoint_connect(&twp);
        this->query_history(twp);
    }
    return 0x0;
}

int taint_plugin::trace_watchpoint_connect(TRACE_WATCHPOINT* twp)
{
    if(strstr(twp->name, "EAX") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[EAX]);
    }
    else if(strstr(twp->name, "EBX") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[EBX]);
    }
    else if(strstr(twp->name, "ECX") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[ECX]);
    }
    else if(strstr(twp->name, "EDX") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[EDX]);
    }
    else if(strstr(twp->name, "ESI") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[ESI]);
    }
    else if(strstr(twp->name, "EDI") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[EDI]);
    }
    else if(strstr(twp->name, "EBP") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[EBP]);
    }
    else if(strstr(twp->name, "ESP") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[ESP]);
    }
    else if(strstr(twp->name, "EIP") != 0x0)
    {
        twp->watched = &(this->taint_eng->ctx_info[this->taint_eng->tids[twp->tid]].registers[EIP]);
    }
    else if(strtol(twp->name, 0x0, 0x10) != 0x0)
    {
        twp->watched = &(this->taint_eng->memory[strtol(twp->name, 0x0, 0x10)]);
    }

//    d_err_print("0x%08x\n", twp->watched);
    return 0x0;
}

int taint_plugin::dump_cmd(char* string)
{
    char* cur_str;
    DWORD tid;
    unsigned i, len;
    EXCEPTION_INFO info;
    unsigned off;

    if(string[0] == 'd' && string[1] == 'c' && string[2] == 't')
    {
        cur_str = string + 4;

        if(strlen(cur_str) == 0x0) tid = this->taint_eng->ctx_info[0x0].tid;
        else tid = strtol(cur_str, 0x0, 0x10);

        this->taint_eng->print_err_t_context(tid);
    }
    else if(string[0] == 'd' && string[1] == 'c')
    {
        cur_str = string + 3;

        if(strlen(cur_str) == 0x0) tid = this->taint_eng->ctx_info[0x0].tid;
        else tid = strtol(cur_str, 0x0, 0x10);

        this->taint_eng->print_err_context(tid);
    }
    if(string[0] == 'd' && string[1] == 's' && string[2] == 't')
    {
        cur_str = string + 4;
        if(strlen(cur_str) == 0x0) tid = this->taint_eng->ctx_info[0x0].tid;
        else tid = strtol(strtok(cur_str, ","), 0x0, 0x10);

        /*
        len = strtol(strtok(0x0, ","), 0x0, 0x10);
        if(len == 0x0) 
        */
        len = 0x10;

        this->taint_eng->print_err_t_stack(tid, len);
    }
    else if(string[0] == 'd' && string[1] == 's')
    {
        cur_str = string + 3;
        if(strlen(cur_str) == 0x0) tid = this->taint_eng->ctx_info[0x0].tid;
        else tid = strtol(strtok(cur_str, ","), 0x0, 0x10);

        /*
        len = strtol(strtok(0x0, ","), 0x0, 0x10);
        if(len == 0x0) 
        */
        len = 0x10;

        this->taint_eng->print_err_stack(tid, len);
    }
    if(string[0] == 'd' && string[1] == 'e')
    {
        d_print_prompt(1, "There have been %d exceptions: \n", this->taint_eng->exceptions_count);
        for(i = 0x0; i< this->taint_eng->exceptions_count; i++)
        {
            info = this->taint_eng->exceptions[i];
            d_print_prompt(1, "Exception %08x in TID %08x, instr. no: %d, eip: 0x%08x\n", info.er.ExceptionCode, info.tid, this->taint_eng->current_instr_count, info.er.ExceptionAddress);
        }
    }
    if(string[0] == 'd' && string[1] == 'h')
    {
        d_print_prompt(1, "There are %d threads: \n", this->taint_eng->tid_count);
        for(i = 0x0; i< this->taint_eng->tid_count; i++)
        {
            d_print_prompt(1, "0x%08x\n", this->taint_eng->ctx_info[i].tid);
        }
    
    }
    if(string[0] == 'd' && string[1] == 'm' && string[2] == 't')
    {
        cur_str = string + 4;

        if(strlen(cur_str) == 0x0) off = 0x0;
        else off = strtol(cur_str, 0x0, 0x10);

        d_print_prompt(1, "Memory taint dump @ 0x%08x:\n", off);

        this->taint_eng->print_err_t_mem(off, 0x10);
    }
    else if(string[0] == 'd' && string[1] == 'm')
    {
        cur_str = string + 3;

        if(strlen(cur_str) == 0x0) off = 0x0;
        else off = strtol(cur_str, 0x0, 0x10);

        d_print_prompt(1, "Memory dump @ 0x%08x:\n", off);

        this->taint_eng->print_err_mem(off, 0x10);
    }
    if(string[0] == 'd' && string[1] == 'r')
    {
        d_print_prompt(1, "Current status:\n");
        d_print_prompt(1, "Current EIP: \t\t0x%08x\n", this->taint_eng->current_eip);
        d_print_prompt(1, "Current instr. no: \t%d\n", this->taint_eng->current_instr_count);
    }
    if(string[0] == 'd' && string[1] == 't')
    {
        d_print_prompt(1, "There have been %d taints:\n", this->taint_count);
        for(i = 0x0; i< this->taint_count; i++)
        {
            d_err_print("Taint no %d: 0x%08x, 0x%08x\n", i, this->taints[i].off, this->taints[i].size);
            d_print(1, "Taint no %d: 0x%08x, 0x%08x\n", i, this->taints[i].off, this->taints[i].size);
        }
    }
    if(string[0] == 'd' && string[1] == 'l' && string[2] == 'i')
    {
        unsigned count, current;

        cur_str = string + 4;

        if(strlen(cur_str) == 0x0) count = 0x5;
        else count = strtol(cur_str, 0x0, 0x10);
        
        d_err_print("Last %d instructions:\n", count);

        current = this->taint_eng->current_last;
        for(i = 0x0; i< count; i++)
        {
            d_print_prompt(1, "0x%08x\n", this->taint_eng->last_instructions[current]);
            current --;
            if((current < 0x0) || (current > MAX_LAST_INSTRUCTIONS)) current = MAX_LAST_INSTRUCTIONS-1;
        }
    }

    return 0x0;
}

int taint_plugin::add_taint(OFFSET start, UDWORD length)
{
    UDWORD i;

    if(this->taint_count == MAX_TAINTS_OBSERVED)
    {
        d_print(1, "MAX_TAINTS_OBSERVED reached, unable to register taint\n");
        return 0x0;
    }

    // tainting
    for(i = 0x0; i< length; i++)
    {
        this->taint_eng->memory[start+i].set_BYTE_t(0xff);
    }

    this->taints[this->taint_count].off = start;
    this->taints[this->taint_count].size = length;
    this->taint_count++;

    return 0x0;
}

int taint_plugin::register_taint(char* line)
{
    char* cmd;
    DWORD len;
    OFFSET off;

    cmd = strtok(line, ",");
    off = (OFFSET)strtol(strtok(0x0, ","), 0x0, 0x10);
    len = (DWORD)strtol(strtok(0x0, ","), 0x0, 0x10);

    printf("Registering taint @ 0x%08x, with length: 0x%08x\n", off, len);
    this->add_taint(off, len);

    return 0x0;
}



/* parsing out options end */

int taint_plugin::parse_option(char* line)
{
    if(line[0] == 'R' && line[1] == 'N')
        this->register_taint(line);

    if(line[0] == 'S' && line[1] == 'P')
        return 0x0;
//        this->register_prefix(line);

    if(line[0] == 'B' && line[1] == 'L')
        return 0x0;
//        this->register_blacklist(line);

    if(line[0] == 'B' && line[1] == 'A')
        return 0x0;
//        this->register_blacklist_addr(line);

    if(line[0] == 'S' && line[1] == 'A')
        return 0x0;
//        this->register_silenced_addr(line);

    if(line[0] == 'I' && line[1] == 'N')
        return 0x0;
//        this->register_included(line);

    if(line[0] == 'F' && line[1] == 'W')
        return 0x0;
//        this->register_wanted(line);

    if(line[0] == 'I' && line[1] == 'W')
        return 0x0;
//        this->register_wanted_i(line);

    if(line[0] == 'E' && line[1] == 'W')
        return 0x0;
//        this->register_wanted_e(line);

    if(line[0] == 'J' && line[1] == 'E')
        return 0x0;
//        this->jxx_set(0x1);

    if(line[0] == 'J' && line[1] == 'D')
        return 0x0;
//        this->jxx_set(0x0);

    if(line[0] == 'J' && line[1] == 'C')
        return 0x0;
//        this->jxx_clear();

    if(line[0] == 'O' && line[1] == 'U')
        return 0x0;
//        this->register_comment(line);

    if(line[0] == 'R' && line[1] == 'L')
        return 0x0;
//        this->register_lib(line);
    
    if(line[0] == 'D' && line[1] == 'L')
        return 0x0;
//        this->deregister_lib(line);
            
    if(line[0] == 'S' && line[1] == 'Y')
        return 0x0;
//        this->register_symbol(line);

    return 0x0;
}

