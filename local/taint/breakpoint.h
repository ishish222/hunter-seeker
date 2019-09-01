#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <limits.h>
#include <types.h>

class taint_x86;

typedef struct WATCHPOINT_
{
    OFFSET init_instruction_no;
    OFFSET offset;
    BYTE init_value;
    char name[MAX_NAME];
    DWORD tid;
} WATCHPOINT;

typedef struct BREAKPOINT_
{
    OFFSET instruction_no;
    OFFSET offset;
    BYTE mode;
    char name[MAX_NAME];
    DWORD tid;
} BREAKPOINT;

BREAKPOINT parse_breakpoint(char* string);
WATCHPOINT parse_watchpoint(char* string);
int parse_mem_watchpoints(char* string, taint_x86* taint_eng);
int parse_taint_watchpoints(char* string, taint_x86* taint_eng);
int parse_mem_breakpoints(char* string, taint_x86* taint_eng);
int parse_taint_breakpoints(char* string, taint_x86* taint_eng);

#endif
