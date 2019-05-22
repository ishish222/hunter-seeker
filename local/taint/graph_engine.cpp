#include <graph_engine.h>
#include <taint_emul_x86.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <debug.h>
#include <plugin.h>
#include <utils.h>

#define GRAPH_OFF 11
#define CODE_BLACK  0x0
#define CODE_BLUE   0x1
#define CODE_GREEN  0x2
#define CODE_RED    0x3
#define CODE_COMMENT 0x4

char node_color[0x10][0x10] = {"#000000", "#0000FF", "#00FF00", "#FF0000", "#0055AA"};

/*
int detox(char* s)
{
    unsigned size;
    unsigned i;

    size  = strlen(s);
    for(i = 0; i< size; i++)
    {
        if(s[i] == '\"') s[i] = ' ';
        if(s[i] == '\'') s[i] = ' ';
        if(s[i] == '<') s[i] = ' ';
        if(s[i] == '>') s[i] = ' ';
    }

    return 0x0;
}
*/ //move it here

int detox(char* s);

int graph_engine::check_fence(CALL_LEVEL* cur_level)
{
    /* check fences for activation */
    unsigned i;
    LOOP_FENCE* cur_fence;

    for(i=0x0; i < this->loop_fences_count; i++)
    {
        cur_fence = &this->loop_fences[i];
        if(cur_fence->entry == cur_level->entry)
        {
            cur_level->cur_fence = cur_fence;
            cur_level->loop_status = FENCE_ACTIVE;
            cur_level->loop_limit = cur_fence->limit;
            cur_level->loop_struct_size = cur_fence->struct_size;
            cur_level->loop_struct_count = cur_fence->struct_count;

            d_print(1, "!!! Activated fence: 0x%08x - 0x%08x - 0x%08x\n", cur_fence->entry, cur_fence->start, cur_fence->limit);
        }
    }

    return 0x0;
}

int graph_engine::add_fence(OFFSET entry, OFFSET start, OFFSET struct_size, OFFSET struct_count)
{
    if(this->loop_fences_count >= MAX_LOOP_FENCES)
    {
        d_print(1, "Error, maximum of loop fences reached\n");
        exit(-1);
    }

    this->loop_fences[this->loop_fences_count].entry = entry;
    this->loop_fences[this->loop_fences_count].start = start;
    this->loop_fences[this->loop_fences_count].struct_size = struct_size;
    this->loop_fences[this->loop_fences_count].struct_count = struct_count;
    this->loop_fences[this->loop_fences_count].limit = struct_size * struct_count;
    this->loop_fences_count++;
    d_print(1, "Fence: Entry: 0x%08x Start: 0x%08x Size: 0x%08x Count: 0x%08x\n", entry, start, struct_size, struct_count);

    return 0x0;
}

