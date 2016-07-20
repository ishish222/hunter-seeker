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

TRACE_CONFIG* my_trace;

int add_roadsign(void* data)
{
    ROADSIGN* sign;

    sign = (ROADSIGN*)data;

    /* add breakpoint & handler */

    return 0x0;
}

char process_fname[0x200];
char spawned = 0x0;
FILE* log;
char started = 0x0;
unsigned long long instr_count;
char instr_count_s[0x20];
unsigned long long instr_limit;
unsigned long long instr_dbg;
int full_log = 0x0;
DWORD scan_on;
unsigned scan_count;
READ_RECORD last_read_record;
WATCHED watched[WATCH_LIMIT];
HANDLE procHandle;
HANDLE file_handle;
DWORD addr_st, addr_end;
DWORD sysenter_esp;
DWORD sysenter_no;
char mod_st[MAX_NAME];
char mod_end[MAX_NAME];
HANDLE pipe;
FILE* file;
FILE* modifications;
char buffer[buf_size];
int index = 0;
HANDLE eventLock, eventUnlock;
HANDLE mutex;
PROCESS_INFORMATION pi;
STARTUPINFO si;
char path[0x200];
char cur_path[0x200];
int path_i;
char dumpPath[0x200];
char iniPath[0x200];
char modPath[0x200];
wchar_t filePath[0x200];
char instr[0x60];
int last_eip = 0x0;
DWORD last_tid = 0x0;
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
THREAD_ENTRY threads2[0x100000000];
DWORD thread_list[0x1000];
DWORD thread_count = 0x0;
DWORD lib_count = 0x0;
LIB_ENTRY libs[MAX_LIBS];
int add_breakpoint(DWORD addr, handler_routine handler);
DWORD find_lib(char* name);
BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);
SIZE_T dump_mem(FILE*, void*, SIZE_T);
HOOK hooks[MAX_HOOKS];
unsigned hook_count = 0x0;
bpt my_bpt[0x30];
int my_bpt_count = 0x0;
bool res = 0x0;
char* prefix;
DEBUG_EVENT de;
DWORD status = DBG_CONTINUE;
DWORD tids[0x100];
int tid_count = 0x0;
char* blacklist_lib[] = {"kernel32.dll", "ntdll.dll", "user32.dll"};
DWORD blacklist_addr[] = {};
char line2[MAX_LINE];

int d_print(const char* format, ...)
{
    va_list argptr;

    if(log)
    {
        va_start(argptr, format);
        vfprintf(log, format, argptr);
        va_end(argptr);
    }
    else
    {
        va_start(argptr, format);
        vfprintf(stdout, format, argptr);
        va_end(argptr);
    }

    return 0x0;
}


FILE* configure_file()
{
    FILE* f;
    
    sprintf(cur_path, "%s_%d.txt", path, path_i);
    d_print("Writing instructions to %s\n", cur_path);
    

    f = fopen(cur_path, "w+");
    if(f == 0x0)
    {
        d_print("Error opening file %s: 0x%08x\n", cur_path, GetLastError());
    }
    return f;
} 



int add_to_buffer(char* line)
{
    DWORD written;
    written = 0x0;
//    sprintf(line2, "%s\n", line);
    //written = fwrite(line2, strlen(line2), 1, file);
    written = fwrite(line, strlen(line), 1, file);
    fflush(file);
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
    d_print("Unsetting SS for 0x%08x\n", last_tid);
    unset_ss(last_tid);
    set_ss(0x0);
}

void read_memory(HANDLE handle, void* from, void* to, SIZE_T size, SIZE_T* read)
{
    ReadProcessMemory(handle, from, to, size, read);
}

int dec_eip(DWORD id)
{
    int i;
    HANDLE myHandle = (HANDLE)-0x1;
    myHandle = my_trace->threads[id].handle;
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
    d_print("Registering: TID 0x%08x, handle 0x%08x\n", tid, handle);
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

    d_print("Registering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", tid, my_trace->threads[tid_pos].handle, my_trace->thread_count);

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
    d_print("Deregistering: TID 0x%08x, handle 0x%08x\n", tid, handle);
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
    d_print("Deregistering: TID 0x%08x, handle 0x%08x\n", tid, handle);

    d_print("Deregistering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", tid, my_trace->threads[tid_pos].handle, my_trace->thread_count);

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

    d_print("Deregistering TID: 0x%08x with handle: 0x%08x\n", tid, my_trace->threads[tid_pos].handle);
    sprintf(line, "DT,0x%08x\n", tid);
 
    add_to_buffer(line);
    return;
}


void register_lib(LOAD_DLL_DEBUG_INFO info)
{
    char path[MAX_NAME];
    char line[MAX_LINE];
    unsigned size = 0;

    d_print("Handle: %p\n", info.hFile);
 
#ifdef LIB_VER_W7
    GetFinalPathNameByHandleA(info.hFile, libs[lib_count].lib_name, MAX_NAME, VOLUME_NAME_NONE);
#endif
#ifdef LIB_VER_WXP
    strcpy(libs[lib_count].lib_name, "UNKNOWN");
#endif
    d_print("Name pointer: %p, len: 0x%08x\n", libs[lib_count].lib_name, strlen(libs[lib_count].lib_name));

    libs[lib_count].lib_addr = (DWORD)info.lpBaseOfDll;
//    size = GetFileSize(info.hFile, 0x0);
/*
    d_print("RL,0x%08x,%s,0x%08x\n", libs[lib_count].lib_addr, libs[lib_count].lib_name, size);
    sprintf(line, "RL,0x%08x,%s,0x%08x\n", libs[lib_count].lib_addr, libs[lib_count].lib_name, size);
*/
    d_print("RL,0x%08x,%s\n", libs[lib_count].lib_addr, libs[lib_count].lib_name);
    sprintf(line, "RL,0x%08x,%s\n", libs[lib_count].lib_addr, libs[lib_count].lib_name);

    libs[lib_count].alive = 0x1;
    lib_count++;

    add_to_buffer(line);
    return;
}

void deregister_lib(DWORD i)
{
    char line[MAX_LINE];
    sprintf(line, "DL,0x%08x,%s\n", libs[i].lib_addr, libs[i].lib_name);
    libs[i].alive = 0x0;

    add_to_buffer(line);
}

