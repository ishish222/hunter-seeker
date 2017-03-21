#include <iostream>
#include <execinfo.h>
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <caller_taint_x86.h>
#include "debug.h"
#include <iniparser.h>
#include "inc/pe_bliss.h"
#include <fstream>

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

char colors[0x10][0x10] = {"#000000", "#0000FF", "#00FF00", "#FF0000", "#0055AA"};

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

int taint_x86::handle_sigsegv()
{
    
    d_print(1, "\n********* SEGMENTATION FAULT *********\n\n");
    d_print(1, "Instruction no: %d\n", this->current_instr_count);
    d_print(1, "EIP: 0x%08x\n", this->current_eip);
    d_print(1, "Opcode: 0x%02x\n", this->current_instr_byte->get_BYTE());

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
    return 0x0;
}

int taint_x86::handle_sigint()
{
    this->finished = 0x1;
    this->aborted = 0x1;
    d_print(1, "Eip: 0x%08x, this->end_addr: 0x%08x, limit: %d, count: %d, finishing\n", this->current_eip, this->end_addr, this->instr_limit, this->current_instr_count);
    
    return 0x0;
}

int strcmpi(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}

/*
*  symbols 
*/

int taint_x86::copy_symbol(SYMBOL** dst, SYMBOL* src)
{
    SYMBOL* sp;
    SYMBOL* old;

    old = *dst;

    sp = (SYMBOL*)malloc(sizeof(SYMBOL));
    sp->addr = src->addr;
    sp->lib_name = (char*)malloc(strlen(src->lib_name)+1);
    sp->func_name = (char*)malloc(strlen(src->func_name)+1);
    strcat(sp->lib_name, src->lib_name);
    strcat(sp->func_name, src->func_name);
    sp->next = old;

    *dst = sp;

    return 0x0;
}

int taint_x86::add_symbol(SYMBOL** s, OFFSET addr, char* lib_name, char* func_name)
{
    SYMBOL* sp;
    SYMBOL* old;

    old = *s;

    if(this->symbols_count >= MAX_SYMBOL_COUNT)
    {
        d_print(1, "Error, maximum of symbols reached\n");
        exit(-1);
    }

    sp = (SYMBOL*)malloc(sizeof(SYMBOL));
    sp->addr = addr;
    sp->lib_name = (char*)malloc(strlen(lib_name)+1);
    sp->func_name = (char*)malloc(strlen(func_name)+1);
    sp->resolved = 0x0;
    sp->wanted = this->check_func_wanted(func_name);
    sp->included = this->check_func_included(func_name);
    strcat(sp->lib_name, lib_name);
    strcat(sp->func_name, func_name);
    sp->next = old;
    this->symbols_count++;

    *s = sp;

    return 0x0;
}

int taint_x86::remove_symbol(SYMBOL* sp)
{
    if(sp->lib_name) free(sp->lib_name);
    if(sp->func_name) free(sp->func_name);
    free(sp);
    return 0x0;
}

/* [TODO] need to optimize. Somehow. */
SYMBOL* taint_x86::get_symbol(OFFSET addr)
{
    SYMBOL* s;
    LIB_INFO* lib;

    lib = this->get_lib(addr);
    if(lib == 0x0) return 0x0;

    s = lib->symbols;

    if(s == 0x0) return 0x0;
    do
    {
        if((s->addr == addr) && (s->resolved)) 
        {
            return s; 
        }
        else s = s->next;
    } while(s);

    return 0x0;
}

/*
* adding to lists
*/

int taint_x86::add_blacklist(char* str)
{
    if(this->blacklist_count >= MAX_BLACKLIST)
    {
        d_print(1, "Error, maximum of blacklisted functions reached\n");
        exit(-1);
    }

    strcpy(this->lib_blacklist[this->blacklist_count], str);
    this->blacklist_count++;
    return 0x0;
}

int taint_x86::add_blacklist_addr(DWORD addr)
{
    if(this->addr_blacklist_count >= MAX_BLACKLIST)
    {
        d_print(1, "Error, maximum of blacklisted functions reached\n");
        exit(-1);
    }

    this->addr_blacklist[this->addr_blacklist_count] = addr;
    this->addr_blacklist_count++;
    return 0x0;
}

int taint_x86::add_wanted_i(unsigned instr)
{
    if(this->wanted_count_i >= MAX_WANTED)
    {
        d_print(1, "Error, maximum of wanted functions reached\n");
        exit(-1);
    }
    this->instr_wanted[this->wanted_count_i] = instr;
    this->wanted_count_i++;
    return 0x0;
}

int taint_x86::add_wanted_e(DWORD addr)
{
    if(this->wanted_count_e >= MAX_WANTED)
    {
        d_print(1, "Error, maximum of wanted functions reached\n");
        exit(-1);
    }
    this->addr_wanted[this->wanted_count_e] = addr;
    this->wanted_count_e++;
    return 0x0;
}

int taint_x86::add_wanted(char* str)
{
    if(this->wanted_count >= MAX_WANTED)
    {
        d_print(1, "Error, maximum of wanted functions reached\n");
        exit(-1);
    }
    strcpy(this->func_wanted[this->wanted_count], str);
    this->func_wanted[this->wanted_count][strlen(this->func_wanted[this->wanted_count])] = 0x0;
    this->func_wanted[this->wanted_count][strlen(this->func_wanted[this->wanted_count])-1] = 0x0;
    d_print(1, "%s\n", this->func_wanted[this->wanted_count]);
    this->wanted_count++;
    return 0x0;
}

/*
* looking up lists
*/

int taint_x86::check_func_wanted(char* name)
{
    unsigned i = 0x0;

    //replace MAX_WANTED with wanted_count
    for(i = 0x0; i < MAX_WANTED; i++)
    {
        if(strlen(this->func_wanted[i]) == 0x0) 
            continue;

        if(strcmp(name, this->func_wanted[i]) == 0x0)
        {
            return 0x1;
        }
    }

    return 0x0;
}

int taint_x86::check_func_included(char* name)
{
    unsigned i = 0x0;

    //replace MAX_WANTED with included_count
    for(i = 0x0; i < MAX_WANTED; i++)
    {
        if(strlen(this->func_included[i]) == 0x0) 
            continue;

        if(strcmp(name, this->func_included[i]) == 0x0)
        {
            return 0x1;
        }
    }

    return 0x0;
}

int taint_x86::start()
{
    char out_line[MAX_NAME];

    sprintf(out_line, "[ST]");
    print_empty_call(&this->ctx_info[this->tids[this->cur_tid]], out_line, colors[CODE_RED]);

    this->started = 0x1;
    return 0x0;
}

int taint_x86::add_breakpoint(BREAKPOINT bp)
{
    if(this->new_bpt_count >= MAX_BREAKPOINTS) return -1;

    this->new_bps[this->new_bpt_count].offset = bp.offset;
    this->new_bps[this->new_bpt_count].mem_offset = bp.mem_offset;
    this->new_bps[this->new_bpt_count].mode = bp.mode;

    this->new_bpt_count++;

    return 0x0;
}

int taint_x86::add_taint_breakpoint(BREAKPOINT bp)
{
    if(this->new_bpt_t_count >= MAX_BREAKPOINTS) return -1;

    this->new_bps_t[this->new_bpt_t_count].offset = bp.offset;
    this->new_bps_t[this->new_bpt_t_count].mem_offset = bp.mem_offset;
    this->new_bps_t[this->new_bpt_t_count].mode = bp.mode;

    this->new_bpt_t_count++;

    return 0x0;
}

int taint_x86::update_watchpoints(DWORD tid)
{
    unsigned i;
    for(i=0x0; i< this->new_wpt_count; i++)
        if(this->new_wps[i].tid == tid)
        {
            if(strstr(this->new_wps[i].name, "EAX") != 0x0)
            {
                this->new_wps[i].watched = &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[EAX];
            }
            else if(strstr(this->new_wps[i].name, "EBX") != 0x0)
            {
                this->new_wps[i].watched = &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[EBX];
            }
            else if(strstr(this->new_wps[i].name, "ECX") != 0x0)
            {
                this->new_wps[i].watched = &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[ECX];
            }
            else if(strstr(this->new_wps[i].name, "EDX") != 0x0)
            {
                this->new_wps[i].watched = &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[EDX];
            }
            else if(strstr(this->new_wps[i].name, "ESI") != 0x0)
            {
                this->new_wps[i].watched = &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[ESI];
            }
            else if(strstr(this->new_wps[i].name, "EDI") != 0x0)
            {
                this->new_wps[i].watched = &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[EDI];
            }
            else if(strstr(this->new_wps[i].name, "EBP") != 0x0)
            {
                this->new_wps[i].watched = &(this->ctx_info[this->tids[this->new_wps[i].tid]].registers[EBP]);
            }
            else if(strstr(this->new_wps[i].name, "ESP") != 0x0)
            {
                this->new_wps[i].watched = &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[ESP];
            }
            else if(strstr(this->new_wps[i].name, "EIP") != 0x0)
            {
                this->new_wps[i].watched = &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[EIP];
            }
            else if(strtol(this->new_wps[i].name, 0x0, 0x10) != 0x0)
            {
                this->new_wps[i].watched = &this->memory[strtol(this->new_wps[i].name, 0x0, 0x10)];
            }
        
        }

    return 0x0;
}

int taint_x86::add_trace_watchpoint(TRACE_WATCHPOINT wp)
{
    if(this->new_wpt_count >= MAX_BREAKPOINTS) return -1;

    this->new_wps[this->new_wpt_count].offset = wp.offset;
    this->new_wps[this->new_wpt_count].mem_offset = wp.mem_offset;
    this->new_wps[this->new_wpt_count].watched = wp.watched;
    this->new_wps[this->new_wpt_count].interactive = wp.interactive;
    this->new_wps[this->new_wpt_count].tid = wp.tid;
    strcpy(this->new_wps[this->new_wpt_count].name, wp.name);

    this->new_wpt_count++;

    return 0x0;
}

/* blacklist routines */

int taint_x86::add_fence(OFFSET entry, OFFSET start, OFFSET struct_size, OFFSET struct_count)
{
    if(this->loop_fences_count >= MAX_LOOP_FENCES)
    {
        d_print(1, "Error, maximum of loop fences reached\n");
        exit(-1);
    }

    this->loop_fences[this->loop_fences_count].entry = entry;
    this->loop_fences[this->loop_fences_count].start = start;
    this->loop_fences[this->loop_fences_count].struct_size = struct_size;
    this->loop_fences[this->loop_fences_count].struct_count = struct_count;
    this->loop_fences[this->loop_fences_count].limit = struct_size * struct_count;
    this->loop_fences_count++;
    d_print(1, "Fence: Entry: 0x%08x Start: 0x%08x Size: 0x%08x Count: 0x%08x\n", entry, start, struct_size, struct_count);

    return 0x0;
}

int taint_x86::add_included(char* str)
{
    if(this->included_count >= MAX_WANTED)
    {
        d_print(1, "Error, maximum of wanted functions reached\n");
        exit(-1);
    }
    strcpy(this->func_included[this->included_count], str);
    this->func_included[this->included_count][strlen(this->func_included[this->included_count])] = 0x0;
    this->func_included[this->included_count][strlen(this->func_included[this->included_count])-1] = 0x0;
    d_print(1, "%s\n", this->func_included[this->included_count]);
    this->included_count++;
    return 0x0;
}

int taint_x86::check_lib_blacklist(LIB_INFO* lib)
{
    unsigned i;

    d_print(2, "Checking lib\n");

    for(i=0x0; i<MAX_BLACKLIST; i++)
    {
        d_print(2, "%s - %s\n", this->lib_blacklist[i], lib->name);
            
        if(strlen(lib->name) == 0x0) break;
        if(strlen(this->lib_blacklist[i]) == 0x0) break;
        if(strstr(this->lib_blacklist[i], lib->name))
        {
            d_print(1, "Module blacklisted\n");
            lib->blacklisted = 0x1;
            return 0x1;
        }
    }
    return 0x0;
}

int taint_x86::check_addr_blacklist(OFFSET offset)
{
    LIB_INFO* lib;

    unsigned i;

    for(i = 0x0; i< this->addr_blacklist_count; i++)
    {
        if(this->addr_blacklist[i] == offset)
            return 0x1;
    }

    lib = this->get_lib(offset);

    if(lib!=0x0)
    {
        d_print(2, "Lib: %s, from 0x%08x to 0x%08x\n", lib->name, lib->offset, lib->offset + lib->length);
        if(lib->blacklisted)
        {
            d_print(2, "Lib blacklisted\n");
            return 0x1;
        }
    }

    return 0x0;
}

LIB_INFO* taint_x86::get_lib(OFFSET offset)
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


/* graph output routines */

void taint_x86::print_call_open(CONTEXT_INFO* cur_ctx, char* line, const char* color)
{
    char out_line[MAX_NAME];
    char working_line[MAX_NAME];
    FILE* f = cur_ctx->graph_file;   
    unsigned i;

//    if(check_collecting(cur_ctx)) return;

    d_print(1, "Printing call into: %s\n", cur_ctx->graph_filename);

    strcpy(out_line, "");

    for(i = this->call_level_start-this->call_level_offset; i< cur_ctx->call_level; i++)
        strcat(out_line, " ");

    sprintf(working_line, "<node COLOR=\"%s\" CREATED=\"6666666666666\" ID=\"ID_1208439975\" MODIFIED=\"6666666666666\" TEXT=\"%s\">\n", color, line);

    strcat(out_line, working_line);
    fwrite(out_line, strlen(out_line), 0x1, f);
}

void taint_x86::print_call(CONTEXT_INFO* cur_ctx, char* line, const char* color)
{
    char out_line[MAX_NAME];
    char working_line[MAX_NAME];
    FILE* f = cur_ctx->graph_file;   
    unsigned i;

//    if(check_collecting(cur_ctx)) return;

    d_print(1, "Printing call into: %s\n", cur_ctx->graph_filename);

    strcpy(out_line, "");

    for(i = this->call_level_start-this->call_level_offset; i< cur_ctx->call_level; i++)
        strcat(out_line, " ");

    sprintf(working_line, "<node COLOR=\"%s\" CREATED=\"6666666666666\" FOLDED=\"true\" ID=\"ID_1208439975\" MODIFIED=\"6666666666666\" TEXT=\"%s\">\n", color, line);

    strcat(out_line, working_line);
    fwrite(out_line, strlen(out_line), 0x1, f);
}

void taint_x86::print_empty_call(CONTEXT_INFO* cur_ctx, char* line, const char* color)
{
    unsigned i;
    FILE* f = cur_ctx->graph_file;   
    char out_line[MAX_NAME];
    char working_line[MAX_NAME];
    
    d_print(1, "Printing empty call into: %s\n", cur_ctx->graph_filename);

    strcpy(out_line, "");
    
    for(i = this->call_level_start-this->call_level_offset; i< cur_ctx->call_level; i++)
        strcat(out_line, " ");
    
    sprintf(working_line, "<node COLOR=\"%s\" CREATED=\"6666666666666\" FOLDED=\"true\" ID=\"ID_1208439975\" MODIFIED=\"6666666666666\" TEXT=\"%s\"></node>\n", color, line);

    strcat(out_line, working_line);
    fwrite(out_line, strlen(out_line), 0x1, f);
}

void taint_x86::print_ret(CONTEXT_INFO* cur_ctx)
{
    FILE* f = cur_ctx->graph_file;   
    char out_line[MAX_NAME];
    unsigned i;

    d_print(1, "Printing ret into: %s\n", cur_ctx->graph_filename);

    strcpy(out_line, "");

    for(i = this->call_level_start-this->call_level_offset; i< cur_ctx->call_level; i++)
        strcat(out_line, " ");


    d_print(1, "Printing ret\n");
    strcat(out_line, "</node>\n");
    fwrite(out_line, strlen(out_line), 0x1, f);
}

/* handling conditional jumps  */

int taint_x86::jxx_set(unsigned state)
{
    char out_line[MAX_NAME]; 

    this->cur_info->levels[this->cur_info->call_level].jxx_handling = state;

    sprintf(out_line, "[x] JXX_STATUS: 0x%02x", state);
    print_empty_call(&this->ctx_info[this->tids[this->cur_tid]], out_line, colors[CODE_RED]);

    if(state == 0x0)
    {
        memset(this->cur_info->list, 0x0, MAX_LIST_JXX);
        this->cur_info->list_len = 0x0;
    }

    return 0x0;
}

int is_on_list(CONTEXT_INFO* info, DWORD eip)
{
    unsigned i;

    for(i = 0x0; i< info->list_len; i++)
    {
        if(eip == info->list[i]) return 0x1;
    }

    return 0x0;
}