int graph_engine::enter_loop_demo(CONTEXT_GRAPH* info)
{
    if(!this->taint_eng->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    char out_line[MAX_NAME];
    FILE* f = info->graph_file;

    unsigned cur_call_level;
    cur_call_level = this->cur_info->call_level;

    {
        d_print(1, "Entering loop demo\n");
        sprintf(out_line, "[loop]");
        print_call(info, out_line, node_color[CODE_BLACK]);
//        sprintf(out_line, "<!-- started loop demo -->\n");
//        fwrite(out_line, strlen(out_line), 0x1, f);
    }

    //info->loop_pos[cur_call_level]++;

}

int graph_engine::exit_loop_demo(CONTEXT_GRAPH* info)
{
    if(!this->taint_eng->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    unsigned level;
    char out_line[MAX_NAME];
    FILE* f = info->graph_file;
    CALL_LEVEL* cur_level;

    level = this->cur_info->call_level;

    d_print(1, "Exiting loop demo\n");
    cur_level->cur_fence = 0x0;
//    sprintf(out_line, "<!-- loop demo ended \n");
//    fwrite(out_line, strlen(out_line), 0x1, f);

    print_ret(info);
}

int graph_engine::enter_loop(CONTEXT_GRAPH* info)
{
    if(!this->taint_eng->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    char out_line[MAX_NAME];
    FILE* f = info->graph_file;

    unsigned cur_call_level;
    cur_call_level = this->cur_info->call_level;

    {
        d_print(1, "Entering loop\n");
//        sprintf(out_line, "[loop]");
//        print_call(info, out_line, node_color[CODE_BLACK]);
//        sprintf(out_line, "<!-- started loop -->\n");
//        fwrite(out_line, strlen(out_line), 0x1, f);
    }

    //info->loop_pos[cur_call_level]++;

}

int graph_engine::exit_loop(CONTEXT_GRAPH* info)
{
    if(!this->taint_eng->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    unsigned level;
    char out_line[MAX_NAME];
    FILE* f = info->graph_file;
    CALL_LEVEL* cur_level;

    level = this->cur_info->call_level;

    d_print(1, "Exiting loop\n");
    cur_level->cur_fence = 0x0;
//    sprintf(out_line, "loop ended -->\n");
//    fwrite(out_line, strlen(out_line), 0x1, f);

//    print_ret(info);
}

int graph_engine::check_collecting(CONTEXT_GRAPH* info)
{
    unsigned i;

    d_print(1, "Entered collecting verification\n");

    for(i = info->call_level; i >= info->call_level_smallest; i--)
    {
        d_print(1, "Veryfing level: %d\n", i);
        if(info->levels[i].cur_fence)
            if(info->levels[i].cur_fence->collecting) return 0x1;
    }

    return 0x0;
}


int graph_engine::comment_out(char* comment, DWORD tid)
{
    if(!(this->taint_eng->started))
    {
        return 0x0;
    }

    /* we need to find proper ctx_info */
    CONTEXT_GRAPH* info;
    DWORD tid_no;

    tid_no = this->taint_eng->tids[tid];

    info = &this->ctx_info[tid_no];

    detox(comment);

    d_print(1, "Writing out comment @ %d: %s", this->taint_eng->last_instr_count, comment);
    print_empty_call(info, comment, node_color[CODE_COMMENT]);
    return 0x0;
}

int graph_engine::register_comment(char* line)
{
    char* cmd;
    char* comment;
    DWORD tid;

    cmd = strtok(line, ",");
    tid = strtol(strtok(0x0, ","), 0x0, 0x10);
    comment = strtok(0x0, ",");
    comment[strlen(comment)-1] = 0x0;

    this->comment_out(comment, tid);

    return 0x0;
}
/* returns 1 if in loop, 0 otherwise */
int graph_engine::check_loop_2(CONTEXT_GRAPH* info)
{
    if(!this->taint_eng->options & OPTION_ANALYZE_LOOPS)
        return 0x0;

    OFFSET offset;
    unsigned cur_fence_idx;
    unsigned cur_loop_addr_idx;
    unsigned cur_loop_limit;
    unsigned cur_loop_struct_size;
    unsigned cur_loop_struct_count;
    unsigned level;
    LOOP_FENCE* cur_fence;
    CALL_LEVEL* cur_level;


    level = info->call_level;
    cur_level = &info->levels[level];

    offset = this->taint_eng->current_eip;
    cur_fence = info->levels[level].cur_fence;

    d_print(1, "Fence for graph pos: %d: 0x%08x\n", level, cur_fence);

    if(cur_fence != 0x0)
    {

        d_print(1, "---\nActive fence: \nEntry: \t0x%08x\nStart: \t0x%08x\nLimit: \t%d\n", cur_fence->entry, cur_fence->start, cur_fence->limit);

        /* on specific level, but not yet collecting */
        if(cur_level->loop_status == FENCE_ACTIVE)
        {
/*            d_print(1, "Check for starting: %p - %p\n", cur_fence->start, offset);*/
            d_print(1, "Check for starting: %p - %p\n", cur_fence->start, cur_info->source);
            if(cur_fence->start == cur_info->source)
            {
                d_print(1, "Starting collecting\n");
                cur_level->loop_status = FENCE_COLLECTING;
                enter_loop(info);
                enter_loop_demo(info);
            }
        }

        /* in loop */
        cur_loop_addr_idx   = cur_level->loop_addr_idx;
        cur_loop_limit      = cur_level->loop_limit;
        cur_loop_struct_size    = cur_level->loop_struct_size;
        cur_loop_struct_count   = cur_level->loop_struct_count;

        if(cur_level->loop_status == FENCE_NOT_COLLECTING)
        {
            cur_loop_addr_idx ++;
            cur_level->loop_addr_idx = cur_loop_addr_idx;

            d_print(1, "Not collected: %p\n", offset);

            if(cur_loop_limit != 0x0)
            if(cur_loop_addr_idx >= cur_loop_limit)
            {
                /* collecting finished */
                cur_level->loop_addr_idx = 0x0;

                cur_level->loop_status = FENCE_FINISHED;
                exit_loop(info);

                d_print(1, "Collected addrs:\n");

                unsigned i;
                for(i=0x0; i<cur_loop_struct_count; i++)
                {
                    d_print(1, "0x%08x\n", cur_level->loop_addr[i]);
                }

            }
            return 0;
        }

        if(cur_level->loop_status == FENCE_COLLECTING)
        {
            d_print(1, "1");
            cur_level->loop_addr[cur_loop_addr_idx] = offset;
            d_print(1, "2");
            cur_loop_addr_idx ++;
            cur_level->loop_addr_idx = cur_loop_addr_idx;

            d_print(1, "Collected: %p\n", offset);
            d_print(1, "Level %d collection: idx: %d, limit: %d\n", level, cur_level->loop_addr_idx, cur_level->loop_limit);

            if(cur_loop_addr_idx > cur_loop_struct_size)
            {
                /* we collected enough for demonstration */
                cur_level->loop_status = FENCE_NOT_COLLECTING;
                exit_loop_demo(info);
            }

            return 0;
        } 
    }
    return 0x0;
}


void graph_engine::print_call_open(CONTEXT_GRAPH* cur_ctx, char* line, const char* color)
{
    char out_line[MAX_NAME];
    char working_line[MAX_NAME];
    FILE* f = cur_ctx->graph_file;
    unsigned i;

//    if(check_collecting(cur_ctx)) return;

    d_print(1, "Printing call into: %s\n", cur_ctx->graph_filename);

    strcpy(out_line, "");

    for(i = this->call_level_start-this->call_level_offset; i< cur_ctx->call_level; i++)
        strcat(out_line, " ");

    sprintf(working_line, "<node COLOR=\"%s\" CREATED=\"6666666666666\" ID=\"ID_1208439975\" MODIFIED=\"6666666666666\" TEXT=\"%s\">\n", color, line);

    strcat(out_line, working_line);
    fwrite(out_line, strlen(out_line), 0x1, f);
}


void graph_engine::print_call(CONTEXT_GRAPH* cur_ctx, char* line, const char* color)
{
    char out_line[MAX_NAME];
    char working_line[MAX_NAME];
    FILE* f = cur_ctx->graph_file;
    unsigned i;

//    if(check_collecting(cur_ctx)) return;

    d_print(1, "Printing call into: %s\n", cur_ctx->graph_filename);

    strcpy(out_line, "");

    for(i = this->call_level_start-this->call_level_offset; i< cur_ctx->call_level; i++)
        strcat(out_line, " ");

    sprintf(working_line, "<node COLOR=\"%s\" CREATED=\"6666666666666\" FOLDED=\"true\" ID=\"ID_1208439975\" MODIFIED=\"6666666666666\" TEXT=\"%s\">\n", color, line);

    strcat(out_line, working_line);
    fwrite(out_line, strlen(out_line), 0x1, f);
}

void graph_engine::print_empty_call(CONTEXT_GRAPH* cur_ctx, char* line, const char* color)
{
    unsigned i;
    FILE* f = cur_ctx->graph_file;
    char out_line[MAX_NAME];
    char working_line[MAX_NAME];

    d_print(1, "Printing empty call into: %s\n", cur_ctx->graph_filename);

    strcpy(out_line, "");

    for(i = this->call_level_start-this->call_level_offset; i< cur_ctx->call_level; i++)
        strcat(out_line, " ");

    sprintf(working_line, "<node COLOR=\"%s\" CREATED=\"6666666666666\" FOLDED=\"true\" ID=\"ID_1208439975\" MODIFIED=\"6666666666666\" TEXT=\"%s\"></node>\n", color, line);

    strcat(out_line, working_line);
    fwrite(out_line, strlen(out_line), 0x1, f);
}

void graph_engine::print_ret(CONTEXT_GRAPH* cur_ctx)
{
    FILE* f = cur_ctx->graph_file;
    char out_line[MAX_NAME];
    unsigned i;

    d_print(1, "Printing ret into: %s\n", cur_ctx->graph_filename);

    strcpy(out_line, "");

    for(i = this->call_level_start-this->call_level_offset; i< cur_ctx->call_level; i++)
        strcat(out_line, " ");


    d_print(1, "Printing ret\n");
    strcat(out_line, "</node>\n");
    fwrite(out_line, strlen(out_line), 0x1, f);
}

int graph_engine::set_prefix(char* prefix)
{
    strcpy(this->prefix, prefix);
    return 0x0;
}

int graph_engine::register_prefix(char* line)
{
    char* cmd;
    char* prefix;

    cmd = strtok(line, ",");
    prefix = strtok(0x0, "\n");

    printf("Setting prefix to: %s\n", prefix);
    filter_str_2(prefix, '\x0d');
    this->set_prefix(prefix);

    return 0x0;
}


int graph_engine::d_print(int level, const char* format, ...)
{
    va_list argptr;

//    if(level <= DEBUG_LEVEL && this->bp_hit)
    if(level <= DEBUG_LEVEL)
    {
        va_start(argptr, format);
        vfprintf(stderr, format, argptr);
//        vfprintf(stdout, format, argptr);
        va_end(argptr);
    }

    return 0x0;
}

int graph_engine::add_blacklist(char* str)
{
    if(this->blacklist_count >= MAX_BLACKLIST)
    {
        d_print(1, "Error, maximum of blacklisted functions reached\n");
        exit(-1);
    }

    strcpy(this->lib_blacklist[this->blacklist_count], str);
    this->blacklist_count++;
    return 0x0;
}

int graph_engine::add_silenced_addr(DWORD addr)
{
    if(this->addr_silenced_count >= MAX_BLACKLIST)
    {
        d_print(1, "Error, maximum of silenced functions reached\n");
        exit(-1);
    }

    this->addr_silenced[this->addr_silenced_count] = addr;
    this->addr_silenced_count++;
    return 0x0;
}

int graph_engine::add_blacklist_addr(DWORD addr)
{
    if(this->addr_blacklist_count >= MAX_BLACKLIST)
    {
        d_print(1, "Error, maximum of blacklisted functions reached\n");
        exit(-1);
    }

    this->addr_blacklist[this->addr_blacklist_count] = addr;
    this->addr_blacklist_count++;
    return 0x0;
}

int graph_engine::add_wanted_i(unsigned instr)
{
    if(this->wanted_count_i >= MAX_WANTED)
    {
        d_print(1, "Error, maximum of wanted functions reached\n");
        exit(-1);
    }
    this->instr_wanted[this->wanted_count_i] = instr;
    this->wanted_count_i++;
    return 0x0;
}

int graph_engine::add_wanted_e(DWORD addr)
{
    if(this->wanted_count_e >= MAX_WANTED)
    {
        d_print(1, "Error, maximum of wanted functions reached\n");
        exit(-1);
    }
    this->addr_wanted[this->wanted_count_e] = addr;
    this->wanted_count_e++;
    return 0x0;
}

int graph_engine::add_wanted(char* str)
{
    if(this->wanted_count >= MAX_WANTED)
    {
        d_print(1, "Error, maximum of wanted functions reached\n");
        exit(-1);
    }
    strcpy(this->func_wanted[this->wanted_count], str);
    this->func_wanted[this->wanted_count][strlen(this->func_wanted[this->wanted_count])] = 0x0;
    this->func_wanted[this->wanted_count][strlen(this->func_wanted[this->wanted_count])-1] = 0x0;
    d_print(1, "%s\n", this->func_wanted[this->wanted_count]);
    this->wanted_count++;
    return 0x0;
}

int graph_engine::check_func_wanted(char* name)
{
    unsigned i = 0x0;

    //replace MAX_WANTED with wanted_count
    for(i = 0x0; i < MAX_WANTED; i++)
    {
        if(strlen(this->func_wanted[i]) == 0x0) 
            continue;

        if(strcmp(name, this->func_wanted[i]) == 0x0)
        {
            return 0x1;
        }
    }

    return 0x0;
}

int graph_engine::check_func_included(char* name)
{
    unsigned i = 0x0;

    //replace MAX_WANTED with included_count
    for(i = 0x0; i < MAX_WANTED; i++)
    {
        if(strlen(this->func_included[i]) == 0x0)
            continue;

        if(strcmp(name, this->func_included[i]) == 0x0)
        {
            return 0x1;
        }
    }

    return 0x0;
}

int graph_engine::add_included(char* str)
{
    if(this->included_count >= MAX_WANTED)
    {
        d_print(1, "Error, maximum of wanted functions reached\n");
        exit(-1);
    }
    strcpy(this->func_included[this->included_count], str);
    this->func_included[this->included_count][strlen(this->func_included[this->included_count])] = 0x0;
    this->func_included[this->included_count][strlen(this->func_included[this->included_count])-1] = 0x0;
    d_print(1, "%s\n", this->func_included[this->included_count]);
    this->included_count++;
    return 0x0;
}

int graph_engine::check_lib_blacklist(LIBRARY* lib)
{
    unsigned i;

    d_print(2, "Checking lib\n");

    for(i=0x0; i<MAX_BLACKLIST; i++)
    {
        d_print(2, "%s - %s\n", this->lib_blacklist[i], lib->name);
            
        if(strlen(lib->name) == 0x0) break;
        if(strlen(this->lib_blacklist[i]) == 0x0) break;
        if(strstr(this->lib_blacklist[i], lib->name))
        {
            d_print(1, "Module blacklisted\n");
            lib->blacklisted = 0x1;
            return 0x1;
        }
    }
    return 0x0;
}

int graph_engine::check_addr_silenced(OFFSET offset)
{
    LIBRARY* lib;

    unsigned i;

    for(i = 0x0; i< this->addr_silenced_count; i++)
    {
        if(this->addr_silenced[i] == offset)
            return 0x1;
    }

    return 0x0;
}

int graph_engine::check_addr_blacklist(OFFSET offset)
{
    LIBRARY* lib;

    unsigned i;

    for(i = 0x0; i< this->addr_blacklist_count; i++)
    {
        if(this->addr_blacklist[i] == offset)
            return 0x1;
    }

    lib = this->get_lib(offset);

    if(lib!=0x0)
    {
        d_print(2, "Lib: %s, from 0x%08x to 0x%08x\n", lib->name, lib->offset, lib->offset + lib->length);
        if(lib->blacklisted)
        {
            d_print(2, "Lib blacklisted\n");
            return 0x1;
        }
    }

    return 0x0;
}

CONTEXT_GRAPH* graph_engine::get_context_graph(DWORD tno)
{
    CONTEXT_GRAPH* ret = 0x0;
    ret = &this->ctx_info[this->taint_eng->tids[tno]];
    return ret;
}

/* callbacks */

int graph_engine::pre_execute_instruction_callback(DWORD eip)
{
    fprintf(stderr, "graph_engine::pre_execute_instruction_callback\n");
    /* graph start */
    if((this->taint_eng->start_addr == eip) || (this->taint_eng->current_instr_count == this->taint_eng->start_instr))
    {
        d_print(1, "Got ST at 0x%08x, starting\n", eip);
        this->taint_eng->started = 0x1;
    }

    this->cur_info = this->get_context_graph(this->taint_eng->cur_tid);

    if(cur_info->returning)
    {
        cur_info->before_returning = 1;
        cur_info->returning = 0;
    }

    if(cur_info->calling)
    {
        cur_info->before_calling = 1;
        cur_info->calling = 0;
    }

    if(cur_info->jumping)
    {
        cur_info->before_jumping = 1;
        cur_info->before_jmp_code = cur_info->jmp_code;
        cur_info->jumping = 0;
    }

    if(abs(int(cur_info->waiting) - int(eip) )<0x5) 
    {
        /* stop waiting */
        d_print(1, "[0x%08x] Waiting: 0x%08x, eip: 0x%08x\n", this->taint_eng->cur_tid, cur_info->waiting, eip);
        cur_info->waiting = 0x0;
        cur_info->before_waiting = 0x1;
        if((cur_info->last_emit_decision == DECISION_EMIT) || (cur_info->last_emit_decision == DECISION_EMIT_NESTED))
        {
            print_ret(cur_info);
            cur_info->last_emit_decision = 0x0; 
        }
    }

    return 0x0;
}

int graph_engine::post_execute_instruction_callback(DWORD eip)
{
    unsigned i;

    fprintf(stderr, "graph_engine::post_execute_instruction_callback\n");
    /* graph */

    CONTEXT_GRAPH* cur_ctx;
    cur_ctx = &this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]];

    /* handle waiting rets */

//    if(cur_ctx->returning > 0x0) cur_ctx->returning--;

    if((cur_ctx->before_returning == 1))
    {
        this->handle_ret(cur_ctx, eip);
        cur_ctx->before_returning = 0;
    }

    if(cur_ctx->before_calling)
    {
        cur_ctx->target = eip;
        d_print(1, "Next call target = 0x%08x\n", eip);
        handle_call(cur_ctx);
        cur_ctx->before_calling = 0;
    }

    if(cur_ctx->before_jumping)
    {
        cur_ctx->target = eip;
        handle_jxx(cur_ctx);
        cur_ctx->before_jumping = 0;
    }

    /* handle surface rets */

    char out_line[MAX_NAME];

    /* wanted */
    for(i=0x0; i<this->wanted_count_i; i++)
        if(this->instr_wanted[i] == this->taint_eng->current_instr_count)
        {
            sprintf(out_line, "[x] (%d)0x%08x", this->taint_eng->current_instr_count ,this->taint_eng->current_eip);
            print_call(cur_ctx, out_line, node_color[CODE_RED]);
            print_ret(cur_ctx);
 
        }

    for(i=0x0; i<this->wanted_count_e; i++)
        if(this->addr_wanted[i] == this->taint_eng->current_eip)
        {
            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x", this->taint_eng->current_instr_count ,this->taint_eng->current_eip);
            else sprintf(out_line, "[x] 0x%08x", this->taint_eng->current_eip);
            print_call(cur_ctx, out_line, node_color[CODE_RED]);
            print_ret(cur_ctx);
        }

    return 0x0;
}

int graph_engine::start_callback()
{
    char out_line[MAX_NAME];

    sprintf(out_line, "[ST]");
    print_empty_call(&this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]], out_line, node_color[CODE_RED]);

    return 0x0;
}

