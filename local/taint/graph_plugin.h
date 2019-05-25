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

typedef struct LOOP_FENCE_
{
    OFFSET entry;
    OFFSET start;
    OFFSET limit;
    OFFSET struct_size;
    OFFSET struct_count;

    char collecting;
    char status;

} LOOP_FENCE;

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

typedef struct _CALL_LEVEL
{
    DWORD ret;
    OFFSET entry;
    /* loops handling */

    unsigned call_src_register_idx;
    unsigned loop_start;

    /* new loop handling */
    unsigned loop_pos;
    LOOP_FENCE* cur_fence;

    /* new new loop handling */
    unsigned loop_addr_idx;
    unsigned loop_limit;
    unsigned loop_struct_size;
    unsigned loop_struct_count;
    OFFSET   loop_addr[MAX_LOOP_ADDR];
    char     loop_status;
    unsigned jxx_handling;

} CALL_LEVEL;

typedef struct _GRAPH_CONTEXT
{
    DWORD tid;
    BYTE_t registers[REG_SIZE];
    OFFSET seg_map[0x6];

    CALL_LEVEL* levels;

    /* graph handling */
    DWORD call_level;
    DWORD call_level_smallest;
    DWORD call_level_largest;
    DWORD call_level_ignored;
    char graph_filename[MAX_NAME];
    FILE* graph_file;
    OFFSET waiting;
    unsigned lock_level;

    /* call level handling */ 
    unsigned ret_idx;
    char calling;
    char returning;
    char been_calling;
    char been_returning;
    char been_waiting;
    char last_emit_decision;

    /* jmp analysis processing */
    char jumping;
    char been_jumping;
    char jmp_code;
    char been_jmp_code;

    OFFSET source;
    OFFSET target;
    OFFSET next;
    OFFSET last_eip;
    DWORD list[MAX_CALL_LEVELS][MAX_LIST_JXX];
    unsigned list_len[MAX_CALL_LEVELS];
    unsigned jxx_total[MAX_CALL_LEVELS];

} GRAPH_CONTEXT;