int add_to_list(CONTEXT_INFO* info, DWORD eip)
{
    if(info->list_len == MAX_LIST_JXX-1) return -0x1;
    info->list[info->list_len] = eip;
    info->list_len++;
    return 0x0;
}

int taint_x86::handle_jxx(CONTEXT_INFO* info, char* str)
{
    /*
    if((info->levels[info->call_level].jxx_handling != 0x1) || (info->waiting != 0x0))
    {
        return 0x0;
    }
    */

    if(info->waiting != 0x0)
    {
        return 0x0;
    }

    if(is_on_list(info, this->current_eip)) return 0x0;

    char out_line[MAX_NAME];

    if(this->enumerate) sprintf(out_line, "(%d)0x%08x %s", this->current_instr_count ,this->current_eip, str);
    else sprintf(out_line, "0x%08x %s", this->current_eip, str);
    print_empty_call(info, out_line, colors[CODE_GREEN]);

    add_to_list(info, this->current_eip);

    return 0x0;
}

int taint_x86::handle_ja(CONTEXT_INFO* info)
{
    return handle_jxx(info, "ja");
}

int taint_x86::handle_jae(CONTEXT_INFO* info)
{
    return handle_jxx(info, "jae");
}

/* precise jmp analysis */
int taint_x86::handle_jmp(CONTEXT_INFO* info)
{
    if(!this->options & OPTION_ANALYZE_JUMPS)
        return 0x0;

    SYMBOL* s;
    char out_line[MAX_NAME];
    char* func_name;
    DWORD_t current;
    DWORD_t waiting;
    OFFSET target = info->target;
    OFFSET next = info->next;

    unsigned color;
    s = this->get_symbol(target);

    if(info->waiting != 0x0)
    {
        /* wanted */
        if((s != 0x0) && (s->wanted) && (this->options & OPTION_ANALYZE_WANTED_IN_SYMBOLS))
        {
            d_print(1, "Got wanted!\n");
            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x jmp %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
            else sprintf(out_line, "[x] 0x%08x jmp %s!%s", this->current_eip, s->lib_name, s->func_name);
            print_call(info, out_line, colors[CODE_RED]);
            this->print_ret(info);
        }
        else
        {
            return 0x0;
        }
    }
    else
    {
        if(s != 0x0)
        {
            if(s->wanted)
            {
                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x jmp %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
                else sprintf(out_line, "[x] 0x%08x jmp %s!%s", this->current_eip, s->lib_name, s->func_name);
                print_call(info, out_line, colors[CODE_RED]);
                print_ret(info);
            }
            else
            {
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x jmp %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
                else sprintf(out_line, "0x%08x jmp %s!%s", this->current_eip, s->lib_name, s->func_name);
                print_call(info, out_line, colors[CODE_BLUE]);
                print_ret(info);
            }
        }
        else
        {
            return 0x0;
        }
    }

    return 0x0;
}

/* loop routines */ 
int taint_x86::enter_loop_demo(CONTEXT_INFO* info)
{
    if(!this->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    char out_line[MAX_NAME];
    FILE* f = info->graph_file;   

    unsigned cur_call_level;
    cur_call_level = this->cur_info->call_level;

    {
        d_print(1, "Entering loop demo\n");
        sprintf(out_line, "[loop]");
        print_call(info, out_line, colors[CODE_BLACK]);
//        sprintf(out_line, "<!-- started loop demo -->\n");
//        fwrite(out_line, strlen(out_line), 0x1, f);
    }

    //info->loop_pos[cur_call_level]++;

}

int taint_x86::exit_loop_demo(CONTEXT_INFO* info)
{
    if(!this->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    unsigned level;
    char out_line[MAX_NAME];
    FILE* f = info->graph_file;   
    CALL_LEVEL* cur_level;

    level = this->cur_info->call_level;

    d_print(1, "Exiting loop demo\n");
    cur_level->cur_fence = 0x0;
//    sprintf(out_line, "<!-- loop demo ended \n");
//    fwrite(out_line, strlen(out_line), 0x1, f);

    print_ret(info);
}

int taint_x86::enter_loop(CONTEXT_INFO* info)
{
    if(!this->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    char out_line[MAX_NAME];
    FILE* f = info->graph_file;   

    unsigned cur_call_level;
    cur_call_level = this->cur_info->call_level;

    {
        d_print(1, "Entering loop\n");
//        sprintf(out_line, "[loop]");
//        print_call(info, out_line, colors[CODE_BLACK]);
//        sprintf(out_line, "<!-- started loop -->\n");
//        fwrite(out_line, strlen(out_line), 0x1, f);
    }

    //info->loop_pos[cur_call_level]++;

}

int taint_x86::exit_loop(CONTEXT_INFO* info)
{
    if(!this->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    unsigned level;
    char out_line[MAX_NAME];
    FILE* f = info->graph_file;   
    CALL_LEVEL* cur_level;

    level = this->cur_info->call_level;

    d_print(1, "Exiting loop\n");
    cur_level->cur_fence = 0x0;
//    sprintf(out_line, "loop ended -->\n");
//    fwrite(out_line, strlen(out_line), 0x1, f);

//    print_ret(info);
}

int taint_x86::check_collecting(CONTEXT_INFO* info)
{
    unsigned i;

    d_print(1, "Entered collecting verification\n");

    for(i = info->call_level; i >= info->call_level_smallest; i--)
    {
        d_print(1, "Veryfing level: %d\n", i);
        if(info->levels[i].cur_fence)
            if(info->levels[i].cur_fence->collecting) return 0x1;
    }

    return 0x0;
}

int detox(char* s)
{
    unsigned size;
    unsigned i;

    size  = strlen(s);
    for(i = 0; i< size; i++)
    {
        if(s[i] == '\"') s[i] = ' ';
        if(s[i] == '\'') s[i] = ' ';
        if(s[i] == '<') s[i] = ' ';
        if(s[i] == '>') s[i] = ' ';
    }

    return 0x0;
}

int taint_x86::comment_out(char* comment, DWORD tid)
{
    if(!(this->started))
    {
        return 0x0;
    }

    /* we need to find proper ctx_info */
    CONTEXT_INFO* info;
    DWORD tid_no;

    tid_no = this->tids[tid];

    info = &this->ctx_info[tid_no];

    detox(comment);

    d_print(1, "Writing out comment @ %d: %s", this->last_instr_count, comment);
    print_empty_call(info, comment, colors[CODE_COMMENT]);
    return 0x0;
}

/* returns 1 if in loop, 0 otherwise */
int taint_x86::check_loop_2(CONTEXT_INFO* info)
{
    if(!this->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    OFFSET offset;
    unsigned cur_fence_idx;
    unsigned cur_loop_addr_idx;
    unsigned cur_loop_limit;
    unsigned cur_loop_struct_size;
    unsigned cur_loop_struct_count;
    unsigned level;
    LOOP_FENCE* cur_fence;
    CALL_LEVEL* cur_level;


    level = info->call_level;
    cur_level = &info->levels[level];

    offset = this->current_eip;
    cur_fence = info->levels[level].cur_fence;

    d_print(1, "Fence for graph pos: %d: 0x%08x\n", level, cur_fence);

    if(cur_fence != 0x0)
    {

        d_print(1, "---\nActive fence: \nEntry: \t0x%08x\nStart: \t0x%08x\nLimit: \t%d\n", cur_fence->entry, cur_fence->start, cur_fence->limit);

        /* on specific level, but not yet collecting */
        if(cur_level->loop_status == FENCE_ACTIVE)
        {
            d_print(1, "Check for starting: %p - %p\n", cur_fence->start, offset);
            if(cur_fence->start == offset)
            {
                d_print(1, "Starting collecting\n");
                cur_level->loop_status = FENCE_COLLECTING;
                enter_loop(info);
                enter_loop_demo(info);
            }
        }

        /* in loop */
        cur_loop_addr_idx   = cur_level->loop_addr_idx;
        cur_loop_limit      = cur_level->loop_limit;
        cur_loop_struct_size    = cur_level->loop_struct_size;
        cur_loop_struct_count   = cur_level->loop_struct_count;

        if(cur_level->loop_status == FENCE_NOT_COLLECTING)
        {
            cur_loop_addr_idx ++;
            cur_level->loop_addr_idx = cur_loop_addr_idx;

            d_print(1, "Not collected: %p\n", offset);

            if(cur_loop_limit != 0x0)
            if(cur_loop_addr_idx >= cur_loop_limit)
            {
                /* collecting finished */
                cur_level->loop_addr_idx = 0x0;

                cur_level->loop_status = FENCE_FINISHED;
                exit_loop(info);

                d_print(1, "Collected addrs:\n");

                unsigned i;
                for(i=0x0; i<cur_loop_struct_count; i++)
                {
                    d_print(1, "0x%08x\n", cur_level->loop_addr[i]);
                }

            }
            return 0;
        }

        if(cur_level->loop_status == FENCE_COLLECTING)
        {
            d_print(1, "1");
            cur_level->loop_addr[cur_loop_addr_idx] = offset;
            d_print(1, "2");
            cur_loop_addr_idx ++;
            cur_level->loop_addr_idx = cur_loop_addr_idx;

            d_print(1, "Collected: %p\n", offset);
            d_print(1, "Level %d collection: idx: %d, limit: %d\n", level, cur_level->loop_addr_idx, cur_level->loop_limit);

            if(cur_loop_addr_idx > cur_loop_struct_size)
            {
                /* we collected enough for demonstration */
                cur_level->loop_status = FENCE_NOT_COLLECTING;
                exit_loop_demo(info);
            }

            return 0;
        } 
    }
    return 0x0;
}

/* handling call, diving, surfacing, outputting graph content */

int taint_x86::handle_call(CONTEXT_INFO* info)
{
    d_print(1, "[handle call]\n");
    d_print(1, "LL: 0x%08x\n", info->lock_level);
    SYMBOL* s;
    char out_line[MAX_NAME];
    char* func_name;
    DWORD_t current;
    DWORD_t waiting;
    OFFSET source = info->source;
    OFFSET target = info->target;
    OFFSET next = info->next;
    CALL_LEVEL* cur_level;

    d_print(1, "source: 0x%08x, target: 0x%08x\n", source, target);
    #define DECISION_NO_EMIT        0x0
    #define DECISION_EMIT           0x1
    #define DECISION_EMIT_NESTED    0x2

    #define DECISION_NO_DIVE        0x0
    #define DECISION_DIVE           0x1

    #define DECISION_LAYOUT_REGULAR 0x0
    #define DECISION_LAYOUT_SYMBOL  0x1
    #define DECISION_LAYOUT_SYMBOL_WANTED  0x2
    #define DECISION_LAYOUT_4       0x3
    #define DECISION_LAYOUT_5       0x4

    char decision_emit;
    char decision_dive;
    char decision_template;
    
    unsigned color;
    s = this->get_symbol(target);
    unsigned i;

    if((!this->started) || (this->finished))
    {
        d_print(1, "Not yet started\n");
        return 0x0;
    }

    if(info->waiting != 0x0)
    {
        /* increase lock level */
        info->lock_level++;
    }

    d_print(1, "Call\n");
//    d_print(2, "Call: 0x%08x\n", this->reg_restore_32(EIP).get_DWORD());
    d_print(2, "Call: 0x%08x\n", info->source);

    /* decision about emission */

    d_print(2, "Decision about emission\n");
    if(info->waiting != 0x0)
    {
        /* we are waiting for return */ 
        d_print(2, "We are waiting for return \n");
        if((s != 0x0) && (s->wanted))
        {
            /* wanted */
            d_print(1, "Got wanted!\n");
            decision_emit = DECISION_EMIT_NESTED;
        }
        else
        {
            /* not wanted */
            d_print(2, "We are waiting and we do not want\n");
            decision_emit = DECISION_NO_EMIT;
        }
        if(!(this->options & OPTION_ANALYZE_WANTED_IN_SYMBOLS))
        {
            decision_emit = DECISION_NO_EMIT;
        }
    }
    else
    {
        d_print(2, "We are not waiting\n");

//        current = this->reg_restore_32(EIP);
        current = info->source;

        /* check for loop bypasses */
        this->check_loop_2(info);
        cur_level = &info->levels[info->call_level];

        if(cur_level->loop_status == FENCE_NOT_COLLECTING)
        {
            /* we are traversing known loop, do not want this, we wait for next */
            d_print(2, "We are not collecting these calls\n");
            decision_emit = DECISION_NO_EMIT;
        }
        else
        {
            /* we are not traversing a fully known loop, we want this */
            d_print(2, "We are not traversing known loop, we are in active fence before start or we are collecting . Either way we want this\n");
            decision_emit = DECISION_EMIT;
        }
    }
            
    /* decision about diving */
    d_print(2, "Decision about diving\n");
    if(info->waiting != 0x0)
    {
        d_print(2, "We are waiting, not diving\n");
        decision_dive = DECISION_NO_DIVE;
    }
    else
    {
        d_print(2, "We are not waiting, checking for symbol\n");
        if(s != 0x0)
        {
            d_print(2, "We have symbol\n");
            if(s->included)
            {
                d_print(2, "The symbol is included, we dive\n");
                decision_dive = DECISION_DIVE;
            }
            else
            {
                d_print(2, "The symbol is not included, we do not dive\n");
                decision_dive = DECISION_NO_DIVE;
            }
            
        }
        else
        {
            d_print(2, "We do not have symbol\n");
            if(this->check_addr_blacklist(target))
            {
                /* target is blacklisted, we do not dive*/
                d_print(2, "Target 0x%08x is blacklisted, we do not dive\n", target);
                decision_dive = DECISION_NO_DIVE;
                if(this->options & OPTION_NOT_EMITTING_BLACKLISTED)
                {
                    decision_emit = DECISION_NO_EMIT;
                }
            }
            else
            {
                /* target is not blacklisted, we dive */
                d_print(2, "Target is not blacklisted, we dive\n");
                decision_dive = DECISION_DIVE;
            }
        }
    }

    /* decision about layout */
    d_print(2, "Decision about layout\n");

    if(s != 0x0)
    {
        /* we have symbol */
        d_print(2, "We have symbol\n");
        if(s->wanted)
        {
            d_print(2, "We have wanted symbol\n");
            decision_template = DECISION_LAYOUT_SYMBOL_WANTED;
        }
        else
        {
            d_print(2, "Symbol is not wanted\n");
            decision_template = DECISION_LAYOUT_SYMBOL;
        }
    }
    else
    {
        d_print(2, "We do not have symbol\n");
        decision_template = DECISION_LAYOUT_REGULAR;
    }

    d_print(2, "Executing decisions\n");

    if(decision_emit == DECISION_EMIT)
    {
        d_print(2, "Emitting\n");

        if(decision_dive == DECISION_DIVE)
        {
            d_print(2, "Emitting diving\n");
        
            if(decision_template == DECISION_LAYOUT_SYMBOL_WANTED)
            {
                /* we assume we have symbol */
//                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
//                else sprintf(out_line, "[x] 0x%08x call %s!%s", this->current_eip, s->lib_name, s->func_name);
                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->current_instr_count-1 ,source, s->lib_name, s->func_name);
                else sprintf(out_line, "[x] 0x%08x call %s!%s", source, s->lib_name, s->func_name);
                print_call(info, out_line, colors[CODE_RED]);
            }
            else if(decision_template == DECISION_LAYOUT_SYMBOL)
            {
                /* we assume we have symbol */
//                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
//                else sprintf(out_line, "0x%08x call %s!%s", this->current_eip, s->lib_name, s->func_name);
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->current_instr_count-1 ,source, s->lib_name, s->func_name);
                else sprintf(out_line, "0x%08x call %s!%s", source, s->lib_name, s->func_name);
                print_call(info, out_line, colors[CODE_BLUE]);
            }
            else
            {
                /* regular emission with dive */
//                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call 0x%08x", this->current_instr_count ,this->current_eip, target);
//                else sprintf(out_line, "0x%08x call 0x%08x", this->current_eip, target);
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call 0x%08x", this->current_instr_count-1 ,source, target);
                else sprintf(out_line, "0x%08x call 0x%08x", source, target);
                print_call(info, out_line, colors[CODE_BLACK]);
            }
        }
        else /* DECISION_NO_DIVE */
        {
            d_print(2, "Emitting not diving\n");

            if(decision_template == DECISION_LAYOUT_SYMBOL_WANTED)
            {
                /* we assume we have symbol */
//                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
//                else sprintf(out_line, "[x] 0x%08x call %s!%s", this->current_eip, s->lib_name, s->func_name);
                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->current_instr_count-1 ,source, s->lib_name, s->func_name);
                else sprintf(out_line, "[x] 0x%08x call %s!%s", source, s->lib_name, s->func_name);
                print_call_open(info, out_line, colors[CODE_RED]);
            }
            else if(decision_template == DECISION_LAYOUT_SYMBOL)
            {
                /* we assume we have symbol */
//                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
//                else sprintf(out_line, "0x%08x call %s!%s", this->current_eip, s->lib_name, s->func_name);
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->current_instr_count-1 ,source, s->lib_name, s->func_name);
                else sprintf(out_line, "0x%08x call %s!%s", source, s->lib_name, s->func_name);
                print_call_open(info, out_line, colors[CODE_BLUE]);
            }
            else
            {
                /* regular emission with dive */
//                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call 0x%08x", this->current_instr_count ,this->current_eip, target);
//                else sprintf(out_line, "0x%08x call 0x%08x", this->current_eip, target);
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call 0x%08x", source, this->current_eip, target);
                else sprintf(out_line, "0x%08x call 0x%08x", source, target);
                print_call_open(info, out_line, colors[CODE_BLACK]);
            }
        
        }
        
        /* log emission */
        for(i=this->call_level_start; i< info->call_level; i++)
        {
            d_print(1, " ");
        }
    
/*        d_print(1, "[0x%08x] (%d)0x%08x call 0x%08x, pos: %d, small: %d, ignored: %d: \n", 
                this->cur_tid, this->current_instr_count, this->current_eip, target, 
                this->ctx_info[this->tids[this->cur_tid]].call_level, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_smallest, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_ignored, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_largest);*/
    
        d_print(1, "[0x%08x] (%d)0x%08x call 0x%08x, pos: %d, small: %d, ignored: %d: \n", 
                this->cur_tid, this->current_instr_count-1, source, target, 
                this->ctx_info[this->tids[this->cur_tid]].call_level, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_smallest, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_ignored, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_largest);

    }
    else if(decision_emit == DECISION_EMIT_NESTED)
    {
        d_print(2, "Emitting nested\n");

        if(decision_template == DECISION_LAYOUT_SYMBOL_WANTED)
        {
            /* we assume we have symbol */
//            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
//            else sprintf(out_line, "[x] 0x%08x call %s!%s", this->current_eip, s->lib_name, s->func_name);
            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->current_instr_count-1 ,source, s->lib_name, s->func_name);
            else sprintf(out_line, "[x] 0x%08x call %s!%s", source, s->lib_name, s->func_name);
            print_call(info, out_line, colors[CODE_RED]);
            print_ret(info);
        }
        else
        {
            /* we assume we have symbol */
//            if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->current_instr_count ,this->current_eip, s->lib_name, s->func_name);
//            else sprintf(out_line, "0x%08x call %s!%s", this->current_eip, s->lib_name, s->func_name);
            if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->current_instr_count-1 ,source, s->lib_name, s->func_name);
            else sprintf(out_line, "0x%08x call %s!%s", source, s->lib_name, s->func_name);
            print_call(info, out_line, colors[CODE_BLACK]);
            print_ret(info);
        
        }

        /* log emission */
        for(i=this->call_level_start; i< info->call_level; i++)
        {
            d_print(1, " ");
        }
/*    
        d_print(1, "[0x%08x] (%d)0x%08x call 0x%08x, pos: %d, small: %d, ignored: %d: \n", 
                this->cur_tid, this->current_instr_count, this->current_eip, target, 
                this->ctx_info[this->tids[this->cur_tid]].call_level, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_smallest, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_ignored, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_largest);*/
    
        d_print(1, "[0x%08x] (%d)0x%08x call 0x%08x, pos: %d, small: %d, ignored: %d: \n", 
                this->cur_tid, this->current_instr_count-1, source, target, 
                this->ctx_info[this->tids[this->cur_tid]].call_level, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_smallest, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_ignored, 
                this->ctx_info[this->tids[this->cur_tid]].call_level_largest);
    }
    else
    {
        d_print(2, "Not emitting\n");
    }

    if(decision_dive == DECISION_DIVE)
    {
        d_print(2, "Diving!\n");
        this->dive(info, target, next);
    }
    else
    {
        d_print(2, "Not diving!\n");
        if(info->waiting == 0x0) 
        {
            info->waiting = next;
            info->last_emit_decision = decision_emit;
        }

    }
        
    d_print(1, "[handle call finishes]\n");
    return 0x0;
}