void deregister_lib(UNLOAD_DLL_DEBUG_INFO info)
{
    char line[MAX_LINE];
    unsigned i;

    for (i = 0x0; i< lib_count; i++)
    {
        if(libs[i].lib_addr == (DWORD)info.lpBaseOfDll) break;
    }

    sprintf(line, "DL,0x%08x,%s\n", libs[i].lib_addr, libs[i].lib_name);

    libs[i].alive = 0x0;
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


    d_print("Thread: %08x\n", de->dwThreadId);

    int i = 0x0;

    for(i = 0x0; i<my_trace->thread_count; i++)
    {
        if(my_trace->threads[thread_list[i]].alive)
        d_print("TID: 0x%08x, handle: 0x%08x\n", thread_list[i], my_trace->threads[thread_list[i]].handle);

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
    read_memory(cpdi.hProcess, (void*)(tmp), (void*)&buffer_addr, 0x4, &read);

    tmp = ctx.Esp + (0x7 * 0x4);
    read_memory(cpdi.hProcess, (void*)(tmp), (void*)&size_addr, 0x4, &read);

    d_print("off: 0x%08x, size: 0x%08x\n", buffer_addr, size_addr);

    add_breakpoint(nt2_off, ntmap_2_callback);
//    CloseHandle(myHandle);
}

void ntmap_2_callback(void* data)
{
    char line[MAX_LINE];
    d_print("ntmap_2\n");

    DWORD offset, size, size_wrote, read;

    read_memory(cpdi.hProcess, (void*)(buffer_addr), (void*)&offset, 0x4, &read);
    read_memory(cpdi.hProcess, (void*)(size_addr), (void*)&size, 0x4, &read);

    d_print("Block 0x%08x - 0x%08x, 0x%08x bytes\n", offset, offset + size, size);

    size_wrote = dump_mem(modifications, (void*)offset, size);
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
//    d_print("ESP: %p\n", ctx.Esp);
    sysenter_no = ctx.Eax;
    sysenter_esp = ctx.Esp;

//    d_print("Deregister thread @ SYSENTER: %08x\n", tid);
    deregister_thread(tid, my_trace->threads[tid_pos].handle);
    add_breakpoint(sysret_off, sysret_callback);
    add_breakpoint(sysret_off, sysret_refresh);
    set_ss(0x0);
}

void sysret_callback(void* data)
{
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
    size = dump_mem(modifications, (void*)sysenter_esp, 0x50);
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
            syscall_out_args_dump_list[i].off = last_location.off;
    */

    d_print("[[Syscall: 0x%08x @ 0x%08x]]\n", sysenter_no, ctx.Eip);
    for(i = 0x0; i<MAX_SYSCALL_OUT_ARGS; i++)
    {
        d_print("Arg no: 0x%02x\n", i);
        if(syscall_out_args[sysenter_no][i].off_location == last_arg.off_location) 
        {
            d_print("Last arg, finishing\n");
            break;
        }
        if(syscall_out_args[sysenter_no][i].eax_val_success != STATUS_ANY)
        {
            d_print("Arg not any\n");
            if(syscall_out_args[sysenter_no][i].eax_val_success != ctx.Eax) 
            {
                d_print("Wrong EAX\n");
                continue;
            }
        }
        if(1)
        {
            d_print("Locating buffer\n");
            arg_val = 0x0;
            arg_addr = 0x0;
            off = 0x0;
            size = 0x0;

            d_print("ESP: 0x%08x\n", sysenter_esp);

            // decide offset
            switch(syscall_out_args[sysenter_no][i].off_location)
            {
                case LOCATION_CONST:
                    syscall_out_args_dump_list[i].off = syscall_out_args[sysenter_no][i].off;
                    arg_val = syscall_out_args_dump_list[i].off;
                    d_print("Arg off: 0x%08x\n", arg_val);
                    break;
                case LOCATION_MEM:
                    arg_addr = syscall_out_args[sysenter_no][i].off;
                    read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    d_print("Arg off: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].off * 0x4;
                    arg_val = arg_addr;
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    d_print("Arg off: 0x%08x\n", arg_val);
                    if(arg_val == 0x0) break;
                    syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_ADDR_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].off * 0x4;
                    read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    d_print("Arg off: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_ADDR_ADDR_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].off * 0x4;
                    read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    arg_addr = arg_val;
                    read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    d_print("Arg off: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].off = arg_val;
                    break;
            }

            if(arg_val == 0x0) break;
            off = arg_val;
            arg_val = 0x0;
            arg_addr = 0x0;
            d_print("Locating size\n");

            // decide size
            switch(syscall_out_args[sysenter_no][i].size_location)
            {
                case LOCATION_CONST:
                    syscall_out_args_dump_list[i].size = syscall_out_args[sysenter_no][i].size;
                    arg_val = syscall_out_args_dump_list[i].size;
                    d_print("Arg size: 0x%08x\n", arg_val);
                    break;
                case LOCATION_MEM:
                    arg_addr = syscall_out_args[sysenter_no][i].size;
                    read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    d_print("Arg size: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].size = arg_val;
                    break;
                case LOCATION_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].size * 0x4;
                    read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    d_print("Arg size: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].size = arg_val;
                    break;
                case LOCATION_ADDR_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].size * 0x4;
                    read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    arg_addr = arg_val;
                    read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    d_print("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    d_print("Arg size: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].size = arg_val;
                    break;
            }

            size = arg_val;

            size_wrote = dump_mem(modifications, (void*)off, size);
            if(size_wrote == size)
            {
                d_print("[Out argument: 0x%08x, size: 0x%08x]\n", off, size);
                sprintf(line, "UP,0x%08x,0x%08x\n", off, size);
                add_to_buffer(line);
            }
        }
    }


    // special cases

    if(sysenter_no == 0x42) // CreateFile
    {
        // first one
        if(file_handle == 0x0) 
        {
            arg_addr = sysenter_esp + 0x8;
            arg_addr += syscall_out_args[sysenter_no][i].size * 0x4;
            read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
            arg_addr = arg_val;
            read_memory(cpdi.hProcess, (void*)arg_addr, (void*)&file_handle, 0x4, &read);

            d_print("Hunting for reads from: 0x%08x\n", file_handle);
        }
    }

    if(sysenter_no == 0x111) // ReadFile
    {
        if(file_handle == (HANDLE)syscall_out_args_dump_list[0].off)
        {
            // our file, enter taint
            sprintf(line, "RN,0x%08x,0x%08x\n", syscall_out_args_dump_list[3].off, syscall_out_args_dump_list[3].size);
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
    

    add_breakpoint(sysenter_off, sysenter_callback);
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

    ReadProcessMemory(cpdi.hProcess, (void*)(esp), (void*)&val, 0x4, &read);
    
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

                    thread_list[my_trace->thread_count] = te32.th32ThreadID;
                    my_trace->thread_count ++;
                    d_print("Registering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", te32.th32ThreadID, my_trace->threads[my_trace->thread_count].handle, my_trace->thread_count);
 
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
            cur_tid = thread_list[i];
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
    int i;
    DWORD cur_tid;
    char line[MAX_LINE];

    if(tid == 0x0)
    {
        for(i = 0x0; i< my_trace->thread_count; i++)
        {
            cur_tid = thread_list[i];
            unset_ss(cur_tid);
        }
    }
    else
    {
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
    int i;
    DWORD cur_tid;

    if(tid == 0x0)
    {
        for(i = 0x0; i< my_trace->thread_count; i++)
        {
            cur_tid = thread_list[i];
            set_ss(cur_tid);
        }
    }
    else
    {
        HANDLE tHandle;
        ctx.ContextFlags = CONTEXT_CONTROL;
        tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);
        //tHandle = my_trace->threads[tid_pos].handle;
        if(!tHandle) return -1;

        GetThreadContext(tHandle, &ctx);
        ctx.EFlags |= SS_FLAGS;
//        sprintf(line, "# Enabling trap for 0x%08x\n", tid);
//        add_to_buffer(line);
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
            d_print("Instr_no: %lld\n", instr_count);
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
    tid_pos = my_trace->thread_map[tid];

    if(!started) return;

    WaitForSingleObject(mutex, INFINITE);

    eip = (DWORD)(de->u.Exception.ExceptionRecord.ExceptionAddress);
    last_eip = eip;
    tid = de->dwThreadId;
    last_tid = tid;

    //sprintf(line, "0x%x 0x%08x", de->dwThreadId, eip);

    //printf("0x%08x\n", eip);
/*
    if(scan_on)
    {
        //printf("Scan is on\n");
        if(scan_count < REG_DEBUG_INTERVAL)
        {
            register_thread_debug(tid, my_trace->threads[tid_pos].handle);
            scan_count ++;
        }
        else
        {
            scan_on = 0x0;
            scan_count = 0x0;
            d_print("Scan end @ %lld\n", instr_count);
            fwrite("# Scan end\n", strlen("# Scan end\n"), 1, file);
        }
    }
    else
    {
        if(REG_DEBUG)
        {
            if(!(instr_count % REG_DEBUG_INTERVAL))
            {
                d_print("Debug interval\n");
                register_thread_debug(tid, my_trace->threads[tid_pos].handle);
            }
        }
    }
*/
    //check_debug(eip, instr_count, de->dwThreadId);


    if(1)
    {
        if(register_thread_debug(tid, my_trace->threads[tid_pos].handle) <= 0x0)
        {
            d_print("Error writing to file: 0x%x\n", GetLastError());
            exit(1);
        }
    }

    if(!(instr_count % INSTRUCTION_INTERVAL) && instr_count>0x0)
    {
        fclose(file);
        path_i++;
        file = configure_file();
    }

/*
    if(!(instr_count % INSTRUCTION_SMALL_INTERVAL) && instr_count>0x0)
    {
        d_print("%d\n", instr_count);   
    }
*/
    instr_count++;
    sprintf(line, "0x%x 0x%08x %lld\n", tid, eip, instr_count);
    add_to_buffer(line);

    if(instr_limit)
    {
        if(instr_count >= instr_limit)
        {
            HandlerRoutine(0x0);
        }
    }

//    set_ss(de->dwThreadId);

    ReleaseMutex(mutex);

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
        ReadProcessMemory(cpdi.hProcess, (void*)(from+i), (void*)mem_buf, buf_size, &read);
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
        ReadProcessMemory(cpdi.hProcess, (void*)(from+i), (void*)mem_buf, part, &read);
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
    FILE* f;

    d_print("dumping mem start\n");
#ifdef MEM_DUMP
    f = fopen(dumpPath, "wb");

    MEMORY_BASIC_INFORMATION mbi;
    
    for(addr = 0x0; addr<0xffffffff; addr += mbi.RegionSize)
    {
        VirtualQueryEx(cpdi.hProcess, (void*)addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
        if(GetLastError() == ERROR_INVALID_PARAMETER)
            break;
        d_print("Block 0x%08x - 0x%08x, 0x%08x bytes\n", mbi.BaseAddress, mbi.RegionSize + mbi.BaseAddress, mbi.RegionSize);
//        if(mbi.State == MEM_COMMIT && (mbi.Type == MEM_MAPPED || mbi.Type == MEM_PRIVATE) && page_accessible(mbi))
        if(mbi.State == MEM_COMMIT && page_accessible(mbi))
        {
            read = dump_mem(f, mbi.BaseAddress, mbi.RegionSize);
        }
        else
        {
            read = dump_zeros(f, mbi.RegionSize);
        }

    }

    fclose(f);
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
    for(i=0x0; i<lib_count; i++)
    {
        if(libs[i].alive == 0x1) 
        {
            deregister_lib(i);
        }
    }


    WritePrivateProfileString("general", "instruction count", itoa(instr_count, instr_count_s, 10), iniPath);

    SetEvent(eventLock);
    WaitForSingleObject(eventUnlock, INFINITE);
    exit(1);

}

DWORD WINAPI writer(LPVOID lpParam)
{
    while(1)
    {
        WaitForSingleObject(eventLock, INFINITE);
        ResetEvent(eventLock);
        fwrite(buffer, index, 1, file);
        fflush(file);
        //fwrite("# Flushed\n", strlen("# Flushed\n"), 1, file);
        fflush(file);
//        d_print("Flushed\n");
        SetEvent(eventUnlock);
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
/*
    VirtualProtectEx(cpdi.hProcess, (void*)(my_bpt[my_bpt_idx].addr), 0x1, PAGE_EXECUTE_READWRITE, &oldProt);

    ReadProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_count].addr), (void*)(&bpt_char), 0x1, 0x0);
    WriteProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_idx].addr), &(my_bpt[my_bpt_idx].saved_byte), 0x1, 0x0);

    VirtualProtectEx(cpdi.hProcess, (void*)(my_bpt[my_bpt_idx].addr), 0x1, oldProt, &oldProt);
*/
    my_bpt[my_bpt_idx].enabled = 0x0;
    return 0x0;
}

int del_breakpoint_handler(unsigned bp_idx, unsigned handler_idx)
{
    d_print("del_h\n");
    my_bpt[bp_idx].handlers[handler_idx].enabled = 0x0;
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

    for(i = 0x0; i<my_bpt_count; i++)
        if(my_bpt[i].addr == addr)
        {
            my_bpt_idx = i;
            break;
        }

    if(my_bpt_idx == -0x1) return 0x1;

    VirtualProtectEx(cpdi.hProcess, (void*)(my_bpt[my_bpt_idx].addr), 0x1, PAGE_EXECUTE_READWRITE, &oldProt);

    ReadProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_count].addr), (void*)(&bpt_char), 0x1, 0x0);
    WriteProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_idx].addr), &(my_bpt[my_bpt_idx].saved_byte), 0x1, 0x0);

    VirtualProtectEx(cpdi.hProcess, (void*)(my_bpt[my_bpt_idx].addr), 0x1, oldProt, &oldProt);
    my_bpt[my_bpt_idx].enabled = 0x0;

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

    d_print("Hnadling, my count: %d\n", my_bpt_count);
    for(i = 0x0; i<my_bpt_count; i++)
    {
        d_print("checking: %d\n", i);
        if(my_bpt[i].addr == addr) our_bp = 1;
        if((my_bpt[i].addr == addr) && (my_bpt[i].enabled == 0x1))
        {
            my_bpt_idx = i;
            //handle
            handler_count = my_bpt[my_bpt_idx].handler_count;
            d_print("Hit breakpoint @ 0x%08x, enumerating %d handlers\n", addr, handler_count);

            for(j = handler_count-1; j>-1; j--)
            {
                //printf("Enabled: %d\n", my_bpt[my_bpt_idx].handlers[j].enabled);
                if(my_bpt[my_bpt_idx].handlers[j].enabled)
                {
                    d_print("Executing last handler\n");
                    my_bpt[my_bpt_idx].handlers[j].handler(data);
                    del_breakpoint_handler(my_bpt_idx, j);
                    my_bpt[my_bpt_idx].handler_count--;
                    //dec_eip(de->dwThreadId);
                    goto handled;
                }
            }
            d_print("Removing\n");
            del_breakpoint(addr);
            handled:
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
    d_print("5\n");
/*
    if(my_bpt[my_bpt_idx].enabled == 0x0)
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

    d_print("Adding handler @ 0x%08x\n", my_bpt[idx].addr);
    cur_idx = my_bpt[idx].handler_count;
    my_bpt[idx].handlers[cur_idx].handler = handler;
    my_bpt[idx].handlers[cur_idx].enabled = 0x1;
    my_bpt[idx].handler_count++;

    return 0x0;
}

int add_breakpoint(DWORD addr, handler_routine handler)
{
    d_print("add_b\n");
    int i;
    int my_bpt_index;

    my_bpt_index = -1;

    /* do we have this addr? */
    for(i = 0x0; i<my_bpt_count; i++)
        if(my_bpt[i].addr == addr)
        {
            my_bpt_index = i;
        }

    /* if not, create */
    if(my_bpt_index == -1) 
    {
        d_print("Creating new bp\n");
        my_bpt_index = my_bpt_count;
        my_bpt[my_bpt_index].enabled = 0x0;
        my_bpt[my_bpt_index].addr = addr;
        my_bpt_count ++;
    }

    /* is it enabled? */
    if(my_bpt[my_bpt_index].enabled == 0x0)
    {
        DWORD oldProt;
        char bpt_char = '\xcc';
        d_print("Adding breakpoint opcode @ 0x%08x\n", addr);

        VirtualProtectEx(cpdi.hProcess, (void*)my_bpt[my_bpt_index].addr, 0x1, PAGE_EXECUTE_READWRITE, &oldProt);

        ReadProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_index].addr), (void*)(&my_bpt[my_bpt_index].saved_byte), 0x1, 0x0);
        WriteProcessMemory(cpdi.hProcess, (void*)my_bpt[my_bpt_index].addr, &bpt_char, 0x1, 0x0);

        VirtualProtectEx(cpdi.hProcess, (void*)my_bpt[my_bpt_index].addr, 0x1, oldProt, 0x0);
        my_bpt[my_bpt_index].enabled = 0x1;
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

    for(i=0x0; i<lib_count; i++)
    {
        if(strstr(libs[i].lib_name, name))
        {
            ret = libs[i].lib_addr;
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
        printf("Line: %s\n", desc_line);
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
    char line2[MAX_LINE];

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

    printf("Sending: %s, %d bytes\n", line2, strlen(line2));
 
    send(my_trace->socket, line2, strlen(line2), 0x0);


    return 0x0;
}

int list_tebs()
{
    printf("Listings TEBs\n");
    unsigned i;
    char buffer2[MAX_LINE];

    my_trace->report_code = REPORT_INFO;

    printf("Currently have %d threads\n", my_trace->thread_count);

    for(i = 0x0; i< my_trace->thread_count; i++)
    {
        serialize_thread(&my_trace->threads[i], buffer2);
        strcat(my_trace->report_buffer, buffer2);
    //    strcat(my_trace->report_buffer, "\n");
    }
    printf("Reporting: %s\n", my_trace->report_buffer);

    return 0x0;
}

int read_context(DWORD tid_id, CONTEXT* ctx)
{
    HANDLE myHandle;

    myHandle = my_trace->threads[tid_id].handle;
    ctx->ContextFlags = CONTEXT_CONTROL;
    if(GetThreadContext(myHandle, ctx) == 0x0)
    {
        d_print("Failed to get context, error: 0x%08x\n", GetLastError());
    }

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

    return 0x0;
}

int process_last_event()
{
        switch(my_trace->last_event.dwDebugEventCode)
        {
            case CREATE_PROCESS_DEBUG_EVENT:
                /* this is not our responsibility, inform TracerController and wait for orders */
                /* get process info */
                my_trace->cpdi = my_trace->last_event.u.CreateProcessInfo;

                /* register main thread (does this not belong to other event code? */
                register_thread(my_trace->last_event.dwThreadId, my_trace->last_event.u.CreateProcessInfo.hThread);

                /*TODO: inform TracerConrtoller*/

/*
                //configure breakpoints
                if(strstr(mod_st, "0x0"))
                {
                    printf("Main module: 0x%08x\n", (DWORD)my_trace->last_event.u.CreateProcessInfo.lpBaseOfImage);
                    add_breakpoint(addr_st + (DWORD)my_trace->last_event.u.CreateProcessInfo.lpBaseOfImage, bp_callback);
                }

                if(strstr(mod_end, "0x0"))
                {
                    printf("Main module: 0x%08x\n", (DWORD)my_trace->last_event.u.CreateProcessInfo.lpBaseOfImage);
                    img_base = (DWORD)my_trace->last_event.u.CreateProcessInfo.lpBaseOfImage;
                }
                //unset_ss(0x0);
*/
                return REPORT_PROCESS_CREATED;
                break;

            case EXCEPTION_DEBUG_EVENT:
                status = DBG_EXCEPTION_NOT_HANDLED;
                my_trace->last_exception = my_trace->last_event.u.Exception.ExceptionRecord;

                if(my_trace->last_exception.ExceptionCode != EXCEPTION_SINGLE_STEP)
                {
                    d_print("Exception: 0x%08x\n", my_trace->last_exception.ExceptionCode);
                    d_print("at: 0x%08x\n", my_trace->last_exception.ExceptionAddress);
                    d_print("First chance: 0x%08x\n", my_trace->last_event.u.Exception.dwFirstChance);
                }

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
                        ss_callback((void*)&my_trace->last_event);
                        /*
                        handle_breakpoint((DWORD)er.ExceptionAddress, &my_trace->last_event);
                        status = DBG_CONTINUE;
                        */
                        /* add detection of specific breakpoints */
                        return REPORT_BREAKPOINT;
                        break;
                    default:
                        /* this is not our responsibility, inform TracerController and wait for orders */
                        register_exception(my_trace->last_event.dwThreadId, my_trace->last_exception);
                        ss_callback((void*)&my_trace->last_event);
                        set_ss(my_trace->last_event.dwThreadId);
                        //my_trace->last_event.u.Exception.dwFirstChance = 0x0;
                        //del_breakpoint(sysenter_off);
                        //unset_ss(my_trace->last_event.dwThreadId);
                        //full_log = 1;
                        //status = DBG_EXCEPTION_NOT_HANDLED;
                        return REPORT_EXCEPTION;
                        break;
                }
                break;

            case CREATE_THREAD_DEBUG_EVENT: 
                /* we are authorized to handle this */

                register_thread(my_trace->last_event.dwThreadId, my_trace->last_event.u.CreateThread.hThread);
                set_ss(my_trace->last_event.dwThreadId);
                my_trace->last_win_status = DBG_CONTINUE;
                return REPORT_CONTINUE;
                break;

            case LOAD_DLL_DEBUG_EVENT:
                /* we are authorized to handle this */

                d_print("Handle: 0x%08x\n", my_trace->last_event.u.LoadDll.hFile);
                register_lib(my_trace->last_event.u.LoadDll);
                d_print("Addr: 0x%08x\n", libs[lib_count-1].lib_addr);

#ifdef LIB_VER_W7
                GetFinalPathNameByHandleA(my_trace->last_event.u.LoadDll.hFile, libs[lib_count].lib_name, MAX_NAME, VOLUME_NAME_NONE);
#endif

                d_print("Testing for start lib, compairng: %s & %s\n", libs[lib_count].lib_name, mod_st);
                if(strstr(libs[lib_count].lib_name, mod_st))
                {
                    d_print("Match!\n");
                    add_breakpoint(addr_st + find_lib(mod_st), bp_callback);
                }
//                if(strstr(libs[lib_count-1].lib_name, mod_end))
//                {
//                }

                /* handle hooks */

                unsigned j;
                for(j = 0x0; j<hook_count; j++)
                {
#ifdef LIB_VER_W7
                    d_print("%s - %s\n",  libs[lib_count-1].lib_name, hooks[j].libname);
                    if(strstr(libs[lib_count-1].lib_name, hooks[j].libname))
#endif
#ifdef LIB_VER_WXP
                    /* todo */
#endif
                    {
                        add_breakpoint((DWORD)(hooks[j].offset + my_trace->last_event.u.LoadDll.lpBaseOfDll), hooks[j].handler);
                        d_print("Adding hook: %d\n", j);
                    }
                }

#ifdef LIB_VER_W7
                if(strstr(libs[lib_count-1].lib_name, "ntdll.dll"))
#endif
#ifdef LIB_VER_WXP
                if(libs[lib_count-1].lib_addr == NTDLL_OFF)
#endif
                {
                    d_print("ntdll loaded\n");
#ifdef LIB_VER_W7
                    nt1_off = find_lib("ntdll.dll") + NTMAPVIEWOFSECTION_1;
                    nt2_off = find_lib("ntdll.dll") + NTMAPVIEWOFSECTION_2;
                    nt3_off = find_lib("ntdll.dll") + NTREADFILE_OFF_1;
                    nt4_off = find_lib("ntdll.dll") + NTREADFILE_OFF_2;
                    sysenter_off = find_lib("ntdll.dll") + NTSYSENTER_OFF;
                    sysret_off = find_lib("ntdll.dll") + NTSYSRET_OFF;
#endif
#ifdef LIB_VER_WXP
                    nt1_off = NTDLL_OFF + NTMAPVIEWOFSECTION_1;
                    nt2_off = NTDLL_OFF + NTMAPVIEWOFSECTION_2;
                    nt3_off = NTDLL_OFF + NTREADFILE_OFF_1;
                    nt4_off = NTDLL_OFF + NTREADFILE_OFF_2;
                    sysenter_off = NTDLL_OFF + NTSYSENTER_OFF;
                    sysret_off = NTDLL_OFF + NTSYSRET_OFF;
#endif
                }
                my_trace->last_win_status = DBG_CONTINUE;
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

int get_pending_events()
{
    int last_report;

    while(1)
    {

        status = DBG_CONTINUE;
        WaitForDebugEvent(&my_trace->last_event, 0x0);

        if(my_trace->last_event.dwDebugEventCode == 0x0) return REPORT_NOTHING;

        d_print("dwDebugEventCode: 0x%08x\n", my_trace->last_event.dwDebugEventCode);

        last_report = process_last_event();
        if(last_report != REPORT_CONTINUE) break;

        ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, my_trace->last_win_status);
    }

    return last_report;
}

/* TODO: continuing for some time */
int continue_routine(DWORD time)
{
    int last_report;

    while(1)
    {

        status = DBG_CONTINUE;
        WaitForDebugEvent(&my_trace->last_event, time);

        d_print("dwDebugEventCode: 0x%08x\n", my_trace->last_event.dwDebugEventCode);

        last_report = process_last_event();
        if(last_report != REPORT_CONTINUE) break;

        ContinueDebugEvent(my_trace->last_event.dwProcessId, my_trace->last_event.dwThreadId, my_trace->last_win_status);
    }

    return last_report;
}

int handle_cmd(char* cmd)
{
    printf("%s\n", cmd);

    my_trace->report_code = REPORT_NOTHING;
    strcpy(my_trace->report_buffer, "");

    if(cmd[0x0] == 'S' && cmd[0x1] == 'N')
    {
        strcpy(my_trace->sample_path, cmd+3);
        printf("Sample path set to: %s\n", my_trace->sample_path);    
    }
    else if(cmd[0x0] == 'S' && cmd[0x1] == 'D')
    {
        strcpy(my_trace->research_dir, cmd+3);
        printf("Research dir set to: %s\n", my_trace->research_dir);
    }
    else if(cmd[0x0] == 'M' && cmd[0x1] == '1')
    {
        char* mod;
        OFFSET off;

        mod = strtok(cmd, " ");
        mod = strtok(0x0, " ");
        off = strtol(strtok(0x0, " "), 0x0, 0x10);

        printf("Start marker fixed at: %s:0x%08x\n", mod, off);
    }
    else if(cmd[0x0] == 'M' && cmd[0x1] == '2')
    {
        char* mod;
        OFFSET off;

        mod = strtok(cmd, " ");
        mod = strtok(0x0, " ");
        off = strtol(strtok(0x0, " "), 0x0, 0x10);

        printf("End marker fixed at: %s:0x%08x\n", mod, off);
    }
    else if(cmd[0x0] == 's' && cmd[0x1] == 'd')
    {
        unsigned report;

        strcpy(my_trace->process_fname, my_trace->research_dir);
        strcat(my_trace->process_fname, "\\");
        strcat(my_trace->process_fname, my_trace->sample_path);

        printf("Starting debugging: %s\n", my_trace->process_fname);

        start_trace_fname();

        my_trace->status = STATUS_CONFIGURED; /* move to other */

        printf("Started debugging, got PID: %d\n", my_trace->PID);
        report = get_pending_events();
        printf("Sending report: 0x%08x\n", report);
    }
    else if(cmd[0x0] == 'l' && cmd[0x1] == 't')
    {
        if(my_trace->status != STATUS_CONFIGURED)
        {
            printf("Trace is not prepared\n");
            goto ret;
        }

        list_tebs();
        send_report();   
    
    }
    else if(cmd[0x0] == 'R' && cmd[0x1] == 'R')
    {
        unsigned tid_id;
        char* reg;

        if(my_trace->status != STATUS_CONFIGURED)
        {
            printf("Trace is not prepared\n");
            goto ret;
        }

        reg = strtok(cmd, " ");
        tid_id = strtol(strtok(0x0, " "), 0x0, 0x10);
        reg = strtok(0x0, " ");

        read_register(tid_id, reg);
        send_report();   
    
    }
    else if(cmd[0x0] == 'c' && cmd[0x1] == 'n')
    {
        unsigned report;
        if(my_trace->status != STATUS_CONFIGURED)
        {
            printf("Trace is not prepared\n");
            goto ret;
        }
        printf("Continuing\n");
        report = continue_routine(INFINITE);
        printf("Sending report: 0x%08x\n", report);
        /* TODO: send report */
//        send_report();   
    }
    else if(cmd[0x0] == 'c' && cmd[0x1] == 'N')
    {
        unsigned report;
        DWORD time;

        strtok(cmd, " ");
        time = strtol(strtok(0x0, " "), 0x0, 0x10);

        if(my_trace->status != STATUS_CONFIGURED)
        {
            printf("Trace is not prepared\n");
            goto ret;
        }
        printf("Continuing\n");
        report = continue_routine(time);
        printf("Sending report: 0x%08x\n", report);
        /* TODO: send report */
//        send_report();   
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

    strcpy(my_trace->host, host);
    my_trace->port = port;

    memset(my_trace->thread_map, -1, sizeof(my_trace->thread_map));

}

/* new main routine */

int main(int argc, char** argv)
{

    int i = 0;
    for(i=0;i<argc;i++)
        printf("%d - %s\n", i, argv[i]);

    if(argc < 3)
    {
        printf("You need do provide host and port\n");
        return -1;
    }

    if(strlen(argv[1]) > MAX_NAME) return -1;
    if(strlen(argv[2]) > MAX_NAME) return -1;

    my_trace = (TRACE_CONFIG*)malloc(sizeof(TRACE_CONFIG));
    if(my_trace == 0x0)
    {
        printf("Unable to allocate trace config\n");
        return -1;
    }

    init_trace(my_trace, argv[1], atoi(argv[2]));


    /* Windows sockets */

    WSADATA wsa;
    SOCKET s;    
    struct sockaddr_in server;
    
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
     
    printf("Initialised\n");

    /* Connect to socket */

    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }

    printf("Socket created\n");
    u_long iMode = 0;

    unsigned iResult;

    iResult = ioctlsocket(s, FIONBIO, &iMode);
    if (iResult != NO_ERROR) printf("ioctlsocket failed with error: %ld\n", iResult);

    /* Creating backconnect */
    server.sin_addr.s_addr = inet_addr(TRACE_CONTROLLER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(TRACE_CONTROLLER_PORT);
 
    iResult = ioctlsocket(s, FIONBIO, &iMode);
    if (iResult != NO_ERROR) printf("ioctlsocket failed with error: %ld\n", iResult);

    if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("connect error");
        return 1;
    }
     
    my_trace->socket = s;

    iResult = ioctlsocket(s, FIONBIO, &iMode);
    if (iResult != NO_ERROR) printf("ioctlsocket failed with error: %ld\n", iResult);

    printf("Connected\n");

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
           // printf("Got part: %s\n", cmd);
           // printf("Finish: %s\n", cmd+cmd_len-6);
        }

        cmd[cmd_len-6] = 0x0;
        //printf("Got cmd: %s\n", cmd);

        if(!strcmp(cmd, "quit")) 
            break;
        
        handle_cmd(cmd);

    }

    return 0x0;
}

int main2(int argc, char** argv)
{
    unsigned i, j;
    scan_on = 0x0;
    instr_count = 0x1;
    instr_limit= 0x0;
    instr_dbg = 0x0;
    char descriptor_file[MAX_LINE];
    

    if(argc < 5)
    {
		printf("You need to provide PID, descriptor, out path and a prefix\n");
		printf("Or: filename, descriptor, out path and a prefix\n");
		return -1;
    }

    path_i = 0x0;

    eventLock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    eventUnlock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    mutex = CreateMutex(0x0, 0x0, 0x0);

    strcpy(descriptor_file, argv[2]);
    parse_descriptor(descriptor_file);

    return 0x0;

    strcpy(mod_st, argv[2]);
    addr_st = strtol(argv[3], 0x0, 0x10);
    strcpy(mod_end, argv[4]);
	addr_end = strtol(argv[5], 0x0, 0x10);
    prefix = argv[7];

    if(argc > 8)
    {
        log = fopen(argv[8], "w+");
    }

    if(argc > 9)
    {
        instr_limit = strtol(argv[9], 0x0, 10);
    }

    sprintf(path, "%s\\%s_%s", argv[6], prefix, "instr");
    sprintf(iniPath, "%s\\%s_%s.ini", argv[6], prefix, "ini");
    sprintf(dumpPath, "%s\\%s_%s.bin", argv[6], prefix, "dump");
    sprintf(modPath, "%s\\%s_%s.bin", argv[6], prefix, "mods");

    SetConsoleCtrlHandler(HandlerRoutine, TRUE);

    /* hooks */

    /* isdebuggerpresent */
    hooks[0x0].offset = ISDEBUGGER_OFF_1;
    strcpy(hooks[0x0].libname, "KernelBase.dll");
    hooks[0x0].handler = isdebuggerpresent_callback_1;

    /* */
    hooks[0x1].offset = CREATETHREAD_OFF;
    strcpy(hooks[0x1].libname, "KernelBase.dll");
    hooks[0x1].handler = createthread_callback;

    /* sysenter */
    /*
    hooks[0x0].offset = NTSYSENTER_OFF;
    strcpy(hooks[0x0].libname, "ntdll.dll");
    hooks[0x0].handler = sysenter_callback;
    */

    hook_count = 0x1;

    for(i=0x0; i<WATCH_LIMIT; i++)
    {
        watched[i].off = 0x0;
        watched[i].instr = 0x0;
        watched[i].hit = 0x0;
        watched[i].count = 0x0;
        watched[i].scan = 0x0;
    }

    i = 0x0;
/*
    watched[i++].instr = 350000; watched[0].scan = 1;
    watched[i++].instr = 355000; watched[1].scan = 1;
    watched[i++].instr = 360000; watched[2].scan = 1;
    watched[i++].instr = 365000; watched[3].scan = 1;
    watched[i++].instr = 370000; watched[4].scan = 1;
    watched[i++].instr = 375000; watched[5].scan = 1;
    watched[i++].instr = 380000; watched[6].scan = 1;
    watched[i++].instr = 385000; watched[7].scan = 1;
    watched[i++].instr = 390000; watched[8].scan = 1;
    watched[i++].instr = 395000; watched[9].scan = 1;
    watched[i++].instr = 2475000; watched[10].scan = 1;
    watched[i++].instr = 2480000; watched[11].scan = 1;
    watched[i++].instr = 2485000; watched[12].scan = 1;
    watched[i++].instr = 4215000; watched[13].scan = 1;
    watched[i++].instr = 4220000; watched[14].scan = 1;
    watched[i++].instr = 4225000; watched[15].scan = 1;
    watched[i++].instr = 4230000; watched[16].scan = 1;
    watched[i++].instr = 4235000; watched[17].scan = 1;
    watched[i++].instr = 4240000; watched[18].scan = 1;
    watched[i++].instr = 4245000; watched[19].scan = 1;
    watched[i++].instr = 4250000; watched[20].scan = 1;
    watched[i++].off = 0x7387ad31;
*/
    file = configure_file();
    modifications = fopen(modPath, "wb+");

    DWORD threadId = 0x0;
    DWORD main_addr = 0x0;
    char main_addr_str[0x20];

    WritePrivateProfileString("general", "dumpPath", dumpPath, iniPath);
    WritePrivateProfileString("general", "instrPath", path, iniPath);

    /* syscall out args table */

    for(i=0x0; i<MAX_SYSCALL_ENTRIES; i++)
        for(j=0x0;j<MAX_SYSCALL_OUT_ARGS; j++)
            syscall_out_args[i][j].off_location = LOCATION_END;

    /* instructions on handling memory modifications made by sysenter */
    
#ifdef LIB_VER_WXP
    // ZwAllocateVirtualMemory
    syscall_out_args[0x11][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY}; 
    syscall_out_args[0x11][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x11][2] = {0x1, 0x3, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0}; 
    syscall_out_args[0x11][3] = last_arg;
   
    // ZwClose
    syscall_out_args[0x19][0] = last_arg;

    // ZwCreateFile
    syscall_out_args[0x25][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x25][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x25][2] = last_arg;

    // ZwCreateSection 
    syscall_out_args[0x32][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x32][1] = last_arg;
   
    // ZwFreeVirtualMemory
    syscall_out_args[0x53][0] = {0x1, 0x2, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0x53][1] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x53][2] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x53][3] = last_arg;

    // ZwMapViewOfSection
    syscall_out_args[0x6c][0] = {0x2, 0x6, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0x6c][1] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x6c][2] = {0x5, 0x8, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x6c][3] = {0x6, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x6c][4] = last_arg;

    // NtOpenFile
    syscall_out_args[0x74][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x74][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x74][2] = last_arg;

    // ZwOpenKey
    syscall_out_args[0x77][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x77][1] = last_arg;

    // ZwOpenProcessToken
    syscall_out_args[0x7b][0] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x7b][1] = last_arg;

    // ZwOpenSection
    syscall_out_args[0x7d][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x7d][1] = last_arg;

    // ZwProtectVirtualMemory
    syscall_out_args[0x89][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x89][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x89][2] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x89][3] = last_arg;

    // NtQueryAttributesFile
    syscall_out_args[0x8b][0] = {0x0, sizeof(FILE_BASIC_INFORMATION), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x8b][1] = last_arg;

    // ZwQueryInformationProcess
    syscall_out_args[0x9a][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x9a][1] = {0x2, 0x3, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0x9a][2] = last_arg;

    // ZwQueryInformationToken
    syscall_out_args[0x9c][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x9c][1] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0x9c][2] = last_arg;

    // NtQueryPerformanceCounter
    syscall_out_args[0xa5][0] = {0x0, 0x8, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    syscall_out_args[0xa5][1] = {0x1, 0x8, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    syscall_out_args[0xa5][2] = last_arg;

    // NtQuerySection
    // TODO: check 1st
    syscall_out_args[0xa7][0] = {0x2, 0x4, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    syscall_out_args[0xa7][1] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0xa7][2] = last_arg;


    // ZwQuerySystemInformation
    syscall_out_args[0xad][0] = {0x1, 0x2, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    syscall_out_args[0xad][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0};
    syscall_out_args[0xad][2] = last_arg;

    // ZwQueryValueKey
    syscall_out_args[0xb1][0] = {0x3, 0x5, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0xb1][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb1][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb1][3] = last_arg;

    // ZwReadFile
    syscall_out_args[0xb7][0] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb7][1] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb7][2] = {0x5, 0x6, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    syscall_out_args[0xb7][3] = last_arg;

    // ZwRequestWaitReplyPort
    syscall_out_args[0xc8][0] = {0x2, 0x148, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0xc8][1] = last_arg;

    // ZwUnmapViewOfSection
    syscall_out_args[0x101][0] = last_arg;

    // ZwTraceControl
/*
    syscall_out_args[0x177][0] = {0x3, 0x4, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0x0};
    syscall_out_args[0x177][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x177][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x177][3] = last_arg;
*/
    // ZwWriteVirtualMemory
    syscall_out_args[0x115][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x115][1] = last_arg;
#endif
    
#ifdef LIB_VER_W7 
    // ZwAllocateVirtualMemory
    syscall_out_args[0x13][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY}; 
    syscall_out_args[0x13][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x13][2] = {0x1, 0x3, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0}; 
    syscall_out_args[0x13][3] = last_arg;
   
    // ZwClose
    syscall_out_args[0x32][0] = last_arg;

    // 0x40

    // ZwCreateFile
    syscall_out_args[0x42][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x42][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x42][2] = last_arg;

    // ZwCreateSection 
    syscall_out_args[0x54][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x54][1] = last_arg;
  
    // ZwDeviceIoControlFile (not verified)
    syscall_out_args[0x6b][0] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x6b][1] = {0x8, 0x9, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0x0};
    syscall_out_args[0x6b][2] = last_arg;
 
    // ZwFreeVirtualMemory
    syscall_out_args[0x83][0] = {0x1, 0x2, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0x83][1] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x83][2] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x83][3] = last_arg;

    // ZwMapViewOfSection
    syscall_out_args[0xa8][0] = {0x2, 0x6, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0xa8][1] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xa8][2] = {0x5, 0x8, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xa8][3] = {0x6, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xa8][4] = last_arg;

    // ZwOpenDirectoryObject
    syscall_out_args[0xaf][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xaf][1] = last_arg;

    // NtOpenFile
    syscall_out_args[0xb3][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb3][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb3][2] = last_arg;

    // ZwOpenKey
    syscall_out_args[0xb6][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb6][1] = last_arg;

    // ZwRegisterThreadTerminatePort
    syscall_out_args[0xbb][0] = last_arg;

    // ZwOpenProcessToken
    syscall_out_args[0xbf][0] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xbf][1] = last_arg;

    // c0

    // ZwOpenSection
    syscall_out_args[0xc2][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xc2][1] = last_arg;

    // ZwOpenSymbolicLinkObject
    syscall_out_args[0xc5][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xc5][1] = last_arg;

    // ZwProtectVirtualMemory
    syscall_out_args[0xd7][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xd7][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xd7][2] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xd7][3] = last_arg;

    // NtQueryAttributesFile
    syscall_out_args[0xd9][0] = {0x0, sizeof(FILE_BASIC_INFORMATION), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xd9][1] = last_arg;

    // NtQueryDefaultLocale
    syscall_out_args[0xdd][0] = {0x1, sizeof(LCID), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xdd][1] = last_arg;

    // ZwQueryDirectoryFile
    syscall_out_args[0xdf][0] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xdf][1] = {0x5, 0x6, LOCATION_ADDR_STACK, LOCATION_STACK, STATUS_ANY};
    syscall_out_args[0xdf][2] = last_arg;

    // ZwQueryInformationFile
    syscall_out_args[0xe7][0] = {0x1, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xe7][1] = {0x2, 0x3, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    syscall_out_args[0xe7][2] = last_arg;

    // ZwQueryInformationProcess
    syscall_out_args[0xea][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xea][1] = {0x2, 0x3, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0xea][2] = last_arg;

    // ZwQueryInformationToken
    syscall_out_args[0xed][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xed][1] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0xed][2] = last_arg;

    // NtQueryPerformanceCounter
    syscall_out_args[0xfb][0] = {0x0, 0x8, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    syscall_out_args[0xfb][1] = {0x1, 0x8, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    syscall_out_args[0xfb][2] = last_arg;

    // NtQuerySection
    // TODO: check 1st
    syscall_out_args[0xfe][0] = {0x2, 0x4, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, STATUS_ANY};
    syscall_out_args[0xfe][1] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0xfe][2] = last_arg;


    // ZwQuerySystemInformation
    syscall_out_args[0x105][0] = {0x1, 0x2, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    syscall_out_args[0x105][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0};
    syscall_out_args[0x105][2] = last_arg;

    // NtQuerySystemTime
    syscall_out_args[0x107][0] = {0x0, sizeof(LARGE_INTEGER), LOCATION_ADDR_STACK, LOCATION_CONST, 0};
    syscall_out_args[0x107][1] = last_arg;

    // ZwQueryValueKey
    syscall_out_args[0x10a][0] = {0x3, 0x5, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0x10a][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x10a][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x10a][3] = last_arg;

    // ZwQueryVolumeInformationFile
    syscall_out_args[0x10c][0] = {0x1, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x10c][1] = {0x2, 0x3, LOCATION_ADDR_STACK, LOCATION_STACK, STATUS_ANY};
    syscall_out_args[0x10c][2] = last_arg;

    // ZwReadFile
    syscall_out_args[0x111][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x111][1] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x111][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x111][3] = {0x5, 0x6, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    syscall_out_args[0x111][4] = last_arg;

    // ZwRequestWaitReplyPort
    syscall_out_args[0x12b][0] = {0x1, 0x180, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x12b][1] = {0x2, 0x180, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x12b][2] = last_arg;

    // ZwSetInformationFile
    syscall_out_args[0x149][0] = {0x1, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x149][1] = last_arg;

    // ZwTraceControl
    syscall_out_args[0x177][0] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x177][1] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x177][2] = last_arg;

    // ZwUnmapViewOfSection
    syscall_out_args[0x181][0] = last_arg;

    // NtWaitForSingleObject
    syscall_out_args[0x187][0] = last_arg;

    // ZwWriteVirtualMemory
    syscall_out_args[0x18f][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x18f][1] = last_arg;

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

    myPID = strtol(argv[1], 0x0, 10);
    if(myPID == 0x0)
    {
        spawned = 1;
        strcpy(process_fname, argv[1]);
        start_trace_fname();
    }
    else
    {
        start_trace_pid();
    }

    CreateThread(0x0, 0x0, (LPTHREAD_START_ROUTINE)&writer, 0x0, 0x0, &threadId);

    my_trace->thread_count = 0x0;
    while(1)
    {
        status = DBG_CONTINUE;

        WaitForDebugEvent(&de, INFINITE);
        
        if(full_log)
            d_print("dwDebugEventCode: 0x%08x\n", de.dwDebugEventCode);
        
        switch(de.dwDebugEventCode)
        {
            case CREATE_PROCESS_DEBUG_EVENT:
                cpdi = de.u.CreateProcessInfo;

                // register main thread
                register_thread(de.dwThreadId, de.u.CreateProcessInfo.hThread);

                //configure breakpoints
                if(strstr(mod_st, "0x0"))
                {
                    printf("Main module: 0x%08x\n", (DWORD)de.u.CreateProcessInfo.lpBaseOfImage);
                    add_breakpoint(addr_st + (DWORD)de.u.CreateProcessInfo.lpBaseOfImage, bp_callback);
                }

                if(strstr(mod_end, "0x0"))
                {
                    printf("Main module: 0x%08x\n", (DWORD)de.u.CreateProcessInfo.lpBaseOfImage);
                    img_base = (DWORD)de.u.CreateProcessInfo.lpBaseOfImage;
                }
                //unset_ss(0x0);
                break;

            case EXCEPTION_DEBUG_EVENT:
                EXCEPTION_RECORD er;
                status = DBG_EXCEPTION_NOT_HANDLED;
                er = de.u.Exception.ExceptionRecord;
/*
                if(full_log)
                {
                    d_print("Exception: 0x%08x\n", er.ExceptionCode);
                    d_print("at: 0x%08x\n", er.ExceptionAddress);
                    d_print("First chance: 0x%08x\n", de.u.Exception.dwFirstChance);
                }
*/
                switch(er.ExceptionCode)
                {
                    case EXCEPTION_SINGLE_STEP:
                        ss_callback((void*)&de);
                        //verify_ss(0x0);
                        set_ss(de.dwThreadId);
                        //verify_ss(0x0);
                        status = DBG_CONTINUE;
                        //if(full_log) status = DBG_EXCEPTION_NOT_HANDLED;
                        break;
                
                    case EXCEPTION_BREAKPOINT:
                        ss_callback((void*)&de);
                        handle_breakpoint((DWORD)er.ExceptionAddress, &de);
                        //verify_ss(0x0);
                        //set_ss(de.dwThreadId);
                        //verify_ss(0x0);
                        status = DBG_CONTINUE;
                        //if(full_log) status = DBG_EXCEPTION_NOT_HANDLED;
                        break;
                    default:
                        register_exception(de.dwThreadId, er);
                        //de.u.Exception.dwFirstChance = 0x0;
                        //del_breakpoint(sysenter_off);
                        //unset_ss(de.dwThreadId);
                        //full_log = 1;
                        ss_callback((void*)&de);
                        //verify_ss(0x0);
                        set_ss(de.dwThreadId);
                        //verify_ss(0x0);
                        status = DBG_EXCEPTION_NOT_HANDLED;
                        break;
                }
                /*
                if(er.ExceptionCode != EXCEPTION_SINGLE_STEP && er.ExceptionCode != EXCEPTION_BREAKPOINT)
                    d_print("Will continue with status: 0x%08x\n", status);
                */
                break;

            case CREATE_THREAD_DEBUG_EVENT: 
                register_thread(de.dwThreadId, de.u.CreateThread.hThread);
                set_ss(de.dwThreadId);
                break;

            case LOAD_DLL_DEBUG_EVENT:
                d_print("Handle: 0x%08x\n", de.u.LoadDll.hFile);
                register_lib(de.u.LoadDll);
                d_print("Addr: 0x%08x\n", libs[lib_count-1].lib_addr);

#ifdef LIB_VER_W7
                GetFinalPathNameByHandleA(de.u.LoadDll.hFile, libs[lib_count].lib_name, MAX_NAME, VOLUME_NAME_NONE);
#endif

                d_print("Testing for start lib, compairng: %s & %s\n", libs[lib_count].lib_name, mod_st);
                if(strstr(libs[lib_count].lib_name, mod_st))
                {
                    d_print("Match!\n");
                    add_breakpoint(addr_st + find_lib(mod_st), bp_callback);
                }
//                if(strstr(libs[lib_count-1].lib_name, mod_end))
//                {
//                }

                /* handle hooks */

                unsigned j;
                for(j = 0x0; j<hook_count; j++)
                {
#ifdef LIB_VER_W7
                    d_print("%s - %s\n",  libs[lib_count-1].lib_name, hooks[j].libname);
                    if(strstr(libs[lib_count-1].lib_name, hooks[j].libname))
#endif
#ifdef LIB_VER_WXP
                    /* todo */
#endif
                    {
                        add_breakpoint((DWORD)(hooks[j].offset + de.u.LoadDll.lpBaseOfDll), hooks[j].handler);
                        d_print("Adding hook: %d\n", j);
                    }
                }

#ifdef LIB_VER_W7
                if(strstr(libs[lib_count-1].lib_name, "ntdll.dll"))
#endif
#ifdef LIB_VER_WXP
                if(libs[lib_count-1].lib_addr == NTDLL_OFF)
#endif
                {
                    d_print("ntdll loaded\n");
#ifdef LIB_VER_W7
                    nt1_off = find_lib("ntdll.dll") + NTMAPVIEWOFSECTION_1;
                    nt2_off = find_lib("ntdll.dll") + NTMAPVIEWOFSECTION_2;
                    nt3_off = find_lib("ntdll.dll") + NTREADFILE_OFF_1;
                    nt4_off = find_lib("ntdll.dll") + NTREADFILE_OFF_2;
                    sysenter_off = find_lib("ntdll.dll") + NTSYSENTER_OFF;
                    sysret_off = find_lib("ntdll.dll") + NTSYSRET_OFF;
#endif
#ifdef LIB_VER_WXP
                    nt1_off = NTDLL_OFF + NTMAPVIEWOFSECTION_1;
                    nt2_off = NTDLL_OFF + NTMAPVIEWOFSECTION_2;
                    nt3_off = NTDLL_OFF + NTREADFILE_OFF_1;
                    nt4_off = NTDLL_OFF + NTREADFILE_OFF_2;
                    sysenter_off = NTDLL_OFF + NTSYSENTER_OFF;
                    sysret_off = NTDLL_OFF + NTSYSRET_OFF;
#endif
                }
                break;

            case UNLOAD_DLL_DEBUG_EVENT:
                deregister_lib(de.u.UnloadDll);
                break;

            case EXIT_THREAD_DEBUG_EVENT:
                d_print("Exiting thread\n");
                deregister_thread(de.dwThreadId, de.u.CreateThread.hThread);
                my_trace->threads[de.dwThreadId].alive = 0x0;

                if(my_trace->threads[de.dwThreadId].handle != 0x0) 
                    CloseHandle(my_trace->threads[de.dwThreadId].handle);

                my_trace->threads[de.dwThreadId].handle = 0x0;
                my_trace->threads[de.dwThreadId].open = 0x0;
                break;

            case EXIT_PROCESS_DEBUG_EVENT:
                d_print("Exiting process\n");
                HandlerRoutine(0x0);
                break;

        }

        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, status);
    }


    CloseHandle(file);
    CloseHandle(modifications);
    if(log) fclose(log);

    return 0;
}

