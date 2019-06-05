#include <taint_plugin.h>
#include <emul_x86.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <debug.h>
#include <plugin.h>
#include <utils.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <limits.h>

#ifdef USE_DISTORM
#include <distorm.h>
#endif

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
    d_print(2, "taint_id: 0x%08x\n", taint_id);

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

int taint_plugin::resolve_affected(BYTE_t* affected, char* region, OFFSET* offset)
{
    long start, end;

    start = this->taint_eng->memory;
    end = start+(sizeof(BYTE_t)*MEM_SIZE);

    if((start <= affected) && (affected < end))
    {
        *region = MODRM_MEM;
        *offset = ((long)affected-start)/sizeof(BYTE_t);
        return 0x0;
    }

    CONTEXT_INFO* cur_context;
    unsigned i;

    for(i=0x0; i<this->taint_eng->tid_count; i++)
    {
        cur_context = &this->taint_eng->ctx_info[i];

        start = cur_context->registers;
        end = start+REG_SIZE;

        if((start <= affected) && (affected < end))
        {
            *region = MODRM_REG;
            *offset = ((long)affected-start)/sizeof(BYTE_t);
            return 0x0;
        }
    }

    return 0x1;
}

int taint_plugin::print_propagation(unsigned id, unsigned branches)
{
    unsigned i,j;
    unsigned count;

    CAUSE* cur_elem;
    
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
        d_print(2, "Propagations elem count: %d\n", this->taint_eng->propagations[id].cause_count);
       
        if(branches == 0x0) count = this->taint_eng->propagations[id].cause_count;
        else count = branches;

        unsigned last_instr = 0x0;
 
        for(i=0x0; i<count;i++)
        {
            if(this->taint_eng->propagations[cur_elem->cause_id].instr_count != last_instr)
            {
                d_print(2, "%d -> %d\n", id, cur_elem->cause_id);
                last_instr = this->taint_eng->propagations[cur_elem->cause_id].instr_count;
            }
            if(cur_elem->next)
            {
                d_print(2, "Traversing: 0x%08x -> 0x%08x\n");
                cur_elem = cur_elem->next;
            }
            else break;
        } 
    }

    for(j=0x0; j<this->out_tab; j++)
    {
        d_print_prompt(1, " ");
    }

    PROPAGATION* current_propagation;
    current_propagation = &this->taint_eng->propagations[id];

#ifdef USE_DISTORM
    char buf[0x20];
    int k;
    for(k=0x0; k<0x20; k++)
        buf[k] = this->taint_eng->memory[current_propagation->instruction +k].get_BYTE();

    /* decoding */

    unsigned decoded;
    _DecodeResult res;
 _DecodedInst decodedInstructions[0x1];
    
    res = distorm_decode(current_propagation->instruction, (const unsigned char*)buf, 0x20, Decode32Bits, decodedInstructions, 0x1, &decoded);

    d_print_prompt(1, "%d: (%d)0x%08x: \t%s%s%s, ", id, current_propagation->instr_count, current_propagation->instruction, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
#else
    d_print_prompt(1, "%d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d, ", id, current_propagation->instruction, this->taint_eng->memory[current_propagation->instruction].get_BYTE(), current_propagation->instr_count);
#endif
    d_print_prompt(1, "\tcauses: ");

    CAUSE* cur_cause;
    for(i=0x0,cur_cause=current_propagation->causes; i<current_propagation->cause_count; i++,cur_cause=cur_cause->next)
    {
        d_print_prompt(1, "%d, ", cur_cause->cause_id);
    }

    d_print_prompt(1, "\tresults: ");

    RESULT* cur_result;
    for(i=0x0,cur_result=current_propagation->results; i<current_propagation->result_count; i++,cur_result=cur_result->next)
    {
        d_print(3, "0x%08x, ", cur_result->affected);
        char region;
        OFFSET offset;
        if(resolve_affected(cur_result->affected, &region, &offset) != 0x0)
        {
            d_print_prompt(1, "Error resolving affected, ");
        }
        else
        {
            d_print(3, "Region = 0x%02x, Offset = 0x%08x\n", region, offset);
            if(region == MODRM_REG)
            {
                if((EAX <= offset) &&  (offset < EBX)) d_print_prompt(1, "EAX, ");
                if((EBX <= offset) &&  (offset < ECX)) d_print_prompt(1, "EBX, ");
                if((ECX <= offset) &&  (offset < EDX)) d_print_prompt(1, "ECX, ");
                if((EDX <= offset) &&  (offset < ESI)) d_print_prompt(1, "EDX, ");
                if((ESI <= offset) &&  (offset < EDI)) d_print_prompt(1, "ESI, ");
                if((EDI <= offset) &&  (offset < EBP)) d_print_prompt(1, "EDI, ");
                if((EBP <= offset) &&  (offset < ESP)) d_print_prompt(1, "EBP, ");
                if((ESP <= offset) &&  (offset < EFLAGS)) d_print_prompt(1, "ESP, ");
                if((EFLAGS <= offset) && (offset < EIP)) d_print_prompt(1, "EFLAGS, ");
                if((EIP <= offset)) d_print_prompt(1, "EIP, ");
            }
            else if(region == MODRM_MEM)
            {
                d_print_prompt(1, "MEM:0x%08x, ", offset);
            }
            else
                d_print_prompt(1, "unknown, ", offset);
        }
    }
    d_print_prompt(1, "\n");

    this->out_tab--;
    return 0x0;
}