int taint_x86::check_fence(CALL_LEVEL* cur_level)
{
    /* check fences for activation */
    unsigned i;
    LOOP_FENCE* cur_fence;

    for(i=0x0; i < this->loop_fences_count; i++)
    {
        cur_fence = &this->loop_fences[i];
        if(cur_fence->entry == cur_level->entry)
        {
            cur_level->cur_fence = cur_fence;
            cur_level->loop_status = FENCE_ACTIVE;
            cur_level->loop_limit = cur_fence->limit;
            cur_level->loop_struct_size = cur_fence->struct_size;
            cur_level->loop_struct_count = cur_fence->struct_count;

            d_print(1, "!!! Activated fence: 0x%08x - 0x%08x - 0x%08x\n", cur_fence->entry, cur_fence->start, cur_fence->limit);
        }
    }

    return 0x0;
}


int taint_x86::dive(CONTEXT_INFO* info, OFFSET target, OFFSET next)
{
    unsigned i, level;
    CALL_LEVEL* prev_level;
    CALL_LEVEL* cur_level;

    /* check if we don't exceed ret table size */
    level = info->call_level;
    if(level == this->max_call_levels-1) 
    {
        d_print(1, "We reached max ret table depth. If you need to register following calls, you need to extend ret table\n");
        exit(-1);
        return 0x1; 
    }

    /* things to do in previous level */
    prev_level = &info->levels[level];
    prev_level->ret = next;
    info->call_level++;

    /* OK, new level */
    level = info->call_level;
    cur_level = &info->levels[level];
    memset(cur_level, 0x0, sizeof(CALL_LEVEL));

//    cur_level->loop_addr = (OFFSET*)malloc(sizeof(OFFSET) * MAX_LOOP_ADDR);
    cur_level->entry = target;

    d_print(1, "Entry at level %d is: %p\n", level, cur_level->entry);
    cur_level->cur_fence = 0x0;

    /* prepare loop detection structures OBSOLETE
    info->call_src_register_idx[level] = 0x0;
    info->loop_start[level] = NO_LOOP;
    */

    /* if there is collecting going on on previous level, we do not check fences, we just collect */
    if(prev_level->loop_status == FENCE_COLLECTING) 
    {
        cur_level->loop_status = FENCE_COLLECTING;
    }
    else if(prev_level->loop_status == FENCE_NOT_COLLECTING) 
    {
        cur_level->loop_status = FENCE_NOT_COLLECTING;
        cur_level->loop_limit = 0x99999999;
    }
    else
    {
        this->check_fence(cur_level);
    }

    /* other stuff */ 

    d_print(1, "[0x%08x] Ret table:\n", this->cur_tid);
    for(i=info->call_level_smallest; i<info->call_level; i++)
    {
        d_print(1, "[0x%08x] 0x%08x\n", this->cur_tid, this->ctx_info[this->tids[this->cur_tid]].levels[i].ret);
    }

    return 0x0;
}

int taint_x86::surface(CONTEXT_INFO* info)
{
    CALL_LEVEL* cur_level;
    
    cur_level = &info->levels[info->call_level];
    
    info->call_level--;

    if(info->call_level < 0x0) 
    {
        d_print(1, "Error, minimum available level reached");
        d_print(1, "Rerun with larger max_level (-M <level>). Current setting is: 0x%08x\n", this->max_call_levels);
        exit(-1);
    }

    if(info->call_level_smallest > info->call_level) info->call_level_smallest = info->call_level;

    return 0x0;
}

int taint_x86::check_rets(OFFSET ret)
{
    return 0x0;
    /*
    unsigned i;
    for(i = 0x0; i< this->ret_idx; i++)
    {
        if(this->rets[i] == ret)
            return i;
    }
    return -1;
    */
}

int taint_x86::handle_ret(CONTEXT_INFO* cur_ctx, OFFSET eip)
{
    if((!this->started) || (this->finished))
        return 0x0;

    d_print(1, "[handle ret]\n");
    d_print(1, "Eip: 0x%08x\n", eip);
    d_print(1, "LL: 0x%08x\n", cur_ctx->lock_level);
    /* verify if ret points to a symbol */
    
    if(cur_ctx->waiting != 0x0) 
    {
        /* decrease lock_level */
        if(cur_ctx->lock_level > 0x0) cur_ctx->lock_level--;
    }

    if(this->options & OPTION_VERIFY_ROP_RETS)
    {
        d_print(1, "[Checking for ROP ret]\n");
        SYMBOL* s;
        char out_line[MAX_NAME]; 
        
        d_print(1, "Searching for symbol: 0x%08x\n", eip);

        s = this->get_symbol(eip);

        if(s != 0x0)
        {
            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->current_instr_count, this->current_eip, s->lib_name, s->func_name);
            else sprintf(out_line, "[x] 0x%08x call %s!%s", this->current_eip, s->lib_name, s->func_name);
            print_empty_call(cur_ctx, out_line, colors[CODE_BLACK]);
        }
        d_print(1, "[Checking for ROP ret finishes]\n");
    }

    unsigned i,j,diff;
    /* new begins */


    /* check surface */

    d_print(1, "We are on level: %d\n", cur_ctx->call_level);

    if(cur_ctx->call_level <= 0x0) return -1;

    d_print(1, "[0x%08x] Ret table:\n", this->cur_tid);
    for(i=cur_ctx->call_level_smallest; i<cur_ctx->call_level; i++)
    {
        d_print(1, "[0x%08x] 0x%08x\n", this->cur_tid, cur_ctx->levels[i].ret);
    }

    d_print(1, "Trying to match ret addr: 0x%08x\n", eip);
    /* Ret matched */
    for(i = cur_ctx->call_level-1; i >= cur_ctx->call_level_smallest, i > 0; i--)
    {
        if(abs(cur_ctx->levels[i].ret - eip) < 0x5)
        {
            diff = cur_ctx->call_level - i;
            d_print(1, "[0x%08x] (%d) Matched ret 0x%08x on pos: %d, handling diff: %d\n", this->cur_tid, this->current_instr_count, cur_ctx->levels[i].ret, i, diff);

            /* is this correct? */
            if(cur_ctx->waiting != 0x0)
            {
                print_ret(cur_ctx);
            }
            if(cur_ctx->waiting != 0x0) cur_ctx->waiting = 0x0;
            for(j=0x0; j<diff; j++)
            {
                if(cur_ctx->levels[cur_ctx->call_level].loop_status != FENCE_NOT_COLLECTING)
                {
                    print_ret(cur_ctx);
                }
                surface(cur_ctx);
            }
            d_print(1, "[handle ret finishes]\n");
            return 0x0;
        }
    }

    d_print(1, "Failed do match\n");
    /* unmatched rets ignored rets */
    if(cur_ctx->waiting != 0x0) 
    {
        d_print(2, "We are waiting\n");
        return 0x0;
    }
    else if(cur_ctx->before_waiting)
    {
        cur_ctx->before_waiting = 0x0;
        d_print(2, "We just matched waiting\n");
        d_print(1, "[handle ret finishes]\n");
        return 0x0;
    }
    else
    {
        d_print(2, "We are not waiting\n");
        
        /* reset lock_level */
        /* [TODO] this should be somewhere else, but it's not consuimng */
        cur_ctx->lock_level = 0x0;
    }

    /* ret unmatched */
    if(this->options & OPTION_UNMATCHED_RET_INVALIDATES_STACK)
    {
        d_print(1, "Enabled: OPTION_UNMATCHED_RET_INVALIDATES_STACK\n");
        /* handle under surface */
        if(cur_ctx->call_level == cur_ctx->call_level_smallest) //we have to use all stacked rets
        {
                /* pos */
                d_print(1, "[0x%08x] Unmatched ret 0x%08x on pos: %d\n", this->cur_tid, eip, cur_ctx->call_level);
                if(cur_ctx->levels[cur_ctx->call_level].loop_status != FENCE_NOT_COLLECTING)
                {
                    print_ret(cur_ctx);
                }
                surface(cur_ctx);
                
                /* smallest */
                cur_ctx->call_level_smallest--;
        }
    }
    else if(this->options & OPTION_UNMATCHED_RET_CREATES_CALL)
    {
    /* handle under surface */
        d_print(1, "Enabled: OPTION_UNMATCHED_RET_CREATES_CALL\n");
        if(cur_ctx->call_level == cur_ctx->call_level_smallest) //we have to use all stacked rets
        {
                /* pos */
                d_print(1, "[0x%08x] Unmatched ret 0x%08x on pos: %d\n", this->cur_tid, eip, cur_ctx->call_level);
                if(cur_ctx->levels[cur_ctx->call_level].loop_status != FENCE_NOT_COLLECTING)
                {
                    print_ret(cur_ctx);
                }
                surface(cur_ctx);
                
                /* smallest */
                cur_ctx->call_level_smallest--;
        }
    }
    else
    {
        d_print(1, "Enabled: default\n");
        d_print(1, "ignoring\n");
        /*
        if(cur_ctx->levels[cur_ctx->call_level].loop_status != FENCE_NOT_COLLECTING)
        {
            print_ret(cur_ctx);
        }
        surface(cur_ctx);
        */
    }

    /* new ends */
    d_print(1, "[handle ret finishes]\n");
    return 0x0;
}

inline int taint_x86::verify_seg_sec(OFFSET off)
{
    unsigned i;
    
    for(i = 0x0; i< this->security_layer_count; i++)
    {
        if((this->security_layer[i].off <= off) && (this->security_layer[i].off + this->security_layer[i].size >= off))
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
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, this->mem_length) != 0x0) return;
#endif

if(this->options & OPTION_VERIFY_SEG_SEC) 
    if(verify_seg_sec(off))
        return;

#ifdef REVERSE_ANALYSIS
    DWORD_t lost_val;
    this->restore_32(off, lost_val);
    this->a_push_lost_32(lost_val.get_DWORD());
#endif
#if 0
    for(int i = 0x0; i< this->new_bpt_count; i++)
        if(((off - this->new_bps[i].mem_offset) <= 0x4) && (this->new_bps[i].mode & BP_MODE_WRITE))
        {
            d_err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: WRITE to 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
            v.to_mem(&this->memory[off], 1);
            print_mem(1, off, 0x10);
            return;
        }
#endif
    v.to_mem(&this->memory[off], 1);
}

void taint_x86::restore_32(OFFSET off, DWORD_t& ret)
{
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, this->mem_length) != 0x0) 
    {
        ret = this->invalid_dword;
        return;
    }
#endif

    for(int i = 0x0; i< this->new_bpt_count; i++)
        if((this->new_bps[i].mem_offset == off) && (this->new_bps[i].mode & BP_MODE_READ))
        {
            d_err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: READ from 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
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
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, REG_SIZE) != 0x0) return;
#endif

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
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
    {
        return this->invalid_dword;
    }
#endif

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
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, this->mem_length) != 0x0) return;
#endif

if(this->options & OPTION_VERIFY_SEG_SEC) 
    if(verify_seg_sec(off))
        return;

#ifdef REVERSE_ANALYSIS
    WORD_t lost_val;
    this->restore_16(off, lost_val);
    this->a_push_lost_16(lost_val.get_WORD());
#endif
#if 0
    for(int i = 0x0; i<  this->new_bpt_count; i++)
        if(((off - this->new_bps[i].mem_offset) <= 0x2) && (this->new_bps[i].mode & BP_MODE_WRITE))
        {
            d_err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: WRITE to 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
            v.to_mem(&this->memory[off], 1);
            print_mem(1, off, 0x10);
            return;
        }
#endif
    v.to_mem(&this->memory[off], 1);
}

void taint_x86::restore_16(OFFSET off, WORD_t& ret)
{
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, this->mem_length) != 0x0) 
    {
        ret = this->invalid_word;
        return;
    }
#endif

    for(int i = 0x0; i<  this->new_bpt_count; i++)
        if((this->new_bps[i].mem_offset == off) && (this->new_bps[i].mode & BP_MODE_READ))
        {
            d_err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: READ from 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
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
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, REG_SIZE) != 0x0) return;
#endif

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
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
    {
        return this->invalid_word;
    }
#endif

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
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, REG_SIZE) != 0x0) return;
#endif

if(this->options & OPTION_VERIFY_SEG_SEC) 
    if(verify_seg_sec(off))
        return;