int graph_engine::finish_callback()
{
    fprintf(stderr, "graph_engine::finish_callback\n");

    unsigned i, j, k;
    CONTEXT_GRAPH* cur_tid;
    char out_line[MAX_NAME];
    int diff_last, diff_first;
    unsigned open;

    d_print(1, "Closing %d tids\n", this->tid_count);
    for(i=0x0; i<this->tid_count; i++)
    {
        cur_tid = &this->ctx_info[i];
        d_print(1, "Closing 0x%08x\n", cur_tid->tid);

        if(cur_tid->waiting != 0x0)
        {
            if((cur_tid->last_emit_decision == DECISION_EMIT) || (cur_tid->last_emit_decision == DECISION_EMIT_NESTED))
            {
                print_ret(cur_tid);
                cur_tid->last_emit_decision = 0x0;
            }
        }

        open = cur_tid->call_level - cur_tid->call_level_smallest;
        d_print(1, "[0x%08x] Left with %d nodes open\n", cur_tid->tid, open);
        d_print(1, "[0x%08x] First: %d - %d = %d\n", cur_tid->tid, cur_tid->call_level_smallest, abs(int(this->call_level_start) - int(cur_tid->call_level_smallest)));

        diff_first = abs(int(this->call_level_start) - int(cur_tid->call_level_smallest));
        diff_last = open;

        d_print(1, "[0x%08x] Diff_last: %d\n", cur_tid->tid, diff_last);
        for(j=0x0; j < diff_last; j++)
        {
/*
#ifdef ANALYZE_LOOPS
            for(k = 0x0; k<cur_tid->loop_pos[cur_tid->call_level]; k++)
            {
                exit_loop(cur_tid);
            }
#endif
*/
            print_ret(cur_tid);
            cur_tid->call_level--;
        }

/*
#ifdef ANALYZE_LOOPS
        for(k = 0x0; k<cur_tid->loop_pos[cur_tid->call_level]; k++)
        {
            exit_loop(cur_tid);
        }
#endif
*/

        sprintf(out_line, "</node></map>\n");
        fwrite(out_line, strlen(out_line), 0x1, cur_tid->graph_file);

        d_print(1, "[0x%08x] Diff_first: %d\n", cur_tid->tid, diff_first);
        sprintf(out_line, "<map version=\"1.0.1\">\n<node TEXT=\"start\">\n");
        fwrite(out_line, strlen(out_line), 0x1, cur_tid->graph_file);

        cur_tid->call_level--;
        for(j=0x0; j < diff_first; j++)
        {
            cur_tid->call_level++;
            print_call(cur_tid, "unknown", node_color[CODE_BLACK]);
        //print_call(cur_tid->graph_file, "unknown", node_color[CODE_BLACK]);
        }

        unsigned k;
        d_print(1, "JXX stats:\n");
        for(k = cur_tid->call_level_smallest; k< cur_tid->call_level_largest; k++)
        {
            d_print(1, "Level %d: %d\n", cur_tid->jxx_total[k]);
        }

    }
    return 0x0;    
}

int graph_engine::add_thread_callback(CONTEXT_OUT ctx_out)
{
    unsigned i;
    DWORD new_tid = ctx_out.thread_id;

    tid_count = this->taint_eng->tid_count;

    if(!this->taint_eng->already_added(new_tid))
    {
        if(strlen(this->prefix) > 0x1)
        {
            sprintf(this->ctx_info[tid_count].graph_filename, "%s_TID_%08X_2.mm", this->prefix, new_tid);
        }
        else
        {
            sprintf(this->ctx_info[tid_count].graph_filename, "TID_%08X_2.mm", new_tid);
        }
        d_print(1, "Creating graph file: %s\n", this->ctx_info[tid_count].graph_filename);
        this->ctx_info[tid_count].graph_file = fopen(this->ctx_info[tid_count].graph_filename, "w");
        this->ctx_info[tid_count].call_level = (this->max_call_levels/3); // starting at level 1/3 of max_call_levels
        this->ctx_info[tid_count].call_level_smallest = this->ctx_info[tid_count].call_level;
        this->ctx_info[tid_count].levels = (CALL_LEVEL*)malloc(sizeof(CALL_LEVEL)*this->max_call_levels);
        if(this->ctx_info[tid_count].levels == 0x0)
        {
            d_print(1, "Unable to allocate\n");
            exit(-1);
        }
        memset(this->ctx_info[tid_count].levels, 0x0, sizeof(CALL_LEVEL)*this->max_call_levels);
        this->ctx_info[tid_count].waiting = 0x0;
//        this->ctx_info[tid_count].list = (DWORD*)malloc(sizeof(DWORD) * MAX_LIST_JXX);
//        this->ctx_info[tid_count].list_len = 0x0;

        /* clear loop structures */
        unsigned call_level;
        call_level = this->ctx_info[tid_count].call_level;
//        this->ctx_info[tid_count].loop_start[call_level] = NO_LOOP;

        /* for graphs */
        unsigned level;
        CALL_LEVEL* cur_level;
    
        level = this->ctx_info[tid_count].call_level;
        cur_level = &this->ctx_info[tid_count].levels[level];
    
        cur_level->entry = 0xffffffff;
        this->check_fence(cur_level);

        /* output marker */
        char out_line[MAX_NAME];

        strcpy(out_line, "");
        for(i = this->call_level_start-this->call_level_offset; i< call_level; i++)
            strcat(out_line, " ");
        fwrite(out_line, strlen(out_line), 0x1, this->ctx_info[tid_count].graph_file);

        sprintf(out_line, "<node TEXT=\"[ENTRY]\"></node>\n");
        fwrite(out_line, strlen(out_line), 0x1, this->ctx_info[tid_count].graph_file);

        /* fnalize */
        this->ctx_info[tid_count].tid = ctx_out.thread_id;
    }
    d_print(1, "ER_9 TID: 0x%08x lock_level: 0x%08x\n", ctx_out.thread_id, this->ctx_info[this->tid_count].lock_level);

    return 0x0;
}

