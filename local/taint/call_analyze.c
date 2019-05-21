/*
*   Tomasz Salacinski
*   Korrino
*   tomasz.salacinski@korrino.com
*/

#include <taint_emul_x86.h>
#include <out_utils.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <utils.h>
#include <breakpoint.h>
#include <graph_engine.h>

#define STRUCTURED_BUFFER_LENGTH 0x10000
#define MAX_LINE 0x1000
#define INSTR_REPORT_INTERVAL 1000000

taint_x86 taint_eng;
graph_engine graph_eng;


int decode_instruction(DWORD* tid, OFFSET* addr, long long unsigned* instr_count, char* line)
{
    char* tid_s;
    char* addr_s;
    char* ic_s;

    tid_s = strtok(line, " ");
    addr_s = strtok(0x0, " ");
    ic_s = strtok(0x0, " ");


    *tid = strtol(tid_s, 0x0, 0x10);
    *addr = strtoul(addr_s, 0x0, 0x10);
    *instr_count = strtoul(ic_s, 0x0, 10);

    return 0x0;
}


void handle_sigint(int signum)
{
    taint_eng.handle_sigint();
    return;
}

void handle_sigsegv(int signum)
{
    taint_eng.handle_sigsegv();
    return;
}

int main(int argc, char** argv)
{
    char structured_buffer[STRUCTURED_BUFFER_LENGTH];
    unsigned instr_report_interval = INSTR_REPORT_INTERVAL;

    // new
    char dump1_file_path[MAX_NAME];
    char instr_file_path[MAX_NAME];
    char history_path[MAX_NAME];

    DWORD start_addr;
    DWORD start_instr;
    DWORD end_addr;
    DWORD depth;

    // ignored deprecated
    char dump2_file_path[MAX_NAME];
    char dump2_taint_file_path[MAX_NAME];
    char lost_file_path[MAX_NAME];
    char additional_info_prefix[MAX_NAME];
    char results_human_path[MAX_NAME];
    char results_structured_path[MAX_NAME];

    // optargs for breakpoints
    char breakpoint_optarg[MAX_NAME];
    char breakpoint_t_optarg[MAX_NAME];
    char watchpoint_optarg[MAX_NAME];

    OFFSET eip;
    DWORD tid;

    FILE* out;

    unsigned long long instr_limit, instr_count, last_instr_count;
    instr_limit = 0x0;
    last_instr_count = 0x0;
    instr_count = 0x0;
    unsigned max_levels = 0x0;

    char enumerate = 0x0;


    /* started parsing args and preparing params */

    int opt;
    extern char *optarg;

    while ((opt = getopt(argc, argv, "M:i:d:s:E:e:T:l:m:b:t:w:h:D:I")) != -1) 
    {
        switch (opt) 
        {
            case 'M': 
                max_levels = strtoul(optarg, 0x0, 0x10);
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
            case 'E': 
                end_addr = -1;
                if(optarg[0] == '0' && optarg[1] == 'x')
                    end_addr = strtol(optarg, 0x0, 0x10);
                else
                { 
                    instr_limit = strtol(optarg, 0x0, 10);
                    instr_limit *= 1000000;
                }
                    
                break;
            case 'e': 
                end_addr = -1;
                if(optarg[0] == '0' && optarg[1] == 'x')
                    end_addr = strtol(optarg, 0x0, 0x10);
                else
                { 
                    instr_limit = strtol(optarg, 0x0, 10);
                }
                    
                break;
            case 'T': 
                strcpy(dump2_taint_file_path, optarg); 
                break;
            case 'l': 
                depth = strtol(optarg, 0x0, 0x10);
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

    if(strlen(instr_file_path) == 0x0) 
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

    printf("Here we go - 3.0\n");

    printf("BYTE_t size: 0x%08x\n", sizeof(BYTE_t));

    /* handle SIGSEGV */
   // struct sigaction sa;

    
    signal(SIGSEGV, handle_sigsegv);
    signal(SIGINT, handle_sigint);

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
    for(i=0x0; i< taint_eng.bpt_count; i++)
        if((taint_eng.bps[i].offset != 0x0) || (taint_eng.bps[i].mem_offset != 0x0))
        {
            fprintf(stderr, "Bp: %lld, 0x%08x, %d\n", taint_eng.bps[i].offset, taint_eng.bps[i].mem_offset, taint_eng.bps[i].mode);
        }

    for(i=0x0; i< taint_eng.bpt_t_count; i++)
        if((taint_eng.bps_t[i].offset != 0x0) || (taint_eng.bps_t[i].mem_offset != 0x0))
        {
            fprintf(stderr, "Bp_t: %lld, 0x%08x, %d\n", taint_eng.bps_t[i].offset, taint_eng.bps_t[i].mem_offset, taint_eng.bps_t[i].mode);
        }

    for(i=0x0; i< taint_eng.wpt_count; i++)
        if((taint_eng.wps[i].offset != 0x0) || (taint_eng.wps[i].mem_offset != 0x0))
        {
            fprintf(stderr, "Wp: %lld, 0x%08x\n", taint_eng.wps[i].offset, taint_eng.wps[i].mem_offset);
        }

    /* load data to engine */
    //taint_eng.print_all_contexts();

    taint_eng.load_instr_from_file(instr_file_path);

    /* pass graph parameters */
    taint_eng.start_addr = start_addr;
    taint_eng.start_instr = start_instr;

    taint_eng.end_addr = end_addr;
    taint_eng.instr_limit = instr_limit;

    taint_eng.depth = depth;
    taint_eng.depth += taint_eng.call_level_start; /* because we do not start at 0x0 */

    taint_eng.bp_hit = 0x0;

    /* registering callbacks */
    taint_eng.start_callback = (callback_type_2)&graph_engine::start_callback;
    taint_eng.finish_callback = (callback_type_2)&graph_engine::finish_callback;
    taint_eng.pre_execute_instruction_callback = (callback_type_1)&graph_eng.pre_execute_instruction_callback;
    taint_eng.post_execute_instruction_callback = (callback_type_1)&graph_eng.post_execute_instruction_callback;
    printf("pre_execute_instruction_callback: %d\n", taint_eng.pre_execute_instruction_callback);
 
    /* executing instructions */
    char* line;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, taint_eng.instr_file)) != -1) 
    {
        if(taint_eng.aborted) 
        {
            printf("\nABORTED!!!\n\n");
            break;
        }
        if(line[0] != '0')
        {
            //printf("%s\n", line);
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
            
            if(line[0] == 'S' && line[1] == 'P')
                set_prefix(line, &taint_eng);

            if(line[0] == 'S' && line[1] == 'Y')
                register_symbol(line, &taint_eng);

            if(line[0] == 'B' && line[1] == 'L')
                register_blacklist(line, &taint_eng);

            if(line[0] == 'I' && line[1] == 'R')
                instr_report_interval = get_instr_interval(line);

            if(line[0] == 'B' && line[1] == 'A')
                register_blacklist_addr(line, &taint_eng);

            if(line[0] == 'S' && line[1] == 'A')
                register_silenced_addr(line, &taint_eng);

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

            if(line[0] == 'J' && line[1] == 'E')
                taint_eng.jxx_set(0x1);

            if(line[0] == 'J' && line[1] == 'D')
                taint_eng.jxx_set(0x0);

            if(line[0] == 'J' && line[1] == 'C')
                taint_eng.jxx_clear();

            if(line[0] == 'P' && line[1] == 'A')
                _pause();

            if(line[0] == 'O' && line[1] == 'M')
                load_mod(line, &taint_eng);

            if(line[0] == 'L' && line[1] == 'M')
                load_mem(line, &taint_eng);

            if(line[0] == 'L' && line[1] == 'F')
                if(load_file(line, &taint_eng) != 0x0)
                    taint_eng.aborted = 1;

            if(line[0] == 'S' && line[1] == 'T')
//                if(!(taint_eng.start_addr || taint_eng.start_instr))
                {
                    taint_eng.start();
                }

            if(line[0] == 'F' && line[1] == 'I')
                taint_eng.finished = 0x1;

            if(line[0] == 'O' && line[1] == 'U')
                comment_out(line, &taint_eng);

            if(line[0] == 'O' && line[1] == 'P')
                parse_option(line, &taint_eng);

            if(line[0] == '#'); //comment
            
            continue;
        }
        else
        {
            last_instr_count = instr_count;
            decode_instruction(&tid, &eip, &instr_count, line);
            //fprintf(stderr, "instruction %lld in TID: 0x%08x, offset: 0x%08x\n", instr_count, tid, eip);
            taint_eng.current_instr_count = instr_count;
            taint_eng.execute_instruction_at_eip(eip, tid);
//            printf("Affected thread:\n");
//            taint_eng.print_t_context(tid);
            taint_eng.verify_t_context(tid);
            //taint_eng.print_stack(3, 0x20);
            fflush(stdout);
        }

        if(!(instr_count % instr_report_interval))
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

    if(taint_eng.options & OPTION_COUNT_INSTRUCTIONS)
    {
        printf("Calculating executed unique instructions\n");
        unsigned i;
        unsigned executed = 0x0;
        for(i=0x0; i< taint_eng.mem_length; i++)
        {
            if(taint_eng.memory[i].get_BYTE_t())
            {
                executed++;
            }
        }
        printf("Executed 0x%08x (%d) unique instructions\n", executed, executed);
    }

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
