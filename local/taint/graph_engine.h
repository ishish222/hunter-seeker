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

typedef struct SYMBOL_
{
    OFFSET addr;
    char* lib_name;
    char* func_name;
    struct SYMBOL_* next;

    DWORD resolved;
    DWORD wanted;
    DWORD included;
} SYMBOL;

typedef struct _LIBRARY
{
    OFFSET offset;
    char name[MAX_NAME];
    char path[MAX_NAME];
    DWORD length;
    DWORD loaded;
    char* content;
    SYMBOL* symbols;

    char blacklisted;
} LIBRARY;

typedef struct _CONTEXT_GRAPH
{
    DWORD tid;
    BYTE_t registers[REG_SIZE];
    OFFSET seg_map[0x6];

    CALL_LEVEL* levels;

    /* call level handling */ 
    unsigned ret_idx;
    char calling;
    char returning;
    char before_calling;
    char before_returning;
    char before_waiting;
    char last_emit_decision;

    /* jmp analysis processing */
    char jumping;
    char before_jumping;
    char jmp_code;
    char before_jmp_code;

    OFFSET source;
    OFFSET target;
    OFFSET next;
    OFFSET last_eip;
    DWORD list[MAX_CALL_LEVELS][MAX_LIST_JXX];
    unsigned list_len[MAX_CALL_LEVELS];
    unsigned jxx_total[MAX_CALL_LEVELS];

} CONTEXT_GRAPH;



class graph_engine : Plugin
{
    public:

    /* graph parameters */
    unsigned max_call_levels;
    unsigned call_level_start;
    unsigned call_level_offset;
    DWORD depth;
    char enumerate;

    int d_print(int, const char*, ...);

    /* graph prefixes */
    int set_prefix(char*);
    char prefix[MAX_NAME];

    /* graph stuff */
    char* lib_blacklist[MAX_NAME];
    unsigned blacklist_count;
    DWORD addr_silenced[MAX_BLACKLIST];
    unsigned addr_silenced_count;
    DWORD addr_blacklist[MAX_BLACKLIST];
    unsigned addr_blacklist_count;
    char* func_wanted[MAX_NAME];
    char* func_included[MAX_NAME];
    unsigned instr_wanted[MAX_WANTED];
    DWORD addr_wanted[MAX_WANTED];
    unsigned wanted_count;
    unsigned included_count;
    unsigned wanted_count_e;
    unsigned wanted_count_i;
    unsigned ret_idx;

    /* handling context info */
    DWORD tids[MAX_THREAD_NUMBER];
    DWORD tid_count;
    DWORD cur_tid;
    CONTEXT_INFO* cur_info;
    CONTEXT_INFO* ctx_info;

    /* graph stuff - loop fences - new approach */
    LOOP_FENCE loop_fences[MAX_LOOP_FENCES]; 
    unsigned loop_fences_count;
    int add_fence(OFFSET, OFFSET, OFFSET, OFFSET);
    int check_fence(CALL_LEVEL*);
    int enter_loop_demo(CONTEXT_INFO*);
    int exit_loop_demo(CONTEXT_INFO*);
    int enter_loop(CONTEXT_INFO*);
    int exit_loop(CONTEXT_INFO*);
    int check_loop(CONTEXT_INFO*);
    int check_loop_2(CONTEXT_INFO*);
    int check_collecting(CONTEXT_INFO*);
    int comment_out(char*, DWORD);

    /* parsing options from out file */
    int register_blacklist(char*);
    int register_blacklist_addr(char*);
    int register_silenced_addr(char*);
    int register_wanted(char*);
    int register_wanted_i(char*);
    int register_wanted_e(char*);
    int register_fence(char*);
    int register_included(char* line);
    int register_comment(char* line);
    

    /* graph stuff - emitting configuration */
    int add_blacklist(char*);
    int add_blacklist_addr(DWORD);
    int add_silenced_addr(DWORD);
    int add_included(char*);
    int add_wanted(char*);
    int add_wanted_e(DWORD);
    int add_wanted_i(unsigned);
    int check_lib_blacklist(LIBRARY*);
    int check_addr_blacklist(OFFSET);
    int check_addr_silenced(OFFSET);
    int check_func_wanted(char*);
    int check_func_included(char*);
    int check_rets(OFFSET);