int graph_engine::del_thread_callback(DWORD tid)
{
    return 0x0;
}

int graph_engine::del_thread_srsly_callback(DWORD tid)
{
    d_print(1, "Plugin: Removing  thread: 0x%08x\n", tid);
    unsigned info_pos;
    info_pos = this->taint_eng->tids[tid];

    fclose(this->ctx_info[info_pos].graph_file);

    return 0x0;
}

int graph_engine::add_lib(OFFSET off, char* name)
{
    LIBRARY new_lib;
    FILE* f;

    unsigned i;
    for(i=0x0; i<strlen(name); i++)
    {
        if(name[i] == '\r') name[i]='\x00';
        if(name[i] == '\\') name[i]='/';
    }

    strcpy(new_lib.path, this->lib_dir_path);
    strcat(new_lib.path, basename(name));

    d_print(1, "Loading lib: %s\n", new_lib.path);

    strcpy(new_lib.name, basename(name));

    d_print(1, "Loading symbols for %s @ 0x%08x\n", new_lib.name, off);
    new_lib.offset = off;
    this->add_symbols(&new_lib);

    new_lib.loaded = 1;

    this->check_lib_blacklist(&new_lib);
    this->libs[libs_count] = new_lib;
    this-> libs_count ++; 

    d_print(2, "Loaded lib: %s at 0x%08x to 0x%08x\n", new_lib.path, new_lib.offset, new_lib.offset+new_lib.length);
    return 0x0;
}

int graph_engine::add_symbols(LIBRARY* info)
{
    SYMBOL* s;
    SYMBOL* s1;
    SYMBOL* old;
    SYMBOL* temp;

    s = this->symbols;
    if(s == 0x0) 
    {
        info->symbols = 0x0;
        return 0x0;
    }
    old = 0x0;

    do
    {
        if(s == 0x0) break;
        if(s->lib_name == 0x0) break;

        if(!strcmpi(info->name, s->lib_name))
        {
            s->addr += info->offset;
            //d_print(1, "Resolved symbol: %s!%s @ 0x%08x\n", s->lib_name, s->func_name, s->addr);
            s->resolved = 0x1;
    
            //move
            if(old) old->next = s->next;
            else this->symbols = this->symbols->next;

            s->next = info->symbols;
            info->symbols = s;

            if(old) s = old->next;
            else s = this->symbols;
        }
        else
        {
            old = s;
            s = s->next;
        }
    }
    while(s);
    
    return 0x0;
}

int graph_engine::copy_symbol(SYMBOL** dst, SYMBOL* src)
{
    SYMBOL* sp;
    SYMBOL* old;

    old = *dst;

    sp = (SYMBOL*)malloc(sizeof(SYMBOL));
    sp->addr = src->addr;
    sp->lib_name = (char*)malloc(strlen(src->lib_name)+1);
    sp->func_name = (char*)malloc(strlen(src->func_name)+1);
    strcat(sp->lib_name, src->lib_name);
    strcat(sp->func_name, src->func_name);
    sp->next = old;

    *dst = sp;

    return 0x0;
}

int graph_engine::add_symbol(SYMBOL** s, OFFSET addr, char* lib_name, char* func_name)
{
    SYMBOL* sp;
    SYMBOL* old;

    old = *s;

    if(this->symbols_count >= MAX_SYMBOL_COUNT)
    {
        d_print(1, "Error, maximum of symbols reached\n");
        exit(-1);
    }

    sp = (SYMBOL*)malloc(sizeof(SYMBOL));
    sp->addr = addr;
    sp->lib_name = (char*)malloc(strlen(lib_name)+1);
    sp->func_name = (char*)malloc(strlen(func_name)+1);
    sp->resolved = 0x0;
    sp->wanted = this->check_func_wanted(func_name);
    sp->included = this->check_func_included(func_name);
    strcat(sp->lib_name, lib_name);
    strcat(sp->func_name, func_name);
    sp->next = old;
    this->symbols_count++;

    *s = sp;

    return 0x0;
}

int graph_engine::del_symbol(SYMBOL* sp)
{
    if(sp->lib_name) free(sp->lib_name);
    if(sp->func_name) free(sp->func_name);
    free(sp);
    return 0x0;
}

LIBRARY* graph_engine::get_lib(OFFSET offset)
{
    unsigned i=0x0;
    unsigned highest = -1;

    for(i = 0x0; i < this->libs_count; i++)
    {
        if(this->libs[i].offset < offset)
        {
            if(highest != -1)
            {
                if(this->libs[i].offset > this->libs[highest].offset) 
                {
                    highest = i;
                }
            }
            else 
            {
                highest = i;
            }
        }
    }
    if(highest != -1) 
    {
        return &this->libs[highest];
    }
    else return 0x0;
}


/* [TODO] need to optimize. Somehow. */
SYMBOL* graph_engine::get_symbol(OFFSET addr)
{
    SYMBOL* s;
    LIBRARY* lib;

    lib = this->get_lib(addr);
    if(lib == 0x0) return 0x0;

    s = lib->symbols;

    if(s == 0x0) return 0x0;
    do
    {
        if((s->addr == addr) && (s->resolved)) 
        {
            return s; 
        }
        else s = s->next;
    } while(s);

    return 0x0;
}


int graph_engine::del_lib(OFFSET off)
{
    int i;

    for(i=0x0; i<libs_count; i++)
    {
        if(this->libs[i].offset == off)
            this->libs[i].loaded = 0x0;
    }
    return 0x0;
}

int graph_engine::set_lib_dir_path(char* path)
{
    strcpy(this->lib_dir_path, path);
    return 0x0;
}


int graph_engine::register_silenced_addr(char* line)
{
    char* cmd;
    OFFSET addr;

    cmd = strtok(line, ",");
    addr = strtoul(strtok(0x0, ","), 0x0, 0x10);
    this->add_silenced_addr(addr);

    return 0x0;
}

int graph_engine::register_wanted_i(char* line)
{
    char* cmd;
    unsigned instr;

    cmd = strtok(line, ",");
    instr = strtoul(strtok(0x0, ","), 0x0, 10);
    this->add_wanted_i(instr);

    return 0x0;
}

int graph_engine::register_wanted_e(char* line)
{
    char* cmd;
    OFFSET addr;

    cmd = strtok(line, ",");
    addr = strtoul(strtok(0x0, ","), 0x0, 0x10);
    this->add_wanted_e(addr);

    return 0x0;
}

int graph_engine::register_fence(char* line)
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
    this->add_fence(entry, start, size, count);

    return 0x0;
}

int graph_engine::handle_exception_callback(EXCEPTION_INFO info)
{
    if(!(this->taint_eng->started))
    {
        return 0x0;
    }

    char out_line[MAX_NAME];

    sprintf(out_line, "[x] Exception %08x in TID %08x, instr. no: %d, eip: 0x%08x", info.er.ExceptionCode, info.tid, this->taint_eng->current_instr_count, info.er.ExceptionAddress);
    print_empty_call(&this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]], out_line, node_color[CODE_RED]);
    return 0x0;
}

int graph_engine::jxx_set(unsigned state)
{
    char out_line[MAX_NAME]; 

    this->cur_info->levels[this->cur_info->call_level].jxx_handling = state;

    sprintf(out_line, "[x] JXX_STATUS: 0x%02x", state);
    print_empty_call(&this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]], out_line, node_color[CODE_RED]);

    return 0x0;
}

int graph_engine::jxx_clear()
{
    unsigned level = this->cur_info->call_level;

    jxx_clear_level(level);
    return 0x0;
}

int graph_engine::jxx_clear_level(unsigned level)
{
    this->cur_info->list_len[level] = 0x0;
    d_print(1, "Clearing JXX on level: %d\n", level);
    return 0x0;
}

int is_on_list(CONTEXT_GRAPH* info, DWORD eip)
{
    unsigned i;

    unsigned level = info->call_level;

    for(i = 0x0; i< info->list_len[level]; i++)
    {
        if(eip == info->list[level][i])
        {
            info->jxx_total[level]++;
            return 0x1;
        }
    }

    return 0x0;
}

int add_to_list(CONTEXT_GRAPH* info, DWORD eip)
{
    unsigned level = info->call_level;

    if(info->list_len[level] == MAX_LIST_JXX-1) return -0x1;
    d_print(1, "Adding to list at level %d: 0x%08x\n", level, eip);
    info->list[level][info->list_len[level]] = eip;
    info->list_len[level]++;
    d_print(1, "List len: %d\n", info->list_len[level]);
    return 0x0;
}

