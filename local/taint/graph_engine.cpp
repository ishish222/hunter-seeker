#include <graph_engine.h>
#include <taint_emul_x86.h>
#include <string.h>
#include <debug.h>
#include <plugin.h>

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

int graph_engine::enter_loop_demo(CONTEXT_INFO* info)
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

int graph_engine::exit_loop_demo(CONTEXT_INFO* info)
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

int graph_engine::enter_loop(CONTEXT_INFO* info)
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

int graph_engine::exit_loop(CONTEXT_INFO* info)
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

int graph_engine::check_collecting(CONTEXT_INFO* info)
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
    CONTEXT_INFO* info;
    DWORD tid_no;

    tid_no = this->tids[tid];

    info = &this->ctx_info[tid_no];

    detox(comment);

    d_print(1, "Writing out comment @ %d: %s", this->taint_eng->last_instr_count, comment);
    print_empty_call(info, comment, node_color[CODE_COMMENT]);
    return 0x0;
}

/* returns 1 if in loop, 0 otherwise */
int graph_engine::check_loop_2(CONTEXT_INFO* info)
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


void graph_engine::print_call_open(CONTEXT_INFO* cur_ctx, char* line, const char* color)
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


void graph_engine::print_call(CONTEXT_INFO* cur_ctx, char* line, const char* color)
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

void graph_engine::print_empty_call(CONTEXT_INFO* cur_ctx, char* line, const char* color)
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

void graph_engine::print_ret(CONTEXT_INFO* cur_ctx)
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



int graph_engine::pre_execute_instruction_callback(DWORD eip)
{
    fprintf(stderr, "graph_engine::pre_execute_instruction_callback\n");


    return 0x0;
}

int graph_engine::post_execute_instruction_callback(DWORD eip)
{
    fprintf(stderr, "graph_engine::post_execute_instruction_callback\n");

    return 0x0;
}

/* callbacks */

int graph_engine::start_callback()
{
    char out_line[MAX_NAME];

    sprintf(out_line, "[ST]");
    print_empty_call(&this->ctx_info[this->tids[this->cur_tid]], out_line, node_color[CODE_RED]);

    return 0x0;
}

int graph_engine::finish_callback()
{
    fprintf(stderr, "graph_engine::start_callback\n");

    return 0x0;
}

int graph_engine::add_thread_callback(CONTEXT_info ctx_info)
{
    unsigned i;
    d_print(1, "test");
    d_print(1, "taint_eng: 0x%08x\n", this->taint_eng);
    d_print(1, "tids table addr: 0x%08x\n", this->taint_eng->tids);
    d_print(1, "this->taint_eng->tid_count: 0x%08x\n", this->taint_eng->tid_count);

    tid_count = this->taint_eng->tid_count;
    d_print(1, "Tes1");

    if(!this->taint_eng->already_added(ctx_info.thread_id))
    {
        d_print(1, "Tes2");
        if(strlen(this->prefix) > 0x1)
        {
            sprintf(this->ctx_info[tid_count].graph_filename, "%s_TID_%08X_2.mm", this->prefix, ctx_info.thread_id);
            d_print(1, "Tes3");
        }
        else
        {
            sprintf(this->ctx_info[tid_count].graph_filename, "TID_%08X_2.mm", ctx_info.thread_id);
            d_print(1, "Tes4");
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
        this->ctx_info[tid_count].tid = ctx_info.thread_id;
    }
    d_print(1, "Te99");

    return 0x0;
}

int graph_engine::del_thread_callback(DWORD tid)
{
    return 0x0;
}

int graph_engine::del_thread_srsly_callback(DWORD tid)
{
    d_print(1, "Removing  thread: 0x%08x\n", tid);

    fclose(this->ctx_info[this->taint_eng->tids[tid]].graph_file);

    return 0x0;
}