class graph_plugin : Plugin
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
    GRAPH_CONTEXT* cur_graph_context;
    GRAPH_CONTEXT* graph_contexts;
    GRAPH_CONTEXT* get_graph_context(DWORD);

    /* graph stuff - loop fences - new approach */
    LOOP_FENCE loop_fences[MAX_LOOP_FENCES]; 
    unsigned loop_fences_count;
    int add_fence(OFFSET, OFFSET, OFFSET, OFFSET);
    int check_fence(CALL_LEVEL*);
    int enter_loop_demo(GRAPH_CONTEXT*);
    int exit_loop_demo(GRAPH_CONTEXT*);
    int enter_loop(GRAPH_CONTEXT*);
    int exit_loop(GRAPH_CONTEXT*);
    int check_loop(GRAPH_CONTEXT*);
    int check_loop_2(GRAPH_CONTEXT*);
    int check_collecting(GRAPH_CONTEXT*);
    int comment_out(char*, DWORD);

    /* parsing options from out file */
    int register_prefix(char*);
    int register_blacklist(char*);
    int register_blacklist_addr(char*);
    int register_silenced_addr(char*);
    int register_wanted(char*);
    int register_wanted_i(char*);
    int register_wanted_e(char*);
    int register_fence(char*);
    int register_included(char* line);
    int register_comment(char* line);
    int register_symbol(char* line);
    int register_lib(char* line);
    int deregister_lib(char* line);

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
    int handle_call(GRAPH_CONTEXT*);
    int handle_ret(GRAPH_CONTEXT*, OFFSET);
    int handle_jmp(GRAPH_CONTEXT*);
    int handle_jxx(GRAPH_CONTEXT*);
    int handle_this_jxx(GRAPH_CONTEXT*, char*);

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
    void print_call(GRAPH_CONTEXT*, char*, const char*);
    void print_call_open(GRAPH_CONTEXT*, char*, const char*);
    void print_empty_call(GRAPH_CONTEXT*, char*, const char*);
    void print_a_ret(GRAPH_CONTEXT*);
    void print_ret(GRAPH_CONTEXT*);

    /* diving and surfacing */
    int dive(GRAPH_CONTEXT*, OFFSET, OFFSET);
    int surface(GRAPH_CONTEXT*);

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
    virtual int handle_exception_callback(EXCEPTION_INFO);

    int r_jxx(BYTE_t*);
    int r_jb_jc_jnae(BYTE_t*);
    int r_jae_jnb_jnc(BYTE_t*);
    int r_je_jz(BYTE_t*);
    int r_jne_jnz(BYTE_t*);
    int r_jbe_jna(BYTE_t*);
    int r_ja_jnbe(BYTE_t*);
    int r_js(BYTE_t*);
    int r_jns(BYTE_t*);
    int r_jp_jpe(BYTE_t*);
    int r_jnp_jpo(BYTE_t*);
    int r_jl_jnge(BYTE_t*);
    int r_jge_jnl(BYTE_t*);
    int r_jle_jng(BYTE_t*);
    int r_jg_jnle(BYTE_t*);

    // ret
    int r_retn(BYTE_t*);
    int r_ret(BYTE_t*);

    // call
    int r_call_rel(BYTE_t*);
    int r_call_abs_near(BYTE_t*);
    int r_call_abs_far(BYTE_t*);
    int r_jmp_rel_16_32(BYTE_t*);
    int r_jmp_rel_8(BYTE_t*);
    int r_jmp_rm_16_32(BYTE_t*);

    // decoding calls and jumps
    int r_decode_execute_ff(BYTE_t*);

    graph_plugin()
    {
        printf("Initializing graph_plugin\n");

        printf("Registering routine callbacks\n");

        this->instructions_32_start[0x72] = (Plugin::instruction_routine)&graph_plugin::r_jb_jc_jnae; 
        this->instructions_32_start[0x73] = (Plugin::instruction_routine)&graph_plugin::r_jae_jnb_jnc;
        this->instructions_32_start[0x74] = (Plugin::instruction_routine)&graph_plugin::r_je_jz;
        this->instructions_32_start[0x75] = (Plugin::instruction_routine)&graph_plugin::r_jne_jnz;
        this->instructions_32_start[0x76] = (Plugin::instruction_routine)&graph_plugin::r_jbe_jna;
        this->instructions_32_start[0x77] = (Plugin::instruction_routine)&graph_plugin::r_ja_jnbe;
        this->instructions_32_start[0x78] = (Plugin::instruction_routine)&graph_plugin::r_js;
        this->instructions_32_start[0x79] = (Plugin::instruction_routine)&graph_plugin::r_jns;
        this->instructions_32_start[0x7a] = (Plugin::instruction_routine)&graph_plugin::r_jp_jpe;
        this->instructions_32_start[0x7b] = (Plugin::instruction_routine)&graph_plugin::r_jnp_jpo;
        this->instructions_32_start[0x7c] = (Plugin::instruction_routine)&graph_plugin::r_jl_jnge;
        this->instructions_32_start[0x7d] = (Plugin::instruction_routine)&graph_plugin::r_jge_jnl;
        this->instructions_32_start[0x7e] = (Plugin::instruction_routine)&graph_plugin::r_jle_jng;
        this->instructions_32_start[0x7f] = (Plugin::instruction_routine)&graph_plugin::r_jg_jnle;

        this->instructions_32_start[0xc2] = (Plugin::instruction_routine)&graph_plugin::r_retn;
        this->instructions_32_start[0xc3] = (Plugin::instruction_routine)&graph_plugin::r_ret;

        this->instructions_32_start[0xe8] = (Plugin::instruction_routine)&graph_plugin::r_call_rel;
        this->instructions_32_start[0xe9] = (Plugin::instruction_routine)&graph_plugin::r_jmp_rel_16_32;
        this->instructions_32_start[0xeb] = (Plugin::instruction_routine)&graph_plugin::r_jmp_rel_8;

        this->instructions_32_start[0xff] = (Plugin::instruction_routine)&graph_plugin::r_decode_execute_ff;

        this->instructions_32_extended_start[0x82] = (Plugin::instruction_routine)&graph_plugin::r_jb_jc_jnae;
        this->instructions_32_extended_start[0x83] = (Plugin::instruction_routine)&graph_plugin::r_jae_jnb_jnc; 
        this->instructions_32_extended_start[0x84] = (Plugin::instruction_routine)&graph_plugin::r_je_jz;
        this->instructions_32_extended_start[0x85] = (Plugin::instruction_routine)&graph_plugin::r_jne_jnz;
        this->instructions_32_extended_start[0x86] = (Plugin::instruction_routine)&graph_plugin::r_jbe_jna;
        this->instructions_32_extended_start[0x87] = (Plugin::instruction_routine)&graph_plugin::r_ja_jnbe;
        this->instructions_32_extended_start[0x88] = (Plugin::instruction_routine)&graph_plugin::r_js;
        this->instructions_32_extended_start[0x89] = (Plugin::instruction_routine)&graph_plugin::r_jns;
        this->instructions_32_extended_start[0x8a] = (Plugin::instruction_routine)&graph_plugin::r_jp_jpe;
        this->instructions_32_extended_start[0x8b] = (Plugin::instruction_routine)&graph_plugin::r_jnp_jpo;
        this->instructions_32_extended_start[0x8c] = (Plugin::instruction_routine)&graph_plugin::r_jl_jnge;
        this->instructions_32_extended_start[0x8d] = (Plugin::instruction_routine)&graph_plugin::r_jge_jnl;
        this->instructions_32_extended_start[0x8e] = (Plugin::instruction_routine)&graph_plugin::r_jle_jng;
        this->instructions_32_extended_start[0x8f] = (Plugin::instruction_routine)&graph_plugin::r_jg_jnle; 

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

        this->graph_contexts = (GRAPH_CONTEXT*)malloc(sizeof(GRAPH_CONTEXT)*MAX_THREADS);

        if(this->graph_contexts == 0x0)
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

    ~graph_plugin() 
    {
        free(this->graph_contexts);
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
