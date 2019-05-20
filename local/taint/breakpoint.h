#ifndef BREAKPOINTS_H
#define BREAKPOINTS_H

#include <taint_emul_x86.h>

BREAKPOINT parse_breakpoint(char* string);
TRACE_WATCHPOINT parse_trace_watchpoint(char* string, taint_x86* taint_eng);
int parse_mem_breakpoints(char* string, taint_x86* taint_eng);
int parse_taint_breakpoints(char* string, taint_x86* taint_eng);
int parse_trace_watchpoints(char* string, taint_x86* taint_eng);

#endif
