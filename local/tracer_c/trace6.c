#include "trace6.h"
#include <stdio.h>
#include <stdlib.h>
#include "winsock2.h"

#define TRACE_CONTROLLER_IP "127.0.0.1"
#define TRACE_CONTROLLER_PORT 12341
//#include <winsock.h>

//#pragma comment(lib,"ws2_32.lib") //Winsock Library

/* implemented functions */

/* add_readsign */

int reload_out_file();
TRACE_CONFIG* my_trace;
char spawned = 0x0;
FILE* log;
char started = 0x0;
char instr_count_s[0x20];
int full_log = 0x0;
DWORD scan_on;
unsigned scan_count;
READ_RECORD last_read_record;
WATCHED watched[WATCH_LIMIT];
HANDLE file_handle;
DWORD sysenter_esp;
DWORD sysenter_no;
FILE* file;
char buffer[buf_size];
int index = 0;
char instr[0x60];
const wchar_t* target_lib = L"ntdll.dll";
DWORD target_lib_off = 0x0;
const char* target_call = "NtCreateFile";
DWORD target_call_off = 0x44a10;
DWORD target_off = 0x0;
CREATE_PROCESS_DEBUG_INFO cpdi;
int myPID = 0x0;
int myTID = 0x0;
DWORD img_base;
DWORD nt1_off;
DWORD nt2_off;
DWORD nt3_off;
DWORD nt4_off;
DWORD sysenter_off;
DWORD sysret_off;
LOCATION_DESCRIPTOR last_arg = {0x0, 0x0, LOCATION_END, LOCATION_END, 0x0};
LOCATION last_location = {0x0, 0x0};
LOCATION_DESCRIPTOR syscall_out_args[MAX_SYSCALL_ENTRIES][MAX_SYSCALL_OUT_ARGS];
LOCATION syscall_out_args_dump_list[MAX_SYSCALL_OUT_ARGS];
DWORD buffer_addr;
DWORD size_addr;
CREATE_THREAD_DEBUG_INFO2 threads[0x100000000];
BREAKPOINT* add_breakpoint(char*, REACTION*);
DWORD find_lib(char* name);
BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);
SIZE_T dump_mem(FILE*, void*, SIZE_T);

char* blacklist_lib[] = {"kernel32.dll", "ntdll.dll", "user32.dll"};
DWORD blacklist_addr[] = {};
char line2[MAX_LINE];

void read_memory(HANDLE, void*, void*, SIZE_T, SIZE_T*);
void e_reaction_handler(void* data);
void i_reaction_handler(void* data);
void print_context(CONTEXT*);
int del_breakpoint(DWORD);
int resolve_region(LOCATION_DESCRIPTOR*, LOCATION*);
int add_to_buffer(char*);
int read_context(DWORD, CONTEXT*);
int write_context(DWORD, CONTEXT*);
void write_memory(HANDLE , void* , void* , SIZE_T , SIZE_T* );
int check_lib_loaded(char*);
int enable_reaction(char*);
int disable_reaction(char*);
int unwrite_breakpoint(BREAKPOINT* bp);
int update_breakpoint(BREAKPOINT* bp);
OFFSET resolve_loc_desc(LOCATION_DESCRIPTOR_NEW* d);
REACTION* find_reaction(char*);
int read_dword(DWORD addr);
int report_dword(DWORD addr);
DWORD read_register(DWORD tid_id, char* reg);
int report_register(DWORD tid_id, char* reg);

int d_print(const char* format, ...)
{
    va_list argptr;
    char line[MAX_LINE];

    //if(my_trace != 0x0)
    if(0x0)
    {
        if(my_trace->log != 0x0)
        {
            va_start(argptr, format);
            //vfd_print(line, my_trace->log, format, argptr);
            vsprintf(line, format, argptr);
            va_end(argptr);
            fwrite(line, strlen(line), 1, my_trace->log);
            fflush(my_trace->log);
        }
    }
    else
    {
        va_start(argptr, format);
        vfprintf(stdout, format, argptr);
        va_end(argptr);
        fflush(stdout);
    }

    return 0x0;
}

void react_sample_routine_1(void* data)
{
    d_print("Sample routine 1\n");
    return;
}

void react_set_ZF(void* data)
{
    d_print("Setting ZF\n");

    int i;
    unsigned id, thread_idx;
    HANDLE myHandle = (HANDLE)-0x1;

    id = my_trace->last_event.dwThreadId;
    thread_idx = my_trace->thread_map[id];
    myHandle = my_trace->threads[thread_idx].handle;

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_CONTROL;
    if(GetThreadContext(myHandle, &ctx) == 0x0)
    {
        d_print("Failed to get context, error: 0x%08x\n", GetLastError());
    }
    d_print("before setting: 0x%08x\n", ctx.EFlags);
    /* zeroing */
    print_context(&ctx);
    ctx.EFlags |= SET_ZF_FLAGS;
    d_print("after setting: 0x%08x\n", ctx.EFlags);
    print_context(&ctx);

    SetThreadContext(myHandle, &ctx);

    return;
}

void react_zero_ZF(void* data)
{
    d_print("Zeroing ZF\n");

    int i;
    unsigned id, thread_idx;

    id = my_trace->last_event.dwThreadId;

    CONTEXT ctx;
    read_context(id, &ctx);
    d_print("before zeroing: 0x%08x\n", ctx.EFlags);
    /* zeroing */
    print_context(&ctx);
    ctx.EFlags &= CLEAR_ZF_FLAGS;
    d_print("after zeroing: 0x%08x\n", ctx.EFlags);
    print_context(&ctx);

    write_context(id, &ctx);

    return;
}

void react_zero_SF(void* data)
{
    d_print("Zeroing SF\n");

    int i;
    unsigned id, thread_idx;

    id = my_trace->last_event.dwThreadId;

    CONTEXT ctx;
    read_context(id, &ctx);
    d_print("before zeroing: 0x%08x\n", ctx.EFlags);
    /* zeroing */
    print_context(&ctx);
    ctx.EFlags &= CLEAR_SF_FLAGS;
    d_print("after zeroing: 0x%08x\n", ctx.EFlags);
    print_context(&ctx);

    write_context(id, &ctx);

    return;
}

void update_region_old(LOCATION* location)
{
    DWORD size_wrote;
    char line[MAX_LINE];

    sprintf(line, "# Current mod position: 0x%08x\n", ftell(my_trace->mods));
    add_to_buffer(line);

    size_wrote = dump_mem(my_trace->mods, (void*)location->off, location->size);
    if(size_wrote == location->size)
    {
        d_print("[Updated location: 0x%08x, size: 0x%08x]\n", location->off, location->size);
        sprintf(line, "UP,0x%08x,0x%08x\n", location->off, location->size);
        add_to_buffer(line);
        sprintf(line, "# Current mod position: 0x%08x\n", ftell(my_trace->mods));
        add_to_buffer(line);
    }

    return;
}

void update_region(unsigned id)
{
    DWORD size_wrote;
    char line[MAX_LINE];

    REGION* region = &my_trace->regions[id];
    OFFSET off;
    OFFSET size;

    off = resolve_loc_desc(region->off);
    size = resolve_loc_desc(region->size);

    sprintf(line, "# Current mod position: 0x%08x\n", ftell(my_trace->mods));
    add_to_buffer(line);

    size_wrote = dump_mem(my_trace->mods, (void*)off, size);
    if(size_wrote == size)
    {
        d_print("[Updated location: 0x%08x, size: 0x%08x]\n", off, size);
        sprintf(line, "UP,0x%08x,0x%08x\n", off, size);
        add_to_buffer(line);
        sprintf(line, "# Current mod position: 0x%08x\n", ftell(my_trace->mods));
        add_to_buffer(line);
    }

    return;
}

void react_update_region_1(void* data)
{
    d_print("Updating region 1\n");
    update_region(0x0);
    return;
}

void react_update_region_2(void* data)
{
    d_print("Updating region 2\n");
    update_region(0x1);
    return;
}

void output_register(char* reg)
{
    d_print("Outputting register %s\n", reg);
    char line[MAX_LINE];
    OFFSET val;

    val = read_register(my_trace->last_tid, reg); 

    sprintf(line, "OU,%s: 0x%08x\n", reg, val);
    add_to_buffer(line);

    return;
}

void react_output_eax(void* data)
{
    output_register("EAX");
}

void react_output_ebx(void* data)
{
    output_register("EBX");
}

void react_output_ecx(void* data)
{
    output_register("ECX");
}

void react_output_edx(void* data)
{
    output_register("EDX");
}

void react_output_edi(void* data)
{
    output_register("EDI");
}

void react_output_esi(void* data)
{
    output_register("ESI");
}

void react_output_esp(void* data)
{
    output_register("ESP");
}

void react_output_eip(void* data)
{
    output_register("EIP");
}

void output_arg_unicode_string_x(unsigned x)
{
    d_print("Outputting arg %d\n", x);
    char line[MAX_LINE];
    char snap[SNAP_SIZE*2];

    DWORD read;
    OFFSET esp;
    OFFSET addr;

    CONTEXT ctx;
    read_context(0x0, &ctx);
    esp = ctx.Esp;
    esp += (x * 0x4);

    addr = read_dword(esp);

    read_memory(my_trace->cpdi.hProcess, (void*)addr, (void*)snap, SNAP_SIZE*2, &read);
    if(read == SNAP_SIZE*2)
    {
        sprintf(line, "OU,Arg%d: %ls\n", x, snap);
        add_to_buffer(line);
    }
    else
    {
        sprintf(line, "Failed to read UNICODE string @\n", esp);
        add_to_buffer(line);
    }

    return;
}

void react_output_arg_unicode_str_0(void* data)
{
    output_arg_unicode_string_x(0);
}

void react_output_arg_unicode_str_1(void* data)
{
    output_arg_unicode_string_x(1);
}

void react_output_arg_unicode_str_2(void* data)
{
    output_arg_unicode_string_x(2);
}

void react_output_arg_unicode_str_3(void* data)
{
    output_arg_unicode_string_x(3);
}

void react_output_arg_unicode_str_4(void* data)
{
    output_arg_unicode_string_x(4);
}

void react_output_arg_unicode_str_5(void* data)
{
    output_arg_unicode_string_x(5);
}

void react_output_arg_unicode_str_6(void* data)
{
    output_arg_unicode_string_x(6);
}

void react_output_arg_unicode_str_7(void* data)
{
    output_arg_unicode_string_x(7);
}

void output_arg_string_x(unsigned x)
{
    d_print("Outputting arg %d as ANSI string\n", x);
    char line[MAX_LINE];
    char snap[SNAP_SIZE];

    DWORD read;
    OFFSET esp;
    OFFSET addr;

    CONTEXT ctx;
    read_context(0x0, &ctx);
    d_print("ESP: 0x%08x\n", esp);
    esp = ctx.Esp;
    esp += (x * 0x4);
    d_print("arg addr: 0x%08x\n", esp);

    addr = read_dword(esp);
    d_print("str addr: 0x%08x\n", addr);

    read_memory(my_trace->cpdi.hProcess, (void*)addr, (void*)snap, SNAP_SIZE, &read);
    if(read == SNAP_SIZE)
    {
        unsigned i;
        for(i = 0x0; i< 0x5; i++)
            d_print("%c ", snap[i]);
        d_print("\n");

        sprintf(line, "OU,Arg%d: %s\n", x, snap);
        add_to_buffer(line);
    }
    else
    {
        sprintf(line, "Failed to read ANSI string @\n", esp);
        add_to_buffer(line);
    }

    return;
}

void react_output_arg_str_0(void* data)
{
    output_arg_string_x(0);
}

void react_output_arg_str_1(void* data)
{
    output_arg_string_x(1);
}

void react_output_arg_str_2(void* data)
{
    output_arg_string_x(2);
}

void react_output_arg_str_3(void* data)
{
    output_arg_string_x(3);
}

void react_output_arg_str_4(void* data)
{
    output_arg_string_x(4);
}

void react_output_arg_str_5(void* data)
{
    output_arg_string_x(5);
}

void react_output_arg_str_6(void* data)
{
    output_arg_string_x(6);
}

void react_output_arg_str_7(void* data)
{
    output_arg_string_x(7);
}

void output_arg_x(unsigned x)
{
    d_print("Outputting arg %d\n", x);
    char line[MAX_LINE];

    OFFSET val;
    OFFSET esp;

    CONTEXT ctx;
    read_context(0x0, &ctx);
    esp = ctx.Esp;
    esp += (x * 0x4);

    val = read_dword(esp);

    sprintf(line, "OU,Arg%d: %08x\n", x, val);
    add_to_buffer(line);

    return;
}

void react_output_arg_0(void* data)
{
    output_arg_x(0);
}

void react_output_arg_1(void* data)
{
    output_arg_x(1);
}

void react_output_arg_2(void* data)
{
    output_arg_x(2);
}

void react_output_arg_3(void* data)
{
    output_arg_x(3);
}

void react_output_arg_4(void* data)
{
    output_arg_x(4);
}

void react_output_arg_5(void* data)
{
    output_arg_x(5);
}

void react_output_arg_6(void* data)
{
    output_arg_x(6);
}

void react_output_arg_7(void* data)
{
    output_arg_x(7);
}

void react_cry_antidebug_1(void* data)
{
    OFFSET addr = 0x4103e4;
    DWORD wrote;
    char val1 = 0xf9;
    char val2 = 0x01;

    d_print("Modifying address: 0x%08x\n", addr);
    write_memory(my_trace->procHandle, (void*)addr, (void*)&val1, 0x1, &wrote);
    addr++;
    write_memory(my_trace->procHandle, (void*)addr, (void*)&val2, 0x1, &wrote);
    
    return;
}

void react_skip_on(void* data)
{
    char line[MAX_LINE]; 
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;

    my_trace->threads[my_trace->thread_map[tid]].skipping += 0x1;

    sprintf(line, "# Increasing skipping in TID: 0x%08x @ %p\n", tid, (DWORD)my_trace->last_exception.ExceptionAddress);
    add_to_buffer(line);

    if(my_trace->threads[my_trace->thread_map[tid]].skipping == 0x1)
    {
        unset_ss(tid);
        d_print("Skipping in TID: 0x%08x...\n", tid);
        sprintf(line, "# Started skipping in TID: 0x%08x\n", tid);
        add_to_buffer(line);
    }

    return;
}

void react_skip_off(void* data)
{
    char line[MAX_LINE]; 
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;

    my_trace->threads[my_trace->thread_map[tid]].skipping -= 0x1;

    sprintf(line, "# Decreasing skipping in TID: 0x%08x @ %p\n", tid, (DWORD)my_trace->last_exception.ExceptionAddress);
    add_to_buffer(line);

    if(my_trace->threads[my_trace->thread_map[tid]].skipping < 0x1)
    {
        d_print("Finished skippingin TID: 0x%08x\n", tid);
        my_trace->threads[my_trace->thread_map[tid]].skipping = 0x0;
        set_ss(tid);
        sprintf(line, "# Stopped skipping in TID: 0x%08x\n", tid);
        add_to_buffer(line);
    }

    return;
}

int enable_all_reactions()
{
    unsigned i;

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        enable_reaction(my_trace->reactions[i].reaction_id);
    }
    return 0x0;
}

int disable_all_reactions()
{
    unsigned i;

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        disable_reaction(my_trace->reactions[i].reaction_id);
    }
    return 0x0;
}

int enable_reaction(char* reaction_id)
{
    d_print("[enable_reaction]\n");
    unsigned i;

    char another[MAX_LINE]; 
    char* another_r;

    /* take care of chained enabling */
    strcpy(another, reaction_id);
    d_print("React string: %s\n", another);
    if(strstr(another, ":"))
    {
        another_r = strtok(another, ":");
        while(another_r)
        {
            d_print("Found another reaction: %s\n", another_r);
            enable_reaction(another_r);    
            another_r = strtok(0x0, ":");
        }
        return 0x0;
    }

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        /* locate i_reaction */
        if(!strcmp(reaction_id, my_trace->reactions[i].reaction_id))
        {
            d_print("Enabling reaction %s\n", reaction_id);
            my_trace->reactions[i].enabled = 0x1;
            
            BREAKPOINT* bp;
            bp = my_trace->reactions[i].bp;
            bp->enabled = 0x1; /* any reaction enabled makes bp enabled */
            update_breakpoint(bp);
        }
    }
    d_print("[enable_reaction ends]\n");
    return 0x0;
}
int enable_reaction_rid(unsigned routine_id)
{
    d_print("[enable_reaction_rid]\n");
    unsigned i;

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        /* locate i_reaction */
        if(routine_id == my_trace->reactions[i].routine_id)
        {
            d_print("Enabling reaction by rid 0x%02x\n", routine_id);
            enable_reaction(my_trace->reactions[i].reaction_id);
        }
    }
    d_print("[enable_reaction_rid ends]\n");
    return 0x0;
}


int disable_reaction(char* reaction_id)
{
    d_print("[disable_reaction]\n");
    unsigned i;

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        /* locate i_reaction */
        if((reaction_id[0] == my_trace->reactions[i].reaction_id[0]) && (reaction_id[1] == my_trace->reactions[i].reaction_id[1]))
        {
            d_print("Disabling reaction %s\n", reaction_id);
            my_trace->reactions[i].enabled = 0x0;
            break;            
        }
    }

    d_print("Verifying if BP is still enabled\n");

    BREAKPOINT* bp;
    REACTION* cur_reaction;
    bp = my_trace->reactions[i].bp;
    d_print("Got BP @ %p\n", bp);

    char some_enabled = 0x0;

    for(i = 0x0; i< bp->reaction_count; i++)
    {
        d_print("Checking BP reaction %d\n", i);
        cur_reaction = bp->reactions[i];
        if(!cur_reaction)
        {
            d_print("Current reaction is broken!\n");
        }

        if(cur_reaction->enabled)
        {
            some_enabled = 0x1;
            break;
        }
    }

    if(!some_enabled)
    {
        bp->enabled = 0x0; /* If all disabled, bp is disabled */
    }
    update_breakpoint(bp);

    d_print("[disable_reaction ends]\n");
    return 0x0;
}