int graph_engine::handle_jxx(CONTEXT_GRAPH* info)
{
    if(info->waiting != 0x0)
    {
        return 0x0;
    }

    if(is_on_list(info, this->taint_eng->last_eip)) 
    {
        d_print(1, "JXX Is on list, ignoring\n");
        return 0x0;
    }


    this->check_loop_2(info);
    CALL_LEVEL* cur_level;

    cur_level = &info->levels[info->call_level];

    if(cur_level->loop_status == FENCE_NOT_COLLECTING)
    {
        return 0x0;
    }

    switch(info->before_jmp_code)
    {
        case JMP_CODE_JB_JC_JNAE:
            handle_this_jxx(info, "jb_jc_jnae");
            break;

        case JMP_CODE_JAE_JNB_JNC:
            handle_this_jxx(info, "jae_jnb_jnc");
            break;

        case JMP_CODE_JE_JZ:
            handle_this_jxx(info, "je_jz");
            break;

        case JMP_CODE_JNE_JNZ:
            handle_this_jxx(info, "jne_jnz");
            break;

        case JMP_CODE_JBE_JNA:
            handle_this_jxx(info, "jbe_jna");
            break;

        case JMP_CODE_JA_JNBE:
            handle_this_jxx(info, "ja_jnbe");
            break;

        case JMP_CODE_JS:
            handle_this_jxx(info, "js");
            break;

        case JMP_CODE_JNS:
            handle_this_jxx(info, "jns");
            break;

        case JMP_CODE_JP_JPE:
            handle_this_jxx(info, "jp_jpe");
            break;

        case JMP_CODE_JNP_JPO:
            handle_this_jxx(info, "jnp_jpo");
            break;

        case JMP_CODE_JL_JNGE:
            handle_this_jxx(info, "jl_jnge");
            break;

        case JMP_CODE_JGE_JNL:
            handle_this_jxx(info, "jge_jnl");
            break;

        case JMP_CODE_JLE_JNG:
            handle_this_jxx(info, "jle_jng");
            break;

        case JMP_CODE_JG_JNLE:
            handle_this_jxx(info, "jg_jnle");
            break;

        case JMP_CODE_RM:
            handle_this_jxx(info, "switch_jump");
            break;

    }

    return 0x0;
}

int graph_engine::handle_this_jxx(CONTEXT_GRAPH* info, char* str)
{
    char out_line[MAX_NAME];

    if(this->enumerate) sprintf(out_line, "(%d)0x%08x %s 0x%08x", this->taint_eng->current_instr_count-1, info->last_eip, str, this->taint_eng->current_eip);
    else sprintf(out_line, "0x%08x %s 0x%08x", this->taint_eng->last_eip, str, this->taint_eng->current_eip);
    print_empty_call(info, out_line, node_color[CODE_GREEN]);

    add_to_list(info, this->taint_eng->last_eip);

    return 0x0;
}

