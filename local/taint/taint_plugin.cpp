#include <taint_plugin.h>
#include <emul_x86.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <debug.h>
#include <plugin.h>
#include <utils.h>
#include <out_utils.h>
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
    if(bp)
    {
        d_print(1, "taint Breakpoint %s @%lld, 0x%08x, hit\n", bp->name, this->taint_eng->current_instr_count, this->taint_eng->current_eip);
    }
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

int taint_plugin::handle_sigint()
{
    this->prompt_taint();

    return 0x0;
}

int taint_plugin::handle_exception_callback(EXCEPTION_INFO info)
{
    this->prompt_taint();

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

/*
int taint_plugin::print_propagation_history(BYTE_t* target, unsigned branches)
{
    if(target->id == 0x0) return;

    unsigned i;

    PROPAGATION* current_propagation;
    current_propagation = this->taint_eng->propagations[target->id];

    for(i = 0x0; i< current_propagation->cause_count; i++)
    {
        CAUSE* cur_cause;

        if(i < MAX_CAUSES)
        {
            cur_cause = &current_propagation->causes[i];
            this->print_propagation_history
        }
        else
        {

        }
    }
    
    return 0x0;
}
*/

int taint_plugin::print_taint_history(BYTE_t* target, unsigned branches)
{
    unsigned taint_id;
    taint_id = target->get_BYTE_t_id();
    d_print(2, "taint_id: 0x%08x\n", taint_id);

    if(taint_id == 0x0) return 0x0;
    else return this->print_taint_history(taint_id, branches);
}

#include <stdarg.h>

int taint_plugin::d_print_prompt_red(int level, const char* format, ...)
{
    va_list argptr;


    va_start(argptr, format);
    if(this->taint_eng->prompt_file)
        vfprintf(this->taint_eng->prompt_file, format, argptr);
    va_end(argptr);

    fprintf(stdout, "\033[0;31m");
    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);
    fprintf(stdout, "\033[0m");

    return 0x0;
}

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

int taint_plugin::clear_propagation(unsigned id)
{
    unsigned i, count;
    CAUSE* cur_cause;
    CAUSE* next_cause;
    RESULT* cur_result; 
    RESULT* next_result;

    PROPAGATION* current_propagation;

    current_propagation = &this->taint_eng->propagations[id];

    current_propagation->cause_count = 0x0;
    current_propagation->result_count = 0x0;
    
    return 0x0;
}

int taint_plugin::clear_propagations()
{
    unsigned i;

    for(i=0x0; i<this->taint_eng->current_propagation_count; i++)
    {
        d_print(1, "Clearing propagation: %d\n", i);
        clear_propagation(i);
    }

    err_print("Setting current propagation count to 0x0\n");
    this->taint_eng->current_propagation_count = 0x0;

    err_print("Setting current extended causes count to 0x0\n");
    this->taint_eng->current_extended_causes_entry_id = 0x0;

    err_print("Setting current extended result count to 0x0\n");
    this->taint_eng->current_extended_results_entry_id = 0x0;
    return 0x0;
}