#if 0
    for(int i = 0x0; i< this->new_bpt_count; i++)
        if((this->new_bps[i].mem_offset == off) && (this->new_bps[i].mode & BP_MODE_WRITE))
        {
            d_err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: WRITE to 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
            v.to_mem(&this->memory[off]);
            print_mem(1, off, 0x10);
            return;
        }
#endif
    v.to_mem(&this->memory[off]);
}

void taint_x86::restore_8(OFFSET off, BYTE_t& ret)
{
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
    {
        ret = this->invalid_byte;
        return;
    }
#endif

    for(int i = 0x0; i< this->new_bpt_count; i++)
        if((this->new_bps[i].mem_offset == off) && (this->new_bps[i].mode & BP_MODE_READ))
        {
            d_err_print("Mem dump in stderr @ 0x%08x\n", off);
            d_print(1, "Breakpoint RW: READ from 0x%x @ %lld, 0x%08x\n", off, this->current_instr_count, this->current_eip);
            print_mem(1, off, 0x10);
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
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, REG_SIZE) != 0x0) return;
#endif

    CONTEXT_INFO* info;
    info = this->get_tid(tid);
    

    info->registers[off] = v;
    return;
}

BYTE_t taint_x86::reg_restore_8(OFFSET off, int tid)
{
#ifdef VERIFY_OOB
    if(this->verify_oob_offset(off, REG_SIZE) != 0x0) 
    {   
        return this->invalid_byte;
    }
#endif

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
    /* graph start */
    if((this->start_addr == eip) || (this->current_instr_count == this->start_instr))
    {
        d_print(1, "Got ST at 0x%08x, starting\n", eip);
        this->started = 0x1;
//        this->counter = 0x0;
    }
//    if(this->counter <0x10) this->counter++;

    this->cur_info = this->get_tid(this->cur_tid);

    if(cur_info->returning)
    {
        cur_info->before_returning = 1;
        cur_info->returning = 0;
    }

    if(cur_info->calling)
    {
        cur_info->before_calling = 1;
        cur_info->calling = 0;
    }

    if(abs(cur_info->waiting - eip )<0x5) 
    {
        /* stop waiting */
        d_print(1, "[0x%08x] Waiting: 0x%08x, eip: 0x%08x\n", this->cur_tid, cur_info->waiting, eip);
        cur_info->waiting = 0x0;
        cur_info->before_waiting = 0x1;
        if((cur_info->last_emit_decision == DECISION_EMIT) || (cur_info->last_emit_decision == DECISION_EMIT_NESTED))
        {
            print_ret(cur_info);
            cur_info->last_emit_decision = 0x0; 
        }
    }

    this->current_instr_length = 0x0;
    this->current_prefixes = 0x0;
    this->current_prefix_length = 0x0;
    this->current_eip = eip;
    this->reg_store_32(EIP, eip);
    this->current_instr_length = 0x0;
    this->current_instr_is_jump = 0x0;
    return 0x0;
}

int taint_x86::post_execute_instruction(DWORD eip)
{
    unsigned i, j, diff;

    this->extended = 0x0;

    this->last_eip = this->reg_restore_32(EIP).get_DWORD();
    this->last_instr_byte = this->current_instr_byte;
    this->last_instr_count = this->current_instr_count;

    /* graph */

    CONTEXT_INFO* cur_ctx;
    cur_ctx = &this->ctx_info[this->tids[this->cur_tid]];

    /* handle waiting rets */

//    if(cur_ctx->returning > 0x0) cur_ctx->returning--;

    if((cur_ctx->before_returning == 1))
    {
        handle_ret(cur_ctx, eip);
        cur_ctx->before_returning = 0;
    }

    if(cur_ctx->before_calling)
    {
        cur_ctx->target = eip;
        d_print(1, "Next call target = 0x%08x\n", eip);
        handle_call(cur_ctx);
        cur_ctx->before_calling = 0;
    }

    /* handle surface rets */

    char out_line[MAX_NAME];

    /* wanted */
    for(i=0x0; i<this->wanted_count_i; i++)
        if(this->instr_wanted[i] == this->current_instr_count)
        {
            sprintf(out_line, "[x] (%d)0x%08x", this->current_instr_count ,this->current_eip);
            print_call(cur_ctx, out_line, colors[CODE_RED]);
            print_ret(cur_ctx);
 
        }

    for(i=0x0; i<this->wanted_count_e; i++)
        if(this->addr_wanted[i] == this->current_eip)
        {
            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x", this->current_instr_count ,this->current_eip);
            else sprintf(out_line, "[x] 0x%08x", this->current_eip);
            print_call(cur_ctx, out_line, colors[CODE_RED]);
            print_ret(cur_ctx);
        }

    // probable eip
    if(!this->current_instr_is_jump)
        reg_store_32(EIP, this->reg_restore_32(EIP) + this->current_instr_length);

    fflush(stdout);

    /* search propagation watchpoints */

    unsigned esp;
    esp = (UDWORD)this->reg_restore_32(ESP).get_DWORD();

    /* regular breakpoints */

    for(i=0x0; i<this->new_bpt_count; i++)
        if((this->new_bps[i].offset == this->current_instr_count) && (this->new_bps[i].offset) && (this->new_bps[i].mode & BP_MODE_EXECUTE))
        {
            d_err_print("Breakpoint @ %lld, 0x%08x, hit, dumping contexts & stacks\n", this->new_bps[i].offset, this->current_eip);
            this->print_err_all_contexts();
            this->print_err_all_stacks();
            d_err_print("---\n");
        }
        else if((this->new_bps[i].mem_offset == this->current_eip) && (this->new_bps[i].mem_offset) && (this->new_bps[i].mode & BP_MODE_EXECUTE))
        {
            d_err_print("Breakpoint @ %lld, 0x%08x, hit, dumping contexts & stacks\n", this->new_bps[i].offset, this->current_eip);
            this->print_err_all_contexts();
            this->print_err_all_stacks();
            d_err_print("---\n");
        }

    /* taint breakpoints */

    for(i=0x0; i<this->new_bpt_t_count; i++)
        if((this->new_bps_t[i].offset == this->current_instr_count) && (this->new_bps_t[i].offset))
        {
            d_err_print("Breakpoint taint @ %lld, 0x%08x, hit, dumping contexts & stacks\n", this->new_bps[i].offset, this->current_eip);
            this->print_err_all_t_contexts();
            this->print_err_all_t_stacks();
            d_err_print("---\n");
        }
        else if((this->new_bps_t[i].mem_offset == this->current_eip) && (this->new_bps_t[i].mem_offset))
        {
            d_err_print("Breakpoint taint @ %lld, 0x%08x, hit, dumping contexts & stacks\n", this->new_bps[i].offset, this->current_eip);
            this->print_err_all_t_contexts();
            this->print_err_all_t_stacks();
            d_err_print("---\n");
        }

    /* trace watchpoints */

    for(i=0x0; i<this->new_wpt_count; i++)
        if((this->new_wps[i].offset == this->current_instr_count) && (this->new_wps[i].offset))
        {
            d_err_print("Watchpoint @ %lld hit, dumping taint transfer history for %s for tid 0x%08x\n", this->new_wps[i].offset, this->new_wps[i].name, this->new_wps[i].tid);
            this->print_taint_history(this->new_wps[i].watched);
//            d_err_print("ptr: 0x%08x\n", &this->ctx_info[this->tids[this->new_wps[i].tid]].registers[EBP]);
//            this->print_taint_history(&this->ctx_info[this->tids[this->new_wps[i].tid]].registers[EBP]);
            d_err_print("---\n");
            if(this->new_wps[i].interactive)
            {
                this->prompt_taint();
            }
        }
        else if((this->new_wps[i].mem_offset == this->current_eip) && (this->new_wps[i].mem_offset))
        {
            d_err_print("Watchpoint @ %lld hit, dumping taint transfer history for %s\n", this->new_wps[i].mem_offset, this->new_wps[i].name);
            this->print_taint_history(this->new_wps[i].watched);
            d_err_print("---\n");
            if(this->new_wps[i].interactive)
            {
                this->prompt_taint();
            }
        }
    if((this->end_addr) || (this->instr_limit))
    {
        if((eip == this->end_addr) || (this->instr_limit == this->current_instr_count)) 
        {
            this->finished = 0x1;
            this->aborted = 0x1;
            d_print(1, "Eip: 0x%08x, this->end_addr: 0x%08x, limit: %d, count: %d, finishing\n", eip, this->end_addr, this->instr_limit, this->current_instr_count);
        }
    }

#ifdef DEBUG_PRINT_CONTEXT
    this->print_context();
#endif

#ifdef DEBUG_PRINT_STACK
    this->print_stack(1, 5);
#endif

    return 0x0;
}

int taint_x86::execute_instruction(DWORD eip, DWORD tid)
{
    d_print(3, "[0x%08x] Inst: 0x%08x, count: %d\n", this->cur_tid, eip, this->current_instr_count);
    int ret = 0x0;

    this->cur_tid = tid;

    /* debug */
    for(int i = 0x0; i< 0x10; i++)
        if((this->my_bps[i].offset == eip) & (this->my_bps[i].mode == BP_MODE_EXECUTE))
        {
            d_print(3, "Breakpoint EXECUTE at 0x%x\n", eip);
            this->bp_hit = 0x1;
            //getchar();
        }
            

    this->current_instr_byte = &this->memory[eip];
/*
    if((this->started) && (this->counter <0x10))
        d_print(1, "[0x%08x] 0x%08x: 0x%02x, count: %d\n", this->cur_tid, eip, *(this->current_instr_byte), this->current_instr_count);
*/
    //this->current_instr_byte->set_BYTE_t(0xff); // taint executed?
    //this->current_propagation->instruction = eip;
    this->current_instr_length += 1;

    // process prefixes
    while(a_is_prefix(this->current_instr_byte))
    {
        ret = (this->*(instructions_32[current_instr_byte->get_BYTE()]))(this->current_instr_byte);
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
        ret = (this->*(instructions_32_extended[current_instr_byte->get_BYTE()]))(current_instr_byte);
    }
    else
    {
        ret = (this->*(instructions_32[current_instr_byte->get_BYTE()]))(current_instr_byte);
    }

    return ret;
}

int taint_x86::execute_instruction_at_eip(DWORD eip, DWORD tid)
{

    //this->propagations[this->current_propagation_count].instruction = eip;
    //this->propagations[this->current_propagation_count].instr_count = this->current_instr_count;
    this->cur_tid = tid;
    this->pre_execute_instruction(eip);
    this->execute_instruction(eip, tid);
    this->post_execute_instruction(eip);

    return 0x0;
}


int taint_x86::execute_instruction_at_eip(DWORD eip)
{
    return this->execute_instruction_at_eip(eip, this->cur_tid);
}

CONTEXT_INFO* taint_x86::get_tid(DWORD tno)
{
    CONTEXT_INFO* ret = 0x0;
    ret = &this->ctx_info[this->tids[tno]];
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
    return ret;
}

int taint_x86::check_thread(CONTEXT_info ctx_info)
{
    DWORD already_added = 0x0;
    DWORD tid;
    unsigned i;
    char graph_filename[MAX_NAME];
    char out_line[MAX_NAME];

    tid = ctx_info.thread_id;

    if(this->reg_restore_32(EAX, tid).get_DWORD() != ctx_info.ctx.Eax) goto error;
    if(this->reg_restore_32(ECX, tid).get_DWORD() != ctx_info.ctx.Ecx) goto error;
    if(this->reg_restore_32(EDX, tid).get_DWORD() != ctx_info.ctx.Edx) goto error;
    if(this->reg_restore_32(EBX, tid).get_DWORD() != ctx_info.ctx.Ebx) goto error;
    if(this->reg_restore_32(ESI, tid).get_DWORD() != ctx_info.ctx.Esi) goto error;
    if(this->reg_restore_32(EDI, tid).get_DWORD() != ctx_info.ctx.Edi) goto error;
    if(this->reg_restore_32(EBP, tid).get_DWORD() != ctx_info.ctx.Ebp) goto error;
    if(this->reg_restore_32(ESP, tid).get_DWORD() != ctx_info.ctx.Esp) goto error;
    if(this->reg_restore_32(EIP, tid).get_DWORD() != ctx_info.ctx.Eip) goto error;

    goto noerror;

    error:
        if(last_inconsistent == 0x0)
            d_print(4, "WARNING: New inconsistency detected @ %d, eip: 0x%08x\n", this->current_instr_count, this->current_eip);
        else
            d_print(4, "WARNING: inconsistency detected\n");

        d_print(4, "Is:\n");
        //this->print_context();

        d_print(4, "\nShould be:\n");
        d_print(4, "EAX: 0x%08x\n", ctx_info.ctx.Eax);
        d_print(4, "EBX: 0x%08x\n", ctx_info.ctx.Ebx);
        d_print(4, "ECX: 0x%08x\n", ctx_info.ctx.Ecx);
        d_print(4, "EDX: 0x%08x\n", ctx_info.ctx.Edx);
        d_print(4, "ESI: 0x%08x\n", ctx_info.ctx.Esi);
        d_print(4, "EDI: 0x%08x\n", ctx_info.ctx.Edi);
        d_print(4, "EBP: 0x%08x\n", ctx_info.ctx.Ebp);
        d_print(4, "ESP: 0x%08x\n", ctx_info.ctx.Esp);
        d_print(4, "EIP: 0x%08x\n", ctx_info.ctx.Eip);

        this->last_inconsistent = 0x1;

#ifdef UPDATE_THREAD
        this->mod_thread(ctx_info);
#endif

    return 0x0;

    noerror:
        this->last_inconsistent = 0x0;

    return 0x0;
}

int taint_x86::finish()
{
    unsigned i, j, k;
    CONTEXT_INFO* cur_tid;
    char out_line[MAX_NAME];
    int diff_last, diff_first;
    unsigned open;

    d_print(1, "Closing %d tids\n", this->tid_count);
    for(i=0x0; i<this->tid_count; i++)
    {
        cur_tid = &this->ctx_info[i];
        d_print(1, "Closing 0x%08x\n", cur_tid->tid);

        if(cur_tid->waiting != 0x0)
        {
            if((cur_tid->last_emit_decision == DECISION_EMIT) || (cur_tid->last_emit_decision == DECISION_EMIT_NESTED))
            {
                print_ret(cur_tid);
                cur_tid->last_emit_decision = 0x0;
            }
        }

        open = cur_tid->call_level - cur_tid->call_level_smallest;
        d_print(1, "[0x%08x] Left with %d nodes open\n", cur_tid->tid, open);
        d_print(1, "[0x%08x] First: %d - %d = %d\n", cur_tid->tid, cur_tid->call_level_smallest, abs(this->call_level_start - cur_tid->call_level_smallest));

        diff_first = abs(this->call_level_start - cur_tid->call_level_smallest);
        diff_last = open;

        d_print(1, "[0x%08x] Diff_last: %d\n", cur_tid->tid, diff_last);
        for(j=0x0; j < diff_last; j++)
        {
/*
#ifdef ANALYZE_LOOPS
            for(k = 0x0; k<cur_tid->loop_pos[cur_tid->call_level]; k++)
            {
                exit_loop(cur_tid);
            }
#endif
*/
            print_ret(cur_tid);
            cur_tid->call_level--;
        }

/*
#ifdef ANALYZE_LOOPS
        for(k = 0x0; k<cur_tid->loop_pos[cur_tid->call_level]; k++)
        {
            exit_loop(cur_tid);
        }
#endif
*/

        sprintf(out_line, "</node></map>\n");
        fwrite(out_line, strlen(out_line), 0x1, cur_tid->graph_file);

        d_print(1, "[0x%08x] Diff_first: %d\n", cur_tid->tid, diff_first);
        sprintf(out_line, "<map version=\"1.0.1\">\n<node TEXT=\"start\">\n");
        fwrite(out_line, strlen(out_line), 0x1, cur_tid->graph_file);

        cur_tid->call_level--;
        for(j=0x0; j < diff_first; j++)
        {
            cur_tid->call_level++;
            print_call(cur_tid, "unknown", colors[CODE_BLACK]);
        //print_call(cur_tid->graph_file, "unknown", colors[CODE_BLACK]);
        }
    }
    return 0x0;    
}

