#include <breakpoint.h>
#include <taint_emul_x86.h>
#include <string.h>

BREAKPOINT parse_breakpoint(char* string)
{
    BREAKPOINT bp;
    char* current;

    fprintf(stderr, "Parsing: %s\n", string);

    current = strtok(string, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    bp.offset = strtoul(current, 0x0, 10);

    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    bp.mem_offset = strtoul(current, 0x0, 0x10);

    current = strtok(0x0, "+");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    bp.mode = 0x0;

    if(strstr(current, "READ") != 0x0)
        bp.mode |= BP_MODE_READ;
    if(strstr(current, "WRITE") != 0x0)
        bp.mode |= BP_MODE_WRITE;
    if(strstr(current, "EXECUTE") != 0x0)
        bp.mode |= BP_MODE_EXECUTE;

    return bp;
}

TRACE_WATCHPOINT parse_trace_watchpoint(char* string, taint_x86* taint_eng)
{
    TRACE_WATCHPOINT twp;

    fprintf(stderr, "Parsing: %s\n", string);

    char* current;

    current = strtok(string, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }

    if(current[0] == 'i') 
    {
        twp.interactive = 1;
        current ++;
    }
    else twp.interactive = 0x0;
    twp.offset = strtoul(current, 0x0, 10);
    fprintf(stderr, "Instr count: %lld\n", twp.offset);

    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    twp.mem_offset = strtoul(current, 0x0, 0x10);
    fprintf(stderr, "Offset: 0x%08x\n", twp.mem_offset);

    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    twp.tid = strtoul(current, 0x0, 0x10);
    fprintf(stderr, "TID: 0x%08x\n", twp.tid);

    current = strtok(0x0, "+");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    strcpy(twp.name, current);
    fprintf(stderr, "%s\n", twp.name);

    return twp;
}

int parse_mem_breakpoints(char* string, taint_x86* taint_eng)
{
    fprintf(stderr, "Received bp string: %s\n", string);

    char* current;
    char buf[MAX_NAME];

    current = string;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        taint_eng->add_breakpoint(parse_breakpoint(buf));
        //current = strpbrk(current+1, "+");
        current = strpbrk(current, "+");
        if(current)
            current++;
    }

    return 0x0;
}

int parse_taint_breakpoints(char* string, taint_x86* taint_eng)
{
    fprintf(stderr, "Received bp taint string: %s\n", string);

    char* current;
    char buf[MAX_NAME];

    current = string;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        taint_eng->add_taint_breakpoint(parse_breakpoint(buf));
        //current = strpbrk(current+1, "+");
        current = strpbrk(current, "+");
        if(current)
            current++;
    }

    return 0x0;
}

int parse_trace_watchpoints(char* string, taint_x86* taint_eng)
{
    fprintf(stderr, "Received watchpoint string: %s\n", string);

    char* current;
    char buf[MAX_NAME];

    current = string;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        taint_eng->add_trace_watchpoint(parse_trace_watchpoint(buf, taint_eng));
        current = strpbrk(current, "+");
        if(current)
            current++;
    }

    return 0x0;
}