int taint_plugin::print_propagation(unsigned propagation_id, unsigned branches)
{
    if(propagation_id == CAUSE_ID_NONE) return 0x0;

    unsigned i,j;
    unsigned cause_count, result_count;

    PROPAGATION* current_propagation;
    CAUSE* cur_elem;
    
    current_propagation = &this->taint_eng->propagations[propagation_id];
    cause_count = current_propagation->cause_count;
    result_count = current_propagation->result_count;
    d_print(2, "Propagations elem count: %d\n", cause_count);

    /* identation */

    if(this->out_tab > MAX_OUT_TAB)
    {
        return 0x0;
    }

    this->out_tab++;

    //if(branches == 0x0) count = this->taint_eng->propagations[propagation_id].cause_count;
    //else count = branches;

    for(j=0x0; j<this->out_tab; j++)
    {
        d_print_prompt(1, "\t");
    }

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

    if(current_propagation->taint_propagation != 0x0)
        d_print_prompt_red(1, "%d: (%d)0x%08x: \t%s%s%s, ", propagation_id, current_propagation->instr_count, current_propagation->instruction, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
    else
        d_print_prompt(1, "%d: (%d)0x%08x: \t%s%s%s, ", propagation_id, current_propagation->instr_count, current_propagation->instruction, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
#else
    if(current_propagation->taint_propagation != 0x0)
        d_print_prompt_red(1, "%d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d, ", propagation_id, current_propagation->instruction, this->taint_eng->memory[current_propagation->instruction].get_BYTE(), current_propagation->instr_count);
    else
        d_print_prompt(1, "%d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d, ", propagation_id, current_propagation->instruction, this->taint_eng->memory[current_propagation->instruction].get_BYTE(), current_propagation->instr_count);
#endif
    d_print_prompt(1, "\tcauses: ");

    CAUSE* cur_cause;
    unsigned extended_id;

    for(i=0x0; i < cause_count; i++)
    {
        if(i < MAX_CAUSES)
        {
            cur_cause = &current_propagation->causes[i];
            d_print_prompt(1, "%d, ", cur_cause->cause_id);
        }
        else
        {
            extended_id = current_propagation->extended_cause_id;
            cur_cause = &this->taint_eng->extended_causes[extended_id][i - MAX_CAUSES];
            d_print_prompt(1, "%d(e), ", cur_cause->cause_id);
        }
    }

    d_print_prompt(1, "\tresults: ");

    RESULT* cur_result;

    for(i=0x0; i<result_count; i++)
    {
        if(i < MAX_RESULTS)
        {
            cur_result = &current_propagation->results[i];
            d_print(3, "0x%08x, ", cur_result->affected);
        }
        else
        {
            extended_id = current_propagation->extended_result_id;
            cur_result = &this->taint_eng->extended_results[extended_id][i - MAX_RESULTS];
            d_print(3, "0x%08x(e), ", cur_result->affected);
        }

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

int taint_plugin::print_taint_history_end(unsigned propagation_id, unsigned branches)
{
    if(propagation_id == CAUSE_ID_NONE) return 0x0;
    /* teoretycznie ponizsze nie powinno stanowic problemu, ale stanowi */
    if(propagation_id == 0x0) return 0x0;

    unsigned i,j;
    unsigned count;

    CAUSE* cur_elem;
    
    /* identation */
    if(this->out_tab > MAX_OUT_TAB)
    {
        return 0x0;
    }

    this->out_tab++;

    PROPAGATION* current_propagation;
    current_propagation = &this->taint_eng->propagations[propagation_id];

    for(j=0x0; j<this->out_tab; j++)
    {
        d_print_prompt(1, "\t");
    }
#ifdef USE_DISTORM
    char buf[0x20];
    int k;
    for(k=0x0; k<0x20; k++)
        buf[k] = this->taint_eng->memory[this->taint_eng->propagations[propagation_id].instruction +k].get_BYTE();

    /* decoding */

    unsigned decoded;
    _DecodeResult res;
 _DecodedInst decodedInstructions[0x1];
    
    res = distorm_decode(this, (const unsigned char*)buf, 0x20, Decode32Bits, decodedInstructions, 0x1, &decoded);

    if(current_propagation->taint_propagation != 0x0)
        d_print_prompt_red(1, "%d: (%d)0x%08x: %s%s%s\n", propagation_id, this->taint_eng->propagations[propagation_id].instr_count, this->taint_eng->propagations[propagation_id].instruction, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
    else
        d_print_prompt(1, "%d: (%d)0x%08x: %s%s%s\n", propagation_id, this->taint_eng->propagations[propagation_id].instr_count, this->taint_eng->propagations[propagation_id].instruction, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
#else
    if(current_propagation->taint_propagation != 0x0)
        d_print_prompt_red(1, "%d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d\n", propagation_id, this->taint_eng->propagations[propagation_id].instruction, this->taint_eng->memory[this->taint_eng->propagations[propagation_id].instruction].get_BYTE(), this->taint_eng->propagations[propagation_id].instr_count);
    else
        d_print_prompt(1, "%d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d\n", propagation_id, this->taint_eng->propagations[propagation_id].instruction, this->taint_eng->memory[this->taint_eng->propagations[propagation_id].instruction].get_BYTE(), this->taint_eng->propagations[propagation_id].instr_count);
#endif
    this->out_tab--;
    return 0x0;
}

int taint_plugin::print_taint_history(unsigned propagation_id, unsigned branches)
{
    if(propagation_id == CAUSE_ID_NONE) return 0x0;
    /* teoretycznie ponizsze nie powinno stanowic problemu, ale stanowi */
    if(propagation_id == 0x0) return 0x0;

    unsigned i,j;
    unsigned count;

    CAUSE* cur_elem;
    
    /* identation */
    if(this->out_tab > MAX_OUT_TAB)
    {
        return 0x0;
    }

    this->out_tab++;

    PROPAGATION* current_propagation;
    current_propagation = &this->taint_eng->propagations[propagation_id];

    for(j=0x0; j<this->out_tab; j++)
    {
        d_print_prompt(1, "\t");
    }
#ifdef USE_DISTORM
    char buf[0x20];
    int k;
    for(k=0x0; k<0x20; k++)
        buf[k] = this->taint_eng->memory[this->taint_eng->propagations[propagation_id].instruction +k].get_BYTE();

    /* decoding */

    unsigned decoded;
    _DecodeResult res;
 _DecodedInst decodedInstructions[0x1];
    
    res = distorm_decode(this, (const unsigned char*)buf, 0x20, Decode32Bits, decodedInstructions, 0x1, &decoded);

    if(current_propagation->taint_propagation != 0x0)
        d_print_prompt_red(1, "%d: (%d)0x%08x: %s%s%s\n", propagation_id, this->taint_eng->propagations[propagation_id].instr_count, this->taint_eng->propagations[propagation_id].instruction, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
    else
        d_print_prompt(1, "%d: (%d)0x%08x: %s%s%s\n", propagation_id, this->taint_eng->propagations[propagation_id].instr_count, this->taint_eng->propagations[propagation_id].instruction, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
#else
    if(current_propagation->taint_propagation != 0x0)
        d_print_prompt_red(1, "%d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d\n", propagation_id, this->taint_eng->propagations[propagation_id].instruction, this->taint_eng->memory[this->taint_eng->propagations[propagation_id].instruction].get_BYTE(), this->taint_eng->propagations[propagation_id].instr_count);
    else
        d_print_prompt(1, "%d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d\n", propagation_id, this->taint_eng->propagations[propagation_id].instruction, this->taint_eng->memory[this->taint_eng->propagations[propagation_id].instruction].get_BYTE(), this->taint_eng->propagations[propagation_id].instr_count);
#endif

    unsigned cause_count;
    cause_count = current_propagation->cause_count;
    d_print(2, "Propagations elem count: %d\n", cause_count);

    if(cause_count == 0x0)
    {
        /* check if not directly from taint source */
        /* but how? */
    }

    CAUSE* cur_cause;

    for(i=0x0; i<cause_count; i++)
    {
        cur_cause = &current_propagation->causes[i];
        this->print_taint_history(cur_cause->cause_id, branches);

/*
        if(this->taint_eng->propagations[cur_cause->cause_id].taint_propagation)
            this->print_taint_history(cur_cause->cause_id, branches);
        else
            this->print_taint_history_end(cur_cause->cause_id, branches);
*/
    } 
    

    this->out_tab--;
    return 0x0;
}

int taint_plugin::disas_instructions(OFFSET offset, unsigned count)
{
#ifdef USE_DISTORM
    char* buf;
    unsigned size = 0x20*count;
    buf = (char*)malloc(size);

    unsigned k;
    for(k=0x0; k<size; k++)
        buf[k] = this->taint_eng->memory[offset+k].get_BYTE();

    /* decoding */

    unsigned decoded;

    _DecodeResult res;
    _DecodedInst* decodedInstructions;

    decodedInstructions = (_DecodedInst*)malloc(sizeof(_DecodedInst)*count);
    
    res = distorm_decode(offset, (const unsigned char*)buf, size, Decode32Bits, decodedInstructions, count, &decoded);

    OFFSET cur_addr = offset;

    for(k=0x0; k<count; k++)
    {
        d_print_prompt(1, "0x%08x: %s%s%s\n", cur_addr, (char*)decodedInstructions[k].mnemonic.p, decodedInstructions[k].operands.length != 0 ? " " : "", (char*)decodedInstructions[k].operands.p);
        cur_addr += decodedInstructions[k].size;
    }

    free(decodedInstructions);
    free(buf);
#endif
    return 0x0;
}

int taint_plugin::disas_instruction(OFFSET offset)
{
#ifdef USE_DISTORM
    char buf[0x20];
    unsigned k;
    for(k=0x0; k<0x20; k++)
        buf[k] = this->taint_eng->memory[offset+k].get_BYTE();

    /* decoding */

    unsigned decoded;

    _DecodeResult res;
    _DecodedInst decodedInstructions[0x1];
    
    res = distorm_decode(offset, (const unsigned char*)buf, 0x20, Decode32Bits, decodedInstructions, 0x1, &decoded);

    d_print_prompt(1, "0x%08x: %s%s%s\n", offset, (char*)decodedInstructions[0].mnemonic.p, decodedInstructions[0].operands.length != 0 ? " " : "", (char*)decodedInstructions[0].operands.p);
#endif
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

    if(taint_id == CAUSE_ID_NONE) return 0x0;

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

int taint_plugin::prompt_taint()
{
    char* command;
    
    this->query_tid = 0x0;
    while(1)
    {
        d_print_prompt(1, "\n---\n");
        //d_print_prompt(1, "Enter taint query and press [ENTER]\n> ");
        command = 0x0;
        while(!command || strlen(command) == 0x0)
        {
            command = readline("Enter taint query and press [ENTER]\n> ");
            filter_str(command);

            if(strlen(command) == 0x0)
            {
                if(history_length > 0x0)
                {
                    HIST_ENTRY* last_entry;
                    last_entry = history_get(history_length);
                    d_print(1, "Command is empty, repeating last: %s\n", last_entry->line);
                    strcpy(command, last_entry->line);
                }
            }
        }

        if(this->taint_eng->prompt_file) fprintf(this->taint_eng->prompt_file, "%s\n", command);
        add_history(command);

        if(strcmp(command, "q") == 0x0)
        {
            d_print(1, "Quitting...\n");
            this->taint_eng->aborted = 0x1;
            break;
        }
        else if(strcmp(command, "c") == 0x0)
        {
            d_print(1, "Continuing...\n");
            break;
        }
        else if(strcmp(command, "s") == 0x0) 
        {
            d_print(1, "Stepping...\n");
            this->taint_eng->step_mode = 0x1;
            break;
        }

        this->parse_cmd(command);
        free(command);
    }
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

    if(this->query_tid == 0x0)
    {
        this->query_tid = this->taint_eng->cur_tid;
    }

    cur_str = strtok(string, " \n\r"); if(cur_str == 0x0) return 0x0;
    if(!strncasecmp(cur_str, "stat", 4))
    {
        d_print_prompt(0, "Current instruction: %d\nCurrent eip: 0x%08x\nCurrent instr_byte: 0x%02x\n", this->taint_eng->current_instr_count, this->taint_eng->reg_restore_32(EIP).get_DWORD(), this->taint_eng->current_instr_byte->get_BYTE() & 0xff);
        d_print_prompt(0, "Query thread: 0x%08x\n", this->query_tid);

    }
    else if(!strncasecmp(cur_str, "reg", 3))
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
            if(!strncasecmp(cur_str, "ta", 2))
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
    else if(!strncasecmp(cur_str, "mem", 3))
    {
        cur_str = strtok(0x0, " \n\r"); if(cur_str == 0x0) return 0x0;
        if(!strncasecmp(cur_str, "ta", 2))
        {
            cur_str = strtok(0x0, " \n\r"); if(cur_str == 0x0) return 0x0;
            addr = strtol(cur_str, 0x0, 0x10); 

            cur_str = strtok(0x0, " \n\r"); 

            if(cur_str == 0x0)
            {
                size = 0x40;
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
                size = 0x40;
            }
            else
            {
                size = strtol(cur_str, 0x0, 0x10);
            }
        
            this->taint_eng->print_err_mem(addr, size);
            return 0x0;
        }
    }
    else if(!strncasecmp(cur_str, "sta", 3))
    {
        cur_str = strtok(0x0, " \n\r"); 
        if(cur_str != 0x0 && !strncasecmp(cur_str, "ta", 2))
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
    else if(!strncasecmp(cur_str, "exc", 3))
    {
        d_print_prompt(0, "There have been %d exceptions: \n", this->taint_eng->exceptions_count);

        for(i = 0x0; i< this->taint_eng->exceptions_count; i++)
        {
            info = this->taint_eng->exceptions[i];
            d_print_prompt(0x0, "Exception %08x in TID %08x, instr. no: %d, eip: 0x%08x\n", info.er.ExceptionCode, info.tid, this->taint_eng->current_instr_count, info.er.ExceptionAddress);
        }

        return 0x0;
    }
    else if(!strncasecmp(cur_str, "thr", 3) || !strncasecmp(cur_str, "tid", 3))
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
    else if(!strncasecmp(cur_str, "bre", 3))
    {
        cur_str = strtok(0x0, " \n\r"); if(cur_str == 0x0) return 0x0;
        d_print_prompt(0, "Setting breakpoint\n");

        register_memory_breakpoints_2(cur_str, this->taint_eng);

    }
    else if(!strncasecmp(cur_str, "tai", 3))
    {
        cur_str = strtok(0x0, " \n\r"); 

        if(cur_str == 0x0)
        {
            d_print_prompt(0, "There have been %d taints:\n", this->taint_count);
            for(i = 0x0; i< this->taint_count; i++)
            {
                unsigned j;

                d_err_print("Taint no %d: \t0x%08x, 0x%08x", i, this->taints[i].off, this->taints[i].size);
                for(j = 0x0; j<this->taint_contexts[i].current_retaint; j++)
                {
                    d_err_print(", @%d", this->taint_contexts[i].instruction_count[j]);
                }
                d_err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "set", 3))
        {
            d_print_prompt(0, "Setting taint\n");
            register_taint2(cur_str);
        }

    }
    else if(!strncasecmp(cur_str, "ins", 3))
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
    else if(!strncasecmp(cur_str, "his", 3))
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
    else if(!strncasecmp(cur_str, "tra", 3))
    {
        unsigned i;

        cur_str = strtok(0x0, " \n\r"); if(cur_str == 0x0) return 0x0;

        if(!strncasecmp(cur_str, "eax", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[EAX+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[EAX+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[EAX+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[EAX+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking eax[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "ebx", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[EBX+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[EBX+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[EBX+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[EBX+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking ebx[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "ecx", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[ECX+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[ECX+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[ECX+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[ECX+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking ecx[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "edx", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[EDX+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[EDX+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[EDX+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[EDX+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking edx[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "esi", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[ESI+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[ESI+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[ESI+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[ESI+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking esi[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "edi", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[EDI+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[EDI+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[EDI+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[EDI+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking edi[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "ebp", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[EBP+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[EBP+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[EBP+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[EBP+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking ebp[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "esp", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[ESP+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[ESP+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[ESP+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[ESP+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking esp[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "eflags", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[EFLAGS+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[EFLAGS+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[EFLAGS+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[EFLAGS+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking eflags[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "eip", 3))
        {
            BYTE_t* reg[0x4];

            reg[0] = &this->taint_eng->cur_info->registers[EIP+0x0];
            reg[1] = &this->taint_eng->cur_info->registers[EIP+0x1];
            reg[2] = &this->taint_eng->cur_info->registers[EIP+0x2];
            reg[3] = &this->taint_eng->cur_info->registers[EIP+0x3];

            for(i = 0x0; i < 0x4; i++)
            {
                err_print("Tracking eip[%d]:\n", i);

                print_taint_history(reg[i], 0x0);
                err_print("\n");
            }
        }
        else if(!strncasecmp(cur_str, "ax", 2)) /* AX, AH, AL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[EAX+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[EAX+0x2];

            err_print("Tracking eax[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking eax[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "al", 2))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EAX+0x3];

            err_print("Tracking eax[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "ah", 2))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EAX+0x2];

            err_print("Tracking eax[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "bx", 2)) /* BX, BH, BL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[EBX+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[EBX+0x2];

            err_print("Tracking ebx[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking ebx[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "bl", 2))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EBX+0x3];

            err_print("Tracking eax[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "bh", 2))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EBX+0x2];

            err_print("Tracking ebx[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "cx", 2)) /* CX, CH, CL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[ECX+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[ECX+0x2];

            err_print("Tracking ecx[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking ecx[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "cl", 2))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[ECX+0x3];

            err_print("Tracking ecx[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "ch", 2))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[ECX+0x2];

            err_print("Tracking ecx[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "dx", 2)) /* DX, DH, DL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[EDX+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[EDX+0x2];

            err_print("Tracking edx[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking edx[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "dl", 2))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EDX+0x3];

            err_print("Tracking edx[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "dh", 2))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EDX+0x2];

            err_print("Tracking edx[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "sil", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[ESI+0x3];

            err_print("Tracking esi[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "sih", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[ESI+0x2];

            err_print("Tracking esi[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "si", 2)) /* SI, SIH, SIL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[ESI+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[ESI+0x2];

            err_print("Tracking esi[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking esi[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "dil", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EDI+0x3];

            err_print("Tracking edi[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "dih", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EDI+0x2];

            err_print("Tracking edi[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "di", 2)) /* DI, DIH, DIL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[EDI+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[EDI+0x2];

            err_print("Tracking edi[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking edi[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "bpl", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EBP+0x3];

            err_print("Tracking ebp[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "bph", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EBP+0x2];

            err_print("Tracking ebp[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "bp", 2)) /* BP, BPH, BPL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[EBP+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[EBP+0x2];

            err_print("Tracking ebp[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking ebp[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "spl", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[ESP+0x3];

            err_print("Tracking esp[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "sph", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[ESP+0x2];

            err_print("Tracking esp[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "sp", 2)) /* SP, SPH, SPL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[ESP+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[ESP+0x2];

            err_print("Tracking esp[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking esp[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "flagsl", 6))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EFLAGS+0x3];

            err_print("Tracking eflags[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "flagsh", 6))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EFLAGS+0x2];

            err_print("Tracking eflags[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "flags", 5)) /* FLAGS, FLAGSH, FLAGSL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[EFLAGS+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[EFLAGS+0x2];

            err_print("Tracking eflags[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking eflags[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "ipl", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EIP+0x3];

            err_print("Tracking eip[0x3]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "iph", 3))
        {
            BYTE_t* reg;

            reg = &this->taint_eng->cur_info->registers[EIP+0x2];

            err_print("Tracking eip[0x2]\n---------------------------------------------\n");

            print_taint_history(reg, 0x0);
            err_print("\n");
        }
        else if(!strncasecmp(cur_str, "ip", 2)) /* IP, IPH, IPL */
        {
            BYTE_t* reg[0x2];

            reg[0x0] = &this->taint_eng->cur_info->registers[EIP+0x3];
            reg[0x1] = &this->taint_eng->cur_info->registers[EIP+0x2];

            err_print("Tracking eip[0x3]\n---------------------------------------------\n");
            print_taint_history(reg[0x0], 0x0);
            err_print("\n");
            err_print("Tracking eip[0x2]\n---------------------------------------------\n");
            print_taint_history(reg[0x1], 0x0);
            err_print("\n");
        }
        else
        {
            OFFSET offset;
            BYTE_t* byte;
    
            if((cur_str[0x0] == '0') && (cur_str[0x1] == 'x'))
            {
                offset = strtol(cur_str, 0x0, 0x10);
                err_print("Tracking mem: 0x%08x\n", offset);
                byte = &this->taint_eng->memory[offset];
                print_taint_history(byte, 0x0);
                err_print("\n");
            }
            else
            {
                offset = strtol(cur_str, 0x0, 10);
                err_print("Tracking propagation: %d\n", offset);
                print_taint_history(offset, 0x0);
                err_print("\n");

            }
        }
    
    }
    else if(!strncasecmp(cur_str, "pro", 3))
    {
        unsigned offset, count;

        cur_str = strtok(0x0, " \n\r"); 
        if((cur_str) && !strncasecmp(cur_str, "res", 3))
        {
            clear_propagations();
        }
        else
        {
            if(cur_str == 0x0) 
            {
                offset = this->taint_eng->current_propagation_count-100;
                count = 0;
            }
            else 
            {
                offset = strtol(cur_str, 0x0, 10);

                cur_str = strtok(0x0, " \n\r"); 
                if(cur_str == 0x0) count = 0;
                else count = strtol(cur_str, 0x0, 10);
            }

            print_propagations(offset, count);
        }
    }
    else if(!strncasecmp(cur_str, "dis", 3))
    {
        OFFSET addr;
        const char* loc_eip = "eip";
        char* location;

        cur_str = strtok(0x0, " \n\r"); 
        if(cur_str == 0x0) 
            location = loc_eip;
        else
            location = cur_str;

        addr = resolve_location(location);
        this->disas_instructions(addr, 0x10);

    }
    else
    {
        /* we assume this is locator */
        unsigned add, size;
        size = 0x60;

        addr = resolve_location(cur_str);
        this->taint_eng->print_err_mem(addr, size);
        
    }
    return 0x0;
}

int taint_plugin::print_propagations(unsigned offset, unsigned count)
{
    PROPAGATION* cur_propagation;
    unsigned current_propagation_count;
    unsigned taint_id;
    unsigned i;

    current_propagation_count = this->taint_eng->current_propagation_count;

    d_print_prompt(1, "Current propagation count is: %d\n", current_propagation_count);
    if(offset > current_propagation_count)
    {
        d_print_prompt(1, "Propagations exceeded\n");
        return 0x0;
    }

    if(count == 0x0)
    {
        d_print_prompt(1, "Printing all propagations from: %d\n", offset);
        for(i = offset; i<current_propagation_count; i++)
        {
            print_propagation(i, 0x0);
        }
    }
    else
    {
        d_print_prompt(1, "Printing %d propagations from: %d\n", count, offset);
        for(i = offset; i<offset+count; i++)
        {
            print_propagation(i, 0x0);
            if(i >= current_propagation_count) break;
        }
    }

    return 0x0;
}

int taint_plugin::add_taint(OFFSET start, UDWORD length)
{
    UDWORD i;

    /* try to retaint existing taint */
    for(i = 0x0; i< this->taint_count; i++)
    {
        if((this->taints[i].off == start) && (this->taints[i].size == length))
        {
            d_print(1, "Taint request matched existing taint no %d, retainting\n", i);
            d_print(1, "Tainting: 0x%08x - 0x%08x\n", start, start+length);
            for(i = 0x0; i< length; i++)
            {
                this->taint_eng->memory[start+i].set_BYTE_t(0xff);
            }

            unsigned current_retaint = this->taint_contexts[this->taint_count].current_retaint;
            this->taint_contexts[this->taint_count].instruction_count[current_retaint] = this->taint_eng->current_instr_count;
            this->taint_contexts[this->taint_count].current_retaint = current_retaint+1;
            return 0x0;
        }
    }

    if(this->taint_count == MAX_TAINTS_OBSERVED)
    {
        d_print(1, "MAX_TAINTS_OBSERVED reached, unable to register taint\n");
        return 0x0;
    }

    d_print(1, "Tainting: 0x%08x - 0x%08x\n", start, start+length);
    for(i = 0x0; i< length; i++)
    {
        this->taint_eng->memory[start+i].set_BYTE_t(0xff);
    }

    this->taints[this->taint_count].off = start;
    this->taints[this->taint_count].size = length;

    unsigned current_retaint = this->taint_contexts[this->taint_count].current_retaint;
    this->taint_contexts[this->taint_count].instruction_count[current_retaint] = this->taint_eng->current_instr_count;
    this->taint_contexts[this->taint_count].current_retaint = current_retaint+1;

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

    d_print(1, "Registering taint @ 0x%08x, with length: 0x%08x\n", off, len);
    this->add_taint(off, len);

    return 0x0;
}

int taint_plugin::register_taint2(char* line)
{
    char* cmd;
    DWORD len;
    OFFSET off;

    off = (OFFSET)strtol(strtok(0x0, ","), 0x0, 0x10);
    len = (DWORD)strtol(strtok(0x0, ","), 0x0, 0x10);

    d_print(1, "Registering taint @ 0x%08x, with length: 0x%08x\n", off, len);
    this->add_taint(off, len);

    return 0x0;
}

int taint_plugin::add_lib(OFFSET off, char* name)
{
    LIBRARY new_lib;
    FILE* f;

    unsigned i, libs_count;
    libs_count = this->libs_count;

    for(i=0x0; i<strlen(name); i++)
    {
        if(name[i] == '\r') name[i]='\x00';
        if(name[i] == '\\') name[i]='/';
    }

    //strcpy(new_lib.path, this->lib_dir_path);
    strcpy(new_lib.path, basename(name));

    d_print(1, "Loading lib: %s\n", new_lib.path);

    strcpy(new_lib.name, basename(name));

    d_print(1, "Loading symbols for %s @ 0x%08x\n", new_lib.name, off);
    new_lib.offset = off;

    new_lib.loaded = 1;

    this->libs[libs_count] = new_lib;
    this->libs_count++;

    d_print(2, "Loaded lib: %s at 0x%08x to 0x%08x\n", new_lib.path, new_lib.offset, new_lib.offset+new_lib.length);
    return 0x0;
}

LIBRARY* taint_plugin::get_lib(OFFSET offset)
{
    unsigned i=0x0;
    unsigned highest = -1;

    for(i = 0x0; i < this->libs_count; i++)
    {
        if(this->libs[i].offset < offset)
        {
            if(highest != -1)
            {
                if(this->libs[i].offset > this->libs[highest].offset) 
                {
                    highest = i;
                }
            }
            else 
            {
                highest = i;
            }
        }
    }
    if(highest != -1) 
    {
        return &this->libs[highest];
    }
    else return 0x0;
}

OFFSET taint_plugin::resolve_loc_desc(LOCATION_DESCRIPTOR* d)
{
    d_print(1, "[resolve_loc_desc]\n");
    OFFSET a1_r, a2_r;
    OFFSET ret;
    CONTEXT_INFO* ctx;

    ctx = this->taint_eng->get_context_info(this->query_tid);

    if(d == 0x0)
        return -1;

    d_print(1, "Processing: %s\n", d->op);

    if(findany(d->op, "[+-"))
    {
       // not leaf, this is operation, we do recurrence 

        if(!strcmp(d->op, "["))
        {
            DWORD_t read;
            a1_r = resolve_loc_desc(d->a1);
            if(a1_r == -1) return -1;
            // read_memory and calculate ret 
            
            this->taint_eng->restore_32(a1_r, read);
            return read.get_DWORD();
        }
        else if(!strcmp(d->op, "-"))
        {
            a1_r = resolve_loc_desc(d->a1);
            a2_r = resolve_loc_desc(d->a2);
            if(a1_r == -1) return -1;
            if(a2_r == -1) return -1;
            ret = a1_r - a2_r;
        }
        else if(!strcmp(d->op, "+"))
        {
            a1_r = resolve_loc_desc(d->a1);
            a2_r = resolve_loc_desc(d->a2);
            if(a1_r == -1) return -1;
            if(a2_r == -1) return -1;
            ret = a1_r + a2_r;
        }
    }
    else
    {
        // this is leaf 

        if((d->op[0] == '0') && (d->op[1] == 'x'))
        {
            // return immediate
            ret = strtoul(d->op, 0x0, 0x10);
        }
        else
        {
            // registers
            if(!strncasecmp(d->op, "ESP", 3))
            {
                ret = this->taint_eng->reg_restore_32(ESP, this->query_tid).get_DWORD();
                d_print(1, "Reading register ESP: 0x%08x\n", ret);
            }
            else if(!strncasecmp(d->op, "EAX", 3))
            {
                ret = this->taint_eng->reg_restore_32(EAX, this->query_tid).get_DWORD();
                d_print(1, "Reading register EAX: 0x%08x\n", ret);
            }
            else if(!strncasecmp(d->op, "EBX", 3))
            {
                ret = this->taint_eng->reg_restore_32(EBX, this->query_tid).get_DWORD();
                d_print(1, "Reading register EBX: 0x%08x\n", ret);
            }
            else if(!strncasecmp(d->op, "ECX", 3))
            {
                ret = this->taint_eng->reg_restore_32(ECX, this->query_tid).get_DWORD();
                d_print(1, "Reading register ECX: 0x%08x\n", ret);
            }
            else if(!strncasecmp(d->op, "EDX", 3))
            {
                ret = this->taint_eng->reg_restore_32(EDX, this->query_tid).get_DWORD();
                d_print(1, "Reading register EDX: 0x%08x\n", ret);
            }
            else if(!strncasecmp(d->op, "ESI", 3))
            {
                ret = this->taint_eng->reg_restore_32(ESI, this->query_tid).get_DWORD();
                d_print(1, "Reading register ESI: 0x%08x\n", ret);
            }
            else if(!strncasecmp(d->op, "EDI", 3))
            {
                ret = this->taint_eng->reg_restore_32(EDI, this->query_tid).get_DWORD();
                d_print(1, "Reading register EDI: 0x%08x\n", ret);
            }
            else if(!strncasecmp(d->op, "EBP", 3))
            {
                ret = this->taint_eng->reg_restore_32(EBP, this->query_tid).get_DWORD();
                d_print(1, "Reading register EBP: 0x%08x\n", ret);
            }
            else if(!strncasecmp(d->op, "EIP", 3))
            {
                ret = this->taint_eng->reg_restore_32(EIP, this->query_tid).get_DWORD();
                d_print(1, "Reading register EIP: 0x%08x\n", ret);
            }
            else
            {
                // we assume it's library 
                d_print(1, "Looking for lib: %s\n", d->op);
                ret = find_lib(d->op);
                if(ret != 0x0)
                {
                    d_print(1, "Found at: 0x%08x\n", ret);
                }
                else if(ret == 0x0) 
                {
                    d_print(1, "Not found\n");
                    ret = -1;
                }
            }
        }
    }
    d_print(1, "[resolve_loc_desc end]\n");
    return ret;
}

LOCATION_DESCRIPTOR* taint_plugin::parse_location_desc(char* str)
{
//    d_print("[parse_location_desc]\n");
    char* op;
    LOCATION_DESCRIPTOR* neww;

    neww = (LOCATION_DESCRIPTOR*)malloc(sizeof(LOCATION_DESCRIPTOR));
    memset(neww, 0x0, sizeof(LOCATION_DESCRIPTOR));
    neww->a1 = 0x0;
    neww->a2 = 0x0;

    if(!neww)
    {
        d_print(1, "Out of memory\n");
    }

//    d_print("Creating new descriptor node\n");

    paint(str, strlen(str));
    op = findany(str, "+-");

    if(op == 0x0) 
    {
        unpaint(str, strlen(str));
        if((op = findany(str, "[")) != 0x0)
        {
            strcpy(neww->op, "[");
            op[0] = 0x0;
            neww->a1 = parse_location_desc(op+1);
            neww->a2 = 0x0;
        }
        else
        {
//            d_print("No operators found, assuming leaf: %s\n", str);
            strcpy(neww->op, str);
        }
    }
    else
    {
        unpaint(str, strlen(str));
//        d_print("Found operator: %s\n", op);
        
        if(op[0] == '+')
        {
            strcpy(neww->op, "+");
            op[0] = 0x0;
            neww->a1 = parse_location_desc(str);
            neww->a2 = parse_location_desc(op+1);
        }
        else if(op[0] == '-')
        {
            strcpy(neww->op, "-");
            op[0] = 0x0;
            neww->a1 = parse_location_desc(str);
            neww->a2 = parse_location_desc(op+1);
        }
    }

//    d_print("[parse_location_desc ends]\n");
    return neww;
}

OFFSET taint_plugin::resolve_location(char* location)
{
    OFFSET addr;
    char line[MAX_NAME];
    LOCATION_DESCRIPTOR desc;
    LOCATION_DESCRIPTOR* desc_;

    desc_ = parse_location_desc(location);
    addr = resolve_loc_desc(desc_);

    return addr;
}

OFFSET taint_plugin::find_lib(char* name)
{
    unsigned i=0x0;
    unsigned highest = -1;

    for(i = 0x0; i < this->libs_count; i++)
    {
        if(!strncasecmp(this->libs[i].name, name, strlen(name)))
        {
            return this->libs[i].offset;
        }
    }
    return 0x0;
}

int taint_plugin::del_lib(OFFSET off)
{
    int i;

    for(i=0x0; i<libs_count; i++)
    {
        if(this->libs[i].offset == off)
            this->libs[i].loaded = 0x0;
    }
    return 0x0;
}


/* plugin czy nie? */
int taint_plugin::register_lib(char* line)
{
    char* cmd;
    OFFSET off, size;
    char* name;

    cmd = strtok(line, ",");
    off = strtoul(strtok(0x0, ","), 0x0, 0x10);
    name = strtok(0x0, ",");
    //name[strlen(name)] = 0x0;
//    size = strtoul(strtok(0x0, ","), 0x0, 0x10);

    //printf("Registering lib: %s at 0x%08x\n", name, off);
    this->add_lib(off, name);

    return 0x0;
}

/* plugin czy nie? */
int taint_plugin::deregister_lib(char* line)
{
    char* cmd;
    OFFSET off;

    cmd = strtok(line, ",");
    off = strtoul(strtok(0x0, ","), 0x0, 0x10);

    //printf("Deregistering lib at 0x%08x\n", off);
    this->del_lib(off);

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
        this->register_lib(line);
    
    if(line[0] == 'D' && line[1] == 'L')
        this->deregister_lib(line);
            
    if(line[0] == 'S' && line[1] == 'Y')
        return 0x0;
//        this->register_symbol(line);

    return 0x0;
}

