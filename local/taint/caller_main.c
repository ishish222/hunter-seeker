/*
*   Tomasz Salacinski
*   CERT Orange Polska
*   tomasz.salacinski@orange.com
*/

#include <caller_taint_x86.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define STRUCTURED_BUFFER_LENGTH 0x10000
#define MAX_LINE 0x1000
#define INSTR_REPORT_INTERVAL 1000000

taint_x86 taint_eng;

int write_region(FILE* f, BYTE_t* src, unsigned int len)
{
    unsigned int i;

    for(i = 0x0; i<len; i++)
        fwrite(&src[i].val, 1, 1, f);

    return i;

}

int write_t_region(FILE* f, BYTE_t* src, unsigned int len)
{
    unsigned int i;

    for(i = 0x0; i<len; i++)
        fwrite(&src[i].val+1, 1, 1, f);

    return i;

}

void print_location(modrm_ptr a)
{
    switch(a.region)
    {
        case MODRM_REG:
            printf("Region: MODRM_REG\n");
            break;
        case MODRM_MEM:
            printf("Region: MODRM_MEM\n");
            break;
        default:
            printf("Undeclared\n");
            break;
    }

    printf("Offset: 0x%x\n", a.offset);
}

void deserialize_exception(EXCEPTION_RECORD* er, char* buffer)
{
    er->ExceptionCode = strtol(strtok(buffer, ","), 0x0, 0x10);
    er->ExceptionFlags = strtol(strtok(0x0, ","), 0x0, 0x10);
    er->ExceptionAddress = strtol(strtok(0x0, ","), 0x0, 0x10);

    return;
}

void deserialize_context(CONTEXT_info* ctx_info, char* buffer)
{
    unsigned i;
    DWORD tempHW;

//    printf("Parsing: %s\n", buffer);

    ctx_info->ctx.Eax = strtol(strtok(buffer, ","), 0x0, 0x10);
    ctx_info->ctx.Ecx = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.Edx = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.Ebx = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.Esi = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.Edi = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.Ebp = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.Esp = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.EFlags = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.Eip = strtol(strtok(0x0, ","), 0x0, 0x10);
    
    ctx_info->ctx.SegGs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.SegFs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.SegEs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.SegDs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.SegCs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_info->ctx.SegSs = strtol(strtok(0x0, ","), 0x0, 0x10);

    for(i=0x0; i<0x6; i++)
    {
        ctx_info->ldt[i].LimitLow = strtol(strtok(0x0, ","), 0x0, 0x10);
//        printf("LimitLow: 0x%08x\n", ctx_info->ldt[i].LimitLow);
        ctx_info->ldt[i].BaseLow = strtol(strtok(0x0, ","), 0x0, 0x10);
//        printf("BaseLow: 0x%08x\n", ctx_info->ldt[i].BaseLow);
        tempHW = strtol(strtok(0x0, ","), 0x0, 0x10);
        *((DWORD*)&ctx_info->ldt[i].HighWord) = tempHW;
//        printf("HighWord: 0x%08x\n", tempHW);
    }

    return;
}

int register_taint(char* line, taint_x86* taint_eng)
{
    char* cmd;
    DWORD len;
    OFFSET off;

    cmd = strtok(line, ",");
    off = (OFFSET)strtol(strtok(0x0, ","), 0x0, 0x10);
    len = (DWORD)strtol(strtok(0x0, ","), 0x0, 0x10);

    printf("Registering taint @ 0x%08x, with length: 0x%08x\n", off, len);
    taint_eng->add_taint(off, len);
    taint_eng->print_t_mem(1, off - 0x5, 0x10);

    return 0x0;
}

int register_symbol(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* lib_name;
    char* func_name;
    DWORD len;
    OFFSET off;

    cmd = strtok(line, ",");
    lib_name = strtok(0x0, ",");
    func_name = strtok(0x0, ",");
    off = (OFFSET)strtol(strtok(0x0, ","), 0x0, 0x10);

    //printf("%s@%s @ 0x%08x\n", lib_name, func_name, off);
    taint_eng->add_symbol(&taint_eng->symbols, off, lib_name, func_name);

    return 0x0;
}

