#include <out_utils.h>
#include <utils.h>
#include <taint_emul_x86.h>
#include <stdio.h>

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
    off = (OFFSET)strtoul(strtok(0x0, ","), 0x0, 0x10);

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

unsigned get_instr_interval(char* line)
{
    char* cmd;
    unsigned interval;

    cmd = strtok(line, ",");
    interval = strtoul(strtok(0x0, ","), 0x0, 10);

    return interval;
}

int register_blacklist_addr(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET addr;

    cmd = strtok(line, ",");
    addr = strtoul(strtok(0x0, ","), 0x0, 0x10);
    taint_eng->add_blacklist_addr(addr);

    return 0x0;
}
int register_silenced_addr(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET addr;

    cmd = strtok(line, ",");
    addr = strtoul(strtok(0x0, ","), 0x0, 0x10);
    taint_eng->add_silenced_addr(addr);

    return 0x0;
}

int register_wanted_i(char* line, taint_x86* taint_eng)
{
    char* cmd;
    unsigned instr;

    cmd = strtok(line, ",");
    instr = strtoul(strtok(0x0, ","), 0x0, 10);
    taint_eng->add_wanted_i(instr);

    return 0x0;
}

int register_wanted_e(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET addr;

    cmd = strtok(line, ",");
    addr = strtoul(strtok(0x0, ","), 0x0, 0x10);
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
    entry = strtoul(strtok(0x0, ","), 0x0, 0x10);
    start = strtoul(strtok(0x0, ","), 0x0, 0x10);
    size = strtoul(strtok(0x0, ","), 0x0, 0x10);
    count  = strtoul(strtok(0x0, ","), 0x0, 0x10);
    taint_eng->add_fence(entry, start, size, count);

    return 0x0;
}

int load_mod(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* file_name;

    cmd = strtok(line, ",");
    file_name = strtok(0x0, ",");
    file_name[strlen(file_name)-1] = 0x0;
    
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
    file_name[strlen(file_name)-1] = 0x0;
    
    printf("Loading memory from: %s\n", file_name);

    filter_str(file_name);

    taint_eng->load_mem_from_file(file_name);

    return 0x0;
}

int load_file(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* file_name;
    int ret;

    cmd = strtok(line, ",");
    file_name = strtok(0x0, ",");
    file_name[strlen(file_name)-1] = 0x0;
    
    printf("Switching instr file to %s\n", file_name);

    fclose(taint_eng->instr_file);
    return taint_eng->load_instr_from_file(file_name);
}

int comment_out(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* comment;
    DWORD tid;

    cmd = strtok(line, ",");
    tid = strtol(strtok(0x0, ","), 0x0, 0x10);
    comment = strtok(0x0, ",");
    comment[strlen(comment)-1] = 0x0;

    taint_eng->comment_out(comment, tid);

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
    else if(!strcmp(option, "ANALYZE_WANTED_IN_SYMBOLS"))
    {
        taint_eng->options |= OPTION_ANALYZE_WANTED_IN_SYMBOLS;
    }

    else if(!strcmp(option, "COUNT_INSTRUCTIONS"))
    {
        taint_eng->options |= OPTION_COUNT_INSTRUCTIONS;
    }

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
    OFFSET offset, size;

    cmd = strtok(line, ",");
    offset = strtoul(strtok(0x0, ","), 0x0, 0x10);
    size = strtoul(strtok(0x0, ","), 0x0, 0x10);

    taint_eng->apply_memory(offset, size);

    return 0x0;
}

int set_prefix(char* line, taint_x86* taint_eng)
{
    char* cmd;
    char* prefix;

    cmd = strtok(line, ",");
    prefix = strtok(0x0, "\n");

    printf("Setting prefix to: %s\n", prefix);
    filter_str_2(prefix, '\x0d');
    taint_eng->set_prefix(prefix);

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

int register_lib(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET off, size;
    char* name;

    cmd = strtok(line, ",");
    off = strtoul(strtok(0x0, ","), 0x0, 0x10);
    name = strtok(0x0, ",");
    name[strlen(name)] = 0x0;
//    size = strtoul(strtok(0x0, ","), 0x0, 0x10);

//    printf("Registering lib: %s at 0x%08x\n", name, off);
    taint_eng->add_lib(off, name);

    return 0x0;
}

int deregister_lib(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET off;

    cmd = strtok(line, ",");
    off = strtoul(strtok(0x0, ","), 0x0, 0x10);

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


