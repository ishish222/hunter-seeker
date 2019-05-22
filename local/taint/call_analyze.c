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
    taint_eng.handle_sigint(signum);
    return;
}

void handle_sigsegv(int signum)
{
    taint_eng.handle_sigsegv(signum);
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
    fprintf(stderr, "taint_eng.ctx_info: 0x%08x\n", &taint_eng.ctx_info[0]);


    taint_eng.bp_hit = 0x0;

    /* registering plugin */
    graph_engine graph_eng;
    taint_eng.plugin = (Plugin*)&graph_eng;
            fprintf(stderr, "taint_eng->ctx_info: 0x%08x\n", taint_eng.ctx_info);
    taint_eng.plugin->taint_eng = &taint_eng;

//    graph_engine* graph_eng;
            fprintf(stderr, "taint_eng->ctx_info: 0x%08x\n", taint_eng.ctx_info);
//    graph_eng = (graph_engine*) &taint_eng.plugin;

    /* configuring plugin */
    if(max_levels == 0x0) max_levels = MAX_CALL_LEVELS;
    graph_eng.enumerate = enumerate;
    graph_eng.max_call_levels = max_levels;
    graph_eng.call_level_start = max_levels/3;
    graph_eng.call_level_offset = max_levels/30;
    printf("Setting max levels to: 0x%08x, start: 0x%08x, offset: 0x%08x\n", max_levels, graph_eng.call_level_start, graph_eng.call_level_offset);
    graph_eng.depth = depth;
    graph_eng.depth += graph_eng.call_level_start; /* because we do not start at 0x0 */
 
    fprintf(stderr, "taint_eng.ctx_info: 0x%08x\n", &taint_eng.ctx_info[0]);
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
            
            if(line[0] == 'E' && line[1] == 'X')
                register_exception(line, &taint_eng);
            
            if(line[0] == 'R' && line[1] == 'N')
                register_taint(line, &taint_eng);

            if(line[0] == 'U' && line[1] == 'P')
                register_update(line, &taint_eng);
            
            if(line[0] == 'S' && line[1] == 'E')
                register_security(line, &taint_eng);
            
            if(line[0] == 'I' && line[1] == 'R')
                instr_report_interval = get_instr_interval(line);

            if(line[0] == 'C' && line[1] == 'T')
                check_consistency(line, &taint_eng);

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

            if(line[0] == 'O' && line[1] == 'P')
                parse_option(line, &taint_eng);

            /* pass to plugin */
            if(line[0] == 'S' && line[1] == 'Y')
                taint_eng.plugin->parse_option(line);
                //register_symbol(line, &taint_eng);

            if(line[0] == 'R' && line[1] == 'L')
                taint_eng.plugin->parse_option(line);
                //register_lib(line, &taint_eng);
            
            if(line[0] == 'D' && line[1] == 'L')
                taint_eng.plugin->parse_option(line);
                //deregister_lib(line, &taint_eng);
            
            if(line[0] == 'F' && line[1] == 'E')
                taint_eng.plugin->parse_option(line);
                //register_fence(line, &taint_eng);

            if(line[0] == 'S' && line[1] == 'P')
                taint_eng.plugin->parse_option(line);

            if(line[0] == 'B' && line[1] == 'L')
                taint_eng.plugin->parse_option(line);
                //register_blacklist(line, &taint_eng);

            if(line[0] == 'B' && line[1] == 'A')
                taint_eng.plugin->parse_option(line);
                //register_blacklist_addr(line, &taint_eng);

            if(line[0] == 'S' && line[1] == 'A')
                taint_eng.plugin->parse_option(line);
                //register_silenced_addr(line, &taint_eng);

            if(line[0] == 'I' && line[1] == 'N')
                taint_eng.plugin->parse_option(line);
                //register_included(line, &taint_eng);

            if(line[0] == 'F' && line[1] == 'W')
                taint_eng.plugin->parse_option(line);
                //register_wanted(line, &taint_eng);

            if(line[0] == 'I' && line[1] == 'W')
                taint_eng.plugin->parse_option(line);
                //register_wanted_i(line, &taint_eng);

            if(line[0] == 'E' && line[1] == 'W')
                taint_eng.plugin->parse_option(line);
                //register_wanted_e(line, &taint_eng);

            if(line[0] == 'J' && line[1] == 'E')
                taint_eng.plugin->parse_option(line);
                //taint_eng.jxx_set(0x1);

            if(line[0] == 'J' && line[1] == 'D')
                taint_eng.plugin->parse_option(line);
                //taint_eng.jxx_set(0x0);

            if(line[0] == 'J' && line[1] == 'C')
                taint_eng.plugin->parse_option(line);
                //taint_eng.jxx_clear();

            if(line[0] == 'O' && line[1] == 'U')
                taint_eng.plugin->parse_option(line);
                //comment_out(line, &taint_eng);

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
    {
        fprintf(stderr, "taint_eng.ctx_info: 0x%08x\n", &taint_eng.ctx_info[0]);
        fprintf(stderr, "Thread entry no 0x%02x: TID: 0x%08x\n", i, taint_eng.ctx_info[i].tid);
        fprintf(stderr, "Removing thread: 0x%08x\n", taint_eng.ctx_info[i].tid);
        taint_eng.del_thread_srsly(taint_eng.ctx_info[i].tid);
    }
   
    fclose(taint_eng.instr_file);
    if(line)
        free(line); 

    taint_eng.close_files();

    return 0x0;
}
