#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <limits.h>
#include <types.h>

class taint_x86;

typedef struct BREAKPOINT_
{
    OFFSET instruction_no;
    OFFSET offset;
    BYTE mode;
    char name[MAX_NAME];
    DWORD tid;
} BREAKPOINT;

BREAKPOINT parse_breakpoint(char* string);
int parse_mem_breakpoints(char* string, taint_x86* taint_eng);
int parse_taint_breakpoints(char* string, taint_x86* taint_eng);
int parse_trace_watchpoints(char* string, taint_x86* taint_eng);

#endif