/* precise jmp analysis */
int graph_engine::handle_jmp(CONTEXT_GRAPH* info)
{
    if(!this->taint_eng->options & OPTION_ANALYZE_JUMPS)
        return 0x0;

    SYMBOL* s;
    char out_line[MAX_NAME];
    char* func_name;
    DWORD_t current;
    DWORD_t waiting;
    OFFSET target = info->target;
    OFFSET next = info->next;

    unsigned color;
    s = this->get_symbol(target);

    if(info->waiting != 0x0)
    {
        /* wanted */
        if((s != 0x0) && (s->wanted) && (this->taint_eng->options & OPTION_ANALYZE_WANTED_IN_SYMBOLS))
        {
            d_print(1, "Got wanted!\n");
            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x jmp %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
            else sprintf(out_line, "[x] 0x%08x jmp %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
            print_call(info, out_line, node_color[CODE_RED]);
            this->print_ret(info);
        }
        else
        {
            return 0x0;
        }
    }
    else
    {
        if(s != 0x0)
        {
            if(s->wanted)
            {
                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x jmp %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
                else sprintf(out_line, "[x] 0x%08x jmp %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
                print_call(info, out_line, node_color[CODE_RED]);
                print_ret(info);
            }
            else
            {
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x jmp %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
                else sprintf(out_line, "0x%08x jmp %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
                print_call(info, out_line, node_color[CODE_BLUE]);
                print_ret(info);
            }
        }
        else
        {
            return 0x0;
        }
    }

    return 0x0;
}

/* loop routines */ 
int detox(char* s)
{
    unsigned size;
    unsigned i;

    size  = strlen(s);
    for(i = 0; i< size; i++)
    {
        if(s[i] == '\"') s[i] = ' ';
        if(s[i] == '\'') s[i] = ' ';
        if(s[i] == '<') s[i] = ' ';
        if(s[i] == '>') s[i] = ' ';
    }

    return 0x0;
}

/* handling call, diving, surfacing, outputting graph content */

int graph_engine::handle_call(CONTEXT_GRAPH* info)
{
    d_print(1, "[handle call]\n");
    d_print(1, "LL: 0x%08x\n", info->lock_level);
    SYMBOL* s;
    char out_line[MAX_NAME];
    char* func_name;
    DWORD_t current;
    DWORD_t waiting;
    OFFSET source = info->source;
    OFFSET target = info->target;
    OFFSET next = info->next;
    CALL_LEVEL* cur_level;

    d_print(1, "source: 0x%08x, target: 0x%08x\n", source, target);
    #define DECISION_NO_EMIT        0x0
    #define DECISION_EMIT           0x1
    #define DECISION_EMIT_NESTED    0x2

    #define DECISION_NO_DIVE        0x0
    #define DECISION_DIVE           0x1

    #define DECISION_LAYOUT_REGULAR 0x0
    #define DECISION_LAYOUT_SYMBOL  0x1
    #define DECISION_LAYOUT_SYMBOL_WANTED  0x2
    #define DECISION_LAYOUT_4       0x3
    #define DECISION_LAYOUT_5       0x4

    char decision_emit;
    char decision_dive;
    char decision_template;
    
    unsigned color;
    s = this->get_symbol(target);
    unsigned i;

    if((!this->taint_eng->started) || (this->taint_eng->finished))
    {
        d_print(1, "Not yet started\n");
        return 0x0;
    }

    if(info->waiting != 0x0)
    {
        /* increase lock level */
        info->lock_level++;
    }

    d_print(1, "Call\n");
//    d_print(2, "Call: 0x%08x\n", this->taint_eng->reg_restore_32(EIP).get_DWORD());
    d_print(2, "Call: 0x%08x\n", info->source);

    /* decision about emission */

    d_print(2, "Decision about emission\n");
    if(this->check_addr_silenced(source))
    {
        decision_emit = DECISION_NO_EMIT;
    }
    else if(info->waiting != 0x0)
    {
        /* we are waiting for return */ 
        d_print(2, "We are waiting for return \n");
        if((s != 0x0) && (s->wanted))
        {
            /* wanted */
            d_print(1, "Got wanted!\n");
            decision_emit = DECISION_EMIT_NESTED;
        }
        else
        {
            /* not wanted */
            d_print(2, "We are waiting and we do not want\n");
            decision_emit = DECISION_NO_EMIT;
        }
        if(!(this->taint_eng->options & OPTION_ANALYZE_WANTED_IN_SYMBOLS))
        {
            decision_emit = DECISION_NO_EMIT;
        }
    }
    else
    {
        d_print(2, "We are not waiting\n");

//        current = this->taint_eng->reg_restore_32(EIP);
        current = info->source;

        /* check for loop bypasses */
        this->check_loop_2(info);
        cur_level = &info->levels[info->call_level];

        if(cur_level->loop_status == FENCE_NOT_COLLECTING)
        {
            /* we are traversing known loop, do not want this, we wait for next */
            d_print(2, "We are not collecting these calls\n");
            decision_emit = DECISION_NO_EMIT;
        }
        else
        {
            /* we are not traversing a fully known loop, we want this */
            d_print(2, "We are not traversing known loop, we are in active fence before start or we are collecting . Either way we want this\n");
            decision_emit = DECISION_EMIT;
        }
    }
            
    /* decision about diving */
    d_print(2, "Decision about diving\n");
    if(info->waiting != 0x0)
    {
        d_print(2, "We are waiting, not diving\n");
        decision_dive = DECISION_NO_DIVE;
    }
    else
    {
        d_print(2, "We are not waiting, checking for symbol\n");
        if(s != 0x0)
        {
            d_print(2, "We have symbol\n");
            if(s->included)
            {
                d_print(2, "The symbol is included, we dive\n");
                decision_dive = DECISION_DIVE;
            }
            else
            {
                d_print(2, "The symbol is not included, we do not dive\n");
                decision_dive = DECISION_NO_DIVE;
            }
            
        }
        else
        {
            d_print(2, "We do not have symbol\n");

            /* check if we do not exceed maximum depth */
            d_print(2, "Current depth: 0%08x, analysis depth: 0x%08x\n", info->call_level, this->depth);
            if(info->call_level+1 > this->depth)
            {
                d_print(2, "Analysis depth reached, not diving\n");
                decision_dive = DECISION_NO_DIVE;
            }
            else if(this->check_addr_blacklist(target))
            {
                /* target is blacklisted, we do not dive*/
                d_print(2, "Target 0x%08x is blacklisted, we do not dive\n", target);
                decision_dive = DECISION_NO_DIVE;
                if(this->taint_eng->options & OPTION_NOT_EMITTING_BLACKLISTED)
                {
                    decision_emit = DECISION_NO_EMIT;
                }
            }
            else
            {
                /* target is not blacklisted, we dive */
                d_print(2, "Target is not blacklisted, we dive\n");
                decision_dive = DECISION_DIVE;
            }
        }
    }

    /* decision about layout */
    d_print(2, "Decision about layout\n");

    if(s != 0x0)
    {
        /* we have symbol */
        d_print(2, "We have symbol\n");
        if(s->wanted)
        {
            d_print(2, "We have wanted symbol\n");
            decision_template = DECISION_LAYOUT_SYMBOL_WANTED;
        }
        else
        {
            d_print(2, "Symbol is not wanted\n");
            decision_template = DECISION_LAYOUT_SYMBOL;
        }
    }
    else
    {
        d_print(2, "We do not have symbol\n");
        decision_template = DECISION_LAYOUT_REGULAR;
    }

    d_print(2, "Executing decisions\n");

    if(decision_emit == DECISION_EMIT)
    {
        d_print(2, "Emitting\n");

        if(decision_dive == DECISION_DIVE)
        {
            d_print(2, "Emitting diving\n");
        
            if(decision_template == DECISION_LAYOUT_SYMBOL_WANTED)
            {
                /* we assume we have symbol */
//                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
//                else sprintf(out_line, "[x] 0x%08x call %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->taint_eng->current_instr_count-1 ,source, s->lib_name, s->func_name);
                else sprintf(out_line, "[x] 0x%08x call %s!%s", source, s->lib_name, s->func_name);
                print_call(info, out_line, node_color[CODE_RED]);
            }
            else if(decision_template == DECISION_LAYOUT_SYMBOL)
            {
                /* we assume we have symbol */
//                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
//                else sprintf(out_line, "0x%08x call %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->taint_eng->current_instr_count-1 ,source, s->lib_name, s->func_name);
                else sprintf(out_line, "0x%08x call %s!%s", source, s->lib_name, s->func_name);
                print_call(info, out_line, node_color[CODE_BLUE]);
            }
            else
            {
                /* regular emission with dive */
//                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call 0x%08x", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, target);
//                else sprintf(out_line, "0x%08x call 0x%08x", this->taint_eng->current_eip, target);
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call 0x%08x", this->taint_eng->current_instr_count-1 ,source, target);
                else sprintf(out_line, "0x%08x call 0x%08x", source, target);
                print_call(info, out_line, node_color[CODE_BLACK]);
            }
        }
        else /* DECISION_NO_DIVE */
        {
            d_print(2, "Emitting not diving\n");

            if(decision_template == DECISION_LAYOUT_SYMBOL_WANTED)
            {
                /* we assume we have symbol */
//                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
//                else sprintf(out_line, "[x] 0x%08x call %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
                if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->taint_eng->current_instr_count-1 ,source, s->lib_name, s->func_name);
                else sprintf(out_line, "[x] 0x%08x call %s!%s", source, s->lib_name, s->func_name);
                print_call_open(info, out_line, node_color[CODE_RED]);
            }
            else if(decision_template == DECISION_LAYOUT_SYMBOL)
            {
                /* we assume we have symbol */
//                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
//                else sprintf(out_line, "0x%08x call %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->taint_eng->current_instr_count-1 ,source, s->lib_name, s->func_name);
                else sprintf(out_line, "0x%08x call %s!%s", source, s->lib_name, s->func_name);
                print_call_open(info, out_line, node_color[CODE_BLUE]);
            }
            else
            {
                /* regular emission with dive */
//                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call 0x%08x", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, target);
//                else sprintf(out_line, "0x%08x call 0x%08x", this->taint_eng->current_eip, target);
                if(this->enumerate) sprintf(out_line, "(%d)0x%08x call 0x%08x", this->taint_eng->current_instr_count-1, source, target);
                else sprintf(out_line, "0x%08x call 0x%08x", source, target);
                print_call_open(info, out_line, node_color[CODE_BLACK]);
            }
        
        }
        
        /* log emission */
        for(i=this->call_level_start; i< info->call_level; i++)
        {
            d_print(1, " ");
        }
    
/*        d_print(1, "[0x%08x] (%d)0x%08x call 0x%08x, pos: %d, small: %d, ignored: %d: \n", 
                this->taint_eng->cur_tid, this->taint_eng->current_instr_count, this->taint_eng->current_eip, target, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_smallest, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_ignored, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_largest);*/
    
        d_print(1, "[0x%08x] (%d)0x%08x call 0x%08x, pos: %d, small: %d, ignored: %d: \n", 
                this->taint_eng->cur_tid, this->taint_eng->current_instr_count-1, source, target, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_smallest, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_ignored, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_largest);

    }
    else if(decision_emit == DECISION_EMIT_NESTED)
    {
        d_print(2, "Emitting nested\n");

        if(decision_template == DECISION_LAYOUT_SYMBOL_WANTED)
        {
            /* we assume we have symbol */
//            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
//            else sprintf(out_line, "[x] 0x%08x call %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->taint_eng->current_instr_count-1 ,source, s->lib_name, s->func_name);
            else sprintf(out_line, "[x] 0x%08x call %s!%s", source, s->lib_name, s->func_name);
            print_call(info, out_line, node_color[CODE_RED]);
            print_ret(info);
        }
        else
        {
            /* we assume we have symbol */
//            if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->taint_eng->current_instr_count ,this->taint_eng->current_eip, s->lib_name, s->func_name);
//            else sprintf(out_line, "0x%08x call %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
            if(this->enumerate) sprintf(out_line, "(%d)0x%08x call %s!%s", this->taint_eng->current_instr_count-1 ,source, s->lib_name, s->func_name);
            else sprintf(out_line, "0x%08x call %s!%s", source, s->lib_name, s->func_name);
            print_call(info, out_line, node_color[CODE_BLACK]);
            print_ret(info);
        
        }

        /* log emission */
        for(i=this->call_level_start; i< info->call_level; i++)
        {
            d_print(1, " ");
        }
    
        d_print(1, "[0x%08x] (%d)0x%08x call 0x%08x, pos: %d, small: %d, ignored: %d: \n", 
                this->taint_eng->cur_tid, this->taint_eng->current_instr_count-1, source, target, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_smallest, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_ignored, 
                this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].call_level_largest);
    }
    else
    {
        d_print(2, "Not emitting\n");
    }

    if(decision_dive == DECISION_DIVE)
    {
        d_print(2, "Diving!\n");
        this->dive(info, target, next);
    }
    else
    {
        d_print(2, "Not diving!\n");
        if(info->waiting == 0x0) 
        {
            info->waiting = next;
            info->last_emit_decision = decision_emit;
        }

    }
        
    d_print(1, "[handle call finishes]\n");
    return 0x0;
}

int graph_engine::dive(CONTEXT_GRAPH* info, OFFSET target, OFFSET next)
{
    unsigned i, level;
    CALL_LEVEL* prev_level;
    CALL_LEVEL* cur_level;

    /* check if we don't exceed ret table size */
    level = info->call_level;
    if(level == this->max_call_levels-1) 
    {
        d_print(1, "We reached max ret table depth. If you need to register following calls, you need to extend ret table\n");
        exit(-1);
        return 0x1; 
    }

    /* things to do in previous level */
    prev_level = &info->levels[level];
    prev_level->ret = next;
    info->call_level++;

    /* OK, new level */
    level = info->call_level;
    d_print(1, "Entering level %d\n", level);
    cur_level = &info->levels[level];
    memset(cur_level, 0x0, sizeof(CALL_LEVEL));

    /* clear jumps for jxx */
    jxx_clear_level(level);

//    cur_level->loop_addr = (OFFSET*)malloc(sizeof(OFFSET) * MAX_LOOP_ADDR);
    cur_level->entry = target;

    d_print(1, "Entry at level %d is: %p\n", level, cur_level->entry);
    cur_level->cur_fence = 0x0;

    /* prepare loop detection structures OBSOLETE
    info->call_src_register_idx[level] = 0x0;
    info->loop_start[level] = NO_LOOP;
    */

    /* if there is collecting going on on previous level, we do not check fences, we just collect */
    if(prev_level->loop_status == FENCE_COLLECTING) 
    {
        cur_level->loop_status = FENCE_COLLECTING;
    }
    else if(prev_level->loop_status == FENCE_NOT_COLLECTING) 
    {
        cur_level->loop_status = FENCE_NOT_COLLECTING;
        cur_level->loop_limit = 0x99999999;
    }
    else
    {
        this->check_fence(cur_level);
    }

    /* other stuff */ 

    d_print(1, "[0x%08x] Ret table:\n", this->taint_eng->cur_tid);
    for(i=info->call_level_smallest; i<info->call_level; i++)
    {
        d_print(1, "[0x%08x] 0x%08x\n", this->taint_eng->cur_tid, this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]].levels[i].ret);
    }

    return 0x0;
}