int taint_x86::mod_thread(CONTEXT_info ctx_info)
{
    DWORD already_added = 0x0;

    d_print(3, "Updating thread: 0x%08x\n", ctx_info.thread_id);

    if(!this->already_added(ctx_info.thread_id))
    {
        return 0x0;
    }

    DWORD_t reg;

    reg = this->reg_restore_32(EAX);
    reg.set_DWORD(ctx_info.ctx.Eax);
    this->reg_store_32(EAX, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(ECX);
    reg.set_DWORD(ctx_info.ctx.Ecx);
    this->reg_store_32(ECX, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(EDX);
    reg.set_DWORD(ctx_info.ctx.Edx);
    this->reg_store_32(EDX, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(EBX);
    reg.set_DWORD(ctx_info.ctx.Ebx);
    this->reg_store_32(EBX, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(ESI);
    reg.set_DWORD(ctx_info.ctx.Esi);
    this->reg_store_32(ESI, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(EDI);
    reg.set_DWORD(ctx_info.ctx.Edi);
    this->reg_store_32(EDI, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(ESP);
    reg.set_DWORD(ctx_info.ctx.Esp);
    this->reg_store_32(ESP, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(EBP);
    reg.set_DWORD(ctx_info.ctx.Ebp);
    this->reg_store_32(EBP, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(EIP);
    reg.set_DWORD(ctx_info.ctx.Eip);
    this->reg_store_32(EIP, reg, ctx_info.thread_id);

    reg = this->reg_restore_32(EFLAGS);
    reg.set_DWORD(ctx_info.ctx.EFlags);
    this->reg_store_32(EFLAGS, reg, ctx_info.thread_id);

    //this->print_context(this->cur_tid);

//    this->cur_tid = 0;
    
    return 0x0;
}

int taint_x86::add_thread(CONTEXT_info ctx_info)
{
    DWORD already_added = 0x0;
    unsigned i;
//    char graph_filename[MAX_NAME];
    char out_line[MAX_NAME];

    d_print(3, "Adding thread: 0x%08x\n", ctx_info.thread_id);

    if(!this->already_added(ctx_info.thread_id))
    {
        sprintf(this->ctx_info[this->tid_count].graph_filename, "TID_%08X.mm", ctx_info.thread_id);
        d_print(1, "Creating graph file: %s\n", this->ctx_info[this->tid_count].graph_filename);
        this->ctx_info[this->tid_count].graph_file = fopen(this->ctx_info[this->tid_count].graph_filename, "w");
        this->ctx_info[this->tid_count].call_level = (this->max_call_levels/3); // starting at level 1/3 of max_call_levels
        this->ctx_info[this->tid_count].call_level_smallest = this->ctx_info[this->tid_count].call_level;
        this->ctx_info[this->tid_count].levels = (CALL_LEVEL*)malloc(sizeof(CALL_LEVEL)*this->max_call_levels);
        if(this->ctx_info[this->tid_count].levels == 0x0)
        {
            d_print(1, "Unable to allocate\n");
            exit(-1);
        }
        memset(this->ctx_info[this->tid_count].levels, 0x0, sizeof(CALL_LEVEL)*this->max_call_levels);
        this->ctx_info[this->tid_count].waiting = 0x0;
        this->ctx_info[this->tid_count].list = (DWORD*)malloc(sizeof(DWORD) * MAX_LIST_JXX);
        this->ctx_info[this->tid_count].list_len = 0x0;

        /* clear loop structures */
        unsigned call_level;
        call_level = this->ctx_info[this->tid_count].call_level;
//        this->ctx_info[this->tid_count].loop_start[call_level] = NO_LOOP;

        /* for graphs */
        unsigned level;
        CALL_LEVEL* cur_level;
    
        level = this->ctx_info[this->tid_count].call_level;
        cur_level = &this->ctx_info[this->tid_count].levels[level];
    
        cur_level->entry = 0xffffffff;
        this->check_fence(cur_level);

        /* output marker */
        char out_line[MAX_NAME];

        strcpy(out_line, "");
        for(i = this->call_level_start-this->call_level_offset; i< call_level; i++)
            strcat(out_line, " ");
        fwrite(out_line, strlen(out_line), 0x1, this->ctx_info[this->tid_count].graph_file);

        sprintf(out_line, "<node TEXT=\"[ENTRY]\"></node>\n");
        fwrite(out_line, strlen(out_line), 0x1, this->ctx_info[this->tid_count].graph_file);

        /* fnalize */
        this->ctx_info[this->tid_count].tid = ctx_info.thread_id;
        //update lookup table
        this->tids[ctx_info.thread_id] = this->tid_count;
        this->tid_count++;
    }

    this->update_watchpoints(ctx_info.thread_id);

    this->reg_store_32(EAX, ctx_info.ctx.Eax, ctx_info.thread_id);
    this->reg_store_32(ECX, ctx_info.ctx.Ecx, ctx_info.thread_id);
    this->reg_store_32(EDX, ctx_info.ctx.Edx, ctx_info.thread_id);
    this->reg_store_32(EBX, ctx_info.ctx.Ebx, ctx_info.thread_id);
    this->reg_store_32(ESI, ctx_info.ctx.Esi, ctx_info.thread_id);
    this->reg_store_32(EDI, ctx_info.ctx.Edi, ctx_info.thread_id);
    this->reg_store_32(ESP, ctx_info.ctx.Esp, ctx_info.thread_id);
    this->reg_store_32(EBP, ctx_info.ctx.Ebp, ctx_info.thread_id);
    this->reg_store_32(EFLAGS, ctx_info.ctx.EFlags, ctx_info.thread_id);

    OFFSET addr;
    CONTEXT_INFO* info;

    info = this->get_tid(ctx_info.thread_id);

    for(i=0x0; i<0x6; i++)
    {
        addr = this->a_calculate_base(ctx_info.ldt[i]);
        d_print(3, "Segment 0x%02x base: 0x%08x\n", i, addr);
        info->seg_map[i] = addr;
    }
   
    //info->call_level_largest = 0x3; 

    //this->print_context(this->cur_tid);

    this->cur_tid = 0;
    
    d_print(1, "ER_9 TID: 0x%08x lock_level: 0x%08x\n", ctx_info.thread_id, this->ctx_info[this->tid_count-1].lock_level);
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

int taint_x86::set_lib_dir_path(char* path)
{
    strcpy(this->lib_dir_path, path);
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

int taint_x86::apply_memory(DWORD offset, DWORD size)
{
    char* buffer;
    int i;
    size_t read;

    buffer = (char*)malloc(size);
    
    d_print(2, "Position before: 0x%08x\n", ftell(this->mod_file));
    read = fread(buffer, 1, size, this->mod_file);
    d_print(2, "Position after: 0x%08x\n", ftell(this->mod_file));
    d_print(2, "Read 0x%08x bytes\n", read);

    d_print(1, "Trying to apply 0x%08x bytes @ 0x%08x, pos after: 0x%08x\n", size, offset, ftell(this->mod_file));

#ifdef VERIFY_OOB
    if(this->verify_oob_offset(offset, this->mem_length) != 0x0) return 0x0;
#endif

    d_print(2, "Before:\n");
    this->print_mem(2, offset, 0x10);
    
    for(i=0x0; i<size; i++)
    {
        char found = 0x0;
        unsigned j;

        for(j = 0x0; j< 0x10; j++)
            if((this->my_bps[j].offset == offset+i) && (this->my_bps[j].mode & BP_MODE_WRITE))
                found = 1;

        if(found)
        {
            d_print(3, "Breakpoint RW: WRITE at 0x%x\n", offset);
            print_mem(3, offset + i, 0x3);
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

int taint_x86::apply_lib_exports(LIB_INFO* lib)
{
    return 0x0;
}

int taint_x86::apply_lib_layout(LIB_INFO* lib)
{
    d_print(2, "Applying lib\n");

    unsigned i;
    std::ifstream pe_file;

    pe_file.open(lib->path, std::ios::in | std::ios::binary);
    pe_bliss::pe_base image(pe_bliss::pe_factory::create_pe(pe_file));

    d_print(2, "Applying sections:\n");
    pe_bliss::section_list sections(image.get_image_sections());

    for(pe_bliss::section_list::const_iterator it = sections.begin(); it != sections.end(); ++it)
	{
		const pe_bliss::section& s = *it; //Секция
        d_print(2, "Applying section [ %s ]: ", s.get_name().c_str());
/*
        d_print(2, "Characteristics: 0x%08x ", s.get_characteristics());
        d_print(2, "Section %s ", s.get_name());
        d_print(2, "Section %s ", s.get_name());
        d_print(2, "Section %s ", s.get_name());
*/
/*
		std::cout << "Section [" << s.get_name() << "]" << std::endl //Имя секции
			<< "Characteristics: " << s.get_characteristics() << std::endl //Характеристики
			<< "Size of raw data: " << s.get_size_of_raw_data() << std::endl //Размер данных в файле
			<< "Virtual address: " << s.get_virtual_address() << std::endl //Виртуальный адрес
			<< "Virtual size: " << s.get_virtual_size() << std::endl //Виртуальный размер
			<< std::endl;
*/
        OFFSET off = lib->offset + s.get_virtual_address();
        OFFSET off_raw = s.get_pointer_to_raw_data();

        d_print(2, "0x%08x - 0x%08x\n", off, off+s.get_size_of_raw_data());


        for(i=0x0; i<s.get_size_of_raw_data(); i++)
        {
            this->memory[off + i].set_BYTE(lib->content[off_raw + i]);
            this->memory[off + i].set_BYTE_t(0x0);
        }

	}
    
    return 0x0;
}

int taint_x86::add_symbols(LIB_INFO* info)
{
    SYMBOL* s;
    SYMBOL* s1;
    SYMBOL* old;
    SYMBOL* temp;

    s = this->symbols;
    if(s == 0x0) 
    {
        info->symbols = 0x0;
        return 0x0;
    }
    old = 0x0;

    do
    {
        if(s == 0x0) break;
        if(s->lib_name == 0x0) break;

        if(!strcmpi(info->name, s->lib_name))
        {
            s->addr += info->offset;
            //d_print(1, "Resolved symbol: %s!%s @ 0x%08x\n", s->lib_name, s->func_name, s->addr);
            s->resolved = 0x1;
    
            //move
            if(old) old->next = s->next;
            else this->symbols = this->symbols->next;

            s->next = info->symbols;
            info->symbols = s;

            if(old) s = old->next;
            else s = this->symbols;
        }
        else
        {
            old = s;
            s = s->next;
        }
    }
    while(s);
    
    return 0x0;
}

int taint_x86::add_lib(OFFSET off, char* name)
{
    LIB_INFO new_lib;
    FILE* f;

    unsigned i;
    for(i=0x0; i<strlen(name); i++)
    {
        if(name[i] == '\r') name[i]='\x00';
        if(name[i] == '\\') name[i]='/';
    }

    strcpy(new_lib.path, this->lib_dir_path);
    strcat(new_lib.path, basename(name));

    d_print(1, "Loading lib: %s\n", new_lib.path);

    strcpy(new_lib.name, basename(name));

    d_print(1, "Loading symbols for %s @ 0x%08x\n", new_lib.name, off);
    new_lib.offset = off;
    this->add_symbols(&new_lib);

    new_lib.loaded = 1;

    this->check_lib_blacklist(&new_lib);
    this->libs[libs_count] = new_lib;
    this-> libs_count ++; 

    d_print(2, "Loaded lib: %s at 0x%08x to 0x%08x\n", new_lib.path, new_lib.offset, new_lib.offset+new_lib.length);
    return 0x0;
}

int taint_x86::handle_exception(EXCEPTION_INFO info)
{
    char out_line[MAX_NAME];

    sprintf(out_line, "[x] Exception %08x in TID %08x, instr. no: %d, eip: 0x%08x", info.er.ExceptionCode, info.tid, this->current_instr_count, info.er.ExceptionAddress);
    print_empty_call(&this->ctx_info[this->tids[this->cur_tid]], out_line, colors[CODE_RED]);
    return 0x0;
}

int taint_x86::add_exception(EXCEPTION_INFO info)
{
    
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
    this->handle_exception(info);
    return 0x0;
}

int taint_x86::del_thread(DWORD tid)
{
    char out_line[MAX_NAME];
    unsigned int i, diff;
    unsigned thread_idx;

/*    thread_idx = this->tids[tid];
    free(this->ctx_info[tid].levels);
*/
/*
    d_print(1, "Removing  thread: 0x%08x\n", tid);
    //tid = this->tids[tid];


    // close remaining nodes

    if(this->ctx_info[tid].call_level <0) this->ctx_info[tid].call_level = 0;
    for(i=0x0; i<this->ctx_info[tid].call_level; i++)
        print_ret(this->ctx_info[tid].graph_file);

    sprintf(out_line, "</node></map>\n");
    fwrite(out_line, strlen(out_line), 0x1, this->ctx_info[tid].graph_file);

    //print thread head
    sprintf(out_line, "call unknown");

    for(i=0x0; i<-this->ctx_info[tid].call_level_smallest ; i++)
        print_call(this->ctx_info[tid].graph_file, out_line, "0xffffffff");

    fclose(this->ctx_info[tid].graph_file);
*/
    return 0x0;
}

int taint_x86::del_thread_srsly(DWORD tid)
{
    char out_line[MAX_NAME];
    unsigned int i, diff;

    d_print(1, "Removing  thread: 0x%08x\n", tid);
    //tid = this->tids[tid];


    // close remaining nodes
/*
    if(this->ctx_info[tid].call_level <0) this->ctx_info[tid].call_level = 0;
    for(i=0x0; i<this->ctx_info[this->tids[tid]].call_level; i++)
        print_ret(this->ctx_info[this->tids[tid]].graph_file);

    sprintf(out_line, "</node></map>\n");
    fwrite(out_line, strlen(out_line), 0x1, this->ctx_info[this->tids[tid]].graph_file);

    //print thread head
    sprintf(out_line, "call unknown");

    for(i=0x0; i<-this->ctx_info[this->tids[tid]].call_level_smallest ; i++)
        print_call(this->ctx_info[this->tids[tid]].graph_file, out_line, "0xffffffff");
*/
    fclose(this->ctx_info[this->tids[tid]].graph_file);

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

int taint_x86::open_lost_file(char* path)
{
#ifdef REVERSE_ANALYSIS
    d_print(3, "Opening lost file: %s\n", path);
    this->lost_file = fopen(path, "wb");
#endif
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
#ifdef REVERSE_ANALYSIS
    if(this->lost_file) fclose(this->lost_file);
#endif
    if(this->mod_file) fclose(this->mod_file);
    return 0x0;
}

int taint_x86::add_taint(OFFSET start, UDWORD length)
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
        this->memory[start+i].set_BYTE_t(0xff);
    }

    this->taints[this->taint_count].off = start;
    this->taints[this->taint_count].size = length;
    this->taint_count++;

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

int taint_x86::attach_current_propagation_r_8(OFFSET off)
{
    return 0x0;
    CONTEXT_INFO* info;
    info = this->get_tid(this->cur_tid);

    if(off > REG_SIZE)
    {
        d_print(3, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }

    info->registers[off+0].set_BYTE_t_id(this->current_propagation_count);
    this->propagations[this->current_propagation_count].result[0x0] = &info->registers[off+0];
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+0]);
    this->current_propagation_count++;

    return 0x0;
}

int taint_x86::attach_current_propagation_r_16(OFFSET off)
{
    return 0x0;
    CONTEXT_INFO* info;
    info = this->get_tid(this->cur_tid);

    if(off > REG_SIZE)
    {
        d_print(3, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }

    info->registers[off+0].set_BYTE_t_id(this->current_propagation_count);
    info->registers[off+1].set_BYTE_t_id(this->current_propagation_count);
    this->propagations[this->current_propagation_count].result[0x0] = &info->registers[off+0];
    this->propagations[this->current_propagation_count].result[0x1] = &info->registers[off+1];
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+0]);
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+1]);
    this->current_propagation_count++;

    return 0x0;
}

int taint_x86::attach_current_propagation_r_32(OFFSET off)
{
    return 0x0;
    CONTEXT_INFO* info;
    info = this->get_tid(this->cur_tid);

    if(off > REG_SIZE)
    {
        d_print(3, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }

    info->registers[off+0].set_BYTE_t_id(this->current_propagation_count);
    info->registers[off+1].set_BYTE_t_id(this->current_propagation_count);
    info->registers[off+2].set_BYTE_t_id(this->current_propagation_count);
    info->registers[off+3].set_BYTE_t_id(this->current_propagation_count);
    this->propagations[this->current_propagation_count].result[0x0] = &info->registers[off+0];
    this->propagations[this->current_propagation_count].result[0x1] = &info->registers[off+1];
    this->propagations[this->current_propagation_count].result[0x2] = &info->registers[off+2];
    this->propagations[this->current_propagation_count].result[0x3] = &info->registers[off+3];
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+0]);
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+1]);
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+2]);
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &info->registers[off+3]);
    this->current_propagation_count++;

    return 0x0;
}

int taint_x86::attach_current_propagation_m_8(OFFSET off)
{
    return 0x0;

    if(off > 0x7fffffff)
    {
        d_print(3, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }
    this->memory[off+0].set_BYTE_t_id(this->current_propagation_count);
    this->propagations[this->current_propagation_count].result[0x0] = &this->memory[off+0];
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+0]);
    this->current_propagation_count++;

    return 0x0;
}

int taint_x86::attach_current_propagation_m_16(OFFSET off)
{
    return 0x0;

    if(off > 0x7fffffff)
    {
        d_print(3, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }
    this->memory[off+0].set_BYTE_t_id(this->current_propagation_count);
    this->memory[off+1].set_BYTE_t_id(this->current_propagation_count);
    this->propagations[this->current_propagation_count].result[0x0] = &this->memory[off+0];
    this->propagations[this->current_propagation_count].result[0x1] = &this->memory[off+1];
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+0]);
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+1]);
    this->current_propagation_count++;

    return 0x0;
}