int register_blacklist(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* lib_name;

    cmd = strtok(line, ",");
    lib_name = strtok(0x0, ",");
    taint_eng->add_blacklist(lib_name);

    return 0x0;
}

int register_blacklist_addr(char* line, taint_x86* taint_eng)
{
    char* cmd;
    DWORD addr;

    cmd = strtok(line, ",");
    addr = strtol(strtok(0x0, ","), 0x0, 0x10);
    taint_eng->add_blacklist_addr(addr);

    return 0x0;
}


int register_wanted_i(char* line, taint_x86* taint_eng)
{
    char* cmd;
    unsigned instr;

    cmd = strtok(line, ",");
    instr = strtol(strtok(0x0, ","), 0x0, 10);
    taint_eng->add_wanted_i(instr);

    return 0x0;
}

int register_wanted_e(char* line, taint_x86* taint_eng)
{
    char* cmd;
    DWORD addr;

    cmd = strtok(line, ",");
    addr = strtol(strtok(0x0, ","), 0x0, 0x10);
    taint_eng->add_wanted_e(addr);

    return 0x0;
}

int register_fence(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET entry;
    OFFSET start;
    OFFSET size;
    OFFSET count;

    cmd = strtok(line, ",");
    entry = strtol(strtok(0x0, ","), 0x0, 0x10);
    start = strtol(strtok(0x0, ","), 0x0, 0x10);
    size = strtol(strtok(0x0, ","), 0x0, 0x10);
    count  = strtol(strtok(0x0, ","), 0x0, 0x10);
    taint_eng->add_fence(entry, start, size, count);

    return 0x0;
}

int load_file(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* file_name;

    cmd = strtok(line, ",");
    file_name = strtok(0x0, ",");
    file_name[strlen(file_name)-1] = 0x0;
    
    printf("Switching instr file to %s\n", file_name);

    fclose(taint_eng->instr_file);
    taint_eng->load_instr_from_file(file_name);

    return 0x0;
}

int parse_option(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* option;

    cmd = strtok(line, ",");
    option = strtok(0x0, ",");
    
    option[strlen(option)-1] = 0x0;

    printf("Enabling analysis option: %s\n", option);

    printf("Before: 0x%08x\n", taint_eng->options);
    if(!strcmp(option, "ANALYZE_JUMPS"))
    {
        taint_eng->options |= OPTION_ANALYZE_JUMPS;
    }
    else if(!strcmp(option, "ANALYZE_LOOPS"))
    {
        taint_eng->options |= OPTION_ANALYZE_LOOPS;
    }
    else if(!strcmp(option, "UNMATCHED_RET_INVALIDATES_STACK"))
    {
        taint_eng->options |= OPTION_UNMATCHED_RET_INVALIDATES_STACK;
    }
    else if(!strcmp(option, "UNMATCHED_RET_CREATES_CALL"))
    {
        taint_eng->options |= OPTION_UNMATCHED_RET_CREATES_CALL;
    }
    else if(!strcmp(option, "NOT_EMITTING_BLACKLISTED"))
    {
        taint_eng->options |= OPTION_NOT_EMITTING_BLACKLISTED;
    }
    else if(!strcmp(option, "VERIFY_ROP_RETS"))
    {
        taint_eng->options |= OPTION_VERIFY_ROP_RETS;
    }
    else if(!strcmp(option, "VERIFY_SEG_SEC"))
    {
        taint_eng->options |= OPTION_VERIFY_SEG_SEC;
    }

    printf("After: 0x%08x\n", taint_eng->options);
    return 0x0;
}


int register_included(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* func_name;

    cmd = strtok(line, ",");
    func_name = strtok(0x0, ",");
    taint_eng->add_included(func_name);

    return 0x0;
}

int register_wanted(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* func_name;

    cmd = strtok(line, ",");
    func_name = strtok(0x0, ",");
    taint_eng->add_wanted(func_name);

    return 0x0;
}

