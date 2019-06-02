#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <limits.h>
#include <types.h>

class taint_x86;

typedef struct LOCATION_DESCRIPTOR_
{
    struct LOCATION_DESCRIPTOR_* a1;
    struct LOCATION_DESCRIPTOR_* a2;
    char op[MAX_NAME];

} LOCATION_DESCRIPTOR;

typedef struct BREAKPOINT_
{
    OFFSET instruction_no;
    OFFSET offset;
    BYTE mode;
    char name[MAX_NAME];
    DWORD tid;
} BREAKPOINT;

typedef struct TRACE_WATCHPOINT_
{
    OFFSET offset;
    OFFSET mem_offset;
    BYTE_t* watched;
    char name[MAX_NAME];
    DWORD tid;
    char interactive;
    char branches;
} TRACE_WATCHPOINT;

BREAKPOINT parse_breakpoint(char* string);
TRACE_WATCHPOINT parse_trace_watchpoint(char* string, taint_x86* taint_eng);
int parse_mem_breakpoints(char* string, taint_x86* taint_eng);
int parse_taint_breakpoints(char* string, taint_x86* taint_eng);
int parse_trace_watchpoints(char* string, taint_x86* taint_eng);

#endif
