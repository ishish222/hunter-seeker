#include <out_utils.h>
#include <utils.h>
#include <emul_x86.h>
#include <stdio.h>
#include <breakpoint.h>

void deserialize_context(CONTEXT_OUT* ctx_out, char* buffer)
{
    unsigned i;
    DWORD tempHW;

//    printf("Parsing: %s\n", buffer);

    ctx_out->ctx.Eax = strtol(strtok(buffer, ","), 0x0, 0x10);
    ctx_out->ctx.Ecx = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.Edx = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.Ebx = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.Esi = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.Edi = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.Ebp = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.Esp = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.EFlags = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.Eip = strtol(strtok(0x0, ","), 0x0, 0x10);

    ctx_out->ctx.SegGs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.SegFs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.SegEs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.SegDs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.SegCs = strtol(strtok(0x0, ","), 0x0, 0x10);
    ctx_out->ctx.SegSs = strtol(strtok(0x0, ","), 0x0, 0x10);

    for(i=0x0; i<0x6; i++)
    {
        ctx_out->ldt[i].LimitLow = strtol(strtok(0x0, ","), 0x0, 0x10);
//        printf("LimitLow: 0x%08x\n", ctx_out->ldt[i].LimitLow);
        ctx_out->ldt[i].BaseLow = strtol(strtok(0x0, ","), 0x0, 0x10);
//        printf("BaseLow: 0x%08x\n", ctx_out->ldt[i].BaseLow);
        tempHW = strtol(strtok(0x0, ","), 0x0, 0x10);
        *((DWORD*)&ctx_out->ldt[i].HighWord) = tempHW;
//        printf("HighWord: 0x%08x\n", tempHW);
    }

    return;
}

unsigned get_instr_interval(char* line)
{
    char* cmd;
    unsigned interval;

    cmd = strtok(line, ",");
    interval = strtoul(strtok(0x0, ","), 0x0, 10);

    return interval;
}

int load_mod(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* file_name;

    cmd = strtok(line, ",");
    file_name = strtok(0x0, ",");
    //file_name[strlen(file_name)-1] = 0x0;
    
    printf("Opening mod file: %s\n", file_name);

    filter_str(file_name);

    taint_eng->open_mod_file(file_name);

    return 0x0;
}

int load_mem(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* file_name;

    cmd = strtok(line, ",");
    file_name = strtok(0x0, ",");
    //file_name[strlen(file_name)-1] = 0x0;
    
    printf("Loading memory from: %s\n", file_name);

    filter_str(file_name);

    taint_eng->load_mem_from_file(file_name);
    printf("Loaded\n");

    return 0x0;
}

int load_file(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* file_name;
    int ret;

    cmd = strtok(line, ",");
    file_name = strtok(0x0, ",");
    //file_name[strlen(file_name)-1] = 0x0;
    
    printf("Switching instr file to %s\n", file_name);

    fclose(taint_eng->instr_file);
    return taint_eng->load_instr_from_file(file_name);
}