int register_thread(char* line, taint_x86* taint_eng)
{
    CONTEXT_info ctx_info;

    char* cmd;
    char* tid_str;
    char* context_str;

    cmd = strtok(line, ",");
    tid_str = strtok(0x0, ",");
    context_str = strtok(0x0, " ");

    deserialize_context(&ctx_info, context_str);
    ctx_info.thread_id = strtol(tid_str, 0x0, 0x10);

    taint_eng->add_thread(ctx_info);

    return 0x0;
}

int register_exception(char* line, taint_x86* taint_eng)
{
    char* cmd;
    EXCEPTION_INFO info;

    cmd = strtok(line, ",");
    info.tid = strtol(strtok(0x0, ","), 0x0, 0x10);
    info.er.ExceptionCode = strtol(strtok(0x0, ","), 0x0, 0x10);
    info.er.ExceptionFlags = strtol(strtok(0x0, ","), 0x0, 0x10);
    info.er.ExceptionAddress = strtol(strtok(0x0, ","), 0x0, 0x10);

    printf("Registering exception code 0x%08x in 0x%08x at 0x%08x\n", info.er.ExceptionCode, info.tid, info.er.ExceptionAddress);
    taint_eng->add_exception(info);

    return 0x0;
}

int check_consistency(char* line, taint_x86* taint_eng)
{
    CONTEXT_info ctx_info;

    char* cmd;
    char* tid_str;
    char* context_str;

    cmd = strtok(line, ",");
    tid_str = strtok(0x0, ",");
    context_str = strtok(0x0, " ");

//    fprintf(stderr, "Checking consistency in TID: %s\n", tid_str);
    
    deserialize_context(&ctx_info, context_str);
    ctx_info.thread_id = strtol(tid_str, 0x0, 0x10);

    taint_eng->check_thread(ctx_info);

    return 0x0;
}

int register_update(char* line, taint_x86* taint_eng)
{
    char* cmd;
    DWORD offset, size;

    cmd = strtok(line, ",");
    offset = strtol(strtok(0x0, ","), 0x0, 0x10);
    size = strtol(strtok(0x0, ","), 0x0, 0x10);

    taint_eng->apply_memory(offset, size);

    return 0x0;
}


int register_security(char* line, taint_x86* taint_eng)
{
    char* cmd;
    DWORD offset, size;

    cmd = strtok(line, ",");
    offset = strtol(strtok(0x0, ","), 0x0, 0x10);
    size = strtol(strtok(0x0, ","), 0x0, 0x10);

    taint_eng->apply_security(offset, size);

    return 0x0;
}


int register_lib(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET off, size;
    char* name;

    cmd = strtok(line, ",");
    off = strtol(strtok(0x0, ","), 0x0, 0x10);
    name = strtok(0x0, ",");
    name[strlen(name)] = 0x0;
//    size = strtol(strtok(0x0, ","), 0x0, 0x10);

//    printf("Registering lib: %s at 0x%08x\n", name, off);
    taint_eng->add_lib(off, name);

    return 0x0;
}

int deregister_lib(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET off;

    cmd = strtok(line, ",");
    off = strtol(strtok(0x0, ","), 0x0, 0x10);

    //printf("Deregistering lib at 0x%08x\n", off);
    taint_eng->del_lib(off);

    return 0x0;

}

int deregister_thread(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* tid_str;
    char* context_str;
    cmd = strtok(line, ",");
    tid_str = strtok(0x0, ",");

    //printf("Deregistering TID: %s\n", tid_str);

    taint_eng->del_thread(strtol(tid_str, 0x0, 0x10));

    return 0x0;
}