int taint_x86::attach_current_propagation_m_32(OFFSET off)
{
    return 0x0;
    if(off > 0x7fffffff)
    {
        d_print(3, "Error registering op @ 0x%08x\n", off);
        return 0x0;
    }
    this->memory[off+0].set_BYTE_t_id(this->current_propagation_count);
    this->memory[off+1].set_BYTE_t_id(this->current_propagation_count);
    this->memory[off+2].set_BYTE_t_id(this->current_propagation_count);
    this->memory[off+3].set_BYTE_t_id(this->current_propagation_count);
    this->propagations[this->current_propagation_count].result[0x0] = &this->memory[off+0];
    this->propagations[this->current_propagation_count].result[0x1] = &this->memory[off+1];
    this->propagations[this->current_propagation_count].result[0x2] = &this->memory[off+2];
    this->propagations[this->current_propagation_count].result[0x3] = &this->memory[off+3];
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+0]);
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+1]);
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+2]);
    d_print(4, "Attach propagation no: %d to BYTE_t: 0x%08x\n", this->current_propagation_count, &this->memory[off+3]);
    this->current_propagation_count++;

    return 0x0;
}

int taint_x86::print_taint_ops(unsigned id)
{
    TAINTED* cur_op;
    
    cur_op = this->propagations[id].first_op;
    if(!cur_op) return 0x0;

    do
    {
        d_print(3, "ops");
        if(cur_op->next) cur_op = cur_op->next;
    }
    while(cur_op->next);
    
    return 0x0;
}

int taint_x86::print_taint_history(unsigned id)
{
    if(id == 0x0) return 0x0;

    unsigned i;

    PROPAGATION_ELEM* cur_elem;

//    d_print(3, "1\n");
    cur_elem = this->propagations[id].causes;
    if(cur_elem)
    {
        d_print(4, "Propagations elem count: %d\n", this->propagations[id].elem_count);
        for(i=0x0; i<0x1;i++)
        {
            d_print(4, "%d -> %d\n", id, cur_elem->cause_id);
            this->print_taint_history(cur_elem->cause_id);
            if(cur_elem->next)
            {
                d_print(4, "Traversing: 0x%08x -> 0x%08x\n");
                cur_elem = cur_elem->next;
            }
            else break;
        } 
    }
    d_print(1, "Taint id: %d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d\n", id, this->propagations[id].instruction, this->memory[this->propagations[id].instruction].get_BYTE(), this->propagations[id].instr_count);
    d_err_print("Taint id: %d, EIP: 0x%08x, instr byte: 0x%02x, instr no: %d\n", id, this->propagations[id].instruction, this->memory[this->propagations[id].instruction].get_BYTE(), this->propagations[id].instr_count);

    return 0x0;
}

int taint_x86::print_taint_history(BYTE_t* target)
{
    unsigned taint_id;
    taint_id = target->get_BYTE_t_id();

    if(taint_id == 0x0) return 0x0;
    else return this->print_taint_history(taint_id);
}

int taint_x86::print_taint_history(BYTE_t* target, OFFSET instr_count)
{
    PROPAGATION* cur_propagation;
    unsigned taint_id;
    taint_id = target->get_BYTE_t_id();

    unsigned i, j;
    DWORD found;

    if(taint_id == 0x0) return 0x0;

    found = 0x0;

    /* search for proper propagation */  
    for(i = this->current_propagation_count; i>0x0; i--)
    {
        cur_propagation = &this->propagations[i];
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
    else return this->print_taint_history(taint_id);
}

int taint_x86::reg_propagation_op(BYTE_t* op)
{
    return 0x0;
    d_print(4, "Registering BYTE_t*: 0x%08x\n", op);
//    if(!op->get_BYTE_t()) return 0x0;
    TAINTED** cur_op;
    TAINTED* new_op;

    PROPAGATION_ELEM* cur_elem;
    PROPAGATION_ELEM* new_elem;
/*
    new_op = (TAINTED*)malloc(sizeof(TAINTED));
    new_op->tainted = op;
    new_op->next = 0x0;
*/
    new_elem = (PROPAGATION_ELEM*)malloc(sizeof(PROPAGATION_ELEM));
    if(new_elem == 0x0)
    {
        fprintf(stderr, "Error allocations memory");
        exit(1);
    }
//    fprintf(stderr, "new_elem = 08%08x\n", new_elem);
    new_elem->cause_id = op->get_BYTE_t_id();
    if(new_elem->cause_id > this->current_propagation_count)
    {
        d_print(3, "Error, registering cause_id larger than propagation count.\n");
        exit(1);
    }

    new_elem->next = 0x0;

    if(op->get_BYTE_t_id()) d_print(4, "Registering non-void cause\n");

/*
    cur_op = &this->propagations[this->current_propagation_count].first_op;

    if(*cur_op == 0x0)
    {
        *cur_op = new_op;
    }
    else
    {
        while((*cur_op)->next)
            cur_op = &(*cur_op)->next;

        (*cur_op)->next = new_op;
    }
*/
//    fprintf(stderr, "Instrocution count: %d\n", this->current_propagation_count);
    //cur_elem = this->propagations[this->current_propagation_count].causes;


    //if(cur_elem == 0x0)
    if(this->propagations[this->current_propagation_count].causes == 0x0)
    {
        this->propagations[this->current_propagation_count].causes = new_elem;
        //*cur_elem = new_elem;
        d_print(4, "Registered cause with id: %d\n", new_elem->cause_id);
    }
    else
    {
        cur_elem = this->propagations[this->current_propagation_count].causes;

        while(cur_elem->next != 0x0)
            cur_elem = cur_elem->next;

//        fprintf(stderr, "*cur_elem = 08%08x\n", *cur_elem);
        cur_elem->next = new_elem;

        d_print(4, "Registered cause with id: %d\n", new_elem->cause_id);
    }

    this->propagations[this->current_propagation_count].elem_count++;

    return 0x0;
}

int taint_x86::reg_propagation_op_r_8(OFFSET off)
{
    return 0x0;
    CONTEXT_INFO* info;
    info = this->get_tid(this->cur_tid);

    this->verify_oob_offset(off, REG_SIZE);

    this->reg_propagation_op(&info->registers[off+0]);
    return 0x0;
}

int taint_x86::reg_propagation_op_r_16(OFFSET off)
{
    return 0x0;
    CONTEXT_INFO* info;
    info = this->get_tid(this->cur_tid);

    this->verify_oob_offset(off, REG_SIZE);

    this->reg_propagation_op(&info->registers[off+0]);
    this->reg_propagation_op(&info->registers[off+1]);
    return 0x0;
}

int taint_x86::reg_propagation_op_r_32(OFFSET off)
{
    return 0x0;
    CONTEXT_INFO* info;
    info = this->get_tid(this->cur_tid);

    this->verify_oob_offset(off, REG_SIZE);

    this->reg_propagation_op(&info->registers[off+0]);
    this->reg_propagation_op(&info->registers[off+1]);
    this->reg_propagation_op(&info->registers[off+2]);
    this->reg_propagation_op(&info->registers[off+3]);
    return 0x0;
}

int taint_x86::reg_propagation_op_m_8(OFFSET off)
{
    return 0x0;
    this->reg_propagation_op(&this->memory[off+0]);

    this->verify_oob_offset(off, this->mem_length);

    return 0x0;
}

int taint_x86::reg_propagation_op_m_16(OFFSET off)
{
    return 0x0;
    this->reg_propagation_op(&this->memory[off+0]);
    this->reg_propagation_op(&this->memory[off+1]);

    this->verify_oob_offset(off, this->mem_length);

    return 0x0;
}

int taint_x86::reg_propagation_op_m_32(OFFSET off)
{
    return 0x0;
    this->reg_propagation_op(&this->memory[off+0]);
    this->reg_propagation_op(&this->memory[off+1]);
    this->reg_propagation_op(&this->memory[off+2]);
    this->reg_propagation_op(&this->memory[off+3]);

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
//                    this->current_instr_length += 0x1;
                    this->current_instr_length += 0x4;
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
            this->current_instr_length += 0x1;
            switch(modrm_rm_part)
            {
                case RM_EAX_BASED: 
                    rm->offset = this->reg_restore_32(EAX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ECX_BASED: 
                    rm->offset = this->reg_restore_32(ECX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDX_BASED: 
                    rm->offset = this->reg_restore_32(EDX).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EBX_BASED: 
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
                    rm->offset = this->reg_restore_32(EBP).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1); 
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_ESI_BASED: 
                    rm->offset = this->reg_restore_32(ESI).get_DWORD();
                    disp8.from_mem(modrm_byte_ptr+1);
                    //rm->offset += disp8.get_BYTE();
                    rm->region = MODRM_MEM;
                    rm->size   = MODRM_SIZE_32;
                    break;
                case RM_EDI_BASED: 
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
                    /* SIB decoder takes care of displacement */
                    /* SIB byte follows */
                    rm->offset = this->a_decode_sib_mod(modrm_byte_ptr+1, modrm_mod_part);
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
//            this->current_instr_length = 0x2;
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
                    this->reg_propagation_op_r_8(rm->offset);
                    break;
                case MODRM_SIZE_16:
                    this->reg_propagation_op_r_16(rm->offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm->offset);
                    break;
            }
            break;
        case MODRM_MEM:
            switch(rm->size)
            {
                case MODRM_SIZE_8:
                    this->reg_propagation_op_m_8(rm->offset);
                    break;
                case MODRM_SIZE_16:
                    this->reg_propagation_op_m_16(rm->offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm->offset);
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

int taint_x86::r_noop(BYTE_t* b)
{
    d_print(3, "Not implemented: 0x%02x\n", b->get_BYTE());
    return 0x0;
}

int taint_x86::r_jb_jc_jnae(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jb_jc_jnae");
    return 0x0;
}

int taint_x86::r_jae_jnb_jnc(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jae_jnb_jnc");
    return 0x0;
}

int taint_x86::r_je_jz(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_je_jz");
    return 0x0;
}

int taint_x86::r_jne_jnz(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jne_jnz");
    return 0x0;
}

int taint_x86::r_jbe_jna(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jbe_jna");
    return 0x0;
}

int taint_x86::r_ja_jnbe(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_ja_jnbe");
    return 0x0;
}

int taint_x86::r_js(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_js");
    return 0x0;
}

int taint_x86::r_jns(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jns");
    return 0x0;
}

int taint_x86::r_jp_jpe(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jp_jpe");
    return 0x0;
}

int taint_x86::r_jnp_jpo(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jnp_jpo");
    return 0x0;
}

int taint_x86::r_jl_jnge(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jl_jnge");
    return 0x0;
}

int taint_x86::r_jge_jnl(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jge_jnl");
    return 0x0;
}

int taint_x86::r_jle_jng(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jle_jng");
    return 0x0;
}

int taint_x86::r_jg_jnle(BYTE_t* b)
{
    handle_jxx(this->cur_info, "r_jg_jnle");
    return 0x0;
}

int taint_x86::r_jxx(BYTE_t* b)
{
    handle_jxx(this->cur_info, "jxx");
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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 += src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 += src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 += src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 += src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 += src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, src_16);
                    dst_16 += src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_m_32(rm.offset);

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

    this->reg_propagation_op_r_8(AL);
    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_16(AX);
        this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr +1);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 += src_16;

        this->reg_store_16(AX, dst_16);
        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_op_r_32(EAX);
        this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 |= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 |= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 |= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 |= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->reg_store_8(r.offset, dst_8);

            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 |= src_16;
                    this->reg_store_16(r.offset, dst_16);
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    d_print(3, "Before: 0x%08x\n", dst_16.get_WORD());
                    this->restore_16(rm.offset, src_16);
                    dst_16 |= src_16;
                    this->reg_store_16(r.offset, dst_16);
                    d_print(3, "After: 0x%08x\n", dst_16.get_WORD());

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_m_32(rm.offset);

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

    this->reg_propagation_op_r_8(AL);
    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_16(AX);
        this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr +1);
        dst_16 |= src_16;
        this->reg_store_16(AX, dst_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_op_r_32(EAX);
        this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8 = this->reg_restore_8(r.offset);
            dst_8 += (src_8 + this->a_check_cf());
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 += (src_8 + this->a_check_cf());
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, src_16);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_m_32(rm.offset);

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

    this->reg_propagation_op_r_8(AL);
    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_16(AX);
        this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_32(EAX);
        this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8 = (this->reg_restore_8(r.offset) + this->a_check_cf());
            dst_8 -= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16 = (this->reg_restore_16(r.offset) + this->a_check_cf());
                    dst_16 -= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16 = (this->reg_restore_16(r.offset) + this->a_check_cf());
                    dst_16 -= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    src_16 = (this->reg_restore_16(rm.offset) + this->a_check_cf());
                    dst_16 -= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    this->restore_16(rm.offset, src_16);
                    src_16 += this->a_check_cf();
                    dst_16 -= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_m_32(rm.offset);

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

    this->reg_propagation_op_r_8(AL);
    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_16(AX);
        this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_32(EAX);
        this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(r.offset);
            dst_8 &= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

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
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

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
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 &= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 &= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    this->restore_16(rm.offset, src_16);
                    dst_16 &= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_m_32(rm.offset);

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

    this->reg_propagation_op_r_8(AL);
    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_16(AX);
        this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr + this->current_instr_length);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 &= src_16;
        this->reg_store_16(AX, dst_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_op_r_32(EAX);
        this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(r.offset);
            dst_8 -= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 -= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 -= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            old_dst_8 = dst_8;
            src_8 = this->reg_restore_8(rm.offset);
            dst_8 -= src_8;
            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    src_16 = this->reg_restore_16(rm.offset);
                    d_print(3, "Substracting: 0x%08x - 0x%08x\n", dst_16.get_WORD(), src_16.get_WORD());
                    dst_16 -= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    old_dst_16 = dst_16;
                    this->restore_16(rm.offset, src_16);
                    d_print(3, "Substracting: 0x%08x - 0x%08x\n", dst_16.get_WORD(), src_16.get_WORD());
                    dst_16 -= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_m_32(rm.offset);

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

    this->reg_propagation_op_r_8(AL);
    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_16(AX);
        this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_32(EAX);
        this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            this->restore_8(rm.offset, dst_8);
            d_print(3, "Before 0x%08x\n", dst_8.get_BYTE());
            dst_8 ^= reg_restore_8(r.offset);
            d_print(3, "After 0x%08x\n", dst_8.get_BYTE());
            this->store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    src_16 = this->reg_restore_16(r.offset);
                    dst_16 = this->reg_restore_16(rm.offset);
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= src_16;
                    d_print(3, "After 0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);
                    
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    src_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, dst_16);
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= src_16;
                    d_print(3, "After 0x%08x\n", dst_16.get_WORD());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_r_8(rm.offset);

            dst_8 = this->reg_restore_8(r.offset);
            src_8 = reg_restore_8(rm.offset);

            d_print(3, "Before 0x%08x\n", dst_8.get_BYTE());
            dst_8 ^= src_8;
            d_print(3, "After 0x%08x\n", dst_8.get_BYTE());

            this->reg_store_8(r.offset, dst_8);

            this->attach_current_propagation_r_8(r.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_op_r_8(r.offset);
            this->reg_propagation_op_m_8(rm.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 = this->reg_restore_16(r.offset);
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= src_16;
                    d_print(3, "After 0x%08x\n", dst_16.get_WORD());

                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, src_16);
                    dst_16 = this->reg_restore_16(r.offset);
                    d_print(3, "Before 0x%08x\n", dst_16.get_WORD());
                    dst_16 ^= src_16;
                    d_print(3, "After 0x%08x\n", dst_16.get_WORD());
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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

    this->reg_propagation_op_r_8(AL);
    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_16(AX);
        this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

        dst_16 = this->reg_restore_16(AX);
        src_16.from_mem(instr_ptr + this->current_instr_length);
        d_print(3, "Imm16: 0x%08x\n", src_16.get_WORD());
        dst_16 ^= src_16;
        this->reg_store_16(AX, dst_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_op_r_32(EAX);
        this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);

            dst_8 ^= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_m_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);

                    dst_16 ^= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 ^= src_16;
                    this->store_16(rm.offset, dst_16);
                    
                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
        this->reg_propagation_op_r_16(AX);
        a_push_16(reg_restore_16(AX));
    }
    else
    {
        this->reg_propagation_op_r_32(EAX);
        a_push_32(reg_restore_32(EAX));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_cx_ecx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_r_16(CX);
        a_push_16(reg_restore_16(CX));
    }
    else
    {
        this->reg_propagation_op_r_32(ECX);
        a_push_32(reg_restore_32(ECX));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_dx_edx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_r_16(DX);
        a_push_16(reg_restore_16(DX));
    }
    else
    {
        this->reg_propagation_op_r_32(EDX);
        a_push_32(reg_restore_32(EDX));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_bx_ebx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_r_16(BX);
        a_push_16(reg_restore_16(BX));
    }
    else
    {
        this->reg_propagation_op_r_32(EBX);
        a_push_32(reg_restore_32(EBX));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_sp_esp(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_r_16(SP);
        a_push_16(reg_restore_16(SP));
    }
    else
    {
        this->reg_propagation_op_r_32(ESP);
        a_push_32(reg_restore_32(ESP));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_bp_ebp(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_r_16(BP);
        a_push_16(reg_restore_16(BP));
    }
    else
    {
        this->reg_propagation_op_r_32(EBP);
        a_push_32(reg_restore_32(EBP));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_si_esi(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_r_16(SI);
        a_push_16(reg_restore_16(SI));
    }
    else
    {
        this->reg_propagation_op_r_32(ESI);
        a_push_32(reg_restore_32(ESI));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_push_di_edi(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_r_16(DI);
        a_push_16(reg_restore_16(DI));
    }
    else
    {
        this->reg_propagation_op_r_32(EDI);
        a_push_32(reg_restore_32(EDI));
    }

    this->attach_current_propagation_m_32(this->reg_restore_32(ESP).get_DWORD());

    return 0x0;
}

int taint_x86::r_pop_ax_eax(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(AX, this->a_pop_16());
        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EAX, this->a_pop_32());
        this->attach_current_propagation_r_32(EAX);
    }
    return 0x0;
}

int taint_x86::r_pop_cx_ecx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(CX, this->a_pop_16());
        this->attach_current_propagation_r_16(CX);
    }
    else
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ECX, this->a_pop_32());
        this->attach_current_propagation_r_32(ECX);
    }
    return 0x0;
}