    /* graph stuff - handlers */
    int handle_exception(EXCEPTION_INFO);
    int handle_call(CONTEXT_INFO*);
    int handle_ret(CONTEXT_INFO*, OFFSET);
    int handle_jmp(CONTEXT_INFO*);
    int handle_jxx(CONTEXT_INFO*);
    int handle_this_jxx(CONTEXT_INFO*, char*);

    /* symbols and libraries */
    LIBRARY* libs;
    unsigned libs_count;
    int add_lib(OFFSET, char*);
    int del_lib(OFFSET);
    LIBRARY* get_lib(OFFSET);

    SYMBOL* symbols; 
    unsigned symbols_count;
    int add_symbol(SYMBOL**, OFFSET, char*, char*);
    int del_symbol(SYMBOL*);
    int add_symbols(LIBRARY*);
    int copy_symbol(SYMBOL**, SYMBOL*);
    SYMBOL* get_symbol(OFFSET);

    char lib_dir_path[MAX_NAME]; //is this necessary?
    int set_lib_dir_path(char*);


    /* graph stuff - prints */
    void print_call(CONTEXT_INFO*, char*, const char*);
    void print_call_open(CONTEXT_INFO*, char*, const char*);
    void print_empty_call(CONTEXT_INFO*, char*, const char*);
    void print_a_ret(CONTEXT_INFO*);
    void print_ret(CONTEXT_INFO*);

    /* diving and surfacing */
    int dive(CONTEXT_INFO*, OFFSET, OFFSET);
    int surface(CONTEXT_INFO*);

    int jxx_set(unsigned);
    int jxx_clear_level(unsigned);
    int jxx_clear();

    /* new implementation */
    virtual int pre_execute_instruction_callback(DWORD);
    virtual int post_execute_instruction_callback(DWORD);
    virtual int start_callback();
    virtual int finish_callback();
    virtual int add_thread_callback(CONTEXT_OUT);
    virtual int del_thread_callback(DWORD);
    virtual int del_thread_srsly_callback(DWORD);
    virtual int parse_option(char*);

    graph_engine()
    {
        printf("Initializing graph_engine\n");

        unsigned i;

        for(i = 0x0; i< MAX_BLACKLIST; i++)
        {
            this->lib_blacklist[i] = (char*)malloc(MAX_NAME);
            if(this->lib_blacklist[0] == 0x0)
            {
                printf("Not enough memory\n");
            }
        }

        for(i = 0x0; i< MAX_WANTED; i++)
        {
            this->func_wanted[i] = (char*)malloc(MAX_NAME);
            if(this->func_wanted[i] == 0x0)
            {
                printf("Not enough memory\n");
            }
        }

        for(i = 0x0; i< MAX_WANTED; i++)
        {
            this->func_included[i] = (char*)malloc(MAX_NAME);
            if(this->func_included[i] == 0x0)
            {
                printf("Not enough memory\n");
            }
        }

        this->ctx_info = (CONTEXT_INFO*)malloc(sizeof(CONTEXT_INFO)*MAX_THREADS);

        if(this->ctx_info == 0x0)
        {
            printf("Not enough memory\n");
        }

        this->libs = (LIBRARY*)malloc(sizeof(LIBRARY)*MAX_LIB_COUNT);
        if(this->libs == 0x0)
        {
            printf("Not enough memory\n");
        }

        this->blacklist_count = 0x0;
        this->wanted_count = 0x0;
        this->wanted_count_i = 0x0;
        this->wanted_count_e = 0x0;

        this->max_call_levels = MAX_CALL_LEVELS;
        this->call_level_start = this->max_call_levels/3;

    }

    ~graph_engine() 
    {
        free(this->ctx_info);
        free(this->libs);

        unsigned i;
        for(i = 0x0; i< MAX_BLACKLIST; i++)
        {
            free(this->lib_blacklist[i]);
        }

        for(i = 0x0; i< MAX_WANTED; i++)
        {
            free(this->func_wanted[i]);
        }

        for(i = 0x0; i< MAX_WANTED; i++)
        {
            free(this->func_included[i]);
        }

    }

};

#endif
