#include "trace5.h"
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
OUT_ARGUMENT last_arg = {0x0, 0x0, LOCATION_END, LOCATION_END, 0x0};
OUT_LOCATION last_location = {0x0, 0x0};
OUT_ARGUMENT syscall_out_args[MAX_SYSCALL_ENTRIES][MAX_SYSCALL_OUT_ARGS];
OUT_LOCATION syscall_out_args_dump_list[MAX_SYSCALL_OUT_ARGS];
DWORD buffer_addr;
DWORD size_addr;
CREATE_THREAD_DEBUG_INFO2 threads[0x100000000];
int add_breakpoint(DWORD addr, handler_routine handler);
DWORD find_lib(char* name);
BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);
SIZE_T dump_mem(FILE*, void*, SIZE_T);

char* blacklist_lib[] = {"kernel32.dll", "ntdll.dll", "user32.dll"};
DWORD blacklist_addr[] = {};
char line2[MAX_LINE];

void marker_handler(void* data);
void reaction_handler(void* data);
void print_context(CONTEXT*);
int del_breakpoint(DWORD);
int resolve_region(OUT_ARGUMENT*, OUT_LOCATION*);
int add_to_buffer(char*);
int read_context(DWORD, CONTEXT*);
int write_context(DWORD, CONTEXT*);

int d_print(const char* format, ...)
{
    va_list argptr;

    if(log)
    {
        va_start(argptr, format);
        vfprintf(log, format, argptr);
        va_end(argptr);
        fflush(log);
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

void sample_routine_1(void* data)
{
    d_print("Sample routine 1\n");
    return;
}

void set_ZF(void* data)
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

void zero_SF(void* data)
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

void update_region(OUT_LOCATION* location)
{
    DWORD size_wrote;
    char line[MAX_LINE];

    size_wrote = dump_mem(my_trace->mods, (void*)location->off, location->size);
    if(size_wrote == location->size)
    {
        d_print("[Updated location: 0x%08x, size: 0x%08x]\n", location->off, location->size);
        sprintf(line, "UP,0x%08x,0x%08x\n", location->off, location->size);
        add_to_buffer(line);
    }

    return;
}

void update_region_1(void* data)
{
    d_print("Updating region 1\n");
    OUT_LOCATION location;
    resolve_region(&my_trace->region_sel[0x0], &location);

    update_region(&location);

    return;
}

int enable_reaction(unsigned reaction)
{
    unsigned i;

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        if(my_trace->reactions[i].id == reaction)
        {
            my_trace->reactions[i].real_offset = my_trace->reactions[i].lib_offset + my_trace->reactions[i].offset;
            add_breakpoint(my_trace->reactions[i].real_offset, reaction_handler);
        }
    }
    return 0x0;
}

int disable_reaction(unsigned reaction)
{
    unsigned i;

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        if(my_trace->reactions[i].id== reaction)
        {
                my_trace->reactions[i].real_offset = my_trace->reactions[i].lib_offset + my_trace->reactions[i].offset;
                del_breakpoint(my_trace->reactions[i].real_offset);
        }
    }
    return 0x0;
}

