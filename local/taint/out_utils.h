/*
*   Tomasz Salacinski
*   Korrino
*   tomasz.salacinski@korrino.com
*/

#ifndef OUT_UTILS_H
#define OUT_UTILS_H

#include <taint_emul_x86.h>
#include <stdio.h>

void deserialize_context(CONTEXT_info* ctx_info, char* buffer);
int register_taint(char* line, taint_x86* taint_eng);
int register_symbol(char* line, taint_x86* taint_eng);
int register_blacklist(char* line, taint_x86* taint_eng);
unsigned get_instr_interval(char* line);
int register_blacklist_addr(char* line, taint_x86* taint_eng);
int register_silenced_addr(char* line, taint_x86* taint_eng);
int register_wanted_i(char* line, taint_x86* taint_eng);
int register_wanted_e(char* line, taint_x86* taint_eng);
int register_fence(char* line, taint_x86* taint_eng);
int load_mod(char* line, taint_x86* taint_eng);
int load_mem(char* line, taint_x86* taint_eng);
int load_file(char* line, taint_x86* taint_eng);
int comment_out(char* line, taint_x86* taint_eng);
int parse_option(char* line, taint_x86* taint_eng);
int register_included(char* line, taint_x86* taint_eng);
int register_wanted(char* line, taint_x86* taint_eng);
int register_thread(char* line, taint_x86* taint_eng);
int register_exception(char* line, taint_x86* taint_eng);
int check_consistency(char* line, taint_x86* taint_eng);
int register_update(char* line, taint_x86* taint_eng);
int set_prefix(char* line, taint_x86* taint_eng);
int register_security(char* line, taint_x86* taint_eng);
int register_lib(char* line, taint_x86* taint_eng);
int deregister_lib(char* line, taint_x86* taint_eng);
int deregister_thread(char* line, taint_x86* taint_eng);

#endif
