/*
*   Tomasz Salacinski
*   CERT Orange Polska
*   tomasz.salacinski@orange.com
*/

#include <taint_x86.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define STRUCTURED_BUFFER_LENGTH 0x10000

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
        fwrite(&src[i].val_t, 1, 1, f);

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

    printf("Parsing: %s\n", buffer);

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

    printf("Registering taint at: 0x%08x, with length: 0x%08x\n", off, len);
    taint_eng->add_taint(off, len);
    taint_eng->print_t_mem(off - 0x5, 0x10);

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

    printf("Registering TID: %s\n", tid_str);
    
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

int register_lib(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET off;
    char* name;

    cmd = strtok(line, ",");
    off = strtol(strtok(0x0, ","), 0x0, 0x10);
    name = strtok(0x0, ",");
    name[strlen(name)] = 0x0;

    printf("Registering lib: %s at 0x%08x\n", name, off);
    taint_eng->add_lib(off, name);

    return 0x0;
}

int deregister_lib(char* line, taint_x86* taint_eng)
{
    char* cmd;
    OFFSET off;

    cmd = strtok(line, ",");
    off = strtol(strtok(0x0, ","), 0x0, 0x10);

    printf("Deregistering lib at 0x%08x\n", off);
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

    printf("Deregistering TID: %s\n", tid_str);

    taint_eng->del_thread(strtol(tid_str, 0x0, 0x10));

    return 0x0;
}

int decode_instruction(DWORD* tid, OFFSET* addr, char* line)
{
    char* tid_s;
    char* addr_s;

    tid_s = strtok(line, " ");
    addr_s = strtok(0x0, " ");


    *tid = strtol(tid_s, 0x0, 0x10);
    *addr = strtol(addr_s, 0x0, 0x10);

    return 0x0;
}

void print_usage()
{
    printf("Usage: taint -i <instr> -d <dump> -o <dump2> -t <taint> -l <lost> -p <prefix> -n <count>\n");
    printf("-i input  - REVERSE forward instruction file\n");
    printf("-d input  - memory dump in t2\n");
    printf("-o output - memory dump in t1 (insignificant)\n");
    printf("-t output - taint in t1\n");
    printf("-l output - path to file with REVERSE lost values\n");
    printf("-p input  - prefix for additional result information\n");
    printf("-n input  - number of instructions\n");
    return;
}