int enable_marker(char* id)
{
    unsigned i;

    for(i = 0x0; i< my_trace->marker_count; i++)
    {
        if(strstr(my_trace->markers[i].id, id))
        {
            my_trace->markers[i].real_offset = my_trace->markers[i].lib_offset + my_trace->markers[i].offset;
            add_breakpoint(my_trace->markers[i].real_offset, marker_handler);
        }
    }
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

void sysret_refresh(void* data)
{

    d_print("Refreshing\n");
    d_print("Unsetting SS for 0x%08x\n", my_trace->last_tid);
    unset_ss(my_trace->last_tid);
    set_ss(0x0);
}

void read_memory(HANDLE handle, void* from, void* to, SIZE_T size, SIZE_T* read)
{
    DWORD ret;
    //printf("read_memory, handle: 0x%08x\n", my_trace->procHandle);
    ret = ReadProcessMemory(my_trace->procHandle, from, to, size, read);

    if(!ret)
    {
        d_print("Error: 0x%08x\n", GetLastError());
    }
}

void write_memory(HANDLE handle, void* to, void* from, SIZE_T size, SIZE_T* written)
{
    DWORD oldProt;
    DWORD ret;
    
    //printf("write_memory, handle: 0x%08x\n", my_trace->procHandle);
    VirtualProtectEx(my_trace->procHandle, to, 0x4, PAGE_EXECUTE_READWRITE, &oldProt);
    ret = WriteProcessMemory(my_trace->procHandle, to, from, size, written);

    if(!ret)
    {
        d_print("Error: 0x%08x\n", GetLastError());
    }

    VirtualProtectEx(my_trace->procHandle, to, 0x4, oldProt, &oldProt);
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
    //printf("Debug registering: TID 0x%08x, handle 0x%08x\n", tid, handle);
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

void reaction_handler(void* data)
{
    unsigned i;

    d_print("In reaction handler\n");

    for(i = 0x0; i< my_trace->reaction_count; i++)
    {
        if((OFFSET)my_trace->last_exception.ExceptionAddress == my_trace->reactions[i].real_offset)
        {
            d_print("Reaction 0x%08x hit!\n", my_trace->reactions[i].id);
            my_trace->routines[my_trace->reactions[i].id](data);
        }
    }

    return;
}

void marker_handler(void* data)
{
    unsigned i;

    for(i = 0x0; i< my_trace->marker_count; i++)
    {
        if((OFFSET)my_trace->last_exception.ExceptionAddress == my_trace->markers[i].real_offset)
        {
            d_print("Marker %s hit!\n", my_trace->markers[i].id);
            my_trace->last_marker = &my_trace->markers[i];
        }
    }

    return;
}

int update_markers(LIB_ENTRY* lib)
{
    unsigned i;

    d_print("Updating markers for %s\n", lib->lib_name);

    for(i = 0x0; i< my_trace->marker_count; i++)
    {
        if(strstr(my_trace->markers[i].lib_name, lib->lib_name))
        {
            d_print("Should register marker at: 0x%08x:0x%08x\n", lib->lib_offset, my_trace->markers[i].offset);
            my_trace->markers[i].lib_offset = lib->lib_offset;
            my_trace->markers[i].real_offset = my_trace->markers[i].lib_offset + my_trace->markers[i].offset;
            add_breakpoint(my_trace->markers[i].real_offset, marker_handler);
        }
    }

    return 0x0;
}

void register_lib(LOAD_DLL_DEBUG_INFO info)
{
    char path[MAX_NAME];
    char line[MAX_LINE];
    unsigned size = 0;

#ifdef LIB_VER_W7
    //d_print("Trying to resolve\n");
    GetFinalPathNameByHandleA(my_trace->last_event.u.LoadDll.hFile, my_trace->libs[my_trace->lib_count].lib_path, MAX_NAME, VOLUME_NAME_NONE);
    //d_print("Resolved 0x%08x to %s\n", my_trace->last_event.u.LoadDll.hFile, my_trace->libs[my_trace->lib_count].lib_path);
    strcpy(my_trace->libs[my_trace->lib_count].lib_name, find_file(my_trace->libs[my_trace->lib_count].lib_path));
#endif
#ifdef LIB_VER_WXP
    strcpy(my_trace->libs[mt_trace->lib_count].lib_name, "UNKNOWN");
    strcpy(my_trace->libs[mt_trace->lib_count].lib_path, "UNKNOWN");
#endif
    //d_print("Name pointer: %p, len: 0x%08x\n", libs[my_trace->lib_count].lib_name, strlen(libs[my_trace->lib_count].lib_name));

    my_trace->libs[my_trace->lib_count].lib_offset = (DWORD)info.lpBaseOfDll;
    //d_print("RL,0x%08x,%s\n", my_trace->libs[my_trace->lib_count].lib_offset, libs[my_trace->lib_count].lib_name);
    sprintf(line, "RL,0x%08x,%s\n", my_trace->libs[my_trace->lib_count].lib_offset, my_trace->libs[my_trace->lib_count].lib_name);

    my_trace->libs[my_trace->lib_count].loaded = 0x1;
    //update_markers(&my_trace->libs[lib_count]);

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
    //printf(line);
    //printf("\n");
    add_to_buffer(line);

    return;
}

void ss_callback(void* data);
void end_callback(void* data);
void ntmap_1_callback(void* data);
void ntmap_2_callback(void* data);
void ntread_1_callback(void* data);
void ntread_2_callback(void* data);
void sysenter_callback(void* data);
void sysret_callback(void* data);

void ntmap_1_callback(void* data)
{
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;

    DWORD tmp;
    DWORD read;
    DWORD tid;


    d_print("Thread: %08x\n", de->dwThreadId);

    int i = 0x0;

    for(i = 0x0; i<my_trace->thread_count; i++)
    {
        tid = my_trace->thread_map[i];
        if(my_trace->threads[tid].alive)
        d_print("TID: 0x%08x, handle: 0x%08x\n", tid, my_trace->threads[tid].handle);

    }

    HANDLE myHandle = (HANDLE)-0x1;
    DWORD esp = 0x0;
//    myHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, de->dwThreadId);
    myHandle = my_trace->threads[de->dwThreadId].handle;
    d_print("Handle: %x\n", myHandle);
    d_print("Handle in my_trace->threads: %x\n", my_trace->threads[de->dwThreadId]);
//    myHandle = my_trace->threads[de->dwThreadId].handle;
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    if(GetThreadContext(myHandle, &ctx) == 0x0) 
    {
        d_print("Failed to get context, error: 0x%08x\n", GetLastError());
        return;
    }
    d_print("ESP: %p\n", ctx.Esp);

    tmp = ctx.Esp + (0x3 * 0x4);
    read_memory(my_trace->cpdi.hProcess, (void*)(tmp), (void*)&buffer_addr, 0x4, &read);

    tmp = ctx.Esp + (0x7 * 0x4);
    read_memory(my_trace->cpdi.hProcess, (void*)(tmp), (void*)&size_addr, 0x4, &read);

    d_print("off: 0x%08x, size: 0x%08x\n", buffer_addr, size_addr);

    add_breakpoint(nt2_off, ntmap_2_callback);
//    CloseHandle(myHandle);
}

void ntmap_2_callback(void* data)
{
    char line[MAX_LINE];
    d_print("ntmap_2\n");

    DWORD offset, size, size_wrote, read;

    read_memory(my_trace->cpdi.hProcess, (void*)(buffer_addr), (void*)&offset, 0x4, &read);
    read_memory(my_trace->cpdi.hProcess, (void*)(size_addr), (void*)&size, 0x4, &read);

    d_print("Block 0x%08x - 0x%08x, 0x%08x bytes\n", offset, offset + size, size);

    size_wrote = dump_mem(my_trace->mods, (void*)offset, size);
    if(size_wrote == size)
    {
        sprintf(line, "UP,0x%08x,0x%08x\n", offset, size);
        add_to_buffer(line);
    }

    add_breakpoint(nt1_off, ntmap_1_callback);
}

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

void sysenter_callback(void* data)
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

    sprintf(line, "# Syscall no: 0x%08x, stack@ 0x%08x\n", sysenter_no, sysenter_esp);
    add_to_buffer(line);

//    d_print("Deregister thread @ SYSENTER: %08x\n", tid);
    deregister_thread(tid, my_trace->threads[tid_pos].handle);
    enable_reaction(0x1);
    enable_reaction(0x2);
//    add_breakpoint(sysret_off, sysret_callback);
//    add_breakpoint(sysret_off, sysret_refresh);
    set_ss(0x0);
}

void sysret_callback(void* data)
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

//    d_print("Register thread @ SYSRET: %08x\n", tid);
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

        OUT_LOCATION location;
        resolve_region(&my_trace->syscall_out_args[sysenter_no][i], &location);
        update_region(&location);

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
    //sprintf(line, "# ret2");
    //add_to_buffer(line);
    
    //ss_callback(data);
    //sprintf(line, "# ret3");
    //add_to_buffer(line);
    

    //add_breakpoint(sysenter_off, sysenter_callback);
    enable_reaction(0x0);
    //sprintf(line, "# ret4");
    //add_to_buffer(line);
    
}