int disable_reaction_rid(unsigned routine_id)
{
    d_print("[disable_reaction_rid]\n");
    unsigned i;

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        /* locate i_reaction */
        if(routine_id == my_trace->reactions[i].routine_id)
        {
            d_print("Disabling reaction by rid 0x%02x\n", routine_id);
            my_trace->reactions[i].enabled = 0x0;
        }
    }
    d_print("[disable_reaction_rid ends]\n");
    return 0x0;
}

int add_to_buffer(char* line)
{
    DWORD written;
    written = 0x0;
//    sprintf(line2, "%s\n", line);
    //written = fwrite(line2, strlen(line2), 1, file);
    written = fwrite(line, strlen(line), 1, my_trace->trace);
    fflush(my_trace->trace);
    return written;
}

void print_context(CONTEXT* ctx)
{
    d_print("Context:\n");
    d_print("EAX:\t0x%08x\n", ctx->Eax);
    d_print("EBX:\t0x%08x\n", ctx->Ebx);
    d_print("ECX:\t0x%08x\n", ctx->Ecx);
    d_print("EDX:\t0x%08x\n", ctx->Edx);
    d_print("ESI:\t0x%08x\n", ctx->Esi);
    d_print("EDI:\t0x%08x\n", ctx->Edi);
    d_print("EBP:\t0x%08x\n", ctx->Ebp);
    d_print("ESP:\t0x%08x\n", ctx->Esp);
    d_print("EFLAGS:\t0x%08x\n", ctx->EFlags);
    d_print("EIP:\t0x%08x\n", ctx->Eip);
}

void react_sysret_refresh(void* data)
{

    d_print("Refreshing\n");
    d_print("Unsetting SS for 0x%08x\n", my_trace->last_tid);
    unset_ss(my_trace->last_tid);
    set_ss(0x0);
}

void read_memory(HANDLE handle, void* from, void* to, SIZE_T size, SIZE_T* read)
{
    DWORD oldProt;
    DWORD ret;

    d_print("read_memory, handle: 0x%08x\n", handle);

    VirtualProtectEx(handle, to, size, PAGE_EXECUTE_READWRITE, &oldProt);

    ret = ReadProcessMemory(handle, from, to, size, read);

    VirtualProtectEx(handle, to, size, oldProt, &oldProt);

    if(!ret)
    {
        d_print("Error: 0x%08x\n", GetLastError());
    }

    return;
}

void write_memory(HANDLE handle, void* to, void* from, SIZE_T size, SIZE_T* written)
{
    DWORD oldProt;
    DWORD ret;
    
    d_print("write_memory, handle: 0x%08x\n", handle);
    VirtualProtectEx(handle, to, size, PAGE_EXECUTE_READWRITE, &oldProt);

    ret = WriteProcessMemory(handle, to, from, size, written);

    if(!ret)
    {
        d_print("Error: 0x%08x\n", GetLastError());
    }

    VirtualProtectEx(handle, to, size, oldProt, &oldProt);
}

int dec_eip(DWORD id)
{
    int i;
    unsigned thread_idx;
    HANDLE myHandle = (HANDLE)-0x1;

    thread_idx = my_trace->thread_map[id];

    myHandle = my_trace->threads[thread_idx].handle;
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_CONTROL;
    if(GetThreadContext(myHandle, &ctx) == 0x0)
    {
        d_print("Failed to get context, error: 0x%08x\n", GetLastError());
    }
    d_print("before: 0x%08x\n", ctx.Eip);
    ctx.Eip -= 0x1;
    d_print("after: 0x%08x\n", ctx.Eip);
    SetThreadContext(myHandle, &ctx);

    return 0x0;
}

void serialize_exception(EXCEPTION_RECORD er, char* buffer)
{
    sprintf(buffer, "0x%08x,0x%08x,0x%08x", 
        er.ExceptionCode,
        er.ExceptionFlags,
        er.ExceptionAddress
        );

    return;
}

void deserialize_exception(EXCEPTION_RECORD* er, char* buffer)
{
    sscanf(buffer, "0x%08x,0x%08x,0x%08x", 
        er->ExceptionCode,
        er->ExceptionFlags,
        er->ExceptionAddress
        );

    return;
}

void serialize_thread(THREAD_ENTRY* thread, char* buffer)
{
    sprintf(buffer, "0x%08x,0x%08x,0x%08x", thread->tid, thread->handle, thread->alive);

}

void serialize_lib(LIB_ENTRY* lib, char* buffer)
{
    sprintf(buffer, "0x%08x,%s", lib->lib_offset, lib->lib_name);
}

void serialize_context(CONTEXT ctx, LDT_ENTRY* ldt, char* buffer)
{
    sprintf(buffer, "0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x", 
        ctx.Eax, 
        ctx.Ecx, 
        ctx.Edx, 
        ctx.Ebx, 
        ctx.Esi, 
        ctx.Edi, 
        ctx.Ebp, 
        ctx.Esp, 
        ctx.EFlags, 
        ctx.Eip,
        ctx.SegGs,
        ctx.SegFs,
        ctx.SegEs,
        ctx.SegDs,
        ctx.SegCs,
        ctx.SegSs,
        ldt[0].LimitLow,
        ldt[0].BaseLow,
        ldt[0].HighWord,
        ldt[1].LimitLow,
        ldt[1].BaseLow,
        ldt[1].HighWord,
        ldt[2].LimitLow,
        ldt[2].BaseLow,
        ldt[2].HighWord,
        ldt[3].LimitLow,
        ldt[3].BaseLow,
        ldt[3].HighWord,
        ldt[4].LimitLow,
        ldt[4].BaseLow,
        ldt[4].HighWord,
        ldt[5].LimitLow,
        ldt[5].BaseLow,
        ldt[5].HighWord
        );

    return;
}

void deserialize_context(CONTEXT* ctx, char* buffer)
{
    sscanf(buffer, 
        "0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x",
        ctx->Eax, 
        ctx->Ecx, 
        ctx->Edx, 
        ctx->Ebx, 
        ctx->Esi, 
        ctx->Edi, 
        ctx->Ebp, 
        ctx->Esp, 
        ctx->EFlags, 
        ctx->Eip
        );

    return;
}

void register_exception(DWORD tid, EXCEPTION_RECORD er)
{
    char line[MAX_LINE];
    char line2[0x100];

    d_print("Registering Exception: code 0x%08x at: 0x%08x\n", er.ExceptionCode, er.ExceptionAddress);

    serialize_exception(er, line2);
    sprintf(line, "EX,0x%08x,%s\n", tid, line2);
    add_to_buffer(line);

    return;
}

void getSelectorEntries(HANDLE handle, CONTEXT ctx, LDT_ENTRY* ldt)
{
    if(GetThreadSelectorEntry(handle, ctx.SegGs, &ldt[0]) == 0x0) d_print("Failed: 0x%08x\n", GetLastError());
    if(GetThreadSelectorEntry(handle, ctx.SegFs, &ldt[1]) == 0x0) d_print("Failed\n");
    if(GetThreadSelectorEntry(handle, ctx.SegEs, &ldt[2]) == 0x0) d_print("Failed\n");
    if(GetThreadSelectorEntry(handle, ctx.SegDs, &ldt[3]) == 0x0) d_print("Failed\n");
    if(GetThreadSelectorEntry(handle, ctx.SegCs, &ldt[4]) == 0x0) d_print("Failed\n");
    if(GetThreadSelectorEntry(handle, ctx.SegSs, &ldt[5]) == 0x0) d_print("Failed\n");
    return;
}

void register_thread(DWORD tid, HANDLE handle)
{
    char line2[0x200];
    char line[MAX_LINE];
    CONTEXT ctx;
    LDT_ENTRY ldt[0x6];

    if(handle == 0x0) 
        handle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);

    DWORD tid_pos;

    if(my_trace->thread_map[tid] == -1)
    {
        tid_pos = my_trace->thread_count;
        
        my_trace->threads[tid_pos].alive = 0x1;
        my_trace->threads[tid_pos].handle = handle;
        my_trace->threads[tid_pos].open = 0x1;
        my_trace->threads[tid_pos].created = 0x1;
        my_trace->threads[tid_pos].tid = tid;
    
        //if(my_trace->threads[tid_pos].handle == 0x0) 
        d_print("Registering: TID 0x%08x, handle 0x%08x\n", tid, handle);

        my_trace->thread_map[tid] = tid_pos;
        my_trace->thread_count ++;

        sprintf(line, "# Thread count: 0x%08x\n", my_trace->thread_count);
        add_to_buffer(line);
    }
    else
    {
        tid_pos = my_trace->thread_map[tid];

        // do not create new, update this one
        d_print("Updating: TID 0x%08x, handle 0x%08x\n", tid, handle);
        my_trace->threads[tid_pos].alive = 0x1;
        my_trace->threads[tid_pos].handle = handle;
    }

    //d_print("Registering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", tid, my_trace->threads[tid_pos].handle, my_trace->thread_count);

    ctx.ContextFlags = CONTEXT_FULL;

    //write info about thread registration
    GetThreadContext(my_trace->threads[tid_pos].handle, &ctx);

    getSelectorEntries(handle, ctx, ldt);
/*
    d_print("GS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegGs, ldt[0].LimitLow, ldt[0].BaseLow, ldt[0].HighWord);
    d_print("FS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegFs, ldt[1].LimitLow, ldt[1].BaseLow, ldt[1].HighWord);
    d_print("ES: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegEs, ldt[2].LimitLow, ldt[2].BaseLow, ldt[2].HighWord);
    d_print("DS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegDs, ldt[3].LimitLow, ldt[3].BaseLow, ldt[3].HighWord);
    d_print("CS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegCs, ldt[4].LimitLow, ldt[4].BaseLow, ldt[4].HighWord);
    d_print("SS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegSs, ldt[5].LimitLow, ldt[5].BaseLow, ldt[5].HighWord);
*/
    //print_context(&ctx);

    serialize_context(ctx, ldt, line2);
    sprintf(line, "RT,0x%08x,%s\n", tid, line2);
    add_to_buffer(line);

    return;
}

int register_thread_debug(DWORD tid, HANDLE handle)
{
    //d_print("Debug registering: TID 0x%08x, handle 0x%08x\n", tid, handle);
    char line2[0x200];
    char line[MAX_LINE];
    CONTEXT ctx;
    LDT_ENTRY ldt[0x6];
    DWORD written;

    written = 0x0;

    if(handle == 0x0) 
        handle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);

   // d_print("Registering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", tid, my_trace->threads[tid_pos].handle, my_trace->thread_count);

    ctx.ContextFlags = CONTEXT_FULL;

    DWORD tid_pos;
    tid_pos = my_trace->thread_map[tid];

    //write info about thread registration
    GetThreadContext(my_trace->threads[tid_pos].handle, &ctx);
    getSelectorEntries(handle, ctx, ldt);


    serialize_context(ctx, ldt, line2);
    sprintf(line, "CT,0x%08x,%s\n", tid, line2);
    written = add_to_buffer(line);

    return written;
}

void deregister_thread(DWORD tid, HANDLE handle)
{
    //d_print("Deregistering: TID 0x%08x, handle 0x%08x\n", tid, handle);
    char line2[0x200];
    char line[MAX_LINE];
    CONTEXT ctx;
    LDT_ENTRY ldt[0x6];

    if(handle == 0x0) 
        handle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);

    DWORD tid_pos;
    tid_pos = my_trace->thread_map[tid];

    my_trace->threads[tid_pos].alive = 0x0;
    my_trace->threads[tid_pos].handle = handle;
    my_trace->threads[tid_pos].open = 0x1;

    //if(my_trace->threads[tid_pos].handle == 0x0) 
    //d_print("Deregistering: TID 0x%08x, handle 0x%08x\n", tid, handle);

    //d_print("Deregistering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", tid, my_trace->threads[tid_pos].handle, my_trace->thread_count);

    ctx.ContextFlags = CONTEXT_FULL;

    //write info about thread registration
    GetThreadContext(my_trace->threads[tid_pos].handle, &ctx);

    getSelectorEntries(handle, ctx, ldt);
    /*
    d_print("GS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegGs, ldt[0].LimitLow, ldt[0].BaseLow, ldt[0].HighWord);
    d_print("FS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegFs, ldt[1].LimitLow, ldt[1].BaseLow, ldt[1].HighWord);
    d_print("ES: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegEs, ldt[2].LimitLow, ldt[2].BaseLow, ldt[2].HighWord);
    d_print("DS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegDs, ldt[3].LimitLow, ldt[3].BaseLow, ldt[3].HighWord);
    d_print("CS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegCs, ldt[4].LimitLow, ldt[4].BaseLow, ldt[4].HighWord);
    d_print("SS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegSs, ldt[5].LimitLow, ldt[5].BaseLow, ldt[5].HighWord);
    */
    //print_context(&ctx);

    serialize_context(ctx, ldt, line2);


    sprintf(line, "DT,0x%08x,%s\n", tid, line2);

    add_to_buffer(line);

    /* not necessary, could be reused for reregistration */
    /*
    if(my_trace->threads[tid_pos].handle != 0x0) 
        CloseHandle(my_trace->threads[tid_pos].handle);

    my_trace->threads[tid_pos].handle = 0x0;
    my_trace->threads[tid_pos].open = 0x0;
    */
    return;
}

void deregister_thread2(DWORD tid)
{
    char line[MAX_LINE];
    DWORD tid_pos;
    tid_pos = my_trace->thread_map[tid];


    if(my_trace->threads[tid_pos].handle != 0x0) 
        CloseHandle(my_trace->threads[tid_pos].handle);

    my_trace->threads[tid_pos].handle = 0x0;
    my_trace->threads[tid_pos].open = 0x0;

    //d_print("Deregistering TID: 0x%08x with handle: 0x%08x\n", tid, my_trace->threads[tid_pos].handle);
    sprintf(line, "DT,0x%08x\n", tid);
 
    add_to_buffer(line);
    return;
}

char* find_file(char* path)
{
    char* ptr;
    char* last;
    unsigned len, i;

    len = strlen(path);

    ptr = path;
    
    for(i = 0x0; i<len; i++)
    {
        if(path[i] == '\\') last = path+i;
    }

    return last+1;
}

int check_lib_loaded(char* lib_name)
{
    unsigned i;

    if(!strcmpi(lib_name, "self"))
    {
        d_print("self is always loaded dummy\n");
        return 0x1;
    }

    for(i = 0x0; i< my_trace->lib_count; i++)
    {
        if(!strcmpi(my_trace->libs[i].lib_name, lib_name))
        {
            if(my_trace->libs[i].loaded)
            {
                d_print("Lib %s is loaded\n", lib_name);
                return i;
            }
        }
    }

    return -1;
}

void register_lib(LOAD_DLL_DEBUG_INFO info)
{
    char path[MAX_LINE];
    char line[MAX_LINE];
    unsigned size = 0;

#ifdef LIB_VER_W7
    //d_print("Trying to resolve\n");
    GetFinalPathNameByHandleA(my_trace->last_event.u.LoadDll.hFile, my_trace->libs[my_trace->lib_count].lib_path, MAX_LINE, VOLUME_NAME_NONE);
    //d_print("Resolved 0x%08x to %s\n", my_trace->last_event.u.LoadDll.hFile, my_trace->libs[my_trace->lib_count].lib_path);
    strcpy(my_trace->libs[my_trace->lib_count].lib_name, find_file(my_trace->libs[my_trace->lib_count].lib_path));
#endif
#ifdef LIB_VER_WXP
    strcpy(my_trace->libs[mt_trace->lib_count].lib_name, "UNKNOWN");
    strcpy(my_trace->libs[mt_trace->lib_count].lib_path, "UNKNOWN");
#endif
    //d_print("Name pointer: %p, len: 0x%08x\n", libs[my_trace->lib_count].lib_name, strlen(libs[my_trace->lib_count].lib_name));

    my_trace->libs[my_trace->lib_count].lib_offset = (DWORD)info.lpBaseOfDll;
    d_print("RL,0x%08x,%s\n", my_trace->libs[my_trace->lib_count].lib_offset, my_trace->libs[my_trace->lib_count].lib_name);
    sprintf(line, "RL,0x%08x,%s\n", my_trace->libs[my_trace->lib_count].lib_offset, my_trace->libs[my_trace->lib_count].lib_name);

    my_trace->libs[my_trace->lib_count].loaded = 0x1;
    //update_e_reactions(&my_trace->libs[lib_count]);

    my_trace->lib_count++;


    add_to_buffer(line);
    return;
}

void deregister_lib(DWORD i)
{
    char line[MAX_LINE];
    sprintf(line, "DL,0x%08x,%s\n", my_trace->libs[i].lib_offset, my_trace->libs[i].lib_name);
    my_trace->libs[i].loaded = 0x0;

    add_to_buffer(line);
}

