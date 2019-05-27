#include <taint_plugin.h>
#include <taint_emul_x86.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <debug.h>
#include <plugin.h>
#include <utils.h>

#define GRAPH_OFF 11
#define CODE_BLACK  0x0
#define CODE_BLUE   0x1
#define CODE_GREEN  0x2
#define CODE_RED    0x3
#define CODE_COMMENT 0x4

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