/* other callbacks */

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

void isdebuggerpresent_callback_1(void* data)
{
    d_print("[antidebug] enter\n");
    add_breakpoint(find_lib("KernelBase.dll") + ISDEBUGGER_OFF_2, isdebuggerpresent_callback_2);

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

    add_breakpoint(find_lib("KernelBase.dll") + ISDEBUGGER_OFF_1, isdebuggerpresent_callback_1);
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
/*
SIZE_T dump_zeros(FILE* f, SIZE_T size)
{
    SIZE_T wrote;
    char zeros[0x1];
    unsigned i;
    for(i=0x0; i<size; i++)
    wrote += fwrite(zeros, 0x1, 0x1, f);
    return wrote;
}
*/
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

/*
void bp_callback(void* data)
{
    char line[MAX_LINE];
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;

    started = 1;

    DWORD eip;
    eip = (DWORD)de->u.Exception.ExceptionRecord.ExceptionAddress;

    d_print("Starting @ 0x%08x\n", eip);
    sprintf(line, "ST,0x%08x\n", eip);
    add_to_buffer(line);

    DWORD tid = de->dwThreadId;

    DWORD tid_pos;
    tid_pos = my_trace->thread_map[tid];

    deregister_thread(tid, my_trace->threads[tid_pos].handle);
    register_thread(tid, my_trace->threads[tid_pos].handle);

    dump_memory();

    //add_breakpoint(nt1_off, ntmap_1_callback);
    //add_breakpoint(nt3_off, ntread_1_callback);
    add_breakpoint(sysenter_off, sysenter_callback);
    if(strstr(mod_end, "0x0"))
    {
        add_breakpoint(addr_end + img_base, end_callback);
    }
    else
    {
        add_breakpoint(addr_end + find_lib(mod_end), end_callback);
    }

    set_ss(0x0);
    ss_callback(data);

}
*/

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
        //fwrite("# Flushed\n", strlen("# Flushed\n"), 1, file);
        fflush(my_trace->trace);
//        d_print("Flushed\n");
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
    DWORD oldProt;
    char bpt_char = '\xcc';

    VirtualProtectEx(my_trace->cpdi.hProcess, (void*)(my_trace->breakpoints[my_bpt_idx].addr), 0x1, PAGE_EXECUTE_READWRITE, &oldProt);

    ReadProcessMemory(my_trace->cpdi.hProcess, (void*)(my_trace->breakpoints[my_trace->bpt_count].addr), (void*)(&bpt_char), 0x1, 0x0);
    WriteProcessMemory(my_trace->cpdi.hProcess, (void*)(my_trace->breakpoints[my_bpt_idx].addr), &(my_trace->breakpoints[my_bpt_idx].saved_byte), 0x1, 0x0);

    VirtualProtectEx(my_trace->cpdi.hProcess, (void*)(my_trace->breakpoints[my_bpt_idx].addr), 0x1, oldProt, &oldProt);

    my_trace->breakpoints[my_bpt_idx].enabled = 0x0;
    return 0x0;
}

int del_breakpoint_handler(unsigned bp_idx, unsigned handler_idx)
{
    d_print("del_h\n");
    my_trace->breakpoints[bp_idx].handlers[handler_idx].enabled = 0x0;
    return 0x0;
}

int del_breakpoint(DWORD addr)
{
    d_print("del_b\n");
    d_print("Deleting breakpoint at: 0x%08x\n", addr);

    DWORD oldProt;
    char bpt_char = '\xcc';
    int i;
    int my_bpt_idx = -0x1;

    for(i = 0x0; i<my_trace->bpt_count; i++)
        if(my_trace->breakpoints[i].addr == addr)
        {
            my_bpt_idx = i;
            break;
        }

    if(my_bpt_idx == -0x1) return 0x1;

    del_breakpoint_idx(my_bpt_idx);
    my_trace->breakpoints[my_bpt_idx].enabled = 0x0;

    return 0x0;
}