void deregister_lib(UNLOAD_DLL_DEBUG_INFO info)
{
    char line[MAX_LINE];
    unsigned i;

    for (i = 0x0; i< my_trace->lib_count; i++)
    {
        if(my_trace->libs[i].lib_offset == (DWORD)info.lpBaseOfDll) break;
    }

    sprintf(line, "DL,0x%08x,%s\n", my_trace->libs[i].lib_offset, my_trace->libs[i].lib_name);

    my_trace->libs[i].loaded  = 0x0;
    //d_print(line);
    //d_print("\n");
    add_to_buffer(line);

    return;
}

void ss_callback(void* data);
void end_callback(void* data);
void ntmap_1_callback(void* data);
void ntmap_2_callback(void* data);
void ntread_1_callback(void* data);
void ntread_2_callback(void* data);
void react_sysenter_callback(void* data);
void react_sysret_callback(void* data);

/*TODO: implement! */
/*
void ntread_1_callback(void* data)
{
    d_print("ntread_1\n");
    add_breakpoint(nt4_off, ntread_2_callback);
}

void ntread_2_callback(void* data)
{
    d_print("ntread_2\n");
    add_breakpoint(nt3_off, ntread_1_callback);
}
*/

/* other callbacks */

void react_sysenter_callback(void* data)
{
    d_print("sysenter\n");
    char line[MAX_LINE];

    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;
    unsigned i;

    DWORD tid_pos;
    tid_pos = my_trace->thread_map[tid];

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    if(GetThreadContext(my_trace->threads[tid_pos].handle, &ctx) == 0x0) 
    {
        d_print("Failed to get context, error: 0x%08x\n", GetLastError());
        return;
    }
    d_print("ESP: %p\n", ctx.Esp);
    sysenter_no = ctx.Eax;
    sysenter_esp = ctx.Esp;

    sprintf(line, "# Syscall in TID: 0x%08x no: 0x%08x, stack@ 0x%08x\n", tid, sysenter_no, sysenter_esp);
    add_to_buffer(line);

    deregister_thread(tid, my_trace->threads[tid_pos].handle);
    set_ss(0x0);
}

void react_sysret_callback(void* data)
{
    d_print("sysret\n");

    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;
    DWORD arg_addr, arg_val;
    DWORD size, read, off, size_wrote;
    unsigned i;
    char line[MAX_LINE];
    DWORD tid_pos;
    tid_pos = my_trace->thread_map[tid];

    register_thread(tid, my_trace->threads[tid_pos].handle);

    // dump 0x50 bytes from stack
    /*
    d_print("Block 0x%08x - 0x%08x, 0x%08x bytes\n", sysenter_esp, sysenter_esp + 0x50, 0x50);
    size = dump_mem(my_trace->mods, (void*)sysenter_esp, 0x50);
    sprintf(line, "UP,0x%08x,0x%08x", sysenter_esp, 0x50);
    add_to_buffer(line);
*/

    // prepare dump list
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    if(GetThreadContext(my_trace->threads[tid_pos].handle, &ctx) == 0x0) 
    {
        d_print("Failed to get context, error: 0x%08x\n", GetLastError());
        return;
    }
    d_print("EAX: 0x%08x\n", ctx.Eax);

    /*
    for(i = 0x0; i<MAX_SYSCALL_OUT_ARGS; i++)
            my_trace->syscall_out_args_dump_list[i].off = last_location.off;
    */

    d_print("[[Syscall: 0x%08x @ 0x%08x]]\n", sysenter_no, ctx.Eip);
    for(i = 0x0; i<MAX_SYSCALL_OUT_ARGS; i++)
    {
        d_print("Arg no: 0x%02x\n", i);
        if(my_trace->syscall_out_args[sysenter_no][i].off_location == last_arg.off_location) 
        {
            d_print("Last arg, finishing\n");
            break;
        }
        if(my_trace->syscall_out_args[sysenter_no][i].eax_val_success != STATUS_ANY)
        {
            d_print("Arg not any\n");
            if(my_trace->syscall_out_args[sysenter_no][i].eax_val_success != ctx.Eax) 
            {
                d_print("Wrong EAX\n");
                continue;
            }
        }
        if(0)
        {
            d_print("Locating buffer\n");
            arg_val = 0x0;
            arg_addr = 0x0;
            off = 0x0;
            size = 0x0;

            d_print("ESP: 0x%08x\n", sysenter_esp);

            // decide offset
            switch(my_trace->syscall_out_args[sysenter_no][i].off_location)
            {
                case LOCATION_CONST:
                    d_print("LOCATION_CONST\n");
                    my_trace->syscall_out_args_dump_list[i].off = my_trace->syscall_out_args[sysenter_no][i].off;
                    arg_val = my_trace->syscall_out_args_dump_list[i].off;
                    d_print("Arg off: 0x%08x\n", arg_val);
                    break;
                case LOCATION_MEM:
                    d_print("LOCATION_MEM\n");
                    arg_addr = my_trace->syscall_out_args[sysenter_no][i].off;
                    read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    d_print("Arg off: 0x%08x\n", arg_val);
                    my_trace->syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_STACK:
                    d_print("LOCATION_STACK\n");
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += my_trace->syscall_out_args[sysenter_no][i].off * 0x4;
                    arg_val = arg_addr;
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    d_print("Arg off: 0x%08x\n", arg_val);
                    if(arg_val == 0x0) break;
                    my_trace->syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_ADDR_STACK:
                    d_print("LOCATION_ADDR_STACK\n");
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += my_trace->syscall_out_args[sysenter_no][i].off * 0x4;
                    read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    d_print("Arg off: 0x%08x\n", arg_val);
                    my_trace->syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_ADDR_ADDR_STACK:
                    d_print("LOCATION_ADDR_ADDR_STACK\n");
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += my_trace->syscall_out_args[sysenter_no][i].off * 0x4;
                    read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    arg_addr = arg_val;
                    read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    d_print("Arg off: 0x%08x\n", arg_val);
                    my_trace->syscall_out_args_dump_list[i].off = arg_val;
                    break;
            }

            if(arg_val == 0x0) break;
            off = arg_val;
            arg_val = 0x0;
            arg_addr = 0x0;
            d_print("Locating size\n");

            // decide size
            switch(my_trace->syscall_out_args[sysenter_no][i].size_location)
            {
                case LOCATION_CONST:
                    d_print("LOCATION_CONST\n");
                    my_trace->syscall_out_args_dump_list[i].size = my_trace->syscall_out_args[sysenter_no][i].size;
                    arg_val = my_trace->syscall_out_args_dump_list[i].size;
                    d_print("Arg size: 0x%08x\n", arg_val);
                    break;
                case LOCATION_MEM:
                    d_print("LOCATION_MEM\n");
                    arg_addr = my_trace->syscall_out_args[sysenter_no][i].size;
                    read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    d_print("Arg size: 0x%08x\n", arg_val);
                    my_trace->syscall_out_args_dump_list[i].size = arg_val;
                    break;
                case LOCATION_STACK:
                    d_print("LOCATION_STACK\n");
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += my_trace->syscall_out_args[sysenter_no][i].size * 0x4;
                    read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    d_print("Arg size: 0x%08x\n", arg_val);
                    my_trace->syscall_out_args_dump_list[i].size = arg_val;
                    break;
                case LOCATION_ADDR_STACK:
                    d_print("LOCATION_ADDR_STACK\n");
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += my_trace->syscall_out_args[sysenter_no][i].size * 0x4;
                    read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    arg_addr = arg_val;
                    read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    d_print("Arg size: 0x%08x\n", arg_val);
                    my_trace->syscall_out_args_dump_list[i].size = arg_val;
                    break;
            }

            size = arg_val;

            size_wrote = dump_mem(my_trace->mods, (void*)off, size);
            if(size_wrote == size)
            {
                d_print("[Out argument: 0x%08x, size: 0x%08x]\n", off, size);
                sprintf(line, "UP,0x%08x,0x%08x\n", off, size);
                add_to_buffer(line);
            }
        }

        /* new update */

        LOCATION location;

        d_print("Resolving location\n");
        resolve_region(&my_trace->syscall_out_args[sysenter_no][i], &location);
        update_region_old(&location);

    }


    // special cases

    if(sysenter_no == 0x42) // CreateFile
    {
        // first one
        if(file_handle == 0x0) 
        {
            arg_addr = sysenter_esp + 0x8;
            arg_addr += my_trace->syscall_out_args[sysenter_no][i].size * 0x4;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            arg_addr = arg_val;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&file_handle, 0x4, &read);

            d_print("Hunting for reads from: 0x%08x\n", file_handle);
        }
    }

    if(sysenter_no == 0x111) // ReadFile
    {
        if(file_handle == (HANDLE)my_trace->syscall_out_args_dump_list[0].off)
        {
            // our file, enter taint
            sprintf(line, "RN,0x%08x,0x%08x\n", my_trace->syscall_out_args_dump_list[3].off, my_trace->syscall_out_args_dump_list[3].size);
            add_to_buffer(line);
            d_print("Marked taint: 0x%08x,0x%08x\n", off, size);
        }
    }

    //sprintf(line, "# ret1");
    //add_to_buffer(line);
    

    set_ss(tid);
    d_print("Setting SS for 0x%08x\n", tid);
}



DWORD get_stack(HANDLE tHandle, unsigned index)
{
    CONTEXT ctx;
    DWORD esp;
    DWORD val;
    DWORD read;
    unsigned i;

    GetThreadContext(tHandle, &ctx);
    esp == ctx.Esp;
    for(i = 0x0; i< index; i++)
    {
        esp += 0x4;
    }

    ReadProcessMemory(my_trace->cpdi.hProcess, (void*)(esp), (void*)&val, 0x4, &read);
    
    return val;
}

void createthread_callback(void* data)
{
    DWORD start_addr;
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;
    unsigned i;
    HANDLE tHandle;
    char line[MAX_LINE];

    tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);
    start_addr = get_stack(tHandle, 3);
    
    sprintf(line, "TS,0x%08x\n", start_addr);
    d_print("TS,0x%08x\n", start_addr);
    add_to_buffer(line);
    
}

void isdebuggerpresent_callback_2(void* data);

#define ISDEBUGGER_LOCATION_1 "KernelBase.dll+0x21f8f"
#define ISDEBUGGER_LOCATION_2 "KernelBase.dll+0x21f9c"

void isdebuggerpresent_callback_1(void* data)
{
    d_print("[antidebug] enter\n");

    return;
    
}

void isdebuggerpresent_callback_2(void* data)
{
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;
    unsigned i;
    HANDLE tHandle;
    CONTEXT ctx;
    
    ctx.ContextFlags = CONTEXT_FULL;
    tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);

    /* zero out eax */

    GetThreadContext(tHandle, &ctx);
    d_print("[antidebug] EAX: 0x%08x\n", ctx.Eax);
    ctx.Eax = 0x0;
    SetThreadContext(tHandle, &ctx);
    CloseHandle(tHandle);

    tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);
    GetThreadContext(tHandle, &ctx);
    CloseHandle(tHandle);

    d_print("[antidebug] IsDebuggerPresent handled\n");
    d_print("[antidebug] EAX: 0x%08x\n", ctx.Eax);

    d_print("[antidebug] exit\n");

    return;
    
}

void empty_callback(void* data)
{

    return;
}


int register_all_threads()
{
        HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
        THREADENTRY32 te32; 
 
        // Take a snapshot of all running threads  
        hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
        if( hThreadSnap == INVALID_HANDLE_VALUE ) 
            return( FALSE ); 
 
        // Fill in the size of the structure before using it. 
        te32.dwSize = sizeof(THREADENTRY32 ); 
 
        // Retrieve information about the first thread,
        // and exit if unsuccessful
        if( !Thread32First( hThreadSnap, &te32 ) ) 
        {
            d_print("Thread32First\n");  // Show cause of failure
            CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
            return -0x1;
        }

        // Now walk the thread list of the system,
        // and display information about each thread
        // associated with the specified process
        do 
        { 
            if( te32.th32OwnerProcessID == myPID)
                {
                    my_trace->threads[te32.th32ThreadID].alive = 0x1;
                    my_trace->threads[te32.th32ThreadID].handle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, te32.th32ThreadID);
                    my_trace->threads[te32.th32ThreadID].open = 0x1;

                    my_trace->thread_map[my_trace->thread_count] = te32.th32ThreadID;
                    my_trace->thread_count ++;
                    //d_print("Registering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", te32.th32ThreadID, my_trace->threads[my_trace->thread_count].handle, my_trace->thread_count);
 
                }
 
        } while( Thread32Next(hThreadSnap, &te32 ) );

    //  Don't forget to clean up the snapshot object.
        CloseHandle( hThreadSnap );
    

    return 0x0;
}

int verify_ss(DWORD tid)
{
    CONTEXT ctx;
    int i;
    DWORD cur_tid;
    char line[MAX_LINE];

    if(tid == 0x0)
    {
        for(i = 0x0; i< my_trace->thread_count; i++)
        {
            cur_tid = my_trace->thread_map[i];
            verify_ss(cur_tid);
        }
    }
    else
    {
        HANDLE tHandle;
        ctx.ContextFlags = CONTEXT_CONTROL;
//        tHandle = my_trace->threads[tid_pos].handle;
//        if(!tHandle) return -1;
        tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);

        GetThreadContext(tHandle, &ctx);

        if(ctx.EFlags & CLEAR_SS_FLAGS) 
        {
            sprintf(line, "# Trap is set for 0x%08x\n", tid);
            add_to_buffer(line);
        }

        SetThreadContext(tHandle, &ctx);
        CloseHandle(tHandle);
    }
    return 0x0;
}

int unset_ss(DWORD tid)
{
    CONTEXT ctx;
    int i, tid_pos;
    DWORD cur_tid;
    char line[MAX_LINE];


    if(tid == 0x0)
    {
        for(i = 0x0; i< my_trace->thread_count; i++)
        {
            /* unset for all */
            cur_tid = my_trace->thread_map[i];
            unset_ss(cur_tid);
        }
    }
    else
    {
        /*
        tid_pos = my_tracer->thread_map[tid];
        my_tracer->threads[tid_pos];
        */

        HANDLE tHandle;
        ctx.ContextFlags = CONTEXT_CONTROL;
        tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);
        //tHandle = my_trace->threads[tid_pos].handle;
        if(!tHandle) return -1;

        GetThreadContext(tHandle, &ctx);
        ctx.EFlags &= CLEAR_SS_FLAGS;
//        sprintf(line, "# Disabling trap for 0x%08x\n", tid);
//        add_to_buffer(line);
        SetThreadContext(tHandle, &ctx);
        CloseHandle(tHandle);
    }
    return 0x0;
}

int set_ss(DWORD tid)
{
    char line[MAX_LINE];
    CONTEXT ctx;
    int i, tid_pos;
    DWORD cur_tid;

    /* avoid turning scanning on while skipping, e.g. during syscalls or i_reactions */
    if(my_trace->threads[my_trace->thread_map[tid]].skipping == 0x1)
        return 0x0;

    if(tid == 0x0)
    {
        for(i = 0x0; i< my_trace->thread_count; i++)
        {
            /* set for all */
            cur_tid = my_trace->threads[i].tid;
            set_ss(cur_tid);
        }
    }
    else
    {
        HANDLE tHandle;
        ctx.ContextFlags = CONTEXT_CONTROL;
        tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);
        if(!tHandle) 
        {
            d_print("Failed to open TID: 0x%08x\n", tid);
            return -1;
        }

        GetThreadContext(tHandle, &ctx);
        ctx.EFlags |= SS_FLAGS;
        SetThreadContext(tHandle, &ctx);
        CloseHandle(tHandle);
    }
    return 0x0;
}

void check_debug(DWORD eip, long long unsigned i_count, DWORD id)
{
    unsigned i;
    HANDLE myHandle = (HANDLE)-0x1;
    myHandle = my_trace->threads[id].handle;

    for(i=0x0; i < WATCH_LIMIT; i++)
    {
        if((watched[i].off == eip) || (watched[i].instr == i_count))
        {
            d_print("Offhit: %08x\n", eip);
            d_print("Instr_no: %lld\n", my_trace->instr_count);
//            if(watched[i-1].hit || (i == 0x0))
            if(watched[i].hit <= watched[i].count)
            {
                CONTEXT ctx;
                ctx.ContextFlags = CONTEXT_FULL;
                if(GetThreadContext(myHandle, &ctx) == 0x0)
                {
                    d_print("Failed to get context, error: 0x%08x\n", GetLastError());
                }
                d_print("Print debug info @ 0x%08x\n", eip);
                print_context(&ctx);
                watched[i].count++;
                if(watched[i].scan == 1) 
                {
                    d_print("Scan start triggered by watchpoint %d @ instr no %lld\n", i, i_count);
                    fwrite("# Scan start\n", strlen("# Scan start\n"), 1, file);
                    scan_on = 1;
                }
            }
            else
                watched[i].count++;
                d_print("Previous not hit\n");
        }
    }
    return;
}