int decode_instruction(DWORD* tid, OFFSET* addr, long long unsigned* instr_count, char* line)
{
    char* tid_s;
    char* addr_s;
    char* ic_s;

    tid_s = strtok(line, " ");
    addr_s = strtok(0x0, " ");
    ic_s = strtok(0x0, " ");


    *tid = strtol(tid_s, 0x0, 0x10);
    *addr = strtol(addr_s, 0x0, 0x10);
    *instr_count = strtol(ic_s, 0x0, 10);

    return 0x0;
}

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

    bp.offset = strtol(current, 0x0, 10);
    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }

    bp.mem_offset = strtol(current, 0x0, 0x10);
    current = strtok(0x0, "+");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    
    bp.mode = 0x0;
//    fprintf(stderr, "%s\n", current);
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

    twp.offset = strtol(current, 0x0, 10);
    fprintf(stderr, "Instr count: %lld\n", twp.offset);

    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    twp.mem_offset = strtol(current, 0x0, 0x10);
    fprintf(stderr, "Offset: 0x%08x\n", twp.mem_offset);

    current = strtok(0x0, ",");
    if(!current)
    {
        fprintf(stderr, "Error parsing breakpoint: %s\n", string);
        exit(1);
    }
    twp.tid = strtol(current, 0x0, 0x10);
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

void print_usage()
{
    printf("Usage: taint -i <instr> -d <dump> -o <dump2> -t <taint> -T <taint_dump> -l <lost> -p <prefix> -n <count>\n");
    printf("-i input  - forward instruction file\n");
    printf("-d input  - memory dump in t1\n");
    printf("-o input  - start addr\n");
    printf("-t input  - end addr\n");
    printf("-t input  - taint dump\n");
    printf("-l input  - analysis depth\n");
    printf("-L input  - loadable dir path\n");
    return;
}

void handle_sigsegv(int signum)
{
    taint_eng.handle_sigsegv();
    return;
}

void _pause()
{
    fprintf(stdout, "Press any key\n");
    getchar();
    return;
}