int handle_breakpoint(DWORD addr, void* data)
{
    DEBUG_EVENT* de;

    de = (DEBUG_EVENT*)data;

    int i, j;
    int my_bpt_idx = -0x1;
    int handler_count;
    int our_bp = 0x0;

    for(i = 0x0; i<my_trace->bpt_count; i++)
    {
        d_print("Breakpoint hit @ 0x%08x\nChecking BP%d\n", addr, i);
        if(my_trace->breakpoints[i].addr == addr) our_bp = 1;
        if((my_trace->breakpoints[i].addr == addr) && (my_trace->breakpoints[i].enabled == 0x1))
        {
            my_bpt_idx = i;
            //handle
            handler_count = my_trace->breakpoints[my_bpt_idx].handler_count;
            d_print("Hit breakpoint @ 0x%08x, enumerating %d handlers\n", addr, handler_count);

            for(j = handler_count-1; j>-1; j--)
            {
                //printf("Enabled: %d\n", my_trace->breakpoints[my_bpt_idx].handlers[j].enabled);
                if(my_trace->breakpoints[my_bpt_idx].handlers[j].enabled)
                {
                    d_print("Executing last handler\n");
                    my_trace->breakpoints[my_bpt_idx].handlers[j].handler(data);
                    del_breakpoint_handler(my_bpt_idx, j);
                    my_trace->breakpoints[my_bpt_idx].handler_count--;
                    //dec_eip(de->dwThreadId);
                    goto handled;
                }
            }
            d_print("Removing\n");
            del_breakpoint(addr);
            handled:
            if(my_trace->breakpoints[my_bpt_idx].handler_count == 0x0) del_breakpoint(addr);
            dec_eip(de->dwThreadId);
        }
    }


    /* finished all handlers */
/*
    if((my_bpt_idx == -0x1) && (our_bp))
    {
        d_print("Removing\n");
        del_breakpoint(addr);
        dec_eip(de->dwThreadId);
    }
*/
//    d_print("5\n");
/*
    if(my_trace->breakpoints[my_bpt_idx].enabled == 0x0)
    {
        return -0x1;
    }

*/


    //enable ss
    //set_ss(de->dwThreadId);
    return 0x0;
}

int add_breakpoint_handler(unsigned idx, handler_routine handler)
{
    d_print("add_h\n");
    unsigned cur_idx;

    d_print("Adding handler @ 0x%08x\n", my_trace->breakpoints[idx].addr);
    cur_idx = my_trace->breakpoints[idx].handler_count;
    my_trace->breakpoints[idx].handlers[cur_idx].handler = handler;
    my_trace->breakpoints[idx].handlers[cur_idx].enabled = 0x1;
    my_trace->breakpoints[idx].handler_count++;

    return 0x0;
}

int add_breakpoint(DWORD addr, handler_routine handler)
{
    d_print("add_b\n");
    int i;
    int my_bpt_index;

    my_bpt_index = -1;

    /* do we have this addr? */
    for(i = 0x0; i<my_trace->bpt_count; i++)
        if(my_trace->breakpoints[i].addr == addr)
        {
            my_bpt_index = i;
        }

    /* if not, create */
    if(my_bpt_index == -1) 
    {
        d_print("Creating new bp\n");
        my_bpt_index = my_trace->bpt_count;
        my_trace->breakpoints[my_bpt_index].enabled = 0x0;
        my_trace->breakpoints[my_bpt_index].addr = addr;
        my_trace->bpt_count ++;
    }

    /* is it enabled? */
    if(my_trace->breakpoints[my_bpt_index].enabled == 0x0)
    {
        DWORD oldProt;
        char bpt_char = '\xcc';
        d_print("Adding breakpoint opcode @ 0x%08x\n", addr);

        VirtualProtectEx(my_trace->cpdi.hProcess, (void*)my_trace->breakpoints[my_bpt_index].addr, 0x1, PAGE_EXECUTE_READWRITE, &oldProt);

        ReadProcessMemory(my_trace->cpdi.hProcess, (void*)(my_trace->breakpoints[my_bpt_index].addr), (void*)(&my_trace->breakpoints[my_bpt_index].saved_byte), 0x1, 0x0);
        WriteProcessMemory(my_trace->cpdi.hProcess, (void*)my_trace->breakpoints[my_bpt_index].addr, &bpt_char, 0x1, 0x0);

        VirtualProtectEx(my_trace->cpdi.hProcess, (void*)my_trace->breakpoints[my_bpt_index].addr, 0x1, oldProt, 0x0);
        my_trace->breakpoints[my_bpt_index].enabled = 0x1;
    }

    /* add handler */
    add_breakpoint_handler(my_bpt_index, handler);

    return 0x0;
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

    if(DebugActiveProcess(myPID) != 0x0)
        d_print("Successfully attached to PID: 0x%x, handle: 0x%x\n", myPID);
    else 
        d_print("Attach failed\n");

}

