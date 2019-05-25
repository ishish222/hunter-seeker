#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <types.h>

#define MAX_NAME                0x200
#define MAX_SYMBOL_NAME         0x50
#define MAX_SYMBOL_COUNT        0x1000000
#define MAX_LIB_COUNT           0x100
#define MAX_THREADS             0x100
#define MAX_THREAD_NUMBER       0x1000
#define MAX_CALL_LEVELS         0x200
#define GRAPH_START             100
#define MAX_LOOP_ADDRS          0x10
//#define MAX_THREADS 0x1000000
#define MAX_PRPAGATIONS_OBSERVED 0x4000000
#define MAX_TAINTS_OBSERVED     0x400
#define MAX_EXCEPTIONS_COUNT    0x100
#define MAX_BREAKPOINTS         0x10
#define MAX_SECURITY_LAYERS     0x1000
#define MAX_BLACKLIST           0x50
#define MAX_WANTED              0x100
#define MAX_LOOP_FENCES         0x10
#define MAX_LOOP_ADDR           0x50
#define MAX_LIST_JXX            0x1000

class taint_x86;

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
} TRACE_WATCHPOINT;

BREAKPOINT parse_breakpoint(char* string);
TRACE_WATCHPOINT parse_trace_watchpoint(char* string, taint_x86* taint_eng);
int parse_mem_breakpoints(char* string, taint_x86* taint_eng);
int parse_taint_breakpoints(char* string, taint_x86* taint_eng);
int parse_trace_watchpoints(char* string, taint_x86* taint_eng);

#endif