void ss_callback(void* data)
{
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD eip;
    DWORD tid;
    DWORD bytes_written;
    int size = 0x0;
    char* disRet;
    int written;
    char line[MAX_LINE];
    char bytes[0x2];
    DWORD tid_pos;

    eip = (DWORD)(de->u.Exception.ExceptionRecord.ExceptionAddress);
        //d_print("%p\n", eip);
    if((my_trace->status != STATUS_DBG_STARTED) && (my_trace->status != STATUS_DBG_SCANNED)) 
    {
        return;
    }

    tid = de->dwThreadId;
    tid_pos = my_trace->thread_map[tid];

    WaitForSingleObject(my_trace->mutex, INFINITE);

    eip = (DWORD)(de->u.Exception.ExceptionRecord.ExceptionAddress);
    my_trace->last_eip = eip;
    my_trace->last_tid = tid;

    if(my_trace->status == STATUS_DBG_SCANNED)
    {
        if(register_thread_debug(tid, my_trace->threads[tid_pos].handle) <= 0x0)
        {
            d_print("Error writing to file: 0x%x\n", GetLastError());
            exit(1);
        }
    }

    if(!(my_trace->instr_count % INSTRUCTION_INTERVAL) && my_trace->instr_count>0x0)
    {
        reload_out_file();
    }

    my_trace->instr_count++;
    sprintf(line, "0x%x 0x%08x %lld\n", tid, eip, my_trace->instr_count);
    add_to_buffer(line);

    if(my_trace->instr_limit)
    {
        if(my_trace->instr_count >= my_trace->instr_limit)
        {
            HandlerRoutine(0x0);
            /* TODO: signal to tracer controller */
        }
    }

    ReleaseMutex(my_trace->mutex);

    return;
}

int page_accessible(MEMORY_BASIC_INFORMATION mbi)
{
//		if ( mbi.AllocationProtect == PAGE_UNKNOWN ) return false;
//		if ( mbi.Protect == MEMORY_PROTECT.PAGE_UNKNOWN ) return false;
		if ((mbi.AllocationProtect & PAGE_GUARD) == PAGE_GUARD) return false;
		if ((mbi.Protect & PAGE_GUARD) == PAGE_GUARD) return false;
		if ((mbi.AllocationProtect & PAGE_NOACCESS) == PAGE_NOACCESS) return false;
		if ((mbi.Protect & PAGE_NOACCESS) == PAGE_NOACCESS) return false;
		
		return true;
}

SIZE_T dump_mem(FILE* f, void* from, SIZE_T len)
{
    
    SIZE_T read, i;
    char mem_buf[buf_size];
    DWORD oldProt;
    SIZE_T wrote_total = 0x0;

    SIZE_T whole;
    SIZE_T part;

    part = len % buf_size;
    whole = len - part;

    unsigned j;

    d_print("Position before: 0x%08x\n", ftell(f));
    for(i=0x0; i<whole; i+= buf_size)
    {
        ReadProcessMemory(my_trace->procHandle, (void*)(from+i), (void*)mem_buf, buf_size, &read);
        d_print("Read: 0x%08x\n", read);
        if(read == 0x0)
        {
            d_print("Failed to read from %p to %p\nError: 0x%08x\n", from,(from+part), GetLastError());
            break;
        }

        fwrite(mem_buf, read, 0x1, f);
        wrote_total += read;
    }

    for(j=0x0; j<0x5; j++)
        d_print("0x%02x ", mem_buf[j]);
    d_print("\n");

    if(part > 0x0)
    {
        ReadProcessMemory(my_trace->procHandle, (void*)(from+i), (void*)mem_buf, part, &read);
        d_print("Read: 0x%08x\n", read);
        if(read == 0x0)
        {
            d_print("Failed to read from %p to %p\nError: 0x%08x\n", from,(from+part), GetLastError());
        }

        fwrite(mem_buf, read, 0x1, f);
        wrote_total += read;
    }

    for(j=0x0; j<0x5; j++)
        d_print("0x%02x ", mem_buf[j]);
    d_print("\n");

    d_print("Position after: 0x%08x\n", ftell(f));

    return wrote_total;
}

SIZE_T dump_zeros(FILE* f, SIZE_T len)
{
    SIZE_T read, i;
    char mem_buf[buf_size];
    DWORD oldProt;
    SIZE_T wrote_total = 0x0;

    SIZE_T whole;
    SIZE_T part;

    part = len % buf_size;
    whole = len - part;


    memset(mem_buf, 0x0, buf_size);

    for(i=0x0; i<whole; i+= buf_size)
    {
        fwrite(mem_buf, buf_size, 0x1, f);
        wrote_total += buf_size;
    }
    if(part > 0x0)
    {
        fwrite(mem_buf, part, 0x1, f);
        wrote_total += part;
    }

    return wrote_total;
}

void dump_memory()
{
    SIZE_T addr;
    SIZE_T read;

    d_print("Dumping mem start\n");
#ifdef MEM_DUMP
    my_trace->dump = fopen(my_trace->out_dump, "wb");

    MEMORY_BASIC_INFORMATION mbi;
    
    for(addr = 0x0; addr<0xffffffff; addr += mbi.RegionSize)
    {
        VirtualQueryEx(my_trace->procHandle, (void*)addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
        if(GetLastError() == ERROR_INVALID_PARAMETER)
            break;
        d_print("Block 0x%08x - 0x%08x, 0x%08x bytes\n", mbi.BaseAddress, mbi.RegionSize + mbi.BaseAddress, mbi.RegionSize);
//        if(mbi.State == MEM_COMMIT && (mbi.Type == MEM_MAPPED || mbi.Type == MEM_PRIVATE) && page_accessible(mbi))
        if(mbi.State == MEM_COMMIT && page_accessible(mbi))
        {
            read = dump_mem(my_trace->dump, mbi.BaseAddress, mbi.RegionSize);
        }
        else
        {
            read = dump_zeros(my_trace->dump, mbi.RegionSize);
        }

    }

    fclose(my_trace->dump);
#endif
    d_print("dumping mem end\n");
}

void dump_contexts()
{
    d_print("Dumping contexts\n");

}

void end_callback(void* data)
{
    char line[MAX_LINE];
    ss_callback(data);
    
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD eip;
    eip = (DWORD)de->u.Exception.ExceptionRecord.ExceptionAddress;

    d_print("Finishing @ 0x%08x, detaching\n", eip);
    sprintf(line, "FI,0x%08x\n", eip);
    add_to_buffer(line);

    HandlerRoutine(0x0);

}

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType)
{
    unsigned int i;
    DWORD tid_pos;

    d_print("Detected ctrl-c\n");
    
    d_print("Deregistering threads\n");

    for(i=0x0; i<my_trace->thread_count; i++)
    {
        tid_pos = i;
        if(my_trace->threads[tid_pos].alive == 0x1) 
        {
            d_print("Deregistering: 0x%08x\n", my_trace->threads[tid_pos].tid);
            deregister_thread(my_trace->threads[tid_pos].tid, my_trace->threads[tid_pos].handle);
        }
    }

    d_print("Deregistering libs\n");
    for(i=0x0; i<my_trace->lib_count; i++)
    {
        if(my_trace->libs[i].loaded == 0x1) 
        {
            deregister_lib(i);
        }
    }


    WritePrivateProfileString("general", "instruction count", itoa(my_trace->instr_count, instr_count_s, 10), my_trace->out_ini);

    SetEvent(my_trace->eventLock);
    WaitForSingleObject(my_trace->eventUnlock, INFINITE);
    exit(1);

}

DWORD WINAPI writer(LPVOID lpParam)
{
    while(1)
    {
        WaitForSingleObject(my_trace->eventLock, INFINITE);
        ResetEvent(my_trace->eventLock);
        fwrite(buffer, index, 1, my_trace->trace);
        fflush(my_trace->trace);
        SetEvent(my_trace->eventUnlock);
    }
}

void default_handler(void *data)
{
    d_print("Default handler!\n");
    return;
}

int del_breakpoint_idx(unsigned my_bpt_idx)
{
    /* TODO: verify */
    unwrite_breakpoint(&my_trace->breakpoints[my_bpt_idx]);
    my_trace->breakpoints[my_bpt_idx].enabled = 0x0;
    return 0x0;
}

int del_breakpoint(DWORD addr)
{
    d_print("Deleting breakpoint at: 0x%08x\n", addr);

    DWORD oldProt;
    char bpt_char = '\xcc';
    int i;
    int my_bpt_idx = -0x1;

    for(i = 0x0; i<my_trace->bpt_count; i++)
        if(my_trace->breakpoints[i].resolved_location == addr)
        {
            my_bpt_idx = i;
            break;
        }

    if(my_bpt_idx == -0x1)
    {
        d_print("Unable to find breakpoint to delete\n");
        return 0x1;
    }

    unwrite_breakpoint(&my_trace->breakpoints[i]);
    my_trace->breakpoints[my_bpt_idx].enabled = 0x0;

    return 0x0;
}

int handle_breakpoint(DWORD addr, void* data)
{
    d_print("[handle_breakpoint]\n");
    DEBUG_EVENT* de;

    de = (DEBUG_EVENT*)data;

    int i, j;
    int my_bpt_idx = -0x1;
    int handler_count;
    int our_bp = 0x0;
    BREAKPOINT* my_bp;

    for(i = 0x0; i<my_trace->bpt_count; i++)
    {
        d_print("Checking %d bp: _0x%08x_ & _0x%08x_\n", i, my_trace->breakpoints[i].resolved_location, addr);
        if(my_trace->breakpoints[i].resolved_location == addr)
        {
            my_bp = &my_trace->breakpoints[i];
            REACTION* cur_reaction;

            for(j = 0x0; j < my_bp->reaction_count; j++)
            {
                cur_reaction = my_bp->reactions[j];
                if(!cur_reaction->enabled) 
                {
                    d_print("Reaction not enabled, continuing\n");
                    continue;
                }

                if(cur_reaction->routine_id == 0x0)
                {
                    /* report rection_id to external controller */
                    d_print("Routine is null, reporting to controller\n");
                    my_trace->last_reaction = cur_reaction;
                    my_trace->report_code = REPORT_BREAKPOINT;
                }
                else
                {
                    d_print("Executing routine 0x%02x\n", cur_reaction->routine_id);
                    my_trace->routines[cur_reaction->routine_id](data);
                }
                /* enable coupled */
                unsigned k; 
                for(k = 0; k< MAX_COUPLES; k++)
                {
                    if(cur_reaction->coupled_id[k][0] != 0x0)
                    {
                        d_print("Current reaction: %s\n", cur_reaction->reaction_id);
                        d_print("Enabling coupled reaction: %s\n", cur_reaction->coupled_id[k]);
                        REACTION* coupled_reaction;
                        coupled_reaction = find_reaction(cur_reaction->coupled_id[k]);
                        d_print("Found coupled reaction: %s\n", coupled_reaction->reaction_id);
                        enable_reaction(coupled_reaction->reaction_id);
                    }
                }
            }
        }
    }

    d_print("Deleting breakpoint, decreasing EIP\n");
    del_breakpoint(addr);
    dec_eip(de->dwThreadId);

    d_print("[handle_breakpoint ends]\n");
    return 0x0;
}

int write_breakpoint(BREAKPOINT* bp)
{
    d_print("[write_breakpoint]\n");
    OFFSET addr;
    DWORD oldProt;
    DWORD read;
    DWORD ret;

    char bpt_char = '\xcc';

    addr = resolve_loc_desc(bp->location);

    d_print("Adding breakpoint opcode @ 0x%08x\n", addr);

    read_memory(my_trace->procHandle, (void*)addr, (void*)&bp->saved_byte, 0x1, &read);
    d_print("Before: 0x%02x\n", bp->saved_byte);
    write_memory(my_trace->procHandle, (void*)addr, (void*)&bpt_char, 0x1, &read);
    read_memory(my_trace->procHandle, (void*)addr, (void*)&bpt_char, 0x1, &read);
    d_print("After: 0x%02x\n", bpt_char);
    bp->written = 0x1;

    d_print("[write_breakpoint ends]\n");
    return 0x0;
}

int unwrite_breakpoint(BREAKPOINT* bp)
{
    d_print("[unwrite_breakpoint]\n");
    OFFSET addr;
    DWORD oldProt;
    char bpt_char = '\xcc';
    DWORD wrote;
    DWORD ret;

    addr = bp->resolved_location;

    write_memory(my_trace->procHandle, (void*)addr, (void*)&bp->saved_byte, 0x1, &wrote);
    read_memory(my_trace->procHandle, (void*)addr, (void*)&bpt_char, 0x1, &wrote);
    d_print("After: 0x%02x\n", bpt_char);
    bp->written = 0x0;

    d_print("[unwrite_breakpoint ends]\n");
    return 0x0;

}

int print_loc_desc(LOCATION_DESCRIPTOR_NEW* e)
{
    d_print("[print_loc_desc]\n");
    if(e == 0x0)
        return 0x0;

    d_print("%s ", e->op);
    print_loc_desc(e->a1);
    print_loc_desc(e->a2);
    d_print("[print_loc_desc ends]\n");
    return 0x0;
}

int print_loc_desc_rev(LOCATION_DESCRIPTOR_NEW* e)
{
    d_print("[print_loc_desc_rev]\n");
    d_print("In reccurence\n");

    if(e == 0x0) return 0x0;

    print_loc_desc_rev(e->a1);
    print_loc_desc_rev(e->a2);

    d_print("%s\n", e->op);

    d_print("[print_loc_desc_rev ends]\n");
    return 0x0;
}

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

        printf("%c", area[i]);
    }
    printf("\n");

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

        printf("%c", area[i]);
    }
    printf("\n");
    return 0x0;
}

