#ifndef PLUGIN_H
#define PLUGIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <breakpoint.h>
#include <emul_x86.h>

class taint_x86;

class Plugin
{
    public:
    taint_x86* taint_eng;

    virtual int pre_execute_instruction_callback(DWORD) = 0;
    virtual int post_execute_instruction_callback(DWORD) = 0;

    virtual int start_callback() = 0;
    virtual int finish_callback() = 0;

    virtual int breakpoint_callback(BREAKPOINT*) = 0;

    virtual int add_thread_callback(CONTEXT_OUT) = 0;
    virtual int del_thread_callback(DWORD) = 0;
    virtual int del_thread_srsly_callback(DWORD) = 0;

    virtual int handle_exception_callback(EXCEPTION_INFO) = 0;
    virtual int handle_sigint() = 0;

    virtual int parse_option(char*) = 0;

    int set_taint_eng(taint_x86*);

    typedef int (Plugin::*instruction_routine)(BYTE_t* args);

    instruction_routine instructions_32_start[0x100];
    instruction_routine instructions_32_end[0x100];

    instruction_routine instructions_32_extended_start[0x100];
    instruction_routine instructions_32_extended_end[0x100];

    int r_noop(BYTE_t*);

    Plugin() 
    {
        printf("Initializing plugin\n");

        memset(this->instructions_32_start, 0x0, sizeof(this->instructions_32_start));
        memset(this->instructions_32_end, 0x0, sizeof(this->instructions_32_end));
        memset(this->instructions_32_extended_start, 0x0, sizeof(this->instructions_32_extended_start));
        memset(this->instructions_32_extended_end, 0x0, sizeof(this->instructions_32_extended_end));

        /* too slow
        // fill the rest with noops
        for(unsigned int i = 0x0; i < 0x100; i++)
        {
            this->instructions_32_start[i] = &Plugin::r_noop;
            this->instructions_32_end[i] = &Plugin::r_noop;
            this->instructions_32_extended_start[i] = &Plugin::r_noop;
            this->instructions_32_extended_end[i] = &Plugin::r_noop;
        }
        */
        // fill the rest with zeroes
        for(unsigned int i = 0x0; i < 0x100; i++)
        {
            this->instructions_32_start[i] = 0x0;
            this->instructions_32_end[i] = 0x0;
            this->instructions_32_extended_start[i] = 0x0;
            this->instructions_32_extended_end[i] = 0x0;
        }

    }
    
    ~Plugin() 
    {
    }
};

#endif