int taint_x86::r_pop_dx_edx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(DX, this->a_pop_16());
        this->attach_current_propagation_r_16(DX);
    }
    else
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EDX, this->a_pop_32());
        this->attach_current_propagation_r_32(EDX);
    }
    return 0x0;
}

int taint_x86::r_pop_bx_ebx(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(BX, this->a_pop_16());
        this->attach_current_propagation_r_16(BX);
    }
    else
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EBX, this->a_pop_32());
        this->attach_current_propagation_r_32(EBX);
    }
    return 0x0;
}

int taint_x86::r_pop_sp_esp(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(SP, this->a_pop_16());
        this->attach_current_propagation_r_16(SP);
    }
    else
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ESP, this->a_pop_32());
        this->attach_current_propagation_r_32(ESP);
    }
    return 0x0;
}

int taint_x86::r_pop_bp_ebp(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(BP, this->a_pop_16());
        this->attach_current_propagation_r_16(BP);
    }
    else
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EBP, this->a_pop_32());
        this->attach_current_propagation_r_32(EBP);
    }
    return 0x0;
}

int taint_x86::r_pop_si_esi(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(SI, this->a_pop_16());
        this->attach_current_propagation_r_16(SI);
    }
    else
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ESI, this->a_pop_32());
        this->attach_current_propagation_r_32(ESI);
    }
    return 0x0;
}