OFFSET resolve_loc_desc(LOCATION_DESCRIPTOR_NEW* d)
{
    d_print("[resolve_loc_desc]\n");
    OFFSET a1_r, a2_r;
    OFFSET ret;

    if(d == 0x0)
        return -1;

    d_print("Processing: %s\n", d->op);

    if(findany(d->op, "[+-"))
    {
        /* not leaf, this is operation, we do recurrence */

        if(!strcmp(d->op, "["))
        {
            DWORD read;
            a1_r = resolve_loc_desc(d->a1);
            if(a1_r == -1) return -1;
            /* read_memory and calculate ret */
            
            read = read_dword(a1_r);
            return read;
        }
        else if(!strcmp(d->op, "-"))
        {
            a1_r = resolve_loc_desc(d->a1);
            a2_r = resolve_loc_desc(d->a2);
            if(a1_r == -1) return -1;
            if(a2_r == -1) return -1;
            ret = a1_r - a2_r;
        }
        else if(!strcmp(d->op, "+"))
        {
            a1_r = resolve_loc_desc(d->a1);
            a2_r = resolve_loc_desc(d->a2);
            if(a1_r == -1) return -1;
            if(a2_r == -1) return -1;
            ret = a1_r + a2_r;
        }
    }
    else
    {
        /* this is leaf */

        if(!strcmp(d->op, "self"))
        {
            /* return self address */
            ret = (OFFSET)my_trace->cpdi.lpBaseOfImage;
            if(ret == 0x0) return -1;
        }
        else if((d->op[0] == '0') && (d->op[1] == 'x'))
        {
            /* return immediate */
            ret = strtoul(d->op, 0x0, 0x10);
        }
        else
        {
            /* registers */
            if(!strcmp(d->op, "ESP"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Esp;
                d_print("Reading register ESP: 0x%08x\n", ret);
            }
            else if(!strcmp(d->op, "EAX"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Eax;
                d_print("Reading register EAX: 0x%08x\n", ret);
            }
            else if(!strcmp(d->op, "EBX"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Ebx;
                d_print("Reading register EBX: 0x%08x\n", ret);
            }
            else if(!strcmp(d->op, "ECX"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Ecx;
                d_print("Reading register ECX: 0x%08x\n", ret);
            }
            else if(!strcmp(d->op, "EDX"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Edx;
                d_print("Reading register EDX: 0x%08x\n", ret);
            }
            else if(!strcmp(d->op, "ESI"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Esi;
                d_print("Reading register ESI: 0x%08x\n", ret);
            }
            else if(!strcmp(d->op, "EDI"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Edi;
                d_print("Reading register EDI: 0x%08x\n", ret);
            }
            else if(!strcmp(d->op, "EBP"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Ebp;
                d_print("Reading register EBP: 0x%08x\n", ret);
            }
            else if(!strcmp(d->op, "EIP"))
            {
                CONTEXT ctx;
                read_context(0x0, &ctx);
                ret = ctx.Eip;
                d_print("Reading register EIP: 0x%08x\n", ret);
            }
            else
            {
                /* we assume it's library */
                d_print("Looking for lib: %s\n", d->op);
                ret = find_lib(d->op);
                if(ret != 0x0)
                {
                    d_print("Found at: 0x%08x\n", ret);
                }
                else if(ret == 0x0) 
                {
                    d_print("Not found\n");
                    ret = -1;
                }
            }
        }
    }
    d_print("[resolve_loc_desc end]\n");
    return ret;
}

LOCATION_DESCRIPTOR_NEW* parse_location_desc(char* str)
{
    d_print("[parse_location_desc]\n");
    char* op;
    LOCATION_DESCRIPTOR_NEW* neww;

    neww = (LOCATION_DESCRIPTOR_NEW*)malloc(sizeof(LOCATION_DESCRIPTOR_NEW));
    memset(neww, 0x0, sizeof(LOCATION_DESCRIPTOR_NEW));
    neww->a1 = 0x0;
    neww->a2 = 0x0;

    if(!neww)
    {
        d_print("Out of memory\n");
    }

    d_print("Creating new descriptor node\n");

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
            d_print("No operators found, assuming leaf: %s\n", str);
            strcpy(neww->op, str);
        }
    }
    else
    {
        unpaint(str, strlen(str));
        d_print("Found operator: %s\n", op);
        
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

    d_print("[parse_location_desc ends]\n");
    return neww;
}

int update_breakpoint(BREAKPOINT* bp)
{
    d_print("[update_breakpoint]\n");
    DWORD ret;
    OFFSET addr;
    d_print("Trying to resolve BP addr\n");
    bp->resolved_location = addr = resolve_loc_desc(bp->location);

    if(addr == -1)
    {
        d_print("Unable to resolve BP address, will not be updated at this time\n");
        return 0x0;
    }
    else
    {
        if(bp->enabled)
        {
            if(!bp->written)
            {
                write_breakpoint(bp); /* in order to write breakpoint, it must be enabled and loaded*/
                bp->written = 0x1;
            }
            d_print("BP written\n");
            ret = 0x1;
        }
        else
        {
            if(bp->written)
            {
                unwrite_breakpoint(bp); /* in order to write breakpoint, it must be enabled and loaded*/
                bp->written = 0x0;
            }
            d_print("BP unwritten\n");

            ret = 0x0;
        }
    }
    d_print("[update_breakpoint ends]\n");
    return ret;
}

BREAKPOINT* add_breakpoint(char* location_str, REACTION*  reaction)
{
    d_print("[add_breakpoint]\n");
    int i;
    int my_bpt_index;
    unsigned cur_reaction_id;

    my_bpt_index = -1;

    /* do we have this location descriptor? */
    for(i = 0x0; i<my_trace->bpt_count; i++)
    {
        d_print("BP creation: comparing _%s_ & _%s_\n", my_trace->breakpoints[i].location_str, location_str);
        if(!strcmp(my_trace->breakpoints[i].location_str, location_str))
        {
            d_print("Found!\n");
            my_bpt_index = i;
            break;
        }
    }

    /* if not, create */
    if(my_bpt_index == -1) 
    {
        d_print("Creating new bp\n");
        my_bpt_index = my_trace->bpt_count;
        my_trace->breakpoints[my_bpt_index].enabled = 0x1;
        my_trace->breakpoints[my_bpt_index].written = 0x0;
        
        /* for identification */
        strcpy(my_trace->breakpoints[my_bpt_index].location_str, location_str);

        d_print("Attempt to parse location string: %s\n", location_str);
        my_trace->breakpoints[my_bpt_index].location = parse_location_desc(location_str);

        my_trace->bpt_count ++;
    }
    d_print("Bp created. Current number of breakpoints: %d\n", my_trace->bpt_count);

    /* connect to reaction */
    cur_reaction_id = my_trace->breakpoints[my_bpt_index].reaction_count;
    d_print("Current reaction count for this BP: %d\n", cur_reaction_id);
    my_trace->breakpoints[my_bpt_index].reactions[cur_reaction_id] = reaction;
    d_print("Assigned new reaction\n");
    my_trace->breakpoints[my_bpt_index].enabled = 0x1; /* is this necessary? it is used in update_breakpoint */
    my_trace->breakpoints[my_bpt_index].reaction_count++;


    d_print("[add_breakpoint ends]\n");
    return &my_trace->breakpoints[my_bpt_index];
}



void start_trace_fname()
{
    BOOL res;

    d_print("Creating process: %s\n", my_trace->process_fname);

    res = CreateProcess(my_trace->process_fname, 0x0, 0x0, 0x0, 0x0, DEBUG_ONLY_THIS_PROCESS, 0x0, 0x0, &my_trace->si, &my_trace->pi);

    if(res == 0x0)
    {
        d_print("Error creating process: 0x%08x\n", GetLastError());
    }

    my_trace->PID = my_trace->pi.dwProcessId;
}

void start_trace_pid()
{
    d_print("Attaching debugger\n");

    if(DebugActiveProcess(my_trace->in_sample_pid) != 0x0)
        d_print("Successfully attached to PID: 0x%x, handle: 0x%x\n", my_trace->in_sample_pid);
    else 
        d_print("Attach failed\n");
}

DWORD find_lib(char* name)
{
    unsigned i, j;
    DWORD ret = 0x0;

    char myname[MAX_NAME];
    char libname[MAX_NAME];

    strcpy(myname, name);
    for(j = 0; myname[j]; j++)
    {
        myname[j] = tolower(myname[j]);
    }

    for(i=0x0; i<my_trace->lib_count; i++)
    {
        strcpy(libname, my_trace->libs[i].lib_name);
        for(j = 0; libname[j]; j++)
        {
            libname[j] = tolower(libname[j]);
        }

        if(strstr(libname, myname))
        {
            ret = my_trace->libs[i].lib_offset;
            break;
        }
    }
    return ret;
}

int parse_descriptor(char* path)
{
    FILE* desc_file;
    unsigned size;
    char desc_line[MAX_LINE];

    desc_file = fopen(path, "rb");

//    while ((read = getline(&desc_line, &len, desc_file)) != -1) 
    while(fgets(desc_line, 80, desc_file) != NULL)
    {
        d_print("Line: %s\n", desc_line);
        sprintf(line2, "# desc: %s\n", desc_line);
        add_to_buffer(line2);
    }

    return 0x0;
}

/* new routines */

int send_report()
{
    char rep_chars[3];
    char rep_chars2[] = "-=OK=-";
    char line2[BUFF_SIZE];

    switch(my_trace->report_code)
    {
        case REPORT_CONTINUE:
            strcpy(rep_chars, "RC");
            break;
        
        case REPORT_PROCESS_CREATED:
            strcpy(rep_chars, "RR");
            break;
        
        case REPORT_PROCESS_EXIT:
            strcpy(rep_chars, "RX");
            break;
        
        case REPORT_ST_BREAKPOINT:
            strcpy(rep_chars, "RS");
            break;
        
        case REPORT_END_BREAKPOINT:
            strcpy(rep_chars, "RD");
            break;
        
        case REPORT_BREAKPOINT:
            strcpy(rep_chars, "RB");
            break;
        
        case REPORT_EXCEPTION:
            strcpy(rep_chars, "RE");
            break;
        
        case REPORT_INFO:
            strcpy(rep_chars, "RI");
            break;
        
        case REPORT_NOTHING:
            strcpy(rep_chars, "RN");
            break;
    }

    strcpy(line2, rep_chars);
    strcat(line2, my_trace->report_buffer);
    strcat(line2, rep_chars2);

    d_print("Sending report: %s\n", line2);
    send(my_trace->socket, line2, strlen(line2), 0x0);

    return 0x0;
}

int list_reactions()
{
    d_print("Listing reactions\n");
    unsigned i, j;
    char buffer2[MAX_LINE];
    REACTION* my_reaction;
    BREAKPOINT* my_bp;
    
    my_trace->report_code = REPORT_INFO;

    strcpy(my_trace->report_buffer, "");

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        my_reaction = &my_trace->reactions[i];
        sprintf(buffer2, "%s:\n", my_reaction->reaction_id);
        for(j = 0x0; j< my_reaction->bps_count; j++)
        {
            my_bp = my_reaction->bp;
            sprintf(buffer2, "%s:0x%08x\n", my_bp->location_str, my_bp->resolved_location);
        }
    }

    strcat(my_trace->report_buffer, buffer2);
    strcat(my_trace->report_buffer, "\n");

    return 0x0;
}


int list_bpts()
{
    d_print("Listing BPTs\n");
    unsigned i;
    char buffer2[MAX_LINE];

    my_trace->report_code = REPORT_INFO;

    strcpy(my_trace->report_buffer, "");

    for(i = 0x0; i< my_trace->bpt_count; i++)
    {
        sprintf(buffer2, "0x%08x\n", my_trace->breakpoints[i].resolved_location);
        strcat(my_trace->report_buffer, buffer2);
        strcat(my_trace->report_buffer, "\n");
    }

    return 0x0;
}



int list_libs()
{
    d_print("Listing LIBs\n");
    unsigned i;
    char buffer2[MAX_LINE];

    my_trace->report_code = REPORT_INFO;

    d_print("Currently have %d libs\n", my_trace->lib_count);
    strcpy(my_trace->report_buffer, "");

    for(i = 0x0; i< my_trace->lib_count; i++)
    {
        serialize_lib(&my_trace->libs[i], buffer2);
        strcat(my_trace->report_buffer, buffer2);
        strcat(my_trace->report_buffer, "\n");
    }
    d_print("Reporting: %s\n", my_trace->report_buffer);

    return 0x0;
}

int list_tebs()
{
    d_print("Listings TEBs\n");
    unsigned i;
    char buffer2[MAX_LINE];

    my_trace->report_code = REPORT_INFO;

    d_print("Currently have %d threads\n", my_trace->thread_count);

    strcpy(my_trace->report_buffer, "");

    for(i = 0x0; i< my_trace->thread_count; i++)
    {
    //    serialize_thread(&my_trace->threads[i], buffer2);
    //    strcat(my_trace->report_buffer, buffer2);
        strcat(my_trace->report_buffer, "x");
    }
    d_print("Reporting: %s\n", my_trace->report_buffer);

    return 0x0;
}

int write_context(DWORD tid, CONTEXT* ctx)
{
    HANDLE myHandle;
    DWORD tid_id;

    tid_id = my_trace->thread_map[tid];

    myHandle = my_trace->threads[tid_id].handle;
    ctx->ContextFlags = CONTEXT_FULL;
    if(SetThreadContext(myHandle, ctx) == 0x0)
    {
        d_print("Failed to set context, error: 0x%08x\n", GetLastError());
    }

    return 0x0;
}

int list_all_tebs()
{
    char buffer2[MAX_LINE];

    HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
    THREADENTRY32 te32; 
 
    hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
    if( hThreadSnap == INVALID_HANDLE_VALUE ) 
        return( FALSE ); 
 
    te32.dwSize = sizeof(THREADENTRY32 ); 
 
    if( !Thread32First( hThreadSnap, &te32 ) ) 
    {
        d_print("Thread32First");
        CloseHandle( hThreadSnap );
        return( FALSE );
    }

    do 
    { 
        sprintf(buffer2, "0x%08x - %08x\n", te32.th32OwnerProcessID, te32.th32ThreadID);
        strcat(my_trace->report_buffer, buffer2);
    } while( Thread32Next(hThreadSnap, &te32 ) );

    CloseHandle( hThreadSnap );
    return 0x0;
}

int release_thread(DWORD tid)
{
    HANDLE myHandle = (HANDLE)-0x1;
    DWORD tid_id;
    char buffer2[MAX_LINE];

    d_print("Trying relese TID: 0x%08x\n", tid);

    if((myHandle = OpenThread(THREAD_SUSPEND_RESUME, 0x0, tid)) == 0x0)
    {
        d_print("Failed to open thread, error: 0x%08x\n", GetLastError());
        sprintf(buffer2, "Error: 0x%08x\n", GetLastError());
        strcpy(my_trace->report_buffer, buffer2);
    }


    if(ResumeThread(myHandle) == -1)
    {
        d_print("Failed to resume thread, error: 0x%08x\n", GetLastError());
        sprintf(buffer2, "Error: 0x%08x\n", GetLastError());
        strcpy(my_trace->report_buffer, buffer2);
    }

    return 0x0;
}

int read_context(DWORD tid, CONTEXT* ctx)
{
    HANDLE myHandle = (HANDLE)-0x1;
    DWORD tid_id;
    char buffer2[MAX_LINE];

    if(tid == 0x0)
    {
        tid = my_trace->last_event.dwThreadId;
    }

    tid_id = my_trace->thread_map[tid];
    myHandle = my_trace->threads[tid_id].handle;

    d_print("Trying to get context of TID: 0x%08x, index: 0x%08x, handle: 0x%08x\n", tid, tid_id, myHandle);

    ctx->ContextFlags = CONTEXT_FULL;
    if(GetThreadContext(myHandle, ctx) == 0x0)
    {
        d_print("Failed to get context, error: 0x%08x\n", GetLastError());
        sprintf(buffer2, "Error: 0x%08x\n", GetLastError());
        strcpy(my_trace->report_buffer, buffer2);
    }

    return 0x0;
}

int write_dword(DWORD addr, DWORD val)
{
    CONTEXT ctx;
    DWORD read;
    HANDLE handle;

    char buffer2[MAX_LINE];

    handle = my_trace->procHandle;
    
    d_print("Trying to write: 0x%08x @ %p, handle: 0x%08x\n", val, addr, my_trace->procHandle);
    write_memory(my_trace->procHandle, (void*)addr, (void*)&val, 0x4, &read);
    
    if(read == 0x4)
    {
        sprintf(buffer2, "DWORD written");
        strcpy(my_trace->report_buffer, buffer2);
    }
    else 
    {
        sprintf(buffer2, "Error: 0x%08x\n", GetLastError());
        strcpy(my_trace->report_buffer, buffer2);
    }
    read_memory(my_trace->procHandle, (void*)addr, (void*)&val, 0x4, &read);
    d_print("New val @ %p: 0x%08x\n", addr, val);

    return 0x0;
}

char read_byte(DWORD addr)
{
    char data;
    DWORD read;

    char buffer2[MAX_LINE];

    read_memory(my_trace->procHandle, (void*)addr, (void*)&data, 0x1, &read);
    
    if(read == 0x1)
    {
        sprintf(buffer2, "%02x", data);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else {
        sprintf(buffer2, "Error");
        strcpy(my_trace->report_buffer, buffer2);
    }

    return data;
}

int write_byte(DWORD addr, char* data)
{
    DWORD read;
    char data_b;

    char buffer2[MAX_LINE];

    data_b = strtoul(data, 0x0, 0x10);
    write_memory(my_trace->procHandle, (void*)addr, (void*)&data_b, 0x1, &read);

    if(read == 0x1)
    {
        sprintf(buffer2, "BYTE written", data);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else {
        sprintf(buffer2, "Error");
        strcpy(my_trace->report_buffer, buffer2);
    }

    return 0x0;
}

int read_word(DWORD addr)
{
    WORD data;
    DWORD read;

    char buffer2[MAX_LINE];

    read_memory(my_trace->procHandle, (void*)addr, (void*)&data, 0x2, &read);
    
    if(read == 0x2)
    {
        sprintf(buffer2, "%04x", data);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else {
        sprintf(buffer2, "Error");
        strcpy(my_trace->report_buffer, buffer2);
    }

    return data;
}

int write_word(DWORD addr, char* data)
{
    DWORD read;
    WORD  data_w;

    char buffer2[MAX_LINE];

    data_w = strtoul(data, 0x0, 0x10);
    write_memory(my_trace->procHandle, (void*)addr, (void*)&data_w, 0x2, &read);

    if(read == 0x2)
    {
        sprintf(buffer2, "WORD written", data);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else {
        sprintf(buffer2, "Error");
        strcpy(my_trace->report_buffer, buffer2);
    }

    return 0x0;
}

int read_dword(DWORD addr)
{
    DWORD data;
    DWORD read;

    char buffer2[MAX_LINE];

    read_memory(my_trace->procHandle, (void*)addr, (void*)&data, 0x4, &read);
    return data;
}


int report_dword(DWORD addr)
{
    DWORD data;
    DWORD read;

    char buffer2[MAX_LINE];

    d_print("Trying to read: @ %p, handle: 0x%08x\n", addr, my_trace->cpdi.hProcess);
    //read_memory(my_trace->procHandle, (void*)addr, (void*)&data, 0x4, &read);
    read_memory(my_trace->cpdi.hProcess, (void*)addr, (void*)&data, 0x4, &read);
    
    if(read == 0x4)
    {
        sprintf(buffer2, "%08x", data);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else {
        sprintf(buffer2, "Error: 0x%08x", GetLastError());
        strcpy(my_trace->report_buffer, buffer2);
    }

    return data;
}

int write_register(DWORD tid_id, char* reg, char* data)
{
    CONTEXT ctx;
    char buffer2[MAX_LINE];
    DWORD data_d;
    WORD data_w;
    char data_b;

    
    read_context(tid_id, &ctx);

    if(!strcmp(reg, "EAX"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Eax = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "EBX"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Ebx = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "ECX"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Ecx = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "EDX"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Edx = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "ESI"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Esi = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "EDI"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Edi = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "EBP"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Ebp = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "ESP"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Esp = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "EIP"))
    {
        data_d = strtoul(data, 0x0, 0x10);
        ctx.Eip = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }

    my_trace->report_code = REPORT_INFO;
    sprintf(buffer2, "%s written\n", reg);
    strcpy(my_trace->report_buffer, buffer2);

    return 0x0;
}

DWORD read_register(DWORD tid_id, char* reg)
{
    CONTEXT ctx;

    if(tid_id == 0x0)
    {
        tid_id = my_trace->last_event.dwThreadId;
    }

    read_context(tid_id, &ctx);

    if(!strcmp(reg, "EAX"))
    {
        return ctx.Eax;
    }
    if(!strcmp(reg, "EBX"))
    {
        return ctx.Ebx;
    }
    if(!strcmp(reg, "ECX"))
    {
        return ctx.Ecx;
    }
    if(!strcmp(reg, "EDX"))
    {
        return ctx.Edx;
    }
    if(!strcmp(reg, "EDI"))
    {
        return ctx.Edi;
    }
    if(!strcmp(reg, "EBP"))
    {
        return ctx.Ebp;
    }
    if(!strcmp(reg, "ESP"))
    {
        return ctx.Esp;
    }
    if(!strcmp(reg, "Eip"))
    {
        return ctx.Eip;
    }
    if(!strcmp(reg, "EFLAGS"))
    {
        return ctx.EFlags;
    }

    d_print("Error reading register %s\n", reg);
    return -1;
}

int report_register(DWORD tid_id, char* reg)
{
    CONTEXT ctx;
    char buffer2[MAX_LINE];
    DWORD val;

    my_trace->report_code = REPORT_INFO;
    
    val = read_register(tid_id, reg);

    sprintf(buffer2, "%08x", val);
    strcpy(my_trace->report_buffer, buffer2);

    return 0x0;
}

int read_stack(DWORD tid_id, DWORD count)
{
    CONTEXT ctx;
    char buffer2[MAX_LINE];
    DWORD esp;
    DWORD data;

    my_trace->report_code = REPORT_INFO;
    
    read_context(tid_id, &ctx);
    esp = ctx.Esp;

    unsigned pos;

    memset(my_trace->report_buffer, 0x0, BUFF_SIZE);

    for(pos = 0x0; pos<count; pos++, esp+=0x4)
    {
        data = read_dword(esp);
        memset(buffer2, 0x0, sizeof(buffer2));
        sprintf(buffer2, "0x%08x:0x%08x", esp, data);
//        d_print("Adding line: %s\n", buffer2);
        strcat(my_trace->report_buffer, buffer2);
        strcat(my_trace->report_buffer, "\n");
//        d_print("new buffer: %p - %s\n", my_trace->report_buffer, my_trace->report_buffer);
    }
    d_print("new buffer: %p - %s\n", my_trace->report_buffer, my_trace->report_buffer);

    return 0x0;
}

int process_last_event()
{
    unsigned status;

        //d_print("process_last_event: 0x%08x\n", my_trace->last_event.dwDebugEventCode);
        switch(my_trace->last_event.dwDebugEventCode)
        {
            case CREATE_PROCESS_DEBUG_EVENT:
                /* this is not our responsibility, inform TracerController and wait for orders */
                /* get process info */
                my_trace->cpdi = my_trace->last_event.u.CreateProcessInfo;
                my_trace->procHandle = my_trace->cpdi.hProcess;

                /* register main thread (does this not belong to other event code? */
                register_thread(my_trace->last_event.dwThreadId, my_trace->last_event.u.CreateProcessInfo.hThread);

                unsigned i;

                /* Checking brakpoints */
                d_print("Created process, checking breakpoints\n");

                for(i = 0x0; i<my_trace->bpt_count; i++)
                {
                    d_print("bp: %s, resolved_location: 0x%08x\n", my_trace->breakpoints[i].location_str, my_trace->breakpoints[i].resolved_location);
                    if(my_trace->breakpoints[i].resolved_location == -1)
                    {
                        update_breakpoint(&my_trace->breakpoints[i]);
                    }
                }

                return REPORT_PROCESS_CREATED;
                break;

            case EXCEPTION_DEBUG_EVENT:
                status = DBG_EXCEPTION_NOT_HANDLED;
                my_trace->last_exception = my_trace->last_event.u.Exception.ExceptionRecord;

                switch(my_trace->last_exception.ExceptionCode)
                {
                    case EXCEPTION_SINGLE_STEP:
                        /* we are authorized to handle this */
                        ss_callback((void*)&my_trace->last_event);
                        set_ss(my_trace->last_event.dwThreadId);
                        my_trace->last_win_status = DBG_CONTINUE;
                        return REPORT_CONTINUE;
                        break;
                
                    case EXCEPTION_BREAKPOINT:
                        /* we are authorized to handle some cases */

                        unsigned i;
                        OFFSET bp_addr;
                        char handled; 

                        bp_addr = (OFFSET)my_trace->last_exception.ExceptionAddress;
                        d_print("Breakpoint hit! @ 0x%08x\n", bp_addr);
                        d_print("[BP handling]\n");

                        handled = 0x0;
                        my_trace->report_code = REPORT_CONTINUE;

                        for(i = 0x0; i< my_trace->bpt_count; i++)
                        {
                            d_print("Comparing 0x%08x and 0x%08x\n", bp_addr, my_trace->breakpoints[i].resolved_location);
                            if(my_trace->breakpoints[i].resolved_location == bp_addr)
                            {
                                d_print("Handling breakpoint @ 0x%08x\n", bp_addr);
                                handle_breakpoint((DWORD)my_trace->last_exception.ExceptionAddress, &my_trace->last_event);
                                handled = 0x1;
                            }
                        }

                        if(!handled)                        
                        {
                            d_print("This BP is not our, we pass it to the debugee\n");
                            my_trace->report_code = REPORT_EXCEPTION_NH;
                        }

                        d_print("[BP handling ends]\n");
                        return my_trace->report_code;
                        break;

                    default:
                        d_print("other\n");
                        /* this is not our responsibility, inform TracerController and wait for orders */
                        register_exception(my_trace->last_event.dwThreadId, my_trace->last_exception);
                        ss_callback((void*)&my_trace->last_event);
                        return REPORT_EXCEPTION;
                        break;
                }
                break;

            case CREATE_THREAD_DEBUG_EVENT: 
                /* we are authorized to handle this */

                register_thread(my_trace->last_event.dwThreadId, my_trace->last_event.u.CreateThread.hThread);
                my_trace->last_win_status = DBG_CONTINUE;
                return REPORT_CONTINUE;
                break;

            case LOAD_DLL_DEBUG_EVENT:
                /* we are authorized to handle this */

                register_lib(my_trace->last_event.u.LoadDll);

                /* Checking brakpoints */
                d_print("Loaded library, checking breakpoints\n");

                for(i = 0x0; i<my_trace->bpt_count; i++)
                {
                    d_print("bp: %s, resolved_location: 0x%08x\n", my_trace->breakpoints[i].location_str, my_trace->breakpoints[i].resolved_location);
                    if(my_trace->breakpoints[i].resolved_location == -1)
                    {
                        update_breakpoint(&my_trace->breakpoints[i]);
                    }
                }

                my_trace->last_win_status = DBG_CONTINUE;
                return REPORT_CONTINUE;
                break;

            case UNLOAD_DLL_DEBUG_EVENT:
                /* we are authorized to handle this */

                deregister_lib(my_trace->last_event.u.UnloadDll);
                return REPORT_CONTINUE;
                break;

            case EXIT_THREAD_DEBUG_EVENT:
                /* we are authorized to handle this */

                d_print("Exiting thread\n");
                deregister_thread(my_trace->last_event.dwThreadId, my_trace->last_event.u.CreateThread.hThread);
                my_trace->threads[my_trace->last_event.dwThreadId].alive = 0x0;

                if(my_trace->threads[my_trace->last_event.dwThreadId].handle != 0x0) 
                    CloseHandle(my_trace->threads[my_trace->last_event.dwThreadId].handle);

                my_trace->threads[my_trace->last_event.dwThreadId].handle = 0x0;
                my_trace->threads[my_trace->last_event.dwThreadId].open = 0x0;
                my_trace->last_win_status = DBG_CONTINUE;
                return REPORT_CONTINUE;
                break;

            case EXIT_PROCESS_DEBUG_EVENT:
                /* this is not our responsibility, inform TracerController and wait for orders */

                d_print("Exiting process\n");
                return REPORT_PROCESS_EXIT;
                break;
        }
}

int create_report(int last_report)
{
    char buffer2[MAX_LINE];
    char buffer3[MAX_LINE];

    switch(last_report)
    {
        case REPORT_PROCESS_CREATED:
            sprintf(buffer2, "REPORT_PROCESS_CREATED\n");
            break;
            
        case REPORT_BREAKPOINT:
            sprintf(buffer2, "%s\n", my_trace->last_reaction->reaction_id);
            break;
            
        case REPORT_EXCEPTION:
            EXCEPTION_RECORD er;
            er = my_trace->last_event.u.Exception.ExceptionRecord;

            sprintf(buffer2, "REPORT_EXCEPTION\n");

            sprintf(buffer3, "Exception: 0x%08x\n", er.ExceptionCode);            
            strcat(buffer2, buffer3);
            sprintf(buffer3, "at: 0x%08x\n", er.ExceptionAddress);
            strcat(buffer2, buffer3);
            sprintf(buffer3, "First chance: 0x%08x\n", my_trace->last_event.u.Exception.dwFirstChance);
            strcat(buffer2, buffer3);

            break;
        
        case REPORT_PROCESS_EXIT:
            sprintf(buffer2, "REPORT_PROCESS_EXIT\n");
            break;

        default:
            sprintf(buffer2, "REPORT_OTHER\n");
            sprintf(buffer3, "at: 0x%08x\n", my_trace->last_eip);
            strcat(buffer2, buffer3);
            sprintf(buffer3, "instr: %d\n", my_trace->instr_count);
            strcat(buffer2, buffer3);
            break;
    }
    strcpy(my_trace->report_buffer, buffer2);

    return 0x0;
}

int handle_continue(DWORD pid, DWORD tid, unsigned status)
{
    /* turn on trap again */
    /* TODO: is this necessary? */
    /*
    if((my_trace->status == STATUS_DBG_STARTED) || (my_trace->status == STATUS_DBG_SCANNED))
    {
        d_print("Enabling trap\n");
        set_ss(0x0);
        set_ss(tid);
    }
    */
    ContinueDebugEvent(pid, tid, status);
    return 0x0;
}

int get_pending_events()
{
    int last_report;
    char buffer2[MAX_LINE];
    unsigned status;

    d_print("Continuing: PID: 0x%08x, TID: 0x%08x\n", my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId);
    if(my_trace->last_event.dwProcessId == 0x0)
    {
//        ContinueDebugEvent(my_trace->pi.dwProcessId, my_trace->pi.dwThreadId, DBG_CONTINUE);
        handle_continue(my_trace->pi.dwProcessId, my_trace->pi.dwThreadId, DBG_CONTINUE);
    }
    else
    {
//        ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, DBG_CONTINUE);
        handle_continue(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, DBG_CONTINUE);
    }
    d_print("Continuing\n");

    while(1)
    {

        WaitForDebugEvent(&(my_trace->last_event), 0x0);
        if(my_trace->last_event.dwDebugEventCode == 0x0) return REPORT_NOTHING;

        last_report = process_last_event();
        if(last_report == REPORT_CONTINUE)
        {
            status = DBG_CONTINUE;
        }
        else if(last_report == REPORT_EXCEPTION_NH)
        {
            status = DBG_EXCEPTION_NOT_HANDLED;
        }
        else
        {
            break;
        }

        //ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
        handle_continue(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
    }


    create_report(last_report);
    d_print("Report buffer: %s\n", my_trace->report_buffer);

    my_trace->report_code = last_report;

    return last_report;
}

/* TODO: continuing for some time */
int continue_routine(DWORD time, unsigned stat)
{
    int last_report;
    char buffer2[MAX_LINE];
    unsigned status;

    status = stat;

    if(my_trace->last_event.dwProcessId == 0x0)
    {
        //ContinueDebugEvent(my_trace->pi.dwProcessId, my_trace->pi.dwThreadId, status);
        handle_continue(my_trace->pi.dwProcessId, my_trace->pi.dwThreadId, status);
    }
    else
    {
        //ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
        handle_continue(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
    }
    d_print("Continuing\n");
    while(1)
    {

        WaitForDebugEvent(&(my_trace->last_event), time);

        //d_print("continue_routine: dwDebugEventCode: 0x%08x\n", my_trace->last_event.dwDebugEventCode);

        last_report = process_last_event();
        if(last_report == REPORT_CONTINUE)
        {
            status = DBG_CONTINUE;
        }
        else if(last_report == REPORT_EXCEPTION_NH)
        {
            status = DBG_EXCEPTION_NOT_HANDLED;
        }
        else
        {
            break;
        }

        //ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
        handle_continue(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
    }

    create_report(last_report);
    d_print("Report buffer: %s\n", my_trace->report_buffer);

    my_trace->report_code = last_report;

    return last_report;
}

int resolve_region(LOCATION_DESCRIPTOR* selector, LOCATION* location)
{
    d_print("Resolving region\n");
    d_print("Locating buffer\n");

    DWORD arg_val, arg_addr, arg_size, off, size, cur_esp;
    DWORD read;
    CONTEXT ctx;

    read_context(my_trace->last_event.dwThreadId, &ctx);
    cur_esp = ctx.Esp;

    arg_val = 0x0;
    arg_addr = 0x0;
    off = 0x0;
    size = 0x0;

    d_print("ESP: 0x%08x\n", cur_esp);

    /* resolve offset */
    switch(selector->off_location)
    {
        case LOCATION_CONST:
            d_print("LOCATION_CONST\n");
            location->off = selector->off;
            d_print("Arg off: 0x%08x\n", location->off);
            arg_val = location->off;
            break;
        
        case LOCATION_MEM:
            d_print("LOCATION_MEM\n");
            arg_addr = selector->off;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            if(arg_val == 0x0) break;
            d_print("Arg off: 0x%08x\n", arg_val);
            location->off = arg_val;
            break;

        case LOCATION_STACK:
            d_print("LOCATION_STACK\n");
            arg_addr = cur_esp + 0x8;
            arg_addr += selector->off * 0x4;
            arg_val = arg_addr;
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            d_print("Arg off: 0x%08x\n", arg_val);
            if(arg_val == 0x0) break;
            location->off = arg_val;
            break;

        case LOCATION_ADDR_STACK:
            d_print("LOCATION_ADDR_STACK\n");
            arg_addr = cur_esp + 0x8;
            arg_addr += selector->off * 0x4;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            if(arg_val == 0x0) break;
            d_print("Arg off: 0x%08x\n", arg_val);
            location->off = arg_val;
            break;

        case LOCATION_ADDR_ADDR_STACK:
            d_print("LOCATION_ADDR_ADDR_STACK\n");
            arg_addr = cur_esp + 0x8;
            arg_addr += selector->off * 0x4;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            arg_addr = arg_val;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            if(arg_val == 0x0) break;
            d_print("Arg off: 0x%08x\n", arg_val);
            location->off = arg_val;
            break;
    }

    if(arg_val == 0x0) return -1;
    off = arg_val;
    arg_val = 0x0;
    arg_addr = 0x0;
    d_print("Locating size\n");

    /* resolve size */
    switch(selector->size_location)
    {
        case LOCATION_CONST:
            d_print("LOCATION_CONST\n");
            location->size = selector->size;
            arg_val = selector->size;
            d_print("Arg size: 0x%08x\n", arg_val);
            break;
    
        case LOCATION_MEM:
            d_print("LOCATION_MEM\n");
            arg_addr = selector->size;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            d_print("Arg size: 0x%08x\n", arg_val);
            location->size = arg_val;
            break;

        case LOCATION_STACK:
            d_print("LOCATION_STACK\n");
            arg_addr = cur_esp + 0x8;
            arg_addr += selector->size * 0x4;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            d_print("Arg size: 0x%08x\n", arg_val);
            location->size = arg_val;
            break;

        case LOCATION_ADDR_STACK:
            d_print("LOCATION_ADDR_STACK\n");
            arg_addr = cur_esp + 0x8;
            arg_addr += selector->size * 0x4;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            arg_addr = arg_val;
            read_memory(my_trace->cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
            d_print("Arg size: 0x%08x\n", arg_val);
            location->size = arg_val;
            break;
    }

    size = arg_val;
    d_print("[Resolved location: 0x%08x, size: 0x%08x]\n", off, size);

    return 0x0;
}

int add_region_sel(DWORD off, DWORD size, char off_location, char size_location)
{
    unsigned cur_region;

    cur_region = my_trace->region_sel_count;

    my_trace->region_sel[cur_region].off = off;
    my_trace->region_sel[cur_region].size = size;
    my_trace->region_sel[cur_region].off_location = off_location;
    my_trace->region_sel[cur_region].size_location = size_location;

    d_print("Registered region id 0x%08x\n", cur_region);

    cur_region++;
    my_trace->region_sel_count = cur_region;

    return 0x0;
}

REACTION* find_reaction(char* id)
{
    unsigned i;

    for(i= 0x0; i< my_trace->reaction_count; i++)
    {
        if(!strcmp(my_trace->reactions[i].reaction_id, id))
        {
            if(strcmp(my_trace->reactions[i].reaction_id, "xx")) /* exclude xx from searches */
            {
                d_print("Found reaction dor id: %s - %s\n", my_trace->reactions[i].reaction_id, id);
                return &my_trace->reactions[i];
            }
        }
    }

    return 0x0;
}

int add_couple(char* id, char* couple_id)
{
    d_print("[add_couple]\n");
    REACTION* target;

    char my_couple[0x3];

    my_couple[0] = couple_id[0];
    my_couple[1] = couple_id[1];
    my_couple[2] = 0;

    target = find_reaction(id);
    strcpy(target->coupled_id[target->couple_id_count], couple_id);
    target->couple_id_count++;
    
    d_print("Added couple: %s\n", my_couple);
    d_print("[add_couple ends]\n");
    return 0x0;
}

int add_reaction(char* location_str, char* reaction_id, unsigned rid)
{
    d_print("[add_reaction]\n");

    unsigned cur_reaction_id = my_trace->reaction_count;

    d_print("Trying to add reaction at: %s with id: %s, rid: 0x%02x\n", location_str, reaction_id, rid);

    memset((void*)&my_trace->reactions[cur_reaction_id], 0x0, sizeof(REACTION));

    strcpy(my_trace->reactions[my_trace->reaction_count].reaction_id, reaction_id);

    my_trace->reactions[cur_reaction_id].reaction_id[0x2] = 0x0;
    my_trace->reactions[cur_reaction_id].bp = add_breakpoint(location_str, &my_trace->reactions[cur_reaction_id]);
    my_trace->reactions[cur_reaction_id].bp->enabled = 0x1; /* deprecated, moved to reaction */
    my_trace->reactions[cur_reaction_id].enabled = 0x1;
    my_trace->reactions[cur_reaction_id].routine_id = rid;
    update_breakpoint(my_trace->reactions[cur_reaction_id].bp);
    d_print("New reaction: %s:%s\n", location_str, my_trace->reactions[cur_reaction_id].reaction_id);
    my_trace->reaction_count ++;

    d_print("[add_reaction ends]\n");
    return 0x0;
}

/*
int parse_region(char* str)
{
    DWORD  off;
    DWORD  size;
    char* label_off_location;
    char* label_size_location;
    char off_location;
    char size_location;

    d_print("Parsing region: %s\n", str);

    off = strtoul(strtok(str, ":"), 0x0, 0x10);
    label_off_location = strtok(0x0, ":");
    size = strtoul(strtok(0x0, ":"), 0x0, 0x10);
    label_size_location = strtok(0x0, ";");

    d_print("Calculating off location for data: 0x%08x:%s\n", off, label_off_location);

    if(!strcmp(label_off_location, "CONST")) 
    {
        off_location = LOCATION_CONST;
        d_print("Off location is CONST\n");
    }
    else if(!strcmp(label_off_location, "STACK")) 
    {
        off_location = LOCATION_STACK;
        d_print("Off location is STACK\n");
    }
    else if(!strcmp(label_off_location, "ADDR_STACK")) 
    {
        off_location = LOCATION_ADDR_STACK;
        d_print("Off location is ADDR_STACK\n");
    }
    else if(!strcmp(label_off_location, "ADDR_ADDR_STACK")) 
    {
        off_location = LOCATION_ADDR_ADDR_STACK;
        d_print("Off location is ADDR_ADDR_STACK\n");
    }
    else if(!strcmp(label_off_location, "REG")) 
    {
        off_location = LOCATION_REG;
        d_print("Off location is REG\n");
    }
    else if(!strcmp(label_off_location, "MEM")) 
    {
        off_location = LOCATION_MEM;
        d_print("Off location is MEM\n");
    }
    else if(!strcmp(label_off_location, "END")) 
    {
        off_location = LOCATION_CONST;
        d_print("Off location is END\n");
    }
    
    d_print("Calculating size location for data: 0x%08x:%s\n", size, label_size_location);

    if(!strcmp(label_size_location, "CONST")) size_location = LOCATION_CONST;
    else if(!strcmp(label_size_location, "STACK")) size_location = LOCATION_STACK;
    else if(!strcmp(label_size_location, "ADDR_STACK")) size_location = LOCATION_ADDR_STACK;
    else if(!strcmp(label_size_location, "ADDR_ADDR_STACK")) size_location = LOCATION_ADDR_ADDR_STACK;
    else if(!strcmp(label_size_location, "REG")) size_location = LOCATION_REG;
    else if(!strcmp(label_size_location, "MEM")) size_location = LOCATION_MEM;
    else if(!strcmp(label_size_location, "END")) size_location = LOCATION_END;
    

    d_print("Adding region selector: 0x%08x, 0x%08x, 0x%02x, 0x%02x\n", off, size, off_location, size_location);

    add_region_sel(off, size, off_location, size_location);

    return 0x0;
}
*/

int parse_region(char* str)
{
    d_print("[parse_region]\n");

    char* loc_str;
    char* size_str;

    loc_str = strtok(str, ":");
    d_print("loc_str: %s\n", loc_str);
    size_str = strtok(0x0, ":");
    d_print("loc_str: %s\n", loc_str);

    REGION* cur_region;
    
    cur_region = &my_trace->regions[my_trace->regions_count];

    cur_region->off = parse_location_desc(loc_str);
    cur_region->size = parse_location_desc(size_str);
    
    my_trace->regions_count++;

    d_print("[parse_region finishes]\n");
    return 0x0;
}

int parse_reaction(char* str)
{
    d_print("[parse_reaction]\n");
    char* loc_str;
    char* id;
    char* coupled_id;
    char coupled_ids[MAX_LINE];
    unsigned rid;

    d_print("Parsing until ;: %s\n", str);

    loc_str = strtok(str, ",");
    d_print("loc_str: %s\n", loc_str);
    id = strtok(0x0, ",");
    d_print("id: %s\n", id);
    rid = strtol(strtok(0x0, ";"), 0x0, 0x10);
    d_print("rid: 0x%02x\n", rid);

    /* registering e_reaction */
    add_reaction(loc_str, id, rid);

    coupled_id = strtok(id, ":");

    d_print("coupled_id: %p, from %p\n", coupled_ids, coupled_id);

    while((coupled_id = strtok(0x0, ":")) != 0x0)
    {
//        *coupled_id = 0x0;
//        coupled_id++;
        d_print("coupled_id_str: %s\n", coupled_id);
        add_couple(id, coupled_id);
    }
    d_print("[parse_reaction ends]\n");
}

int parse_reactions(char* str)
{
    char* current;
    char buf[MAX_LINE];

    current = str;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        parse_reaction(buf);
        current = strpbrk(current, ";");
        if(current)
            current++;
    }

    return 0x0;
}

int parse_regions(char* str)
{
    char* current;
    char buf[MAX_LINE];

    current = str;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        parse_region(buf);
        current = strpbrk(current, ";");
        if(current)
            current++;
    }

    return 0x0;
}

int reload_out_file()
{
    char buffer2[MAX_LINE];
    char line2[MAX_LINE];


    my_trace->out_postfix++;

    strcpy(buffer2, "");
    sprintf(buffer2, "%s\\%s_%d.out", my_trace->out_dir, my_trace->out_prefix, my_trace->out_postfix);
    strcpy(my_trace->out_trace, buffer2);

    sprintf(line2, "LF,%s_%d.out\n", my_trace->out_prefix, my_trace->out_postfix);
    add_to_buffer(line2);

    fclose(my_trace->trace);
    my_trace->trace = fopen(my_trace->out_trace, "w+");

    d_print("Setting out file to: %s\n", my_trace->out_trace);

    return 0x0;
}

int handle_cmd(char* cmd)
{
    d_print("%s\n", cmd);
    char buffer2[MAX_LINE];

    my_trace->report_code = REPORT_NOTHING;
    memset(my_trace->report_buffer, 0x0, sizeof(my_trace->report_buffer));
    strcpy(my_trace->report_buffer, "");

    if(!strncmp(cmd, CMD_SET_NAME, 2))
    {
        strcpy(my_trace->in_sample_path, cmd+3);
        d_print("Sample path set to: %s\n", my_trace->in_sample_path);    
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_PID, 2))
    {
        my_trace->in_sample_pid = strtol(cmd+3, 0x0, 0x10);
        d_print("Sample PID set to: 0x%08x\n", my_trace->in_sample_pid);
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_IN_DIRECTORY, 2))
    {
        strcpy(my_trace->in_research_dir, cmd+3);
        d_print("Research dir set to: %s\n", my_trace->in_research_dir);
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_OUT_DIRECTORY, 2))
    {
        strcpy(my_trace->out_dir, cmd+3);
        d_print("Out dir set to: %s\n", my_trace->out_dir);
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_OUT_PREFIX, 2))
    {
        strcpy(my_trace->out_prefix, cmd+3);
        d_print("Out prefix set to: %s\n", my_trace->out_prefix);
        send_report();
    }
    else if(!strncmp(cmd, CMD_READ_PREFIX, 2))
    {
        my_trace->report_code = REPORT_INFO;
        strcpy(my_trace->report_buffer, my_trace->out_prefix);
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_LIMIT, 2))
    {
        my_trace->instr_limit = strtoul(cmd+3, 0x0, 10);
        d_print("Trace limit set to: %d\n", my_trace->instr_limit);
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_TRACE_NAME, 2))
    {
        /* deprecated, its auotmated
        strcpy(my_trace->out_trace, cmd+3);
        */
        d_print("Trace name set to: %s\n", my_trace->out_trace);
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_DUMP_NAME, 2))
    {
        /* deprecated, its auotmated
        strcpy(my_trace->out_dump, cmd+3);
        */
        d_print("Dump name set to: %s\n", my_trace->out_dump);
        send_report();
    }
    else if(!strncmp(cmd, CMD_INFO_NAME, 2))
    {
        /* deprecated, its auotmated
        strcpy(my_trace->iniPath, cmd+3);
        */
        d_print("Info name set to: %s\n", my_trace->out_ini);
        send_report();
    }
    else if(!strncmp(cmd, CMD_PREPARE_TRACE, 2))
    {
        char buffer2[MAX_LINE];

        if((my_trace->out_dir == 0x0) || (my_trace->out_prefix == 0x0)) 
        {
            d_print("Error, out dir not set");
            exit(-1);
        }

        /* TODO: make it nicer */
        if(my_trace->in_sample_pid != 0x0)
        {
            sprintf(my_trace->out_prefix, "%08x", my_trace->in_sample_pid);
        }

        /* configure out paths */
        /* trace */ 
        my_trace->out_postfix = -1;
        reload_out_file();

        /* log */ 
        strcpy(buffer2, "");
        sprintf(buffer2, "%s\\%s.log", my_trace->out_dir, my_trace->out_prefix);
        strcpy(my_trace->out_log, buffer2);
        my_trace->log = fopen(my_trace->out_log, "w");
        d_print("Log file: %s\n", my_trace->out_log);

        /* dump */ 
        strcpy(buffer2, "");
        sprintf(buffer2, "%s\\%s.dump", my_trace->out_dir, my_trace->out_prefix);
        strcpy(my_trace->out_dump, buffer2);
        d_print("Dump file: %s\n", my_trace->out_dump);
        
        /* mods */ 
        strcpy(buffer2, "");
        sprintf(buffer2, "%s\\%s.mod", my_trace->out_dir, my_trace->out_prefix);
        strcpy(my_trace->out_mods, buffer2);
        my_trace->mods = fopen(my_trace->out_mods, "wb");
        d_print("Mods file: %s\n", my_trace->out_mods);
        
        /* ini */ 
        strcpy(buffer2, "");
        sprintf(buffer2, "%s\\%s.ini", my_trace->out_dir, my_trace->out_prefix);
        strcpy(my_trace->out_ini, buffer2);
        my_trace->ini = fopen(my_trace->out_ini, "w");
        d_print("Ini file: %s\n", my_trace->out_ini);
    
        send_report();
        my_trace->status = STATUS_CONFIGURED;
    }
    else if(!strncmp(cmd, CMD_ENABLE_TRACE, 2))
    {
        char line2[MAX_LINE];

        my_trace->status = STATUS_DBG_STARTED;
        ss_callback((void*)&my_trace->last_event);
        set_ss(0x0);
        d_print("Tracing enabled\n");

        d_print("Starting @ 0x%08x\n", my_trace->last_eip);
        sprintf(line2, "ST,0x%08x\n", my_trace->last_eip);
        add_to_buffer(line2);

        send_report();
    }
    else if(!strncmp(cmd, CMD_ENABLE_DBG_TRACE, 2))
    {
        char line2[MAX_LINE];

        my_trace->status = STATUS_DBG_SCANNED;
        ss_callback((void*)&my_trace->last_event);
        set_ss(0x0);
        d_print("Tracing debugged enabled\n");

        d_print("Starting @ 0x%08x\n", my_trace->last_eip);
        sprintf(line2, "ST,0x%08x\n", my_trace->last_eip);
        add_to_buffer(line2);

        send_report();
    }
    else if(!strncmp(cmd, CMD_DISABLE_TRACE, 2))
    {
        char line2[MAX_LINE];

        my_trace->status = STATUS_DBG_STOPPED;
        d_print("Tracing disabled\n");

        d_print("Ending @ 0x%08x\n", my_trace->last_eip);
        sprintf(line2, "END,0x%08x\n", my_trace->last_eip);
        add_to_buffer(line2);

        send_report();
    }
    else if(!strncmp(cmd, CMD_DUMP_MEMORY, 2))
    {
        dump_memory();
        send_report();
    }
    else if(!strncmp(cmd, CMD_DISABLE_REACTION, 2))
    {
        char* mod;
        char reaction_id[MAX_LINE];

        mod = strtok(cmd, " ");
        strcpy(reaction_id, strtok(0x0, " "));

        d_print("Disabling reaction %s\n", reaction_id);
        disable_reaction(reaction_id);
        send_report();
    }
    else if(!strncmp(cmd, CMD_ENABLE_ALL_REACTIONS, 2))
    {
        d_print("Enabling all reactions\n");
        enable_all_reactions();
        send_report();
    }
    else if(!strncmp(cmd, CMD_DISABLE_ALL_REACTIONS, 2))
    {
        d_print("Disabling all reactions\n");
        disable_all_reactions();
        send_report();
    }
    else if(!strncmp(cmd, CMD_ENABLE_REACTION, 2))
    {
        char* mod;
        char reaction_id[MAX_LINE];

        mod = strtok(cmd, " ");
        strcpy(reaction_id, strtok(0x0, " "));
        
        d_print("Enabling reaction no %s\n", reaction_id);
        enable_reaction(reaction_id);
        send_report();
    }
    else if(!strncmp(cmd, CMD_ATTACH_DEBUG, 2))
    {
        unsigned report;

        d_print("Starting debugging: 0x%08x\n", my_trace->in_sample_pid);

        start_trace_pid();

        my_trace->status = STATUS_CONFIGURED; /* move to other */

        my_trace->report_code = get_pending_events();
        send_report();   
    }
    else if(!strncmp(cmd, CMD_START_DEBUG, 2))
    {
        unsigned report;

        strcpy(my_trace->process_fname, my_trace->in_research_dir);
        strcat(my_trace->process_fname, "\\");
        strcat(my_trace->process_fname, my_trace->in_sample_path);

        d_print("Starting debugging: %s\n", my_trace->process_fname);

        start_trace_fname();

        my_trace->status = STATUS_CONFIGURED; /* move to other */

        my_trace->report_code = get_pending_events();
        send_report();   
    }
    else if(!strncmp(cmd, CMD_LIST_TEBS, 2))
    {
        list_tebs();
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_LIST_ALL_TEBS, 2))
    {
        list_all_tebs();
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_LIST_REACTIONS, 2))
    {
        list_reactions();
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_LIST_BPTS, 2))
    {
        list_bpts();
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_LIST_LIBS, 2))
    {
        list_libs();
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_READ_MEMORY, 2))
    {
        DWORD addr;
        char* cmd_;

        cmd_ = strtok(cmd, " ");
        addr = strtoul(strtok(0x0, " "), 0x0, 0x10);

        d_print("Reading addr: 0x%08x\n", addr);

        report_dword(addr);
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_WRITE_MEMORY, 2))
    {
        char* cmd_;
        DWORD addr;
        DWORD val;

        cmd_ = strtok(cmd, " ");
        addr = strtoul(strtok(0x0, " "), 0x0, 0x10);
        val = strtoul(strtok(0x0, " "), 0x0, 0x10);

        d_print("Writing addr: 0x%08x with 0x%08x\n", addr, val);

        write_dword(addr, val);
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_RELEASE_THREAD, 2))
    {
        unsigned tid_id;
        char* reg;

        reg = strtok(cmd, " ");
        tid_id = strtoul(strtok(0x0, " "), 0x0, 0x10);

        release_thread(tid_id);
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_READ_REGISTER, 2))
    {
        unsigned tid_id;
        char* reg;

        reg = strtok(cmd, " ");
        tid_id = strtoul(strtok(0x0, " "), 0x0, 0x10);
        reg = strtok(0x0, " ");

        report_register(tid_id, reg);
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_WRITE_REGISTER, 2))
    {
        unsigned tid_id;
        char* reg;
        char* data;

        reg = strtok(cmd, " ");
        tid_id = strtoul(strtok(0x0, " "), 0x0, 0x10);
        reg = strtok(0x0, " ");
        data = strtok(0x0, " ");

        write_register(tid_id, reg, data);
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_READ_STACK, 2))
    {
        char* data;
        unsigned tid_id;
        DWORD count;

        data = strtok(cmd, " ");
        tid_id = strtoul(strtok(0x0, " "), 0x0, 0x10);
        count = strtoul(strtok(0x0, " "), 0x0, 0x10);

        read_stack(tid_id, count);
        send_report(); 
    }
    else if(!strncmp(cmd, CMD_CONTINUE, 2))
    {
        unsigned report;
        int status;
        char* data;

        data = strtok(cmd, " ");
        data = strtok(0x0, " ");
        status = strtoul(data, 0x0, 0x10);

        d_print("Continuing with status: 0x%08x\n", status);
        continue_routine(INFINITE, status);
        send_report();   
    }
    else if(!strncmp(cmd, CMD_CONTINUE_TIME, 2))
    {
        unsigned report;
        DWORD time;
        unsigned status;

        strtok(cmd, " ");
        time = strtoul(strtok(0x0, " "), 0x0, 0x10);
        status = strtoul(strtok(0x0, " "), 0x0, 0x10);

        continue_routine(time, status);
        send_report();   
    }
    else if(!strncmp(cmd, CMD_SET_ST, 2))
    {
        char* e_reactions_str;
        DWORD addr;
        char my_str[MAX_LINE];

        strtok(cmd, " ");

        addr = strtoul(strtok(0x0, " "), 0x0, 0x10);
        sprintf(my_str, "0x%08x", addr);

        add_reaction(my_str, "ST", 0x0);
        enable_reaction("ST");
        send_report();
        
    }
    else if(!strncmp(cmd, CMD_AUTO_ST, 2))
    {
        char* e_reactions_str;
        char my_str[MAX_LINE];

        sprintf(my_str, "0x%08x", (OFFSET)(my_trace->cpdi.lpStartAddress));

        add_reaction(my_str, "ST", 0x0);
        enable_reaction("ST");
        send_report();
        
    }
    else if(!strncmp(cmd, CMD_CONFIGURE_REACTIONS, 2))
    {
        char* reactions_str;

        strtok(cmd, " ");
        reactions_str = strtok(0x0, " ");

        d_print("Configuring reactions\n");
        parse_reactions(reactions_str);
        send_report();
        
    }
    else if(!strncmp(cmd, CMD_CONFIGURE_REGIONS, 2))
    {
        char* str;

        strtok(cmd, " ");
        str = strtok(0x0, " ");
        parse_regions(str);
        send_report();
        
    }
    else if(!strncmp(cmd, CMD_ROUTINE_1, 2))
    {
        /* TODO: implement */    
    }
    else
    {
    }

    ret:
    return 0x0;
}