int graph_engine::surface(CONTEXT_GRAPH* info)
{
    CALL_LEVEL* cur_level;
    
    cur_level = &info->levels[info->call_level];
    
    info->call_level--;

    if(info->call_level < 0x0) 
    {
        d_print(1, "Error, minimum available level reached");
        d_print(1, "Rerun with larger max_level (-M <level>). Current setting is: 0x%08x\n", this->max_call_levels);
        exit(-1);
    }

    if(info->call_level_smallest > info->call_level) info->call_level_smallest = info->call_level;

    return 0x0;
}

int graph_engine::check_rets(OFFSET ret)
{
    return 0x0;
    /*
    unsigned i;
    for(i = 0x0; i< this->ret_idx; i++)
    {
        if(this->rets[i] == ret)
            return i;
    }
    return -1;
    */
}

int graph_engine::handle_ret(CONTEXT_GRAPH* cur_ctx, OFFSET eip)
{
    if((!this->taint_eng->started) || (this->taint_eng->finished))
        return 0x0;

    d_print(1, "[handle ret]\n");
    d_print(1, "Eip: 0x%08x\n", eip);
    d_print(1, "LL: 0x%08x\n", cur_ctx->lock_level);
    /* verify if ret points to a symbol */
    
    if(cur_ctx->waiting != 0x0) 
    {
        /* decrease lock_level */
        if(cur_ctx->lock_level > 0x0) cur_ctx->lock_level--;
    }

    if(this->taint_eng->options & OPTION_VERIFY_ROP_RETS)
    {
        d_print(1, "[Checking for ROP ret]\n");
        SYMBOL* s;
        char out_line[MAX_NAME]; 
        
        d_print(1, "Searching for symbol: 0x%08x\n", eip);

        s = this->get_symbol(eip);

        if(s != 0x0)
        {
            if(this->enumerate) sprintf(out_line, "[x] (%d)0x%08x call %s!%s", this->taint_eng->current_instr_count, this->taint_eng->current_eip, s->lib_name, s->func_name);
            else sprintf(out_line, "[x] 0x%08x call %s!%s", this->taint_eng->current_eip, s->lib_name, s->func_name);
            print_empty_call(cur_ctx, out_line, node_color[CODE_BLACK]);
        }
        d_print(1, "[Checking for ROP ret finishes]\n");
    }

    unsigned i,j,diff;
    /* new begins */


    /* check surface */

    d_print(1, "We are on level: %d\n", cur_ctx->call_level);

    if(cur_ctx->call_level <= 0x0) return -1;

    d_print(1, "[0x%08x] Ret table:\n", this->taint_eng->cur_tid);
    for(i=cur_ctx->call_level_smallest; i<cur_ctx->call_level; i++)
    {
        d_print(1, "[0x%08x] 0x%08x\n", this->taint_eng->cur_tid, cur_ctx->levels[i].ret);
    }

    d_print(1, "Trying to match ret addr: 0x%08x\n", eip);
    /* Ret matched */
    for(i = cur_ctx->call_level-1; i >= cur_ctx->call_level_smallest, i > 0; i--)
    {
        if(abs(int(cur_ctx->levels[i].ret) - int(eip)) < 0x5)
        {
            diff = cur_ctx->call_level - i;
            d_print(1, "[0x%08x] (%d) Matched ret 0x%08x on pos: %d, handling diff: %d\n", this->taint_eng->cur_tid, this->taint_eng->current_instr_count, cur_ctx->levels[i].ret, i, diff);

            /* is this correct? */
            if(cur_ctx->waiting != 0x0)
            {
                print_ret(cur_ctx);
            }
            if(cur_ctx->waiting != 0x0) cur_ctx->waiting = 0x0;
            for(j=0x0; j<diff; j++)
            {
                if(cur_ctx->levels[cur_ctx->call_level].loop_status != FENCE_NOT_COLLECTING)
                {
                    print_ret(cur_ctx);
                }
                surface(cur_ctx);
            }
            d_print(1, "[handle ret finishes]\n");
            return 0x0;
        }
    }

    d_print(1, "Failed do match\n");
    /* unmatched rets ignored rets */
    if(cur_ctx->waiting != 0x0) 
    {
        d_print(2, "We are waiting\n");
        return 0x0;
    }
    else if(cur_ctx->before_waiting)
    {
        cur_ctx->before_waiting = 0x0;
        d_print(2, "We just matched waiting\n");
        d_print(1, "[handle ret finishes]\n");
        return 0x0;
    }
    else
    {
        d_print(2, "We are not waiting\n");
        
        /* reset lock_level */
        /* [TODO] this should be somewhere else, but it's not consuimng */
        cur_ctx->lock_level = 0x0;
    }

    /* ret unmatched */
    if(this->taint_eng->options & OPTION_UNMATCHED_RET_INVALIDATES_STACK)
    {
        d_print(1, "Enabled: OPTION_UNMATCHED_RET_INVALIDATES_STACK\n");
        /* handle under surface */
        if(cur_ctx->call_level == cur_ctx->call_level_smallest) //we have to use all stacked rets
        {
                /* pos */
                d_print(1, "[0x%08x] Unmatched ret 0x%08x on pos: %d\n", this->taint_eng->cur_tid, eip, cur_ctx->call_level);
                if(cur_ctx->levels[cur_ctx->call_level].loop_status != FENCE_NOT_COLLECTING)
                {
                    print_ret(cur_ctx);
                }
                surface(cur_ctx);
                
                /* smallest */
                cur_ctx->call_level_smallest--;
        }
    }
    else if(this->taint_eng->options & OPTION_UNMATCHED_RET_CREATES_CALL)
    {
    /* handle under surface */
        d_print(1, "Enabled: OPTION_UNMATCHED_RET_CREATES_CALL\n");
        if(cur_ctx->call_level == cur_ctx->call_level_smallest) //we have to use all stacked rets
        {
                /* pos */
                d_print(1, "[0x%08x] Unmatched ret 0x%08x on pos: %d\n", this->taint_eng->cur_tid, eip, cur_ctx->call_level);
                if(cur_ctx->levels[cur_ctx->call_level].loop_status != FENCE_NOT_COLLECTING)
                {
                    print_ret(cur_ctx);
                }
                surface(cur_ctx);
                
                /* smallest */
                cur_ctx->call_level_smallest--;
        }
    }
    else
    {
        d_print(1, "Enabled: default\n");
        d_print(1, "ignoring\n");
        /*
        if(cur_ctx->levels[cur_ctx->call_level].loop_status != FENCE_NOT_COLLECTING)
        {
            print_ret(cur_ctx);
        }
        surface(cur_ctx);
        */
    }

    /* new ends */
    d_print(1, "[handle ret finishes]\n");
    return 0x0;
}

int graph_engine::register_symbol(char* line)
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
    this->add_symbol(&this->symbols, off, lib_name, func_name);

    return 0x0;
}

/* plugin czy nie? */
int graph_engine::register_lib(char* line)
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
    this->add_lib(off, name);

    return 0x0;
}

/* plugin czy nie? */
int graph_engine::deregister_lib(char* line)
{
    char* cmd;
    OFFSET off;

    cmd = strtok(line, ",");
    off = strtoul(strtok(0x0, ","), 0x0, 0x10);

    //printf("Deregistering lib at 0x%08x\n", off);
    this->del_lib(off);

    return 0x0;

}

int graph_engine::register_blacklist(char* line)
{
    char* cmd;
    char* lib_name;

    cmd = strtok(line, ",");
    lib_name = strtok(0x0, ",");
    this->add_blacklist(lib_name);

    return 0x0;
}

int graph_engine::register_blacklist_addr(char* line)
{
    char* cmd;
    OFFSET addr;

    cmd = strtok(line, ",");
    addr = strtoul(strtok(0x0, ","), 0x0, 0x10);
    this->add_blacklist_addr(addr);

    return 0x0;
}

int graph_engine::register_included(char* line)
{
    char* cmd;
    char* func_name;

    cmd = strtok(line, ",");
    func_name = strtok(0x0, ",");
    this->add_included(func_name);

    return 0x0;
}

int graph_engine::register_wanted(char* line)
{
    char* cmd;
    char* func_name;

    cmd = strtok(line, ",");
    func_name = strtok(0x0, ",");
    this->add_wanted(func_name);

    return 0x0;
}


/* parsing out options end */

int graph_engine::parse_option(char* line)
{
    if(line[0] == 'F' && line[1] == 'E')
        this->register_fence(line);

    if(line[0] == 'S' && line[1] == 'P')
        this->register_prefix(line);

    if(line[0] == 'B' && line[1] == 'L')
        this->register_blacklist(line);

    if(line[0] == 'B' && line[1] == 'A')
        this->register_blacklist_addr(line);

    if(line[0] == 'S' && line[1] == 'A')
        this->register_silenced_addr(line);

    if(line[0] == 'I' && line[1] == 'N')
        this->register_included(line);

    if(line[0] == 'F' && line[1] == 'W')
        this->register_wanted(line);

    if(line[0] == 'I' && line[1] == 'W')
        this->register_wanted_i(line);

    if(line[0] == 'E' && line[1] == 'W')
        this->register_wanted_e(line);

    if(line[0] == 'J' && line[1] == 'E')
        this->jxx_set(0x1);

    if(line[0] == 'J' && line[1] == 'D')
        this->jxx_set(0x0);

    if(line[0] == 'J' && line[1] == 'C')
        this->jxx_clear();

    if(line[0] == 'O' && line[1] == 'U')
        this->register_comment(line);

    if(line[0] == 'R' && line[1] == 'L')
        this->register_lib(line);
    
    if(line[0] == 'D' && line[1] == 'L')
        this->deregister_lib(line);
            
    if(line[0] == 'S' && line[1] == 'Y')
        this->register_symbol(line);

    return 0x0;
}

