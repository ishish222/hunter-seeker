#include <breakpoint.h>
#include <emul_x86.h>
#include <plugin.h>
#include <string.h>

BREAKPOINT parse_breakpoint(char* string)
{
    BREAKPOINT bp;
    char* current;

    fprintf(stderr, "Parsing: %s\n", string);

    current = strtok(string, ":");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    strcpy(bp.name, current);
    fprintf(stderr, "bp.name: %s\n", bp.name);

    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    bp.tid= strtoul(current, 0x0, 0x10);
    fprintf(stderr, "bp.tid: 0x%08x\n", bp.tid);

    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    bp.instruction_no = strtoul(current, 0x0, 10);
    fprintf(stderr, "bp.instruction_no: %d\n", bp.instruction_no);

    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    bp.offset = strtoul(current, 0x0, 0x10);
    fprintf(stderr, "bp.offset: %d\n", bp.offset);

    current = strtok(0x0, "");

    if(strstr(current, "READ") != 0x0)
        bp.mode |= BP_MODE_READ;
    if(strstr(current, "WRITE") != 0x0)
        bp.mode |= BP_MODE_WRITE;
    if(strstr(current, "EXECUTE") != 0x0)
        bp.mode |= BP_MODE_EXECUTE;

    return bp;
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
    return 0x0;
    fprintf(stderr, "Received bp taint string: %s\n", string);

    char* current;
    char buf[MAX_NAME];

    current = string;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        //taint_eng->add_taint_breakpoint(parse_breakpoint(buf));
        current = strpbrk(current, "+");
        if(current)
            current++;
    }

    return 0x0;
}