int main(int argc, char** argv)
{
    char structured_buffer[STRUCTURED_BUFFER_LENGTH];

    // new
    char instr_file_path[MAX_NAME];
    char dump1_file_path[MAX_NAME];
    char history_path[MAX_NAME];

    DWORD start_addr;
    DWORD start_instr;
    DWORD end_addr;
    DWORD depth;

    // ignored deprecated
    char dump2_file_path[MAX_NAME];
    char dump2_taint_file_path[MAX_NAME];
    char lost_file_path[MAX_NAME];
    char loadable_dir_path[MAX_NAME];
    char additional_info_prefix[MAX_NAME];
    char results_human_path[MAX_NAME];
    char results_structured_path[MAX_NAME];

    // optargs for breakpoints
    char breakpoint_optarg[MAX_NAME];
    char breakpoint_t_optarg[MAX_NAME];
    char watchpoint_optarg[MAX_NAME];

    char taint_size[0x10];

    CONTEXT_set ctx_set;
    OFFSET eip;
    DWORD tid;

    FILE* out;

    unsigned long long instr_limit, instr_count, last_instr_count;
    instr_limit = 0x0;
    last_instr_count = 0x0;
    instr_count = 0x0;
    unsigned max_levels = 0x0;

    char* line;
    size_t len = 0;
    ssize_t read;

    char enumerate = 0x0;

    int opt;
    extern char *optarg;

    /* started parsing args and preparing params */

    while ((opt = getopt(argc, argv, "M:i:d:s:e:T:l:m:b:t:w:h:D:I")) != -1) 
    {
        switch (opt) 
        {
            case 'M': 
                max_levels = strtol(optarg, 0x0, 0x10);
                break;
            case 'i': 
                strcpy(instr_file_path, optarg); 
                break;
            case 'd': 
                strcpy(dump1_file_path, optarg); 
                break;
            case 's': 
                if(optarg[0] == '0' && optarg[1] == 'x')
                    start_addr = strtol(optarg, 0x0, 0x10);
                else 
                    start_instr = strtol(optarg, 0x0, 10);
                printf("start_addr: 0x%08x, start_instr: %d\n", start_addr, start_instr);
                break;
            case 'e': 
                end_addr = -1;
                if(optarg[0] == '0' && optarg[1] == 'x')
                    end_addr = strtol(optarg, 0x0, 0x10);
                else 
                    instr_limit = strtol(optarg, 0x0, 10);
                break;
            case 'T': 
                strcpy(dump2_taint_file_path, optarg); 
                break;
            case 'l': 
                depth = strtol(optarg, 0x0, 0x10);
                break;
            case 'm': 
                strcpy(loadable_dir_path, optarg); 
                break;
            case 'b': 
                strcpy(breakpoint_optarg, optarg);
                break;
            case 't': 
                strcpy(breakpoint_t_optarg, optarg);
                break;
            case 'w': 
                strcpy(watchpoint_optarg, optarg);
                break;
            case 'h': 
                strcpy(history_path, optarg);
                break;
            case 'D': 
                depth = strtol(optarg, 0x0, 10);
                break;
            case 'I':
                enumerate = 0x1;
                break;

            default:
                print_usage();
                return 0x1;
        }
    }

    if((strlen(instr_file_path) == 0x0) 
        || (strlen(dump1_file_path) == 0x0)
        || (strlen(loadable_dir_path) == 0x0))
    {
        print_usage();
        return 0x1;
    }

    if(dump2_taint_file_path == 0x0)
    {
        fprintf(stderr, "No taint file path, will skip writing out taint data. If this is not what you wanted, interrupt now.\n");    
    }

    // expand from prefix
    strcat(results_human_path, additional_info_prefix);
    strcat(results_human_path, "_human.txt");
    strcat(results_structured_path, additional_info_prefix);
    strcat(results_structured_path, "_structured.txt");

    /* finished parsing args and preparing params */

    printf("Here we go - 2.0\n");

    printf("BYTE_t size: 0x%08x\n", sizeof(BYTE_t));

    /* handle SIGSEGV */
   // struct sigaction sa;

    
    signal(SIGSEGV, handle_sigsegv);

    taint_eng.bp_hit = 0x1;
    taint_eng.enumerate = enumerate;
    if(max_levels == 0x0) max_levels = MAX_CALL_LEVELS;

    taint_eng.max_call_levels = max_levels;
    taint_eng.call_level_start = max_levels/3;
    taint_eng.call_level_offset = max_levels/30;
    printf("Setting max levels to: 0x%08x, start: 0x%08x, offset: 0x%08x\n", max_levels, taint_eng.call_level_start, taint_eng.call_level_offset);
 
    parse_mem_breakpoints(breakpoint_optarg, &taint_eng);
    parse_taint_breakpoints(breakpoint_t_optarg, &taint_eng);
    parse_trace_watchpoints(watchpoint_optarg, &taint_eng);

    unsigned i;
    for(i=0x0; i< taint_eng.new_bpt_count; i++)
        if((taint_eng.new_bps[i].offset != 0x0) || (taint_eng.new_bps[i].mem_offset != 0x0))
        {
            fprintf(stderr, "Bp: %lld, 0x%08x, %d\n", taint_eng.new_bps[i].offset, taint_eng.new_bps[i].mem_offset, taint_eng.new_bps[i].mode);
        }

    for(i=0x0; i< taint_eng.new_bpt_t_count; i++)
        if((taint_eng.new_bps_t[i].offset != 0x0) || (taint_eng.new_bps_t[i].mem_offset != 0x0))
        {
            fprintf(stderr, "Bp_t: %lld, 0x%08x, %d\n", taint_eng.new_bps_t[i].offset, taint_eng.new_bps_t[i].mem_offset, taint_eng.new_bps_t[i].mode);
        }

    for(i=0x0; i< taint_eng.new_wpt_count; i++)
        if((taint_eng.new_wps[i].offset != 0x0) || (taint_eng.new_wps[i].mem_offset != 0x0))
        {
            fprintf(stderr, "Wp: %lld, 0x%08x\n", taint_eng.new_wps[i].offset, taint_eng.new_wps[i].mem_offset);
        }

    /* load data to engine */
    //taint_eng.print_all_contexts();

    taint_eng.load_mem_from_file(dump1_file_path);
    taint_eng.load_instr_from_file(instr_file_path);
    taint_eng.open_lost_file(lost_file_path);
//    taint_eng.set_lib_dir_path(loadable_dir_path);
    taint_eng.open_mod_file(loadable_dir_path);

    /* pass graph parameters */
    taint_eng.start_addr = start_addr;
    taint_eng.start_instr = start_instr;


    taint_eng.end_addr = end_addr;
    taint_eng.instr_limit = instr_limit;

    taint_eng.depth = depth;

//#define CEBUG 
#define DEBUG 
    /* break points */

#ifdef DEBUG
    int bp_idx = 0x0;

    taint_eng.my_bps[bp_idx].offset = 0x404832; taint_eng.my_bps[bp_idx++].mode = BP_MODE_WRITE;
//    taint_eng.my_bps[bp_idx]_offset = 0x0040308c; taint_eng.my_bps[bp_idx++].mode = BP_MODE_READ | BP_MODE_WRITE;
//    taint_eng.my_bps[bp_idx].offset = 0x0012fda8; taint_eng.my_bps[bp_idx++].mode = BP_MODE_READ | BP_MODE_WRITE;
//    taint_eng.my_bps[bp_idx].offset = 0x0012f79c; taint_eng.my_bps[bp_idx++].mode = BP_MODE_READ | BP_MODE_WRITE;
//    taint_eng.my_bps[bp_idx].offset = 0x0dd24774; taint_eng.my_bps[bp_idx++].mode = BP_MODE_READ | BP_MODE_WRITE;
//    taint_eng.my_bps[bp_idx].offset = 0x00010b6c; taint_eng.my_bps[bp_idx++].mode = BP_MODE_READ | BP_MODE_WRITE;
//    taint_eng.my_bps[bp_idx].offset = 0x0dd241ae; taint_eng.my_bps[bp_idx++].mode = BP_MODE_READ | BP_MODE_WRITE;
#endif

    bp_idx = 0x0;

//    taint_eng.t8[bp_idx].count = ; taint_eng.t8[bp_idx++].watched = &taint_eng.ctx_info[0].registers[EAX];
/*
    taint_eng.t8[bp_idx].count = 3495778; taint_eng.t8[bp_idx++].watched = &taint_eng.ctx_info[0].registers[ECX];
    taint_eng.t8[bp_idx].count = 886427; taint_eng.t8[bp_idx++].watched = &taint_eng.memory[0x0012f6a8];
    taint_eng.t8[bp_idx].count = 992877; taint_eng.t8[bp_idx++].watched = &taint_eng.ctx_info[0].registers[EDI];
    taint_eng.t8[bp_idx].count = 942799; taint_eng.t8[bp_idx++].watched = &taint_eng.ctx_info[0].registers[EAX];
*/

    bp_idx = 0x0;
    
    taint_eng.taint_checks[bp_idx++] = 98336; 

    /* printing values at start */

    printf("Watched RW locations:\n");
  
 
    for(i=0x0; i< bp_idx; i++)
    {
        if((taint_eng.my_bps[i].mode & BP_MODE_READ) || (taint_eng.my_bps[i].mode & BP_MODE_WRITE))
        {
            taint_eng.print_mem(3, taint_eng.my_bps[i].offset, 0x10);
        }
    }

    taint_eng.bp_hit = 0x0;
 
    /* executing instructions */
    while ((read = getline(&line, &len, taint_eng.instr_file)) != -1) 
    {
        if(line[0] != '0')
        {
            //printf("%s\n", line);
            //strcat(structured_buffer, line);
            // other operations
            if(line[0] == 'R' && line[1] == 'T')
                register_thread(line, &taint_eng);
    
            if(line[0] == 'D' && line[1] == 'T')
                deregister_thread(line, &taint_eng);
            
            if(line[0] == 'R' && line[1] == 'L')
                register_lib(line, &taint_eng);
            
            if(line[0] == 'D' && line[1] == 'L')
                deregister_lib(line, &taint_eng);
            
            if(line[0] == 'E' && line[1] == 'X')
                register_exception(line, &taint_eng);
            
            if(line[0] == 'R' && line[1] == 'N')
                register_taint(line, &taint_eng);

            if(line[0] == 'U' && line[1] == 'P')
                register_update(line, &taint_eng);
            
            if(line[0] == 'S' && line[1] == 'E')
                register_security(line, &taint_eng);
            
            if(line[0] == 'S' && line[1] == 'Y')
                register_symbol(line, &taint_eng);

            if(line[0] == 'B' && line[1] == 'L')
                register_blacklist(line, &taint_eng);

            if(line[0] == 'B' && line[1] == 'A')
                register_blacklist_addr(line, &taint_eng);

            if(line[0] == 'I' && line[1] == 'N')
                register_included(line, &taint_eng);

            if(line[0] == 'F' && line[1] == 'W')
                register_wanted(line, &taint_eng);

            if(line[0] == 'I' && line[1] == 'W')
                register_wanted_i(line, &taint_eng);

            if(line[0] == 'E' && line[1] == 'W')
                register_wanted_e(line, &taint_eng);

            if(line[0] == 'C' && line[1] == 'T')
                check_consistency(line, &taint_eng);

            if(line[0] == 'F' && line[1] == 'E')
                register_fence(line, &taint_eng);

            if(line[0] == 'P' && line[1] == 'A')
                _pause();

            if(line[0] == 'L' && line[1] == 'F')
                load_file(line, &taint_eng);

            if(line[0] == 'S' && line[1] == 'T')
                if(!(taint_eng.start_addr || taint_eng.start_instr))
                    taint_eng.started = 0x1;

            if(line[0] == 'F' && line[1] == 'I')
                taint_eng.finished = 0x1;

            if(line[0] == 'O' && line[1] == 'P')
                parse_option(line, &taint_eng);

            if(line[0] == '#'); //comment
            
            continue;
        }
        else
        {
            last_instr_count = instr_count;
            decode_instruction(&tid, &eip, &instr_count, line);
            if(taint_eng.aborted) 
            {
                printf("\nABORTED!!!\n\n");
                break;
            }
            //fprintf(stderr, "instruction %lld in TID: 0x%08x, offset: 0x%08x\n", instr_count, tid, eip);
            taint_eng.current_instr_count = instr_count;
            taint_eng.execute_instruction_at_eip(eip, tid);
//            printf("Affected thread:\n");
//            taint_eng.print_t_context(tid);
            taint_eng.verify_t_context(tid);
            //taint_eng.print_stack(3, 0x20);
            fflush(stdout);
#ifdef DEBUG
            if(taint_eng.bp_hit)
            {
#endif
#ifdef CEBUG
                printf("Press any key\n");
                getchar();
#else
                ;
#endif
            }
/*
            if(instr_limit > 0x0)
                if(instr_count == instr_limit)
                    break;
*/
        }
        if(0)
        if(instr_count == 13700000) 
        //if(instr_count == 13709825) 
        {
            fprintf(stderr, "Problem with instruction after %lld\n", last_instr_count);
            getchar();
        }

        if(!(instr_count % INSTR_REPORT_INTERVAL))
        {
            fprintf(stdout, "Processed %lld instructions\n", instr_count);
        }

    }
    taint_eng.finish();
 
    for(i = 0x0; i<taint_eng.tid_count; i++)
        taint_eng.del_thread_srsly(taint_eng.ctx_info[i].tid);
   
    fclose(taint_eng.instr_file);
    if(line)
        free(line); 

    /* Writing out analysis data */

    // dumping human-readable lib info, contexts & exceptions 
    printf("Writing human-readable taint information to %s\n", results_human_path);

    out = fopen(results_human_path, "w+");

    // last instruction number

    // last instruction offset

    // contexts in dump time
    DWORD tidn;

    fprintf(out, "[Threads]:\n");
    for(i=0x0; i<taint_eng.tid_count; i++)
    {
        tidn = taint_eng.ctx_info[i].tid;
        fprintf(out, "TID: 0x%08x\n", tidn);
        fprintf(out, " EAX:\t 0x%08x\n", taint_eng.reg_restore_32(EAX, tidn).get_DWORD());
        fprintf(out, " ECX:\t 0x%08x\n", taint_eng.reg_restore_32(ECX, tidn).get_DWORD());
        fprintf(out, " EDX:\t 0x%08x\n", taint_eng.reg_restore_32(EDX, tidn).get_DWORD());
        fprintf(out, " EBX:\t 0x%08x\n", taint_eng.reg_restore_32(EBX, tidn).get_DWORD());
        fprintf(out, " ESI:\t 0x%08x\n", taint_eng.reg_restore_32(ESI, tidn).get_DWORD());
        fprintf(out, " EDI:\t 0x%08x\n", taint_eng.reg_restore_32(EDI, tidn).get_DWORD());
        fprintf(out, " EBP:\t 0x%08x\n", taint_eng.reg_restore_32(EBP, tidn).get_DWORD());
        fprintf(out, " ESP:\t 0x%08x\n", taint_eng.reg_restore_32(ESP, tidn).get_DWORD());
        fprintf(out, " EFLAGS: 0x%08x\n", taint_eng.reg_restore_32(EFLAGS, tidn).get_DWORD());
        fprintf(out, " EIP:\t 0x%08x\n", taint_eng.reg_restore_32(EIP, tidn).get_DWORD());
        fprintf(out, "\n");
    }

    // libraries in dump time

    fprintf(out, "[Active libraries]:\n");
    for(i=0x0; i<taint_eng.libs_count; i++)
        if(taint_eng.libs[i].loaded)
            fprintf(out, "0x%08x: %s", taint_eng.libs[i].offset, taint_eng.libs[i].name);
    fprintf(out, "\n");

    fprintf(out, "[Inactive libraries]:\n");
    for(i=0x0; i<taint_eng.libs_count; i++)
        if(taint_eng.libs[i].loaded == 0x0)
            fprintf(out, "0x%08x: %s", taint_eng.libs[i].offset, taint_eng.libs[i].name);
    fprintf(out, "\n");

    fprintf(out, "[Exceptions]:\n");
    for(i=0x0; i<taint_eng.exceptions_count; i++)
        fprintf(out, " Exc. code: \t0x%08x\n TID: \t\t0x%08x\n Address: \t0x%08x\n Flags: \t0x%08x\n", taint_eng.exceptions[i].er.ExceptionCode, taint_eng.exceptions[i].tid, taint_eng.exceptions[i].er.ExceptionAddress, taint_eng.exceptions[i].er.ExceptionFlags);
    fprintf(out, "\n");

    fclose(out);

    // dumping structured lib info, contexts & exceptions 
    printf("Writing structured taint information to %s\n", results_structured_path);

    out = fopen(results_structured_path, "w+");
    fwrite(structured_buffer, strlen(structured_buffer), 0x1, out);
    fclose(out);
/*
    // dumping memory
    printf("Dumping memory: 0x0 - 0x%08x to %s\n", taint_eng.mem_length, dump2_file_path);
    out = fopen(dump2_file_path, "w");    
    write_region(out, taint_eng.memory, taint_eng.mem_length);
    fclose(out);
*/    

    // dumping memory taint
    printf("Dumping taint region: 0x0 - 0x%08x to %s\n", taint_eng.mem_length, dump2_taint_file_path);
    if(strlen(dump2_taint_file_path) > 0x0)
    {
        out = fopen(dump2_taint_file_path, "w");    
        write_t_region(out, taint_eng.memory, taint_eng.mem_length);
        fclose(out);
    }
  
    if(strlen(history_path) > 0x0)
    {
        out = fopen(history_path, "w");    
        taint_eng.write_history(out);
        fclose(out);
    }


    taint_eng.close_files();

    return 0x0;
}