int taint_x86::r_pop_di_edi(BYTE_t* args)
{
    if(this->current_prefixes & PREFIX_OPER_SIZE_OVERRIDE)
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(DI, this->a_pop_16());
        this->attach_current_propagation_r_16(DI);
    }
    else
    {
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
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
        this->reg_propagation_op_r_16(AX);
        this->reg_propagation_op_r_16(CX);
        this->reg_propagation_op_r_16(DX);
        this->reg_propagation_op_r_16(BX);
        this->reg_propagation_op_r_16(SP);
        this->reg_propagation_op_r_16(BP);
        this->reg_propagation_op_r_16(SI);
        this->reg_propagation_op_r_16(DI);

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
        this->reg_propagation_op_r_32(EAX);
        this->reg_propagation_op_r_32(ECX);
        this->reg_propagation_op_r_32(EDX);
        this->reg_propagation_op_r_32(EBX);
        this->reg_propagation_op_r_32(ESP);
        this->reg_propagation_op_r_32(EBP);
        this->reg_propagation_op_r_32(ESI);
        this->reg_propagation_op_r_32(EDI);

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
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(DI, this->a_pop_16());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(SI, this->a_pop_16());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(BP, this->a_pop_16());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        this->a_pop_16();
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(BX, this->a_pop_16());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(DX, this->a_pop_16());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_16(CX, this->a_pop_16());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
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
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EDI, this->a_pop_32());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ESI, this->a_pop_32());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EBP, this->a_pop_32());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        this->a_pop_32();
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EBX, this->a_pop_32());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(EDX, this->a_pop_32());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
        reg_store_32(ECX, this->a_pop_32());
        this->reg_propagation_op_m_32(this->reg_restore_32(ESP).get_DWORD());
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
        this->reg_propagation_op_m_16(src_16_32);
        this->restore_16(src_16_32, val_16);
        a_push_16(val_16);
    }
    else
    {
        this->reg_propagation_op_m_32(src_16_32);
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

    this->reg_propagation_op_m_8(src_8);
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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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

    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

    imm_8.from_mem(instr_ptr + this->current_instr_length);
    src_16 = imm_8;
    src_32 = imm_8;

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_op_r_16(rm.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);

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

    this->reg_propagation_op_r_8(r.offset);

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
                this->reg_propagation_op_r_16(r.offset);

                temp_16 = this->reg_restore_16(r.offset);
                d_print(3, "Read 16: 0x%08x from reg: %02x\n", temp_16.get_WORD(), r.offset);
                this->reg_store_16(rm.offset, temp_16);

                this->attach_current_propagation_r_16(rm.offset);
            }
            else
            {
                this->reg_propagation_op_r_32(r.offset);

                temp_32 = this->reg_restore_32(r.offset);
                d_print(3, "Read 32: 0x%08x\n", temp_32.get_DWORD());
                this->reg_store_32(rm.offset, temp_32);

                this->attach_current_propagation_r_32(rm.offset);
            }
            break;
        case MODRM_MEM:
            if(rm.size == MODRM_SIZE_16)
            {
                this->reg_propagation_op_r_16(r.offset);

                temp_16 = this->reg_restore_16(r.offset);
                d_print(3, "Read 16: 0x%08x from reg: %02x\n", temp_16.get_WORD(), r.offset);
                this->store_16(rm.offset, temp_16);

                this->attach_current_propagation_m_16(rm.offset);
            }
            else
            {
                this->reg_propagation_op_r_32(r.offset);

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
            this->reg_propagation_op_r_8(rm.offset);
            temp = this->reg_restore_8(rm.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_op_m_8(rm.offset);
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
                this->reg_propagation_op_r_16(rm.offset);

                temp_16 = this->reg_restore_16(rm.offset);
                d_print(3, "Read: 0x%08x\n", temp_16.get_WORD());
                this->reg_store_16(r.offset, temp_16);
                
                this->attach_current_propagation_r_16(r.offset);
            }
            else
            {
                this->reg_propagation_op_r_32(rm.offset);

                temp_32 = this->reg_restore_32(rm.offset);
                d_print(3, "Read: 0x%08x\n", temp_32.get_DWORD());
                this->reg_store_32(r.offset, temp_32);
                
                this->attach_current_propagation_r_32(r.offset);
            }
            break;
        case MODRM_MEM:
            if(rm.size == MODRM_SIZE_16)
            {
                this->reg_propagation_op_m_16(rm.offset);

                this->restore_16(rm.offset, temp_16);
                d_print(3, "Read: 0x%08x\n", temp_16.get_WORD());
                this->reg_store_16(r.offset, temp_16);
                
                this->attach_current_propagation_r_16(r.offset);
            }
            else
            {
                this->reg_propagation_op_m_32(rm.offset);

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
            this->reg_propagation_op_m_16(0x0); /* how to solve this? */
            dst_16 = rm.offset;
            this->reg_store_16(r.offset, dst_16);
            this->attach_current_propagation_r_16(r.offset);
            break;
        case MODRM_SIZE_32:
            this->reg_propagation_op_m_32(0x0);
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

    this->reg_propagation_op_m_8(off.get_DWORD());

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
        this->reg_propagation_op_m_16(off.get_DWORD());

        this->reg_store_16(AX, dst_16);
        this->attach_current_propagation_r_16(AX);

    }
    else
    {
        this->restore_32(off, dst_32);
        this->reg_propagation_op_m_32(off.get_DWORD());

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

    this->reg_propagation_op_r_8(AL);

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
        this->reg_propagation_op_r_16(AX);

        dst_16 = this->reg_restore_16(AX);
        this->store_16(off, dst_16);

        this->attach_current_propagation_m_16(off.get_DWORD());
    }
    else
    {
        this->reg_propagation_op_r_32(EAX);

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

    this->reg_propagation_op_m_8(off_src.get_DWORD());

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
        this->reg_propagation_op_m_16(off_src.get_DWORD());

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
        this->reg_propagation_op_m_32(off_src.get_DWORD());

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

    this->reg_propagation_op_m_8(offset);

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

    this->reg_propagation_op_m_8(offset);

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

    this->reg_propagation_op_m_8(offset);

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

    this->reg_propagation_op_m_8(offset);

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

    this->reg_propagation_op_m_8(offset);

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

    this->reg_propagation_op_m_8(offset);

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

    this->reg_propagation_op_m_8(offset);

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

    this->reg_propagation_op_m_8(offset);

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
        this->reg_propagation_op_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(AX, src_16);

        this->attach_current_propagation_r_16(AX);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_op_m_32(offset);

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
        this->reg_propagation_op_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(CX, src_16);

        this->attach_current_propagation_r_16(CX);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_op_m_32(offset);

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
        this->reg_propagation_op_m_16(offset);

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
        this->reg_propagation_op_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(BX, src_16);

        this->attach_current_propagation_r_16(BX);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_op_m_32(offset);

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
        this->reg_propagation_op_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(SP, src_16);

        this->attach_current_propagation_r_16(SP);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_op_m_32(offset);

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
        this->reg_propagation_op_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(BP, src_16);

        this->attach_current_propagation_r_16(BP);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_op_m_32(offset);

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
        this->reg_propagation_op_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(SI, src_16);

        this->attach_current_propagation_r_16(SI);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_op_m_32(offset);

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
        this->reg_propagation_op_m_16(offset);

        src_16.from_mem(&this->memory[offset]);
        this->reg_store_16(DI, src_16);

        this->attach_current_propagation_r_16(DI);
    }
    else
    {
        offset = this->reg_restore_32(EIP).get_DWORD() + 0x1;
        this->reg_propagation_op_m_32(offset);

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
    d_print(3, "retn\n");

    if(this->started && !this->finished)
        this->cur_info->returning = 0x3;

    /*
    if(this->ctx_info[this->tids[this->cur_tid]].call_level <= 0x0)
        this->handle_ret(this->cur_info);
    */
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
    this->current_instr_is_jump = 0x1;

    return 0x0;

}

int taint_x86::r_ret(BYTE_t*)
{
    d_print(3, "ret\n");

    if(this->started && !this->finished)
        this->cur_info->returning = 0x3;

    /*
    if(this->ctx_info[this->tids[this->cur_tid]].call_level <= 0x0)
        this->handle_ret(this->cur_info);
    */
    DWORD_t ret;
    ret = this->a_pop_32();
    d_print(3, "Will return to: 0x%08x\n", ret.get_DWORD());

    this->reg_store_32(EIP, ret);
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
    d_print(1, "In call\n");
    if(this->started && !this->finished)
    {
        this->cur_info->target = target_2.get_DWORD();
        this->cur_info->next = ret_addr.get_DWORD();
        this->cur_info->calling = 1;
        cur_info->source = current_eip;
        d_print(1, "Next call source = 0x%08x\n", current_eip);
    }

    d_print(3, "ret_addr: 0x%08x, target: 0x%08x, target2: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD(), target_2.get_DWORD());

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

    if(this->started && !this->finished)
    {
        cur_ctx->target = target_2.get_DWORD();
        this->handle_jmp(cur_ctx);
    }

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

    if(this->started && !this->finished)
    {
        cur_ctx->target = target.get_DWORD();
        this->handle_jmp(cur_ctx);
    }

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
                    this->reg_propagation_op_r_16(rm.offset);
                    src_16 = this->reg_restore_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    src_32 = this->reg_restore_32(rm.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->reg_propagation_op_m_16(rm.offset);
                    this->restore_16(rm.offset, src_16);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
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

    ret_addr = this->reg_restore_32(EIP);
    ret_addr += 0x5;

    a_push_32(ret_addr);
    
    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());
    this->reg_store_32(EIP, target);

    this->attach_current_propagation_r_32(EIP);

    this->current_instr_is_jump = 0x1;

    if(this->started && !this->finished)
    {
        cur_ctx->target = target.get_DWORD();
        this->handle_jmp(cur_ctx);
    }

    handle_jxx(this->cur_info, "r_jmp_rm (switch/case)");

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
            this->reg_propagation_op_r_8(rm.offset);
            dst_8 = this->reg_restore_8(rm.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_op_m_8(rm.offset);
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
            this->reg_propagation_op_r_16(rm.offset);
            break;
        case MODRM_MEM:
            this->restore_16(rm.offset, dst_16);
            this->reg_propagation_op_m_16(rm.offset);
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
            this->reg_propagation_op_r_8(rm.offset);
            dst_8 = this->reg_restore_8(rm.offset);
            break;
        case MODRM_MEM:
            this->reg_propagation_op_m_8(rm.offset);
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
            this->reg_propagation_op_r_16(rm.offset);
            break;
        case MODRM_MEM:
            this->restore_16(rm.offset, dst_16);
            this->reg_propagation_op_m_16(rm.offset);
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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_r_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    src_16 = this->reg_restore_16(rm.offset);
                    dst_16 *= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(r.offset);
                    this->reg_propagation_op_m_16(rm.offset);

                    dst_16 = this->reg_restore_16(r.offset);
                    this->restore_16(rm.offset, src_16);
                    dst_16 *= src_16;
                    this->reg_store_16(r.offset, dst_16);

                    this->attach_current_propagation_r_16(r.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(r.offset);
                    this->reg_propagation_op_m_32(rm.offset);

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
            this->reg_propagation_op_r_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 += src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_m_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 |= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_m_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 += (src_8 + this->a_check_cf());
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_m_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            src_8 += this->a_check_cf();
            dst_8 -= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_m_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 &= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_m_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
            this->reg_propagation_op_r_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

            dst_8 = this->reg_restore_8(rm.offset);
            old_dst_8 = dst_8;
            src_8.from_mem(instr_ptr + this->current_instr_length);
            dst_8 -= src_8;
            this->reg_store_8(rm.offset, dst_8);

            this->attach_current_propagation_r_8(rm.offset);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_m_8(rm.offset);
            this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 += src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 += src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 |= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 |= src_16;
                    this->store_16(rm.offset, dst_16);
                    
                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 += (src_16 + this->a_check_cf());
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 &= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 &= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    dst_16 = this->reg_restore_16(rm.offset);
                    old_dst_16 = dst_16;
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 -= src_16;
                    this->reg_store_16(rm.offset, dst_16);

                    this->attach_current_propagation_r_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

                    this->restore_16(rm.offset, dst_16);
                    old_dst_16 = dst_16;
                    src_16.from_mem(instr_ptr + this->current_instr_length);
                    dst_16 -= src_16;
                    this->store_16(rm.offset, dst_16);

                    this->attach_current_propagation_m_16(rm.offset);

                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

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
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

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
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
                    this->reg_propagation_op_r_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

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
                    this->reg_propagation_op_r_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
                    this->reg_propagation_op_m_16(rm.offset);
                    this->reg_propagation_op_r_16(r.offset);

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
                    this->reg_propagation_op_m_32(rm.offset);
                    this->reg_propagation_op_r_32(r.offset);

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
//                    this->reg_propagation_op_r_16(rm.offset);
//                    this->reg_propagation_op_r_16(r.offset);

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
//                    this->reg_propagation_op_r_32(rm.offset);
//                    this->reg_propagation_op_r_32(r.offset);

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
//                    this->reg_propagation_op_m_16(rm.offset);
//                    this->reg_propagation_op_r_16(r.offset);

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
//                    this->reg_propagation_op_m_32(rm.offset);
//                    this->reg_propagation_op_r_32(r.offset);

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
//                    this->reg_propagation_op_r_16(rm.offset);
//                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
//                    this->reg_propagation_op_r_32(rm.offset);
//                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
//                    this->reg_propagation_op_r_16(rm.offset);
//                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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
//                    this->reg_propagation_op_r_32(rm.offset);
//                    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);

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

    this->reg_propagation_op_m_8(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
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
                    this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
                    src_16.from_mem(instr_ptr +this->current_instr_length); 
                    this->reg_store_16(rm.offset, src_16);
                    this->attach_current_propagation_r_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
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
                    this->reg_propagation_op_m_16(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
                    src_16.from_mem(instr_ptr +this->current_instr_length); 
                    this->store_16(rm.offset, src_16);
                    this->attach_current_propagation_m_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    this->reg_propagation_op_m_32(this->reg_restore_32(EIP).get_DWORD() + this->current_instr_length);
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
            this->reg_propagation_op_r_8(AL);
            this->reg_propagation_op_r_8(rm.offset);

            al = this->reg_restore_8(AL);
            dst_8 = this->reg_restore_8(rm.offset);

            res = this->a_f_mul_8(al, dst_8);
            ax.set_WORD(res);

            this->reg_store_16(AX, ax);

            this->attach_current_propagation_r_16(AX);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(AL);
            this->reg_propagation_op_m_8(rm.offset);

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
            this->reg_propagation_op_r_8(AL);
            this->reg_propagation_op_r_8(rm.offset);

            al = this->reg_restore_8(AL);
            dst_8 = this->reg_restore_8(rm.offset);

            res = this->a_f_imul_8(al, dst_8);
            ax.set_WORD(res);

            this->reg_store_16(AX, ax);

            this->attach_current_propagation_r_16(AX);
            break;
        case (MODRM_MEM):
            this->reg_propagation_op_r_8(AL);
            this->reg_propagation_op_m_8(rm.offset);

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

    //this->handle_call(this->cur_info, target.get_DWORD(), ret_addr.get_DWORD());
    d_print(1, "In call\n");
    if(this->started && !this->finished)
    {
        this->cur_info->target = target.get_DWORD();
        this->cur_info->next = ret_addr.get_DWORD();
        this->cur_info->calling = 1;
        cur_info->source = current_eip;
        d_print(1, "Next call source = 0x%08x\n", current_eip);
    }

    a_push_32(ret_addr);
    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());
    
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

    //this->handle_call(this->cur_info, target.get_DWORD(), ret_addr.get_DWORD());
    d_print(1, "In call\n");
    if(this->started && !this->finished)
    {
        this->cur_info->target = target.get_DWORD();
        this->cur_info->next = ret_addr.get_DWORD();
        this->cur_info->calling = 1;
        cur_info->source = this->current_eip;
        d_print(1, "Next call source = 0x%08x\n", current_eip);
    }

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
                    this->reg_propagation_op_r_16(AX);
                    this->reg_propagation_op_r_16(rm.offset);

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
                    this->reg_propagation_op_r_32(AX);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(AX);
                    this->reg_propagation_op_m_16(rm.offset);

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
                    this->reg_propagation_op_r_32(AX);
                    this->reg_propagation_op_m_32(rm.offset);

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
                    this->reg_propagation_op_r_16(AX);
                    this->reg_propagation_op_r_16(rm.offset);

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
                    this->reg_propagation_op_r_32(AX);
                    this->reg_propagation_op_r_32(rm.offset);

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
                    this->reg_propagation_op_r_16(AX);
                    this->reg_propagation_op_m_16(rm.offset);

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
                    this->reg_propagation_op_r_32(AX);
                    this->reg_propagation_op_m_32(rm.offset);

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
    for(OFFSET i=start; i<start+len; i++)
    {
        d_err_print("0x%x: 0x%x\n", i, this->memory[i].get_BYTE());
    }
    d_err_print("\n");
}

int taint_x86::print_err_t_mem(OFFSET start, DWORD len)
{
    for(OFFSET i=start; i<start+len; i++)
    {
        d_err_print("0x%x: 0x%x\n", i, this->memory[i].get_BYTE_t());
    }
    d_err_print("\n");
}


int taint_x86::print_err_stack(DWORD tid, DWORD len)
{
    OFFSET start;
    OFFSET addr;
    DWORD_t temp;
    len *= 0x4;

    start = this->reg_restore_32(ESP, tid).get_DWORD();

    d_err_print("Context 0x%08x:\n", tid);
    for(OFFSET i=start; i<start+len; i+=0x4)
    {
        temp.from_mem(&this->memory[i]);
        d_err_print("0x%08x: 0x%08x\n", i, temp.get_DWORD());
    }
    d_err_print("\n");
}

int taint_x86::print_err_t_stack(DWORD tid, DWORD len)
{
    OFFSET start;
    OFFSET addr;
    DWORD_t temp;
    len *= 0x4;

    start = this->reg_restore_32(ESP, tid).get_DWORD();

    d_err_print("Context 0x%08x:\n", tid);
    for(OFFSET i=start; i<start+len; i+=0x4)
    {
        temp.from_mem(&this->memory[i]);
        d_err_print("0x%08x: 0x%08x\n", i, temp.get_DWORD_t());
    }
    d_err_print("\n");
}

int taint_x86::print_stack(DWORD tid, int level, DWORD len)
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
        d_err_print("No such context: 0x%08x\n", tid);
        return 0x0;
    }
    d_err_print("\nWARNING!!! EIP is one instruction late!\n");
    d_err_print("Context 0x%08x:\n", tid);
    d_err_print("EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_DWORD());
    d_err_print("EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_DWORD());
    d_err_print("ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_DWORD());
    d_err_print("EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_DWORD());
    d_err_print("ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_DWORD());
    d_err_print("EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_DWORD());
    d_err_print("EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_DWORD());
    d_err_print("ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_DWORD());
    d_err_print("EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_DWORD());
    d_err_print("EFL: 0x%08x\n", this->reg_restore_32(EFLAGS, tid).get_DWORD());
    d_err_print("\n");

    DWORD eflags;
    eflags = this->reg_restore_32(EFLAGS).get_DWORD();
    if(eflags & EFLAGS_CF) d_err_print("CF\n");
    if(eflags & EFLAGS_PF) d_err_print("PF\n");
    if(eflags & EFLAGS_AF) d_err_print("AF\n");
    if(eflags & EFLAGS_ZF) d_err_print("ZF\n");
    if(eflags & EFLAGS_SF) d_err_print("SF\n");
    if(eflags & EFLAGS_IF) d_err_print("IF\n");
    if(eflags & EFLAGS_DF) d_err_print("DF\n");
    if(eflags & EFLAGS_OF) d_err_print("OF\n");
    d_err_print("\n");
}

int taint_x86::print_err_t_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        d_err_print("No such context: 0x%08x\n", tid);
        return 0x0;
    }
    d_err_print("Taint context 0x%08x:\n", tid);
    d_err_print("EAX: 0x%08x\n", this->reg_restore_32(EAX, tid).get_DWORD_t());
    d_err_print("EBX: 0x%08x\n", this->reg_restore_32(EBX, tid).get_DWORD_t());
    d_err_print("ECX: 0x%08x\n", this->reg_restore_32(ECX, tid).get_DWORD_t());
    d_err_print("EDX: 0x%08x\n", this->reg_restore_32(EDX, tid).get_DWORD_t());
    d_err_print("ESI: 0x%08x\n", this->reg_restore_32(ESI, tid).get_DWORD_t());
    d_err_print("EDI: 0x%08x\n", this->reg_restore_32(EDI, tid).get_DWORD_t());
    d_err_print("EBP: 0x%08x\n", this->reg_restore_32(EBP, tid).get_DWORD_t());
    d_err_print("ESP: 0x%08x\n", this->reg_restore_32(ESP, tid).get_DWORD_t());
    d_err_print("EIP: 0x%08x\n", this->reg_restore_32(EIP, tid).get_DWORD_t());
    d_err_print("EFL: 0x%08x\n", this->reg_restore_32(EFLAGS, tid).get_DWORD_t());
    d_err_print("\n");

    DWORD eflags;
    eflags = this->reg_restore_32(EFLAGS).get_DWORD_t();
    if(eflags & EFLAGS_CF) d_err_print("CF\n");
    if(eflags & EFLAGS_PF) d_err_print("PF\n");
    if(eflags & EFLAGS_AF) d_err_print("AF\n");
    if(eflags & EFLAGS_ZF) d_err_print("ZF\n");
    if(eflags & EFLAGS_SF) d_err_print("SF\n");
    if(eflags & EFLAGS_IF) d_err_print("IF\n");
    if(eflags & EFLAGS_DF) d_err_print("DF\n");
    if(eflags & EFLAGS_OF) d_err_print("OF\n");
    d_err_print("\n");
}

int taint_x86::print_context(int tid)
{
    if(!this->already_added(tid)) 
    {
        d_print(1, "No such context: 0x%08x\n", tid);
        return 0x0;
    }
    d_print(1, "\nWARNING!!! EIP is one instruction late!\n");
    d_print(1, "Context 0x%08x:\n", tid);
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

int taint_x86::write_history(FILE* f)
{
    unsigned i;
    /*
    for(i=0x0; i<this->current_propagation_count; i++) 
    {
        
    }
    */
    return 0x0;
}

int taint_x86::dump_cmd(char* string)
{
    char* cur_str;
    DWORD tid;
    unsigned i, len;
    EXCEPTION_INFO info;
    unsigned off;

    if(string[0] == 'd' && string[1] == 'c' && string[2] == 't')
    {
        cur_str = string + 3;

        if(strlen(cur_str) == 0x0) tid = this->ctx_info[0x0].tid;
        else tid = strtol(cur_str, 0x0, 0x10);

        this->print_err_t_context(tid);
    }
    else if(string[0] == 'd' && string[1] == 'c')
    {
        cur_str = string + 2;

        if(strlen(cur_str) == 0x0) tid = this->ctx_info[0x0].tid;
        else tid = strtol(cur_str, 0x0, 0x10);

        this->print_err_context(tid);
    }
    if(string[0] == 'd' && string[1] == 's' && string[2] == 't')
    {
        cur_str = string + 3;
        if(strlen(cur_str) == 0x0) tid = this->ctx_info[0x0].tid;
        else tid = strtol(strtok(cur_str, ","), 0x0, 0x10);

        /*
        len = strtol(strtok(0x0, ","), 0x0, 0x10);
        if(len == 0x0) 
        */
        len = 0x10;

        this->print_err_t_stack(tid, len);
    }
    else if(string[0] == 'd' && string[1] == 's')
    {
        cur_str = string + 2;
        if(strlen(cur_str) == 0x0) tid = this->ctx_info[0x0].tid;
        else tid = strtol(strtok(cur_str, ","), 0x0, 0x10);

        /*
        len = strtol(strtok(0x0, ","), 0x0, 0x10);
        if(len == 0x0) 
        */
        len = 0x10;

        this->print_err_stack(tid, len);
    }
    if(string[0] == 'd' && string[1] == 'e')
    {
        d_err_print("There have been %d exceptions: \n", this->exceptions_count);
        for(i = 0x0; i< this->exceptions_count; i++)
        {
            info = this->exceptions[i];
            d_err_print("Exception %08x in TID %08x, instr. no: %d, eip: 0x%08x\n", info.er.ExceptionCode, info.tid, this->current_instr_count, info.er.ExceptionAddress);
        }
    }
    if(string[0] == 'd' && string[1] == 'h')
    {
        d_err_print("There are %d threads: \n", this->tid_count);
        for(i = 0x0; i< this->tid_count; i++)
        {
            d_err_print("0x%08x\n", this->ctx_info[i].tid);
        }
    
    }
    if(string[0] == 'd' && string[1] == 'm' && string[2] == 't')
    {
        cur_str = string + 3;

        if(strlen(cur_str) == 0x0) off = 0x0;
        else off = strtol(cur_str, 0x0, 0x10);

        d_err_print("Memory taint dump @ 0x%08x:\n", off);

        this->print_err_t_mem(off, 0x10);
    }
    else if(string[0] == 'd' && string[1] == 'm')
    {
        cur_str = string + 2;

        if(strlen(cur_str) == 0x0) off = 0x0;
        else off = strtol(cur_str, 0x0, 0x10);

        d_err_print("Memory dump @ 0x%08x:\n", off);

        this->print_err_mem(off, 0x10);
    }
    if(string[0] == 'd' && string[1] == 'r')
    {
        d_err_print("Current status:\n");
        d_err_print("Current EIP: \t\t0x%08x\n", this->current_eip);
        d_err_print("Current instr. no: \t%d\n", this->current_instr_count);
    }
    if(string[0] == 'd' && string[1] == 't')
    {
        d_err_print("There have been %d taints:\n", this->taint_count);
        for(i = 0x0; i< this->taint_count; i++)
        {
            d_err_print("Taint no %d: 0x%08x, 0x%08x\n", i, this->taints[i].off, this->taints[i].size);
        }
    }

    return 0x0;
}

int taint_x86::prompt_taint()
{
    TRACE_WATCHPOINT twp;
    char command[MAX_NAME];

    while(1)
    {
        d_err_print("\n---\n");
        d_err_print("Enter taint query and press [ENTER]\n> ");
        scanf("%s", command);
        if(strcmp(command, "q") == 0x0) break;
        if(command[0] == 'd')
        {
            this->dump_cmd(command);
            continue;
        }

        twp = this->parse_trace_string(command);
        this->query_history(twp);
    }
    return 0x0;
}

TRACE_WATCHPOINT taint_x86::parse_trace_string(char* string)
{
    TRACE_WATCHPOINT twp;
    DWORD ctx_index;
    
    d_print(1, "Parsing: %s\n", string);

    char* current;

    current = strtok(string, ",");
    if(!current)
    {
        d_print(1, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    twp.offset = strtol(current, 0x0, 10);
    d_print(1, "Instr count: %lld\n", twp.offset);

    current = strtok(0x0, ",");
    if(!current)
    {
        d_print(1, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    twp.mem_offset = strtol(current, 0x0, 0x10);
    d_print(1, "Offset: 0x%08x\n", twp.mem_offset);

    current = strtok(0x0, ",");
    if(!current)
    {
        d_print(1, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    twp.tid = strtol(current, 0x0, 0x10);
    d_print(1, "TID: 0x%08x\n", twp.tid);

    ctx_index = this->tids[twp.tid];

    current = strtok(0x0, "+");
    if(!current)
    {
        d_print(1, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    strcpy(twp.name, current);
    d_print(1, "%s\n", twp.name);

    if(strstr(twp.name, "EAX") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[EAX]);
    }
    else if(strstr(twp.name, "EBX") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[EBX]);
    }
    else if(strstr(twp.name, "ECX") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[ECX]);
    }
    else if(strstr(twp.name, "EDX") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[EDX]);
    }
    else if(strstr(twp.name, "ESI") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[ESI]);
    }
    else if(strstr(twp.name, "EDI") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[EDI]);
    }
    else if(strstr(twp.name, "EBP") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[EBP]);
    }
    else if(strstr(twp.name, "ESP") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[ESP]);
    }
    else if(strstr(twp.name, "EIP") != 0x0)
    {
        twp.watched = &(this->ctx_info[this->tids[twp.tid]].registers[EIP]);
    }
    else if(strtol(twp.name, 0x0, 0x10) != 0x0)
    {
        twp.watched = &(this->memory[strtol(twp.name, 0x0, 0x10)]);
    }

    d_err_print("0x%08x\n", twp.watched);

    return twp;
}

int taint_x86::query_history(TRACE_WATCHPOINT twp)
{
    d_err_print("Printing history for instruction count: %lld\n", twp.offset);
    return this->print_taint_history(twp.watched, twp.offset);
}
