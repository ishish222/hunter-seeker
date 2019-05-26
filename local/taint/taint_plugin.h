#ifndef GRAPH_ENGINE_H
#define GRAPH_ENGINE_H

#include <taint_emul_x86.h>

#define MAX_CALL_LEVELS         0x200
#define GRAPH_START             100
#define MAX_LOOP_ADDRS          0x10
#define MAX_LOOP_FENCES         0x10
#define MAX_BLACKLIST           0x50
#define MAX_WANTED              0x100
#define MAX_LOOP_ADDR           0x50
#define MAX_LIST_JXX            0x1000

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

#define FENCE_INACTIVE          0x0
#define FENCE_ACTIVE            0x1
#define FENCE_COLLECTING        0x2
#define FENCE_NOT_COLLECTING    0x3
#define FENCE_FINISHED          0x4

class taint_plugin : Plugin
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
    virtual int handle_exception_callback(EXCEPTION_INFO);

    taint_plugin()
    {
        printf("Initializing taint_plugin\n");

        printf("Registering routine callbacks\n");

        unsigned i;
    }

    ~taint_plugin() 
    {

    }

};

#endif