int init_trace(TRACE_CONFIG* trace, char* host, short port)
{
    /* Initial trace config */
    my_trace->thread_count = 0x0;
    my_trace->reaction_count = 0x0;

    strcpy(my_trace->host, host);
    my_trace->port = port;

    memset(my_trace->thread_map, -1, sizeof(my_trace->thread_map));

    my_trace->mutex = CreateMutex(0x0, 0x0, 0x0);
    my_trace->eventLock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    my_trace->eventUnlock = CreateEvent(0x0, 0x0, 0x0, 0x0);

}

/* configure syscalls */

int configure_syscalls()
{
    unsigned i,j;

    /* syscall out args table */

    d_print("Configuring syscall out args\n");

    for(i=0x0; i<MAX_SYSCALL_ENTRIES; i++)
        for(j=0x0;j<MAX_SYSCALL_OUT_ARGS; j++)
            my_trace->syscall_out_args[i][j].off_location = LOCATION_END;

#ifdef LIB_VER_WXP
    // ZwAllocateVirtualMemory
    my_trace->syscall_out_args[0x11][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY}; 
    my_trace->syscall_out_args[0x11][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x11][2] = {0x1, 0x3, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0}; 
    my_trace->syscall_out_args[0x11][3] = last_arg;
   
    // ZwClose
    my_trace->syscall_out_args[0x19][0] = last_arg;

    // ZwCreateFile
    my_trace->syscall_out_args[0x25][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x25][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x25][2] = last_arg;

    // ZwCreateSection 
    my_trace->syscall_out_args[0x32][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x32][1] = last_arg;
   
    // ZwFreeVirtualMemory
    my_trace->syscall_out_args[0x53][0] = {0x1, 0x2, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0x53][1] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x53][2] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x53][3] = last_arg;

    // ZwMapViewOfSection
    my_trace->syscall_out_args[0x6c][0] = {0x2, 0x6, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0x6c][1] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x6c][2] = {0x5, 0x8, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x6c][3] = {0x6, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x6c][4] = last_arg;

    // NtOpenFile
    my_trace->syscall_out_args[0x74][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x74][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x74][2] = last_arg;

    // ZwOpenKey
    my_trace->syscall_out_args[0x77][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x77][1] = last_arg;

    // ZwOpenProcessToken
    my_trace->syscall_out_args[0x7b][0] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x7b][1] = last_arg;

    // ZwOpenSection
    my_trace->syscall_out_args[0x7d][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x7d][1] = last_arg;

    // ZwProtectVirtualMemory
    my_trace->syscall_out_args[0x89][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x89][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x89][2] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x89][3] = last_arg;

    // NtQueryAttributesFile
    my_trace->syscall_out_args[0x8b][0] = {0x0, sizeof(FILE_BASIC_INFORMATION), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x8b][1] = last_arg;

    // ZwQueryInformationProcess
    my_trace->syscall_out_args[0x9a][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x9a][1] = {0x2, 0x3, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0x9a][2] = last_arg;

    // ZwQueryInformationToken
    my_trace->syscall_out_args[0x9c][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x9c][1] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0x9c][2] = last_arg;

    // NtQueryPerformanceCounter
    my_trace->syscall_out_args[0xa5][0] = {0x0, 0x8, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    my_trace->syscall_out_args[0xa5][1] = {0x1, 0x8, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    my_trace->syscall_out_args[0xa5][2] = last_arg;

    // NtQuerySection
    // TODO: check 1st
    my_trace->syscall_out_args[0xa7][0] = {0x2, 0x4, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    my_trace->syscall_out_args[0xa7][1] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0xa7][2] = last_arg;


    // ZwQuerySystemInformation
    my_trace->syscall_out_args[0xad][0] = {0x1, 0x2, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    my_trace->syscall_out_args[0xad][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0};
    my_trace->syscall_out_args[0xad][2] = last_arg;

    // ZwQueryValueKey
    my_trace->syscall_out_args[0xb1][0] = {0x3, 0x5, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0xb1][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xb1][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xb1][3] = last_arg;

    // ZwReadFile
    my_trace->syscall_out_args[0xb7][0] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xb7][1] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xb7][2] = {0x5, 0x6, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    my_trace->syscall_out_args[0xb7][3] = last_arg;

    // ZwRequestWaitReplyPort
    my_trace->syscall_out_args[0xc8][0] = {0x2, 0x148, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0xc8][1] = last_arg;

    // ZwUnmapViewOfSection
    my_trace->syscall_out_args[0x101][0] = last_arg;

    // ZwTraceControl
/*
    my_trace->syscall_out_args[0x177][0] = {0x3, 0x4, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0x0};
    my_trace->syscall_out_args[0x177][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x177][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x177][3] = last_arg;
*/
    // ZwWriteVirtualMemory
    my_trace->syscall_out_args[0x115][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x115][1] = last_arg;
#endif
    
#ifdef LIB_VER_W7 
    // ZwAllocateVirtualMemory
    my_trace->syscall_out_args[0x13][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY}; 
    my_trace->syscall_out_args[0x13][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x13][2] = {0x1, 0x3, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0}; 
    my_trace->syscall_out_args[0x13][3] = last_arg;
   
    // ZwClose
    my_trace->syscall_out_args[0x32][0] = last_arg;

    // 0x40

    // ZwCreateFile
    my_trace->syscall_out_args[0x42][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x42][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x42][2] = last_arg;

    // ZwCreateSection 
    my_trace->syscall_out_args[0x54][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x54][1] = last_arg;
  
    // ZwDeviceIoControlFile (not verified)
    my_trace->syscall_out_args[0x6b][0] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x6b][1] = {0x8, 0x9, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0x0};
    my_trace->syscall_out_args[0x6b][2] = last_arg;
 
    // ZwFreeVirtualMemory
    my_trace->syscall_out_args[0x83][0] = {0x1, 0x2, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0x83][1] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x83][2] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x83][3] = last_arg;

    // ZwMapViewOfSection
    my_trace->syscall_out_args[0xa8][0] = {0x2, 0x6, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0xa8][1] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xa8][2] = {0x5, 0x8, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xa8][3] = {0x6, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xa8][4] = last_arg;

    // ZwOpenDirectoryObject
    my_trace->syscall_out_args[0xaf][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xaf][1] = last_arg;

    // NtOpenFile
    my_trace->syscall_out_args[0xb3][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xb3][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xb3][2] = last_arg;

    // ZwOpenKey
    my_trace->syscall_out_args[0xb6][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xb6][1] = last_arg;

    // ZwRegisterThreadTerminatePort
    my_trace->syscall_out_args[0xbb][0] = last_arg;

    // ZwOpenProcessToken
    my_trace->syscall_out_args[0xbf][0] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xbf][1] = last_arg;

    // c0

    // ZwOpenSection
    my_trace->syscall_out_args[0xc2][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xc2][1] = last_arg;

    // ZwOpenSymbolicLinkObject
    my_trace->syscall_out_args[0xc5][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xc5][1] = last_arg;

    // ZwProtectVirtualMemory
    my_trace->syscall_out_args[0xd7][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xd7][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xd7][2] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xd7][3] = last_arg;

    // NtQueryAttributesFile
    my_trace->syscall_out_args[0xd9][0] = {0x0, sizeof(FILE_BASIC_INFORMATION), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xd9][1] = last_arg;

    // NtQueryDefaultLocale
    my_trace->syscall_out_args[0xdd][0] = {0x1, sizeof(LCID), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xdd][1] = last_arg;

    // ZwQueryDirectoryFile
    my_trace->syscall_out_args[0xdf][0] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xdf][1] = {0x5, 0x6, LOCATION_ADDR_STACK, LOCATION_STACK, STATUS_ANY};
    my_trace->syscall_out_args[0xdf][2] = last_arg;

    // ZwQueryInformationFile
    my_trace->syscall_out_args[0xe7][0] = {0x1, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xe7][1] = {0x2, 0x3, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    my_trace->syscall_out_args[0xe7][2] = last_arg;

    // ZwQueryInformationProcess
    my_trace->syscall_out_args[0xea][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xea][1] = {0x2, 0x3, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0xea][2] = last_arg;

    // ZwQueryInformationToken
    my_trace->syscall_out_args[0xed][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0xed][1] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0xed][2] = last_arg;

    // NtQueryPerformanceCounter
    my_trace->syscall_out_args[0xfb][0] = {0x0, 0x8, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    my_trace->syscall_out_args[0xfb][1] = {0x1, 0x8, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    my_trace->syscall_out_args[0xfb][2] = last_arg;

    // NtQuerySection
    // TODO: check 1st
    my_trace->syscall_out_args[0xfe][0] = {0x2, 0x4, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    my_trace->syscall_out_args[0xfe][1] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0xfe][2] = last_arg;


    // ZwQuerySystemInformation
    my_trace->syscall_out_args[0x105][0] = {0x1, 0x2, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    my_trace->syscall_out_args[0x105][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0};
    my_trace->syscall_out_args[0x105][2] = last_arg;

    // NtQuerySystemTime
    my_trace->syscall_out_args[0x107][0] = {0x0, sizeof(LARGE_INTEGER), LOCATION_ADDR_STACK, LOCATION_CONST, 0};
    my_trace->syscall_out_args[0x107][1] = last_arg;

    // ZwQueryValueKey
    my_trace->syscall_out_args[0x10a][0] = {0x3, 0x5, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    my_trace->syscall_out_args[0x10a][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x10a][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x10a][3] = last_arg;

    // ZwQueryVolumeInformationFile
    my_trace->syscall_out_args[0x10c][0] = {0x1, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x10c][1] = {0x2, 0x3, LOCATION_ADDR_STACK, LOCATION_STACK, STATUS_ANY};
    my_trace->syscall_out_args[0x10c][2] = last_arg;

    // ZwReadFile
    my_trace->syscall_out_args[0x111][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x111][1] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x111][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x111][3] = {0x5, 0x6, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    my_trace->syscall_out_args[0x111][4] = last_arg;

    // ZwRequestWaitReplyPort
    my_trace->syscall_out_args[0x12b][0] = {0x1, 0x180, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x12b][1] = {0x2, 0x180, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x12b][2] = last_arg;

    // ZwSetInformationFile
    my_trace->syscall_out_args[0x149][0] = {0x1, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    my_trace->syscall_out_args[0x149][1] = last_arg;

    // ZwTraceControl
    my_trace->syscall_out_args[0x177][0] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x177][1] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x177][2] = last_arg;

    // ZwUnmapViewOfSection
    my_trace->syscall_out_args[0x181][0] = last_arg;

    // NtWaitForSingleObject
    my_trace->syscall_out_args[0x187][0] = last_arg;

    // ZwWriteVirtualMemory
    my_trace->syscall_out_args[0x18f][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    my_trace->syscall_out_args[0x18f][1] = last_arg;

    // Xara:
    // 10e8 114e 1197 114e 11a4 11bb 11c4 11c6 11de 11e8 11ea 11ef 11f2 1203 1212 1214 1232 1233 1242

    // calc:
    // 1208
    // 118b
    // 1193
    // 1169
    // 11be
    // 11bb
    // 11a9
    // 114d
    // 1253
    // 10e4
    // 11a4
    // 10b0
    // 114e
    // 11cf
    // 112a
    // 112f
    // 1203
    // 1232
    // 11ea
    // 1133
    // 

#endif

    return 0x0;    
}

/* new main routine */

int main(int argc, char** argv)
{

    int i = 0;
    for(i=0;i<argc;i++)
        d_print("%d - %s\n", i, argv[i]);

    if(argc < 3)
    {
        d_print("You need do provide host and port\n");
        return -1;
    }

    d_print("Version 3.0\n");

    if(strlen(argv[1]) > MAX_LINE) return -1;
    if(strlen(argv[2]) > MAX_LINE) return -1;

    my_trace = (TRACE_CONFIG*)malloc(sizeof(TRACE_CONFIG));
    if(my_trace == 0x0)
    {
        d_print("Unable to allocate trace config\n");
        return -1;
    }

    init_trace(my_trace, argv[1], atoi(argv[2]));

    /*configure syscalls */
    configure_syscalls();

    /*configure routines */
    my_trace->routines[0x1] = &react_sysenter_callback;
    my_trace->routines[0x2] = &react_sysret_callback;
    my_trace->routines[0x3] = &react_sysret_refresh;
    my_trace->routines[0x100] = &react_sample_routine_1;
    my_trace->routines[0x101] = &react_zero_SF;
    my_trace->routines[0x102] = &react_set_ZF;
    my_trace->routines[0x103] = &react_zero_ZF;
    my_trace->routines[0x201] = &react_update_region_1;
    my_trace->routines[0x202] = &react_cry_antidebug_1;
    my_trace->routines[0x203] = &react_skip_on;
    my_trace->routines[0x204] = &react_skip_off;
    my_trace->routines[0x205] = &react_update_region_2;

    /* outputting DWORDs */
    my_trace->routines[0x300] = &react_output_arg_0;
    my_trace->routines[0x301] = &react_output_arg_1;
    my_trace->routines[0x302] = &react_output_arg_2;
    my_trace->routines[0x303] = &react_output_arg_3;
    my_trace->routines[0x304] = &react_output_arg_4;
    my_trace->routines[0x305] = &react_output_arg_5;
    my_trace->routines[0x306] = &react_output_arg_6;
    my_trace->routines[0x307] = &react_output_arg_7;

    /* outputting ANSI strings */
    my_trace->routines[0x310] = &react_output_arg_str_0;
    my_trace->routines[0x311] = &react_output_arg_str_1;
    my_trace->routines[0x312] = &react_output_arg_str_2;
    my_trace->routines[0x313] = &react_output_arg_str_3;
    my_trace->routines[0x314] = &react_output_arg_str_4;
    my_trace->routines[0x315] = &react_output_arg_str_5;
    my_trace->routines[0x316] = &react_output_arg_str_6;
    my_trace->routines[0x317] = &react_output_arg_str_7;

    /* outputting UNICODE strings */
    my_trace->routines[0x320] = &react_output_arg_unicode_str_0;
    my_trace->routines[0x321] = &react_output_arg_unicode_str_1;
    my_trace->routines[0x322] = &react_output_arg_unicode_str_2;
    my_trace->routines[0x323] = &react_output_arg_unicode_str_3;
    my_trace->routines[0x324] = &react_output_arg_unicode_str_4;
    my_trace->routines[0x325] = &react_output_arg_unicode_str_5;
    my_trace->routines[0x326] = &react_output_arg_unicode_str_6;
    my_trace->routines[0x327] = &react_output_arg_unicode_str_7;

    /* outputting registers */
    my_trace->routines[0x330] = &react_output_eax;
    my_trace->routines[0x331] = &react_output_ebx;
    my_trace->routines[0x332] = &react_output_ecx;
    my_trace->routines[0x333] = &react_output_edx;
    my_trace->routines[0x334] = &react_output_esi;
    my_trace->routines[0x335] = &react_output_edi;
    my_trace->routines[0x336] = &react_output_esp;
    my_trace->routines[0x337] = &react_output_eip;


    /* Windows sockets */

    WSADATA wsa;
    SOCKET s;    
    struct sockaddr_in server;
    
    d_print("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        d_print("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
     
    d_print("Initialised\n");

    /* Connect to socket */

    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        d_print("Could not create socket : %d" , WSAGetLastError());
    }

    d_print("Socket created\n");
    u_long iMode = 0;

    unsigned iResult;

    iResult = ioctlsocket(s, FIONBIO, &iMode);
    if (iResult != NO_ERROR) d_print("ioctlsocket failed with error: %ld\n", iResult);

    /* Creating backconnect */
    server.sin_addr.s_addr = inet_addr(TRACE_CONTROLLER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(TRACE_CONTROLLER_PORT);
 
    iResult = ioctlsocket(s, FIONBIO, &iMode);
    if (iResult != NO_ERROR) d_print("ioctlsocket failed with error: %ld\n", iResult);

    if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        d_print("connect error");
        return 1;
    }
     
    my_trace->socket = s;

    iResult = ioctlsocket(s, FIONBIO, &iMode);
    if (iResult != NO_ERROR) d_print("ioctlsocket failed with error: %ld\n", iResult);

    d_print("Connected\n");

    /* handle commands in loop */

    char cmd[MAX_LINE];
    unsigned cmd_len;

    char recv_buf[MAX_LINE];

    DWORD recv_size;
    DWORD flags;

    while(1)
    {
        strcpy(cmd, "");
        strcpy(recv_buf, "");
        cmd_len = 0x0;

        while(1)
        {
            recv_size = recv(s, recv_buf, MAX_LINE-1, 0);

            strcat(cmd, recv_buf);
            cmd_len += recv_size;

            if(!strncmp(cmd+cmd_len-6, "-=OK=-", 6)) break;
            //d_print("Got part: %s\n", cmd);
        }

        cmd[cmd_len-6] = 0x0;
        //d_print("Got cmd: %s\n", cmd);

        if(!strcmp(cmd, "quit")) 
            break;
        
        handle_cmd(cmd);
        //d_print("Handled\n");

    }

    return 0x0;
}


