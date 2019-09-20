#include <coverage_plugin.h>
#include <emul_x86.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <debug.h>
#include <plugin.h>
#include <utils.h>
#include <out_utils.h>

#include <limits.h>

int coverage_plugin::pre_execute_instruction_callback(DWORD eip)
{
    return 0x0;
}

int coverage_plugin::post_execute_instruction_callback(DWORD eip)
{
    return 0x0;
}

int coverage_plugin::breakpoint_callback(BREAKPOINT* bp)
{
    return 0x0;
}

int coverage_plugin::start_callback()
{
    return 0x0;
}

int coverage_plugin::finish_callback()
{
    return 0x0;    
}

int coverage_plugin::add_thread_callback(CONTEXT_OUT ctx_out)
{
    return 0x0;
}

int coverage_plugin::del_thread_callback(DWORD tid)
{
    return 0x0;
}

int coverage_plugin::del_thread_srsly_callback(DWORD tid)
{
    return 0x0;
}

int coverage_plugin::handle_sigint()
{
    return 0x0;
}

int coverage_plugin::handle_exception_callback(EXCEPTION_INFO info)
{
    return 0x0;
}

int coverage_plugin::add_lib(OFFSET off, unsigned size, char* name)
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
    new_lib.size = size;

    new_lib.loaded = 1;

    this->libs[libs_count] = new_lib;
    this->libs_count++;

    d_print(1, "Loaded lib: %s at 0x%08x to 0x%08x\n", new_lib.path, new_lib.offset, new_lib.offset+new_lib.length);
    return 0x0;
}

LIBRARY* coverage_plugin::get_lib(OFFSET offset)
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

OFFSET coverage_plugin::find_lib(char* name)
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

int coverage_plugin::del_lib(OFFSET off)
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
int coverage_plugin::register_lib(char* line)
{
    char* cmd;
    OFFSET off, size;
    char* name;

    cmd = strtok(line, ",");
    off = strtoul(strtok(0x0, ","), 0x0, 0x10);
    size = strtoul(strtok(0x0, ","), 0x0, 0x10);
    name = strtok(0x0, ",");

    this->add_lib(off, size, name);

    return 0x0;
}

/* plugin czy nie? */
int coverage_plugin::deregister_lib(char* line)
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

int coverage_plugin::parse_option(char* line)
{
    if(line[0] == 'R' && line[1] == 'L')
        this->register_lib(line);
    
    if(line[0] == 'D' && line[1] == 'L')
        this->deregister_lib(line);
            
    return 0x0;
}