int taint_plugin::print_taint_history(unsigned id, unsigned branches)
{
    if(id == 0x0) return 0x0;

    unsigned i,j;
    unsigned count;

    CAUSE* cur_elem;
    
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
        d_print(2, "Propagations elem count: %d\n", this->taint_eng->propagations[id].cause_count);
       
        if(branches == 0x0) count = this->taint_eng->propagations[id].cause_count;
        else count = branches;

        unsigned last_instr = 0x0;
 
        for(i=0x0; i<count;i++)
        {
            if(this->taint_eng->propagations[cur_elem->cause_id].instr_count != last_instr)
            {
                d_print(2, "%d -> %d\n", id, cur_elem->cause_id);
                this->print_taint_history(cur_elem->cause_id, branches);
                last_instr = this->taint_eng->propagations[cur_elem->cause_id].instr_count;
            }
            if(cur_elem->next)
            {
                d_print(2, "Traversing: 0x%08x -> 0x%08x\n");
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
    d_print_prompt(1, "%d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d\n", id, this->taint_eng->propagations[id].instruction, this->taint_eng->memory[this->taint_eng->propagations[id].instruction].get_BYTE(), this->taint_eng->propagations[id].instr_count);
#endif
    this->out_tab--;
    return 0x0;
}

/*
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
*/

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
/*
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
*/
        RESULT* cur_result;
        cur_result = cur_propagation->results;

        if(cur_result == 0x0) break;
        if(cur_result->affected == target)
        {
            found = 1;
            break;
        }

        while(cur_result->next)
        {
            cur_result = cur_result->next;
            if(cur_result->affected == target)
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
//    char command[MAX_NAME];
    char* command;

    while(1)
    {
        d_print_prompt(1, "\n---\n");
        //d_print_prompt(1, "Enter taint query and press [ENTER]\n> ");

        command = readline("Enter taint query and press [ENTER]\n> ");
        filter_str(command);
        err_print("Received: %s\n", command);

        if(this->taint_eng->prompt_file) fprintf(this->taint_eng->prompt_file, "%s\n", command);

        if(strcmp(command, "q") == 0x0)
        {
            this->taint_eng->aborted = 0x1;
            break;
        }
        else if(strcmp(command, "c") == 0x0) break;
        //else if(strstr(command, "que"))

        add_history(command);
        this->parse_cmd(command);
        free(command);
        /*
        if(this->parse_trace_string(command, &twp) != 0x0) continue;
        this->trace_watchpoint_connect(&twp);
        this->query_history(twp);
        */
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

int taint_plugin::set_query_tid(DWORD tid)
{
    this->query_tid = tid;
    return 0x0;
}

int taint_plugin::get_query_tid()
{
    DWORD tid;

    tid = this->query_tid;

    d_print_prompt(0, "Viewing thread: 0x%08x\n", tid);
    return 0x0;
}

int taint_plugin::parse_cmd(char* string)
{
    char* cur_str;
    DWORD tid;
    unsigned i, len;
    EXCEPTION_INFO info;
    OFFSET addr;
    unsigned size;

    err_print("Parsing: %s\n", string);

    if(this->query_tid == 0x0)
    {
        this->query_tid = this->taint_eng->cur_tid;
    }

    cur_str = strtok(string, " \n\r"); if(cur_str == 0x0) return 0x0;
    if(!strncmp(cur_str, "reg", 3))
    {
        cur_str = strtok(0x0, " \n\r"); 
        if(cur_str == 0x0)
        {
            tid = this->query_tid;                
            this->taint_eng->print_err_context(tid);
            return 0x0;
        }
        else
        {
            if(!strncmp(cur_str, "ta", 2))
            {
                cur_str = strtok(0x0, " \n\r"); 
                if(cur_str == 0x0)
                {
                    tid = this->query_tid;                
                    this->taint_eng->print_err_t_context(tid);
                    return 0x0;
                }
                else
                {
                    tid = strtol(cur_str, 0x0, 0x10); 
                    this->taint_eng->print_err_t_context(tid);
                    return 0x0;
                }

            }
            else
            {                
                tid = strtol(cur_str, 0x0, 0x10); 
                this->taint_eng->print_err_context(tid);
                return 0x0;
            }
        }

    }
    else if(!strncmp(cur_str, "mem", 3))
    {
        cur_str = strtok(0x0, " \n\r"); if(cur_str == 0x0) return 0x0;
        if(!strncmp(cur_str, "ta", 2))
        {
            cur_str = strtok(0x0, " \n\r"); if(cur_str == 0x0) return 0x0;
            addr = strtol(cur_str, 0x0, 0x10); 

            cur_str = strtok(0x0, " \n\r"); 

            if(cur_str == 0x0)
            {
                size = 0x10;
            }
            else
            {
                size = strtol(cur_str, 0x0, 0x10);
            }
      
            this->taint_eng->print_err_t_mem(addr, size);
            return 0x0;
        
        }
        else 
        {
            addr = strtol(cur_str, 0x0, 0x10); 
            cur_str = strtok(0x0, " \n\r"); 

            if(cur_str == 0x0)
            {
                size = 0x10;
            }
            else
            {
                size = strtol(cur_str, 0x0, 0x10);
            }
        
            this->taint_eng->print_err_mem(addr, size);
            return 0x0;
        }
    }
    else if(!strncmp(cur_str, "sta", 3))
    {
        cur_str = strtok(0x0, " \n\r"); 
        if(cur_str != 0x0 && !strncmp(cur_str, "ta", 2))
        {
            cur_str = strtok(0x0, " \n\r"); 
            if(cur_str == 0x0)
            {
                size = 0x10;
            }
            else
            {
                size = strtol(cur_str, 0x0, 0x10);
            }
        
            this->taint_eng->print_err_t_stack(this->query_tid, size);
            return 0x0;
        }
        else if(cur_str == 0x0)
        {
            size = 0x10;
        }
        else
        {
            size = strtol(cur_str, 0x0, 0x10);
        }
        
        this->taint_eng->print_err_stack(this->query_tid, size);
        return 0x0;
    }
    else if(!strncmp(cur_str, "exc", 3))
    {
        d_print_prompt(0, "There have been %d exceptions: \n", this->taint_eng->exceptions_count);

        for(i = 0x0; i< this->taint_eng->exceptions_count; i++)
        {
            info = this->taint_eng->exceptions[i];
            d_print_prompt(0x0, "Exception %08x in TID %08x, instr. no: %d, eip: 0x%08x\n", info.er.ExceptionCode, info.tid, this->taint_eng->current_instr_count, info.er.ExceptionAddress);
        }

        return 0x0;
    }
    else if(!strncmp(cur_str, "thr", 3) || !strncmp(cur_str, "tid", 3))
    {
        cur_str = strtok(0x0, " \n\r");
        if(cur_str == 0x0)
        {
            d_print_prompt(0, "There are %d threads: \n", this->taint_eng->tid_count);
            for(i = 0x0; i< this->taint_eng->tid_count; i++)
            {
                d_print_prompt(0, "0x%08x\n", this->taint_eng->ctx_info[i].tid);
            }
    
        }
        else if(!strcmp(cur_str, "set"))
        {
            cur_str = strtok(0x0, " \n\r"); if(cur_str == 0x0) return 0x0;

            tid = strtol(cur_str, 0x0, 0x10); 

            this->set_query_tid(tid);
            return 0x0;
        }
        else if(!strcmp(cur_str, "get"))
        {
            this->get_query_tid();
            return 0x0;
        }
    }
    else if(!strncmp(cur_str, "tai", 3))
    {
        d_print_prompt(0, "There have been %d taints:\n", this->taint_count);
        for(i = 0x0; i< this->taint_count; i++)
        {
            d_err_print("Taint no %d: 0x%08x, 0x%08x\n", i, this->taints[i].off, this->taints[i].size);
        }

    }
    else if(!strncmp(cur_str, "ins", 3))
    {
        unsigned count, current;

        cur_str = string + 4;

        cur_str = strtok(0x0, " \n\r"); 
        if(cur_str == 0x0) count = 0x5;
        else count = strtol(cur_str, 0x0, 0x10);

        if(count >= MAX_LAST_INSTRUCTIONS)
        {
            d_err_print(0x0, "MAX_LAST_INSTRUCTIONS in limits.h is: 0x%08x\n", MAX_LAST_INSTRUCTIONS);
            return 0x0;
        }
        
        d_err_print("Last %d instructions:\n", count);

        current = this->taint_eng->current_last;
        for(i = 0x0; i< count; i++)
        {
            d_print_prompt(0x0, "0x%08x\n", this->taint_eng->last_instructions[current]);
            current --;
            if((current < 0x0) || (current > MAX_LAST_INSTRUCTIONS)) current = MAX_LAST_INSTRUCTIONS-1;
        }

    }
    else if(!strncmp(cur_str, "his", 3))
    {
        unsigned count;

        cur_str = strtok(0x0, " \n\r"); 
        if(cur_str == 0x0) count = 0x5;
        else count = strtol(cur_str, 0x0, 0x10);

        HISTORY_STATE* myhist = history_get_history_state();
        HIST_ENTRY** mylist = history_list();

        if(count >= myhist->length) count = myhist->length;

        err_print("Printing %d commands in history\n", count);
        for(i =  0x0; i<count; i++)
        {
            err_print("%s\n", mylist[i]->line);
        }
    
    }
    else if(!strncmp(cur_str, "tra", 3))
    {
        unsigned i;

        cur_str = strtok(0x0, " \n\r"); if(cur_str == 0x0) return 0x0;

        if(!strncmp(cur_str, "eax", 3))
        {
            DWORD_t reg;

            reg = this->taint_eng->reg_restore_32(EAX, this->query_tid);

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking eax[%d]:\n", i);

                print_taint_history(&reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncmp(cur_str, "ebx", 3))
        {
            DWORD_t reg;

            reg = this->taint_eng->reg_restore_32(EBX, this->query_tid);

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking ebx[%d]:\n", i);

                print_taint_history(&reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncmp(cur_str, "ecx", 3))
        {
            DWORD_t reg;

            reg = this->taint_eng->reg_restore_32(ECX, this->query_tid);

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking ecx[%d]:\n", i);

                print_taint_history(&reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncmp(cur_str, "edx", 3))
        {
            DWORD_t reg;

            reg = this->taint_eng->reg_restore_32(EDX, this->query_tid);

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking edx[%d]:\n", i);

                print_taint_history(&reg[i], 0x0);
                err_print("\n");
            }
        }
        /* itd. */

    
    }
    else if(!strncmp(cur_str, "pro", 3))
    {
        unsigned count;

        cur_str = strtok(0x0, " \n\r"); 
        if(cur_str == 0x0) count = this->current_propagation_count;
        else count = strtol(cur_str, 0x0, 10);

        print_propagations(count);
    }
    return 0x0;
}

int taint_plugin::print_propagations(unsigned count)
{
    PROPAGATION* cur_propagation;
    unsigned taint_id;
    unsigned i;

    for(i = 0x0; i<count; i++)
    {
        print_propagation(i, 0x0);
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

