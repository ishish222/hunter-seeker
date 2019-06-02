#include <breakpoint.h>
#include <emul_x86.h>
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

int parse_trace_watchpoints(char* string, taint_x86* taint_eng)
{
    return 0x0;
    fprintf(stderr, "Received watchpoint string: %s\n", string);

    char* current;
    char buf[MAX_NAME];

    current = string;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        //taint_eng->add_trace_watchpoint(parse_trace_watchpoint(buf, taint_eng));
        current = strpbrk(current, "+");
        if(current)
            current++;
    }

    return 0x0;
}

/*
char* findany(char* s, char* keys)
{
    char* tmp;
    tmp = s + strcspn(s,keys);
    return *tmp == '\0' ? 0x0 : tmp;
}

int paint(char* area, unsigned len)
{
    unsigned i;
    unsigned count = 0;
    char active = 0x0;

    for(i=0; i<len; i++)
    {
        if(area[i] == '[')
        {
            count++;
            if(count >= 1)  active = 0x1;
            else            active = 0x0;

            if(active)      area[i]+=3;
        }
        else if(area[i] == ']')
        {
            if(active)      area[i]+=3;

            count--;
            if(count >= 1)  active = 0x1;
            else            active = 0x0;
        }
        else
        {
            if(active)      area[i]+=3;
        }

//        printf("%c", area[i]);
    }
//    printf("\n");

    return 0x0;
}

int unpaint(char* area, unsigned len)
{
    unsigned i;
    unsigned count = 0;
    char active = 0x0;

    for(i=0; i<len; i++)
    {
        if(area[i]-3 == '[')
        {
            count++;
            if(count >= 1)  active = 0x1;
            else            active = 0x0;
            if(active)      area[i]-=3;
        }
        else if(area[i]-3 == ']')
        {
            if(active)      area[i]-=3;
            count--;
            if(count >= 1)  active = 0x1;
            else            active = 0x0;
        }
        else
        {
            if(active)      area[i]-=3;
        }

//        printf("%c", area[i]);
//
    }
//    printf("\n");
    return 0x0;
}

OFFSET resolve_loc_desc(LOCATION_DESCRIPTOR* d, taint_x86* emul_eng)
{
    printf("[resolve_loc_desc]\n");
    OFFSET a1_r, a2_r;
    OFFSET ret;

    if(d == 0x0)
        return -1;

    printf("Processing: %s\n", d->op);

    if(findany(d->op, "[+-"))
    {
       // not leaf, this is operation, we do recurrence 

        if(!strcmp(d->op, "["))
        {
            DWORD_t read;
            a1_r = resolve_loc_desc(d->a1, emul_eng);
            if(a1_r == -1) return -1;
            // read_memory and calculate ret 
            
            emul_eng->restore_32(a1_r, read);
            return read.get_DWORD();
        }
        else if(!strcmp(d->op, "-"))
        {
            a1_r = resolve_loc_desc(d->a1, emul_eng);
            a2_r = resolve_loc_desc(d->a2, emul_eng);
            if(a1_r == -1) return -1;
            if(a2_r == -1) return -1;
            ret = a1_r - a2_r;
        }
        else if(!strcmp(d->op, "+"))
        {
            a1_r = resolve_loc_desc(d->a1, emul_eng);
            a2_r = resolve_loc_desc(d->a2, emul_eng);
            if(a1_r == -1) return -1;
            if(a2_r == -1) return -1;
            ret = a1_r + a2_r;
        }
    }
    else
    {
        // this is leaf 

        if(!strcmp(d->op, "self"))
        {
            // return self address
            ret = (OFFSET)my_trace->cpdi.lpBaseOfImage;
            if(ret == 0x0) return -1;
        }
        else if((d->op[0] == '0') && (d->op[1] == 'x'))
        {
            // return immediate
            ret = strtoul(d->op, 0x0, 0x10);
        }
        else
        {
            // registers
            if(!strcmpi(d->op, "ESP"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Esp;
                printf("Reading register ESP: 0x%08x\n", ret);
            }
            else if(!strcmpi(d->op, "EAX"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Eax;
                printf("Reading register EAX: 0x%08x\n", ret);
            }
            else if(!strcmpi(d->op, "EBX"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Ebx;
                printf("Reading register EBX: 0x%08x\n", ret);
            }
            else if(!strcmpi(d->op, "ECX"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Ecx;
                printf("Reading register ECX: 0x%08x\n", ret);
            }
            else if(!strcmpi(d->op, "EDX"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Edx;
                printf("Reading register EDX: 0x%08x\n", ret);
            }
            else if(!strcmpi(d->op, "ESI"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Esi;
                printf("Reading register ESI: 0x%08x\n", ret);
            }
            else if(!strcmpi(d->op, "EDI"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Edi;
                printf("Reading register EDI: 0x%08x\n", ret);
            }
            else if(!strcmpi(d->op, "EBP"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Ebp;
                printf("Reading register EBP: 0x%08x\n", ret);
            }
            else if(!strcmpi(d->op, "EIP"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Eip;
                printf("Reading register EIP: 0x%08x\n", ret);
            }
            else
            {
                // we assume it's library 
                printf("Looking for lib: %s\n", d->op);
                ret = find_lib(d->op);
                if(ret != 0x0)
                {
                    printf("Found at: 0x%08x\n", ret);
                }
                else if(ret == 0x0) 
                {
                    printf("Not found\n");
                    ret = -1;
                }
            }
        }
    }
    printf("[resolve_loc_desc end]\n");
    return ret;
}

LOCATION_DESCRIPTOR* parse_location_desc(char* str)
{
    char* op;
    LOCATION_DESCRIPTOR* neww;

    neww = (LOCATION_DESCRIPTOR*)malloc(sizeof(LOCATION_DESCRIPTOR));
    memset(neww, 0x0, sizeof(LOCATION_DESCRIPTOR));
    neww->a1 = 0x0;
    neww->a2 = 0x0;

    if(!neww)
    {
        printf("Out of memory\n");
    }

    paint(str, strlen(str));
    op = findany(str, "+-");

    if(op == 0x0) 
    {
        unpaint(str, strlen(str));
        if((op = findany(str, "[")) != 0x0)
        {
            strcpy(neww->op, "[");
            op[0] = 0x0;
            neww->a1 = parse_location_desc(op+1);
            neww->a2 = 0x0;
        }
        else
        {
            strcpy(neww->op, str);
        }
    }
    else
    {
        unpaint(str, strlen(str));
        
        if(op[0] == '+')
        {
            strcpy(neww->op, "+");
            op[0] = 0x0;
            neww->a1 = parse_location_desc(str);
            neww->a2 = parse_location_desc(op+1);
        }
        else if(op[0] == '-')
        {
            strcpy(neww->op, "-");
            op[0] = 0x0;
            neww->a1 = parse_location_desc(str);
            neww->a2 = parse_location_desc(op+1);
        }
    }

    return neww;
}



int resolve_location(char* location)
{
    OFFSET addr;
    char line[MAX_LINE];
    LOCATION_DESCRIPTOR desc;
    LOCATION_DESCRIPTOR* desc_;

    desc_ = parse_location_desc(location);
    addr = resolve_loc_desc(desc_);

    my_trace->report_code = REPORT_INFO;
    sprintf(line, "0x%08x", addr);
    strcpy(my_trace->report_buffer, line);
    return 0x0;
}

*/
