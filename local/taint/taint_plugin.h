#ifndef TAINT_PLUGIN_H
#define TAINT_PLUGIN_H

#define MAX_TAINTS_OBSERVED 0x400
#define MAX_EXCEPTIONS_COUNT 0x10000
#define MAX_BREAKPOINTS 0x10
#define MAX_ 0x10
#define MAX_OUT_TAB 0x5

#include <emul_x86.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef struct LOCATION_DESCRIPTOR_
{
    struct LOCATION_DESCRIPTOR_* a1;
    struct LOCATION_DESCRIPTOR_* a2;
    char op[MAX_NAME];

} LOCATION_DESCRIPTOR;

typedef struct _LIBRARY
{
    OFFSET offset;
    char name[MAX_NAME];
    char path[MAX_NAME];
    DWORD length;
    DWORD loaded;
    char* content;

    char blacklisted;
} LIBRARY;

class taint_plugin : Plugin
{
    public:

    REGION* taints;
    unsigned taint_count;

//    PROPAGATION* propagations;
//    unsigned current_propagation_count;

    unsigned out_tab;
    DWORD query_tid;
    unsigned query_id;

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
    virtual int handle_exception_callback(EXCEPTION_INFO);

    int register_taint(char*);
    int register_taint2(char*);
    int add_taint(OFFSET, UDWORD);
    int print_taint_history(BYTE_t*, unsigned);
    int print_taint_history(BYTE_t*);
    int print_taint_history(unsigned, unsigned);
    int print_taint_history(BYTE_t*, OFFSET, unsigned);
    int trace_watchpoint_connect(TRACE_WATCHPOINT*);
    int query_history(TRACE_WATCHPOINT);
    int parse_trace_string(char*, TRACE_WATCHPOINT*);
    int prompt_taint();
    int d_print_prompt_red(int, const char*, ...);
    int d_print_prompt(int, const char*, ...);
    int write_history(FILE* f);
    int print_taint_ops(unsigned);

    /* print propagations */
    int clear_propagation(unsigned);
    int clear_propagations();
    int print_propagations(unsigned, unsigned);
    int print_propagation(unsigned, unsigned);
    int resolve_affected(BYTE_t*, char*, OFFSET*);

    /* queries */
    int parse_cmd(char*);
    int set_query_tid(DWORD);
    int get_query_tid();

    /* libs for localizators */
    LIBRARY* libs;
    unsigned libs_count;
    int add_lib(OFFSET, char*);
    int del_lib(OFFSET);
    LIBRARY* get_lib(OFFSET);
    OFFSET find_lib(char*);
    int register_lib(char* line);
    int deregister_lib(char* line);

    /* locators */
    OFFSET resolve_loc_desc(LOCATION_DESCRIPTOR*);
    OFFSET resolve_location(char*);
    LOCATION_DESCRIPTOR* parse_location_desc(char*);

    /* disassembling */
    int disas_instruction(OFFSET);
    int disas_instructions(OFFSET, unsigned);


    taint_plugin()
    {
        printf("Initializing taint_plugin\n");
        query_tid = 0x0;
        query_id = 0x0;

        printf("Registering routine callbacks\n");

        unsigned i;

        using_history();

        this->libs = (LIBRARY*)malloc(sizeof(LIBRARY)*MAX_LIB_COUNT);
        this->taints = (REGION*)malloc(sizeof(REGION)*MAX_TAINTS_OBSERVED);

    }

    ~taint_plugin() 
    {
        unsigned i;
    
        HISTORY_STATE* myhist = history_get_history_state();
        HIST_ENTRY** mylist = history_list();

        for(i =  0x0; i<myhist->length; i++)
        {
            free_history_entry(mylist[i]);
        }

        if(mylist) free(mylist);
        if(myhist) free(myhist);

        free(this->libs);
        free(this->taints);

    }

};

#endif
