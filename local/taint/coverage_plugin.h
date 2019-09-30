#ifndef COVERAGE_PLUGIN_H
#define COVERAGE_PLUGIN_H

#include <emul_x86.h>
#include <debug.h>

typedef struct _LIBRARY
{
    OFFSET offset;
    char name[MAX_NAME];
    char path[MAX_NAME];
    unsigned size;
    DWORD length;
    DWORD loaded;
    char* content;
    unsigned instructions_touched;

    char blacklisted;
} LIBRARY;

class coverage_plugin : Plugin
{
    public:

    /* new implementation */
    virtual int pre_execute_instruction_callback(DWORD);
    virtual int post_execute_instruction_callback(DWORD);
    virtual int start_callback();
    virtual int finish_callback();
    virtual int breakpoint_callback(BREAKPOINT*);
    virtual int add_thread_callback(CONTEXT_OUT);
    virtual int del_thread_callback(DWORD);
    virtual int del_thread_srsly_callback(DWORD);
    virtual int parse_option(char*);
    virtual int handle_sigint();
    virtual int handle_exception_callback(EXCEPTION_INFO);

    /* libs for localizators */
    LIBRARY libs[MAX_LIB_COUNT];
    unsigned libs_count;
    int add_lib(OFFSET, unsigned, char*);
    int del_lib(OFFSET);
    LIBRARY* get_lib(OFFSET);
    OFFSET find_lib(char*);
    int register_lib(char* line);
    int deregister_lib(char* line);

    coverage_plugin()
    {
        printf("Initializing coverage_plugin\n");
        printf("Registering routine callbacks\n");

    }

    ~coverage_plugin() 
    {
        unsigned i, j;
        LIBRARY* cur_lib;
        FILE* f;
        char fname[MAX_NAME]; 

        for(i=0x0; i < libs_count; i++)
        {
            cur_lib = &this->libs[i];
            free(cur_lib->content);
        }
    }

};

#endif