int main(int argc, char** argv)
{
    char structured_buffer[STRUCTURED_BUFFER_LENGTH];

    // new
    char rev_instr_file_path[MAX_NAME];
    char dump2_file_path[MAX_NAME];
    char dump1_file_path[MAX_NAME];
    char dump1_taint_file_path[MAX_NAME];
    char rev_lost_file_path[MAX_NAME];
    char additional_info_prefix[MAX_NAME];
    char results_human_path[MAX_NAME];
    char results_structured_path[MAX_NAME];
    char taint_size[0x10];

    CONTEXT_set ctx_set;
    OFFSET eip;
    DWORD tid;

    FILE* out;

    unsigned int instr_limit, instr_count;
    instr_limit = 0x0;
    instr_count = 0x0;

    char* line;
    size_t len = 0;
    ssize_t read;

    char reverse = 0x0;

    int opt;
    extern char *optarg;

    /* started parsing args and preparing params */

    while ((opt = getopt(argc, argv, "i:d:o:t:l:p:n:")) != -1) 
    {
        switch (opt) 
        {
            case 'i': 
                strcpy(rev_instr_file_path, optarg); 
                break;
            case 'd': 
                strcpy(dump1_file_path, optarg); 
                break;
            case 'o': 
                strcpy(dump2_file_path, optarg); 
                break;
            case 't': 
                strcpy(dump1_taint_file_path, optarg); 
                break;
            case 'l': 
                strcpy(rev_lost_file_path, optarg); 
                break;
            case 'p': 
                strcpy(additional_info_prefix, optarg); 
                break;
            case 'n': 
                instr_limit = strtol(optarg, 0x0, 0x10); 
                break;
            default:
                print_usage();
                return 0x1;
        }
    }

    if((strlen(rev_instr_file_path) == 0x0) 
        || (strlen(dump1_file_path) == 0x0) 
        || (strlen(dump2_file_path) == 0x0) 
        || (strlen(dump1_taint_file_path) == 0x0) 
        || (strlen(rev_lost_file_path) == 0x0) 
        || (strlen(additional_info_prefix) == 0x0))
    {
        print_usage();
        return 0x1;
    }

    // expand from prefix
    strcat(results_human_path, additional_info_prefix);
    strcat(results_human_path, "_human.txt");
    strcat(results_structured_path, additional_info_prefix);
    strcat(results_structured_path, "_structured.txt");

    /* finished parsing args and preparing params */

    printf("Here we go\n");
    taint_x86 taint_eng;

#define CEBUG 
#define DEBUG 
    /* break points */

#if 1
    int bp_idx = 0x0;

    taint_eng.my_bps[bp_idx++] = 0x018ab18b;
    taint_eng.my_bps[bp_idx++] = 0xffffffff;
    taint_eng.my_bps[bp_idx++] = 0xffffffff;
    taint_eng.my_bps[bp_idx++] = 0xffffffff;
#endif

    /* load data to engine */
    taint_eng.print_all_contexts();

    taint_eng.load_mem_from_file(dump1_file_path);
    taint_eng.load_instr_from_file(rev_instr_file_path);
    taint_eng.open_lost_file(rev_lost_file_path);

    /* executing instructions */
    while ((read = getline(&line, &len, taint_eng.instr_file)) != -1) {
        if(line[0] != '0')
        {
            strcat(structured_buffer, line);
            // other operations
            if(line[0] == 'R' && line[1] == 'T')
                register_thread(line, &taint_eng);
    
            if(line[0] == 'D' && line[1] == 'T')
                deregister_thread(line, &taint_eng);
            
            if(line[0] == 'R' && line[1] == 'L')
                register_lib(line, &taint_eng);
            
            if(line[0] == 'D' && line[1] == 'L')
                deregister_lib(line, &taint_eng);
            
            if(line[0] == 'R' && line[1] == 'E')
                register_exception(line, &taint_eng);
            
            if(line[0] == 'R' && line[1] == 'N')
            {
                register_taint(line, &taint_eng);
            }
            if(line[0] == '#'); //comment
            
            continue;
        }
        else
        {
            decode_instruction(&tid, &eip, line);
            printf("instruction in TID: 0x%08x, offset: 0x%08x\n", tid, eip);
            taint_eng.execute_instruction_at_eip(eip, tid);
            printf("Affected thread:\n");
            taint_eng.print_context(tid);
            taint_eng.print_stack(0x6);
#ifdef DEBUG
            if(taint_eng.bp_hit)
#endif
#ifdef CEBUG
                getchar();
#else
                ;
#endif
            instr_count++;
            if(instr_limit > 0x0)
                if(instr_count == instr_limit)
                    break;
        }
    }
    
    fclose(taint_eng.instr_file);
    if (line)
        free(line); 

    /* Writing out analysis data */

    // dumping human-readable lib info, contexts & exceptions 
    printf("Writing human-readable taint information to %s\n", results_human_path);

    out = fopen(results_human_path, "w+");

    // last instruction number

    // last instruction offset

    // contexts in dump time
    int i;
    DWORD tidn;

    fprintf(out, "[Threads]:\n");
    for(i=0x0; i<taint_eng.tid_count; i++)
    {
        tidn = taint_eng.tids[i];
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

    // dumping memory
    printf("Dumping memory: 0x0 - 0x%08x to %s\n", taint_eng.mem_length, dump2_file_path);
    out = fopen(dump2_file_path, "w");    
    write_region(out, taint_eng.memory, taint_eng.mem_length);
    fclose(out);
    

    // dumping memory taint
    printf("Dumping taint region: 0x0 - 0x%08x to %s\n", taint_eng.mem_length, dump1_taint_file_path);
    out = fopen(dump1_taint_file_path, "w");    
    write_t_region(out, taint_eng.memory, taint_eng.mem_length);
    fclose(out);
    
    taint_eng.close_files();

    return 0x0;
}
