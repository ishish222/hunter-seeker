#include <graph_engine.h>
#include <taint_emul_x86.h>

int graph_engine::pre_execute_instruction_callback(taint_x86* taint_eng, DWORD eip)
{
    fprintf(stderr, "graph_engine::pre_execute_instruction_callback\n");


    return 0x0;
}

int graph_engine::post_execute_instruction_callback(taint_x86* taint_eng, DWORD eip)
{
    fprintf(stderr, "graph_engine::post_execute_instruction_callback\n");

    return 0x0;
}

int graph_engine::start_callback(taint_x86* taint_eng)
{
    fprintf(stderr, "graph_engine::start_callback\n");

    return 0x0;
}

int graph_engine::finish_callback(taint_x86* taint_eng)
{
    fprintf(stderr, "graph_engine::start_callback\n");

    return 0x0;
}