DWORD find_lib(char* name)
{
    unsigned i;
    DWORD ret= 0x0;

    for(i=0x0; i<my_trace->lib_count; i++)
    {
        if(strstr(my_trace->libs[i].lib_name, name))
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

    send(my_trace->socket, line2, strlen(line2), 0x0);

    return 0x0;
}

int list_markers()
{
    d_print("Listing markers\n");
    unsigned i;
    char buffer2[MAX_LINE];

    my_trace->report_code = REPORT_INFO;

    strcpy(my_trace->report_buffer, "");

    for(i = 0x0; i< my_trace->marker_count; i++)
    {
        sprintf(buffer2, "%s: 0x%08x\n", my_trace->markers[i].id, my_trace->markers[i].real_offset);
        strcat(my_trace->report_buffer, buffer2);
        strcat(my_trace->report_buffer, "\n");
    }

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
        sprintf(buffer2, "0x%08x\n", my_trace->breakpoints[i].addr);
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

int read_context(DWORD tid, CONTEXT* ctx)
{
    HANDLE myHandle;
    DWORD tid_id;

    tid_id = my_trace->thread_map[tid];

    myHandle = my_trace->threads[tid_id].handle;
    ctx->ContextFlags = CONTEXT_FULL;
    if(GetThreadContext(myHandle, ctx) == 0x0)
    {
        d_print("Failed to get context, error: 0x%08x\n", GetLastError());
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
    
    write_memory(my_trace->procHandle, (void*)addr, (void*)&val, 0x4, &read);
    
    if(read == 0x4)
    {
        sprintf(buffer2, "DWORD written");
        strcpy(my_trace->report_buffer, buffer2);
    }
    else 
    {
        sprintf(buffer2, "Error");
        strcpy(my_trace->report_buffer, buffer2);
    }

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

    data_b = strtol(data, 0x0, 0x10);
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

    data_w = strtol(data, 0x0, 0x10);
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

int read_dword_q(DWORD addr)
{
    DWORD data;
    DWORD read;

    char buffer2[MAX_LINE];

    read_memory(my_trace->procHandle, (void*)addr, (void*)&data, 0x4, &read);
    return data;
}


int read_dword(DWORD addr)
{
    DWORD data;
    DWORD read;

    char buffer2[MAX_LINE];

    read_memory(my_trace->procHandle, (void*)addr, (void*)&data, 0x4, &read);
    
    if(read == 0x4)
    {
        sprintf(buffer2, "%08x", data);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else {
        sprintf(buffer2, "Error");
        strcpy(my_trace->report_buffer, buffer2);
    }

    return data;
}


int write_dword(DWORD addr, char* data)
{
    DWORD read;
    DWORD  data_d;

    char buffer2[MAX_LINE];

    data_d = strtol(data, 0x0, 0x10);
    write_memory(my_trace->procHandle, (void*)addr, (void*)&data_d, 0x4, &read);

    if(read == 0x4)
    {
        sprintf(buffer2, "DWORD written", data);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else {
        sprintf(buffer2, "Error");
        strcpy(my_trace->report_buffer, buffer2);
    }

    return 0x0;
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
        data_d = strtol(data, 0x0, 0x10);
        ctx.Eax = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "ESP"))
    {
        data_d = strtol(data, 0x0, 0x10);
        ctx.Esp = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "EIP"))
    {
        data_d = strtol(data, 0x0, 0x10);
        ctx.Eip = data_d;
        write_context(tid_id, &ctx);
        strcpy(my_trace->report_buffer, buffer2);
    }

    my_trace->report_code = REPORT_INFO;
    sprintf(buffer2, "%s written\n", reg);
    strcpy(my_trace->report_buffer, buffer2);

    return 0x0;
}

int read_register(DWORD tid_id, char* reg)
{
    CONTEXT ctx;
    char buffer2[MAX_LINE];

    my_trace->report_code = REPORT_INFO;
    
    read_context(tid_id, &ctx);

    if(!strcmp(reg, "EAX"))
    {
        sprintf(buffer2, "%08x", ctx.Eax);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "ESP"))
    {
        sprintf(buffer2, "%08x", ctx.Esp);
        strcpy(my_trace->report_buffer, buffer2);
    }
    else if(!strcmp(reg, "EIP"))
    {
        sprintf(buffer2, "%08x", ctx.Eip);
        strcpy(my_trace->report_buffer, buffer2);
    }

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
        data = read_dword_q(esp);
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

        //printf("process_last_event: 0x%08x\n", my_trace->last_event.dwDebugEventCode);
        switch(my_trace->last_event.dwDebugEventCode)
        {
            case CREATE_PROCESS_DEBUG_EVENT:
                /* this is not our responsibility, inform TracerController and wait for orders */
                /* get process info */
                my_trace->cpdi = my_trace->last_event.u.CreateProcessInfo;
                my_trace->procHandle = my_trace->cpdi.hProcess;

                /* register main thread (does this not belong to other event code? */
                register_thread(my_trace->last_event.dwThreadId, my_trace->last_event.u.CreateProcessInfo.hThread);

                /*TODO: inform TracerConrtoller*/

                d_print("Checking markers\n");
                unsigned ii;

                for(ii = 0x0; ii< my_trace->marker_count; ii++)
                {
                    if(strstr(my_trace->markers[ii].lib_name, "self"))
                    {
                        d_print("Should register marker at: 0x%08x:0x%08x\n", my_trace->cpdi.lpBaseOfImage, my_trace->markers[ii].offset);
                        my_trace->markers[ii].lib_offset = (OFFSET)my_trace->cpdi.lpBaseOfImage;
                        my_trace->markers[ii].real_offset = my_trace->markers[ii].lib_offset + my_trace->markers[ii].offset;
                        add_breakpoint(my_trace->markers[ii].real_offset, marker_handler);
                    }
                }

                /* reactions do not seem appropriate */

                d_print("Checking reactions\n");
                for(ii = 0x0; ii< my_trace->reaction_count; ii++)
                {
                    d_print("Comparing _%s_ and _%s_\n", my_trace->reactions[ii].lib_name, "self");
                    if(strstr(my_trace->reactions[ii].lib_name, "self"))
                    {
                        d_print("Should write reaction for %s\n", my_trace->reactions[ii].lib_name);
                        my_trace->reactions[ii].lib_offset = (OFFSET)my_trace->cpdi.lpBaseOfImage;
                        my_trace->reactions[ii].real_offset = my_trace->reactions[ii].lib_offset + my_trace->reactions[ii].offset;
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
                        /* this is not our responsibility, inform TracerController and wait for orders */
                        d_print("BP\n");
//                        ss_callback((void*)&my_trace->last_event);

                        unsigned i;
                        OFFSET bp_addr;

                        bp_addr = (OFFSET)my_trace->last_exception.ExceptionAddress;

                        for(i = 0x0; i< my_trace->reaction_count; i++)
                        {
                            d_print("Comparing 0x%08x and 0x%08x\n", bp_addr, my_trace->reactions[i].real_offset);
                            if(my_trace->reactions[i].real_offset == bp_addr)
                            {
                                /* this is our, automatic, we can handle */
                                handle_breakpoint((DWORD)my_trace->last_exception.ExceptionAddress, &my_trace->last_event);
                                return REPORT_CONTINUE;
                            }
                        }

                        for(i = 0x0; i< my_trace->marker_count; i++)
                        {
                            d_print("Comparing 0x%08x and 0x%08x\n", bp_addr, my_trace->markers[i].real_offset);
                            if(my_trace->markers[i].real_offset == bp_addr)
                            {
                                /* this is our, we need to handle & report */
                                handle_breakpoint((DWORD)my_trace->last_exception.ExceptionAddress, &my_trace->last_event);
                                return REPORT_BREAKPOINT;
                            }
                        }

                        /* handle breakspoints that are not our */
                        d_print("not our\n");
                        return REPORT_EXCEPTION_NH;
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

                /* writing activated markers */
                unsigned i;

                d_print("Checking markers\n");
                for(i = 0x0; i< my_trace->marker_count; i++)
                {
                    d_print("Comparing _%s_ and _%s_\n", my_trace->markers[i].lib_name, my_trace->libs[my_trace->lib_count-1].lib_name);
                    if(strlen(my_trace->markers[i].lib_name) == 0x0)
                    {
                        if(my_trace->markers[i].lib_offset == my_trace->libs[my_trace->lib_count-1].lib_offset)
                        {
                            d_print("Should write marker for 0x%08x:0x%08x\n", my_trace->markers[i].lib_offset, my_trace->markers[i].offset);
                            my_trace->markers[i].lib_offset = my_trace->libs[my_trace->lib_count-1].lib_offset;
                            my_trace->markers[i].real_offset = my_trace->markers[i].lib_offset + my_trace->markers[i].offset;
                            add_breakpoint(my_trace->markers[i].real_offset, marker_handler);
                        }
                    }
                    else if(!strcmp(my_trace->markers[i].lib_name, my_trace->libs[my_trace->lib_count-1].lib_name))
                    {
                        d_print("Should write marker for %s\n", my_trace->markers[i].lib_name);
                        my_trace->markers[i].lib_offset = my_trace->libs[my_trace->lib_count-1].lib_offset;
                        my_trace->markers[i].real_offset = my_trace->markers[i].lib_offset + my_trace->markers[i].offset;
                        add_breakpoint(my_trace->markers[i].real_offset, marker_handler);
                    }
                
                }

                d_print("Checking reactions\n");
                for(i = 0x0; i< my_trace->reaction_count; i++)
                {
                    d_print("Comparing _%s_ and _%s_\n", my_trace->reactions[i].lib_name, my_trace->libs[my_trace->lib_count-1].lib_name);
                    if(strlen(my_trace->reactions[i].lib_name) == 0x0)
                    {
                        if(my_trace->reactions[i].lib_offset == my_trace->libs[my_trace->lib_count-1].lib_offset)
                        {
                            d_print("Should write reaction for 0x%08x:0x%08x\n", my_trace->reactions[i].lib_offset, my_trace->reactions[i].offset);
                            my_trace->reactions[i].lib_offset = my_trace->libs[my_trace->lib_count-1].lib_offset;
                            my_trace->reactions[i].real_offset = my_trace->reactions[i].lib_offset + my_trace->reactions[i].offset;
                        }
                    }
                    else if(!strcmp(my_trace->reactions[i].lib_name, my_trace->libs[my_trace->lib_count-1].lib_name))
                    {
                        d_print("Should write reaction for %s\n", my_trace->reactions[i].lib_name);
                        my_trace->reactions[i].lib_offset = my_trace->libs[my_trace->lib_count-1].lib_offset;
                        my_trace->reactions[i].real_offset = my_trace->reactions[i].lib_offset + my_trace->reactions[i].offset;
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
                /*
                HandlerRoutine(0x0);
                */
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
            sprintf(buffer2, "%s\n", my_trace->last_marker->id);
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
            break;
    }
    strcpy(my_trace->report_buffer, buffer2);

    return 0x0;
}

int get_pending_events()
{
    int last_report;
    char buffer2[MAX_NAME];
    unsigned status;

    d_print("Continuing: PID: 0x%08x, TID: 0x%08x\n", my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId);
    if(my_trace->last_event.dwProcessId == 0x0)
    {
        ContinueDebugEvent(my_trace->pi.dwProcessId, my_trace->pi.dwThreadId, DBG_CONTINUE);
    }
    else
    {
        ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, DBG_CONTINUE);
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

        ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
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
        ContinueDebugEvent(my_trace->pi.dwProcessId, my_trace->pi.dwThreadId, status);
    }
    else
    {
        ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
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

        ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, status);
    }

    create_report(last_report);
    d_print("Report buffer: %s\n", my_trace->report_buffer);

    my_trace->report_code = last_report;

    return last_report;
}

int add_reaction(char* lib_name, OFFSET offset, unsigned id)
{
    
    if(lib_name[0] == '0' && lib_name[1] == 'x')
    {
        my_trace->reactions[my_trace->reaction_count].lib_offset = strtol(lib_name, 0x0, 0x10);
        strcpy(my_trace->reactions[my_trace->reaction_count].lib_name, "0x00000000");
    }
    else
    {
        strcpy(my_trace->reactions[my_trace->reaction_count].lib_name, lib_name);
    }
    
    my_trace->reactions[my_trace->reaction_count].id = id;
    my_trace->reactions[my_trace->reaction_count].offset = offset;
    d_print("New reaction: %s:0x%08x:%x\n", my_trace->reactions[my_trace->reaction_count].lib_name, my_trace->reactions[my_trace->reaction_count].offset, my_trace->reactions[my_trace->reaction_count].id);
    my_trace->reaction_count ++;

    return 0x0;
}

int resolve_region(OUT_ARGUMENT* selector, OUT_LOCATION* location)
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

int add_marker(char* lib_name, OFFSET offset, char* id)
{
    
    if(lib_name[0] == '0' && lib_name[1] == 'x')
    {
        my_trace->markers[my_trace->marker_count].lib_offset = strtol(lib_name, 0x0, 0x10);
        strcpy(my_trace->markers[my_trace->marker_count].lib_name, "0x00000000");
    }
    else
    {
        strcpy(my_trace->markers[my_trace->marker_count].lib_name, lib_name);
    }
    
    strcpy(my_trace->markers[my_trace->marker_count].id, id);
    my_trace->markers[my_trace->marker_count].id[0x2] = 0x0;
    my_trace->markers[my_trace->marker_count].offset = offset;
    my_trace->markers[my_trace->marker_count].active = 0x0;
    d_print("New marker: %s:0x%08x:%s\n", my_trace->markers[my_trace->marker_count].lib_name, my_trace->markers[my_trace->marker_count].offset, my_trace->markers[my_trace->marker_count].id);
    my_trace->marker_count ++;

    return 0x0;
}

int activate_marker(unsigned i)
{
    my_trace->markers[i].active = 0x1;
    return 0x0;
}

int parse_reaction(char* str)
{
    char* lib;
    char* off;
    unsigned id;

    lib = strtok(str, ":");
    off = strtok(0x0, ":");
    id = strtol(strtok(0x0, "+"), 0x0, 0x10);

    /* registering marker */
    add_reaction(lib, strtol(off, 0x0, 0x10), id);
}

int parse_region(char* str)
{
    DWORD  off;
    DWORD  size;
    char* label_off_location;
    char* label_size_location;
    char off_location;
    char size_location;

    d_print("Parsing region: %s\n", str);

    off = strtol(strtok(str, ":"), 0x0, 0x10);
    label_off_location = strtok(0x0, ":");
    size = strtol(strtok(0x0, ":"), 0x0, 0x10);
    label_size_location = strtok(0x0, "+");

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

    /* registering marker */
    add_region_sel(off, size, off_location, size_location);

    return 0x0;
}

int parse_marker(char* str)
{
    char* lib;
    char* off;
    char* id;

    lib = strtok(str, ":");
    off = strtok(0x0, ":");
    id = strtok(0x0, "+");

    /* registering marker */
    add_marker(lib, strtol(off, 0x0, 0x10), id);
}

int parse_reactions(char* str)
{
    char* current;
    char buf[MAX_NAME];

    current = str;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        parse_reaction(buf);
        current = strpbrk(current, "+");
        if(current)
            current++;
    }

    return 0x0;


}

int parse_markers(char* str)
{
    char* current;
    char buf[MAX_NAME];

    current = str;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        parse_marker(buf);
        current = strpbrk(current, "+");
        if(current)
            current++;
    }

    return 0x0;
}

int parse_regions(char* str)
{
    char* current;
    char buf[MAX_NAME];

    current = str;
    while((current != 0x0) && (strlen(current) > 0x0))
    {
        strcpy(buf, current);
        parse_region(buf);
        current = strpbrk(current, "+");
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
    char buffer2[MAX_NAME];

    my_trace->report_code = REPORT_NOTHING;
    memset(my_trace->report_buffer, 0x0, sizeof(my_trace->report_buffer));
    strcpy(my_trace->report_buffer, "");

    if(!strncmp(cmd, CMD_SET_NAME, 2))
    {
        strcpy(my_trace->in_sample_path, cmd+3);
        d_print("Sample path set to: %s\n", my_trace->in_sample_path);    
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
    else if(!strncmp(cmd, CMD_SET_LIMIT, 2))
    {
        my_trace->instr_limit = strtol(cmd+3, 0x0, 10);
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

        /* configure out paths */
        /* trace */ 
        my_trace->out_postfix = -1;
        reload_out_file();

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
    
        /* log */ 
        /* TODO:fix this
        strcpy(buffer2, "");
        sprintf(buffer2, "%s\\%s.log", my_trace->out_dir, my_trace->out_prefix);
        strcpy(my_trace->out_log, buffer2);
        my_trace->log = fopen(my_trace->out_log, "w");
        d_print("Log file: %s\n", my_trace->out_log);
        if(my_trace->log)
        {
            log = my_trace->log;
        }
        */


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
        unsigned id;

        mod = strtok(cmd, " ");
        id = strtol(strtok(0x0, " "), 0x0, 0x10);

        d_print("Enabling reaction no %d\n", id);
        disable_reaction(id);
        send_report();
    }
    else if(!strncmp(cmd, CMD_ENABLE_REACTION, 2))
    {
        char* mod;
        unsigned id;

        mod = strtok(cmd, " ");
        id = strtol(strtok(0x0, " "), 0x0, 0x10);

        d_print("Enabling reaction no %d\n", id);
        enable_reaction(id);
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_MARKER_1, 2))
    {
        char* mod;
        OFFSET off;

        mod = strtok(cmd, " ");
        mod = strtok(0x0, " ");
        off = strtol(strtok(0x0, " "), 0x0, 0x10);

        d_print("Start marker fixed at: %s:0x%08x\n", mod, off);
        send_report();
    }
    else if(!strncmp(cmd, CMD_SET_MARKER_2, 2))
    {
        char* mod;
        OFFSET off;

        mod = strtok(cmd, " ");
        mod = strtok(0x0, " ");
        off = strtol(strtok(0x0, " "), 0x0, 0x10);

        d_print("End marker fixed at: %s:0x%08x\n", mod, off);
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
    else if(!strncmp(cmd, CMD_LIST_MARKERS, 2))
    {
        list_markers();
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
        addr = strtol(strtok(0x0, " "), 0x0, 0x10);

        d_print("Reading addr: 0x%08x\n", addr);

        read_dword(addr);
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_WRITE_MEMORY, 2))
    {
        char* cmd_;
        DWORD addr;
        DWORD val;

        cmd_ = strtok(cmd, " ");
        addr = strtol(strtok(0x0, " "), 0x0, 0x10);
        val = strtol(strtok(0x0, " "), 0x0, 0x10);

        d_print("Writing addr: 0x%08x with 0x%08x\n", addr, val);

        write_dword(addr, val);
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_READ_REGISTER, 2))
    {
        unsigned tid_id;
        char* reg;

        reg = strtok(cmd, " ");
        tid_id = strtol(strtok(0x0, " "), 0x0, 0x10);
        reg = strtok(0x0, " ");

        read_register(tid_id, reg);
        send_report();   
    
    }
    else if(!strncmp(cmd, CMD_WRITE_REGISTER, 2))
    {
        unsigned tid_id;
        char* reg;
        char* data;

        reg = strtok(cmd, " ");
        tid_id = strtol(strtok(0x0, " "), 0x0, 0x10);
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
        tid_id = strtol(strtok(0x0, " "), 0x0, 0x10);
        count = strtol(strtok(0x0, " "), 0x0, 0x10);

        read_stack(tid_id, count);
        send_report(); 
    }
    else if(!strncmp(cmd, CMD_CONTINUE, 2))
    {
        unsigned report;
        unsigned status;

        strtok(cmd, " ");
        status = strtol(strtok(0x0, " "), 0x0, 0x10);

        continue_routine(INFINITE, status);
        send_report();   
    }
    else if(!strncmp(cmd, CMD_CONTINUE_TIME, 2))
    {
        unsigned report;
        DWORD time;
        unsigned status;

        strtok(cmd, " ");
        time = strtol(strtok(0x0, " "), 0x0, 0x10);
        status = strtol(strtok(0x0, " "), 0x0, 0x10);

        continue_routine(time, status);
        send_report();   
    }
    else if(!strncmp(cmd, CMD_AUTO_ST, 2))
    {
        char* markers_str;

        add_marker("0x0", (OFFSET)(my_trace->cpdi.lpStartAddress), "ST");
        my_trace->markers[my_trace->marker_count-1].real_offset = my_trace->markers[my_trace->marker_count-1].offset;
        add_breakpoint(my_trace->markers[my_trace->marker_count-1].real_offset, marker_handler);
        send_report();
        
    }
    else if(!strncmp(cmd, CMD_CONFIGURE_MARKERS, 2))
    {
        char* markers_str;

        strtok(cmd, " ");
        markers_str = strtok(0x0, " ");
        parse_markers(markers_str);
        send_report();
        
    }
    else if(!strncmp(cmd, CMD_CONFIGURE_REACTIONS, 2))
    {
        char* reactions_str;

        strtok(cmd, " ");
        reactions_str = strtok(0x0, " ");
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
    else if(!strncmp(cmd, CMD_ACTIVATE_MARKERS, 2))
    {
        unsigned i;
        for(i = 0x0; i<my_trace->marker_count; i++)
        {
            activate_marker(i);
        }
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
    my_trace->marker_count = 0x0;

    strcpy(my_trace->host, host);
    my_trace->port = port;

    memset(my_trace->thread_map, -1, sizeof(my_trace->thread_map));

    my_trace->mutex = CreateMutex(0x0, 0x0, 0x0);
    my_trace->eventLock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    my_trace->eventUnlock = CreateEvent(0x0, 0x0, 0x0, 0x0);

}

int try_resolve_marker(MARKER* m)
{

    return 0x0;
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

    d_print("Version 2.0\n");

    if(strlen(argv[1]) > MAX_NAME) return -1;
    if(strlen(argv[2]) > MAX_NAME) return -1;

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
    my_trace->routines[0x0] = &sysenter_callback;
    my_trace->routines[0x1] = &sysret_callback;
    my_trace->routines[0x2] = &sysret_refresh;
    my_trace->routines[0x100] = &sample_routine_1;
    my_trace->routines[0x101] = &zero_SF;
    my_trace->routines[0x102] = &set_ZF;
    my_trace->routines[0x201] = &update_region_1;

    /* Windows sockets */

    WSADATA wsa;
    SOCKET s;    
    struct sockaddr_in server;
    
    d_print("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        d_print("Failed. Error Code : %d",WSAGetLastError());
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
           // d_print("Got part: %s\n", cmd);
           // d_print("Finish: %s\n", cmd+cmd_len-6);
        }

        cmd[cmd_len-6] = 0x0;
//        d_print("Got cmd: %s\n", cmd);

        if(!strcmp(cmd, "quit")) 
            break;
        
        handle_cmd(cmd);
//        d_print("Handled\n");

    }

    return 0x0;
}


