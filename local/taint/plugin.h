#ifndef PLUGIN_H
#define PLUGIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taint_emul_x86.h>

/*
typedef int (*test)(DWORD);
typedef int (*PRE_EXECUTE_INSTRUCTION_CALLBACK)(DWORD);
typedef int (*POST_EXECUTE_INSTRUCTION_CALLBACK)(DWORD);
typedef int (*START_CALLBACK)();
typedef int (*FINISH_CALLBACK)();
typedef int (*ADD_THREAD_CALLBACK)(CONTEXT_info);
typedef int (*DEL_THREAD_CALLBACK)(DWORD);
typedef int (*DEL_THREAD_SRSLY_CALLBACK)(DWORD);
*/

class taint_x86;

class Plugin
{
    public:
    taint_x86* taint_eng;

/*
    PRE_EXECUTE_INSTRUCTION_CALLBACK pre_execute_instruction_callback;
    POST_EXECUTE_INSTRUCTION_CALLBACK post_execute_instruction_callback;
    START_CALLBACK start_callback;
    FINISH_CALLBACK finish_callback();
    ADD_THREAD_CALLBACK add_thread_callback;
    DEL_THREAD_CALLBACK del_thread_callback;
    DEL_THREAD_SRSLY_CALLBACK del_thread_srsly_callback;
*/
    virtual int pre_execute_instruction_callback(DWORD) = 0;
    virtual int post_execute_instruction_callback(DWORD) = 0;
    virtual int start_callback() = 0;
    virtual int finish_callback() = 0;
    virtual int add_thread_callback(CONTEXT_info) = 0;
    virtual int del_thread_callback(DWORD) = 0;
    virtual int del_thread_srsly_callback(DWORD) = 0;

    int set_taint_eng(taint_x86*);

    Plugin() {}
    ~Plugin() {}
};

#endif