/* it will be necessary to move some options to plugin */
int parse_option(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* option;

    cmd = strtok(line, ",");
    option = strtok(0x0, ",");
    
//    option[strlen(option)-1] = 0x0;

    printf("Enabling analysis option: %s\n", option);

    /* positive */
    if(!strcmp(option, "ANALYZE_JUMPS"))                        taint_eng->options |= OPTION_ANALYZE_JUMPS;
    else if(!strcmp(option, "ANALYZE_LOOPS"))                   taint_eng->options |= OPTION_ANALYZE_LOOPS;
    else if(!strcmp(option, "UNMATCHED_RET_INVALIDATES_STACK")) taint_eng->options |= OPTION_UNMATCHED_RET_INVALIDATES_STACK;
    else if(!strcmp(option, "UNMATCHED_RET_CREATES_CALL"))      taint_eng->options |= OPTION_UNMATCHED_RET_CREATES_CALL;
    else if(!strcmp(option, "NOT_EMITTING_BLACKLISTED"))        taint_eng->options |= OPTION_NOT_EMITTING_BLACKLISTED;
    else if(!strcmp(option, "VERIFY_ROP_RETS"))                 taint_eng->options |= OPTION_VERIFY_ROP_RETS;
    else if(!strcmp(option, "VERIFY_SEG_SEC"))                  taint_eng->options |= OPTION_VERIFY_SEG_SEC;
    else if(!strcmp(option, "ANALYZE_WANTED_IN_SYMBOLS"))       taint_eng->options |= OPTION_ANALYZE_WANTED_IN_SYMBOLS;
    else if(!strcmp(option, "COUNT_INSTRUCTIONS"))              taint_eng->options |= OPTION_COUNT_INSTRUCTIONS;
    else if(!strcmp(option, "INDEXES_PROPAGATE"))               taint_eng->options |= OPTION_INDEXES_PROPAGATE;

    /* negative */
    else if(!strcmp(option, "DONT_ANALYZE_JUMPS"))                   taint_eng->options &= OPTION_DONT_ANALYZE_JUMPS;
    else if(!strcmp(option, "DONT_ANALYZE_LOOPS"))                   taint_eng->options &= OPTION_DONT_ANALYZE_LOOPS;
    else if(!strcmp(option, "DONT_UNMATCHED_RET_INVALIDATES_STACK")) taint_eng->options &= OPTION_DONT_UNMATCHED_RET_INVALIDATES_STACK;
    else if(!strcmp(option, "DONT_UNMATCHED_RET_CREATES_CALL"))      taint_eng->options &= OPTION_DONT_UNMATCHED_RET_CREATES_CALL;
    else if(!strcmp(option, "DONT_NOT_EMITTING_BLACKLISTED"))        taint_eng->options &= OPTION_DONT_NOT_EMITTING_BLACKLISTED;
    else if(!strcmp(option, "DONT_VERIFY_ROP_RETS"))                 taint_eng->options &= OPTION_DONT_VERIFY_ROP_RETS;
    else if(!strcmp(option, "DONT_VERIFY_SEG_SEC"))                  taint_eng->options &= OPTION_DONT_VERIFY_SEG_SEC;
    else if(!strcmp(option, "DONT_ANALYZE_WANTED_IN_SYMBOLS"))       taint_eng->options &= OPTION_DONT_ANALYZE_WANTED_IN_SYMBOLS;
    else if(!strcmp(option, "DONT_COUNT_INSTRUCTIONS"))              taint_eng->options &= OPTION_DONT_COUNT_INSTRUCTIONS;
    else if(!strcmp(option, "DONT_INDEXES_PROPAGATE"))               taint_eng->options &= OPTION_DONT_INDEXES_PROPAGATE;

    return 0x0;
}


int register_thread(char* line, taint_x86* taint_eng)
{
    CONTEXT_OUT ctx_out;

    char* cmd;
    char* tid_str;
    char* context_str;

    cmd = strtok(line, ",");
    tid_str = strtok(0x0, ",");
    context_str = strtok(0x0, " ");

    deserialize_context(&ctx_out, context_str);
    ctx_out.thread_id = strtol(tid_str, 0x0, 0x10);

    taint_eng->add_thread(ctx_out);

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
    CONTEXT_OUT ctx_out;

    char* cmd;
    char* tid_str;
    char* context_str;

    cmd = strtok(line, ",");
    tid_str = strtok(0x0, ",");
    context_str = strtok(0x0, " ");

//    fprintf(stderr, "Checking consistency in TID: %s\n", tid_str);
    
    deserialize_context(&ctx_out, context_str);
    ctx_out.thread_id = strtol(tid_str, 0x0, 0x10);

    taint_eng->check_thread(ctx_out);

    return 0x0;
}

int check_location(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET offset, size;

    cmd = strtok(line, ",");
    offset = strtoul(strtok(0x0, ","), 0x0, 0x10);
    size = strtoul(strtok(0x0, ","), 0x0, 0x10);

    taint_eng->check_memory(offset, size);

    return 0x0;
}

int register_update(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET offset, size;

    cmd = strtok(line, ",");
    offset = strtoul(strtok(0x0, ","), 0x0, 0x10);
    size = strtoul(strtok(0x0, ","), 0x0, 0x10);

    taint_eng->apply_memory(offset, size);

    return 0x0;
}

int register_security(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET offset, size;

    cmd = strtok(line, ",");
    offset = strtoul(strtok(0x0, ","), 0x0, 0x10);
    size = strtoul(strtok(0x0, ","), 0x0, 0x10);

    taint_eng->apply_security(offset, size);

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

int register_memory_breakpoints(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* entry;

    cmd = strtok(line, ",");
    entry = strtok(0x0, "");

    parse_mem_breakpoints(entry, taint_eng);

    return 0x0;
}

int register_memory_breakpoints_2(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* entry;

    entry = strtok(line, "");

    parse_mem_breakpoints(entry, taint_eng);

    return 0x0;
}

int register_taint_breakpoints(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* entry;

    cmd = strtok(line, ",");
    entry = strtok(0x0, "");

    parse_taint_breakpoints(entry, taint_eng);

    return 0x0;
}

int parse_prompt_file(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* filepath;

    cmd = strtok(line, ",");
    filepath = strtok(0x0, "");

    printf("Opening prompt file: %s\n", filepath);

    taint_eng->open_prompt_file(filepath);

    return 0x0;
}