/* routines overwritten */

int graph_engine::r_jb_jc_jnae(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JB_JC_JNAE;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jae_jnb_jnc(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JAE_JNB_JNC;
    this->cur_info->jumping = 0x1;

    return 0x0;
}

int graph_engine::r_je_jz(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JE_JZ;
    this->cur_info->jumping = 0x1;

    return 0x0;
}

int graph_engine::r_jne_jnz(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JNE_JNZ;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jbe_jna(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JBE_JNA;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_ja_jnbe(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JA_JNBE;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_js(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JS;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jns(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JNS;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jp_jpe(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JP_JPE;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jnp_jpo(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JNP_JPO;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jl_jnge(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JL_JNGE;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jge_jnl(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JGE_JNL;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jle_jng(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JLE_JNG;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jg_jnle(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JG_JNLE;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_jxx(BYTE_t* b)
{
    this->cur_info->before_jmp_code = JMP_CODE_JXX;
    this->cur_info->jumping = 0x1;
    return 0x0;
}

int graph_engine::r_retn(BYTE_t*)
{
    d_print(3, "retn\n");

    if(this->taint_eng->started && !this->taint_eng->finished)
        this->cur_info->returning = 0x3;

    return 0x0;

}

int graph_engine::r_ret(BYTE_t*)
{
    d_print(3, "ret\n");

    if(this->taint_eng->started && !this->taint_eng->finished)
        this->cur_info->returning = 0x3;

    return 0x0;
}

int graph_engine::r_call_rel(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    DWORD_t target, target_2;
    DWORD disp32_reint, *disp32p;

    ret_addr = this->taint_eng->reg_restore_32(EIP);
    ret_addr += this->taint_eng->current_instr_length;

    target.from_mem(instr_ptr + this->taint_eng->current_instr_length);
    ret_addr += 0x4;

    disp32_reint = target.get_DWORD();
    disp32p = (signed int*)&(disp32_reint);

    target_2 = ret_addr + *disp32p; //signed displacement & operand size

    d_print(1, "In call\n");
    if(this->taint_eng->started && !this->taint_eng->finished)
    {
        this->cur_info->target = target_2.get_DWORD();
        this->cur_info->next = ret_addr.get_DWORD();
        this->cur_info->calling = 1;
        cur_info->source = this->taint_eng->current_eip;
        d_print(1, "Next call source = 0x%08x\n", this->taint_eng->current_eip);
    }

    d_print(3, "ret_addr: 0x%08x, target: 0x%08x, target2: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD(), target_2.get_DWORD());

    return 0x0;
}

int graph_engine::r_call_abs_near(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    DWORD_t target;
    WORD_t target_16;
    modrm_ptr rm, r;
    char out_line[MAX_NAME];

    this->taint_eng->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_UNDEFINED, MODE_UNDEFINED);

    ret_addr = this->taint_eng->reg_restore_32(EIP);

    if(rm.region == MODRM_REG)
    {
            target = this->taint_eng->reg_restore_32(rm.offset);
    }
    else
    {
            this->taint_eng->restore_32(rm.offset, target);
    }

    ret_addr += this->taint_eng->current_instr_length;
    d_print(3, "Adding 0x%02x to ret\n", this->taint_eng->current_instr_length);

    //this->handle_call(this->cur_info, target.get_DWORD(), ret_addr.get_DWORD());
    d_print(1, "In call\n");
    if(this->taint_eng->started && !this->taint_eng->finished)
    {
        this->cur_info->target = target.get_DWORD();
        this->cur_info->next = ret_addr.get_DWORD();
        this->cur_info->calling = 1;
        cur_info->source = this->taint_eng->current_eip;
        d_print(1, "Next call source = 0x%08x\n", this->taint_eng->current_eip);
    }

    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());
    
    return 0x0;
}

int graph_engine::r_call_abs_far(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    DWORD_t target;
    char out_line[MAX_NAME];

    target.from_mem(instr_ptr + this->taint_eng->current_instr_length);
    ret_addr = this->taint_eng->reg_restore_32(EIP);
    ret_addr += 0x5;

    //this->handle_call(this->cur_info, target.get_DWORD(), ret_addr.get_DWORD());
    d_print(1, "In call\n");
    if(this->taint_eng->started && !this->taint_eng->finished)
    {
        this->cur_info->target = target.get_DWORD();
        this->cur_info->next = ret_addr.get_DWORD();
        this->cur_info->calling = 1;
        cur_info->source = this->taint_eng->current_eip;
        d_print(1, "Next call source = 0x%08x\n", this->taint_eng->current_eip);
    }

    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());
    return 0x0;
}

int graph_engine::r_jmp_rel_16_32(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    DWORD_t target;
    char out_line[MAX_NAME];
    CONTEXT_GRAPH* cur_ctx;
    cur_ctx = &this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]];

    target.from_mem(instr_ptr + this->taint_eng->current_instr_length);
    ret_addr = this->taint_eng->reg_restore_32(EIP);
    ret_addr += 0x5;
    target += ret_addr;

    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());

    if(this->taint_eng->started && !this->taint_eng->finished)
    {
        cur_ctx->target = target.get_DWORD();
    }

    return 0x0;
}

int graph_engine::r_jmp_rm_16_32(BYTE_t* instr_ptr)
{
    modrm_ptr rm, r;
    WORD_t src_16;
    DWORD_t src_32;

    this->taint_eng->a_decode_modrm(instr_ptr +1, &r, &rm, MODE_16, MODE_16);

    switch(rm.region)
    {
        case (MODRM_REG):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    src_16 = this->taint_eng->reg_restore_16(rm.offset);
                    break;
                case MODRM_SIZE_32:
                    src_32 = this->taint_eng->reg_restore_32(rm.offset);
                    break;
            }
            break;
        case (MODRM_MEM):
            switch(rm.size)
            {
                case MODRM_SIZE_16:
                    this->taint_eng->restore_16(rm.offset, src_16);
                    break;
                case MODRM_SIZE_32:
                    this->taint_eng->restore_32(rm.offset, src_32);
                    break;
            }
            break;
    }

    DWORD_t ret_addr;
    DWORD_t target;
    target = 0x0 + src_16.get_WORD() + src_32.get_DWORD();

    CONTEXT_GRAPH* cur_ctx;
    cur_ctx = &this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]];

    ret_addr = this->taint_eng->reg_restore_32(EIP);
    ret_addr += 0x5;

    d_print(3, "ret_addr: 0x%08x, target: 0x%08x\n", ret_addr.get_DWORD(), target.get_DWORD());

    if(this->taint_eng->started && !this->taint_eng->finished)
    {
        cur_ctx->target = target.get_DWORD();
    }

    this->cur_info->before_jmp_code = JMP_CODE_RM;
    this->cur_info->jumping = 0x1;


    return 0x0;
}

int graph_engine::r_jmp_rel_8(BYTE_t* instr_ptr)
{
    DWORD_t ret_addr;
    BYTE_t target;
    DWORD_t target_2;
    char disp8_reint, *disp8p;

    CONTEXT_GRAPH* cur_ctx;
    cur_ctx = &this->ctx_info[this->taint_eng->tids[this->taint_eng->cur_tid]];

    ret_addr = this->taint_eng->reg_restore_32(EIP);
    ret_addr += this->taint_eng->current_instr_length;
    ret_addr += 0x1;

    target.from_mem(instr_ptr + this->taint_eng->current_instr_length);
    disp8_reint = target.get_BYTE();
    disp8p = (char*)&(disp8_reint);
    target_2 = ret_addr + *disp8p; //signed displacement & operand size

    d_print(3, "ret_addr: 0x%08x, target: 0x%08x, target2: 0x%08x\n", ret_addr.get_DWORD(), target.get_BYTE(), target_2.get_DWORD());

    if(this->taint_eng->started && !this->taint_eng->finished)
    {
        cur_ctx->target = target_2.get_DWORD();
    }

    /* [TODO:] wydaje sie ze poinno tu byc? */
    this->cur_info->before_jmp_code = JMP_CODE_RM;
    this->cur_info->jumping = 0x1;

    return 0x0;
}

int graph_engine::r_decode_execute_ff(BYTE_t* addr)
{
    BYTE_t* modrm_byte_ptr;
    modrm_ptr r, rm;

    modrm_byte_ptr = addr +1;
    this->taint_eng->a_decode_modrm(modrm_byte_ptr, &r, &rm, MODE_32, MODE_32);

    switch(r.offset)
    {
        case EAX: //0x0 
            break;
        case ECX: //0x1 
            break;
        case EDX: //0x2 
            return this->r_call_abs_near(addr);
            break;
        case EBX: //0x3 
            return this->r_call_abs_far(addr);
            break;
        case ESP: //0x4
            return this->r_jmp_rm_16_32(addr);
            break;
        case ESI: //0x6 
            break;
    }

    d_print(3, "Missing routine for decoding opcode: 0x%x, extension: 0x%x\n", addr->get_BYTE(), r.offset);

    return 0x0;
}



