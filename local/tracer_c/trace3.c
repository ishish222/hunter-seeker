#define _WIN32_WINNT 0x0700

#include <stdio.h>
#include <windows.h>
#include "WinBase.h"
#include <TlHelp32.h>
#include "winternl.h"

#define buf_size 0x100000
#define SS_FLAGS 0x100
#define CLEAR_SS_FLAGS 0xfffffeff
#define MEM_DUMP
#define MAX_NAME 0x200
#define MAX_SYSCALL_OUT_ARGS 0x10
#define MAX_SYSCALL_ENTRIES  0x10000

/* functions offsets in respective libs */
#define EXIT_PROCESS_OFF 0x52acf

#define NTREADFILE_OFF_1        0x45700
#define NTREADFILE_OFF_2        0x4570c
#define NTMAPVIEWOFSECTION_1    0x45070
#define NTMAPVIEWOFSECTION_2    0x4507c
#define NTSYSENTER_OFF          0x464f0
#define NTSYSRET_OFF            0x464f4

#define STATUS_ANY 0xff00ff00

char process_fname[0x200];
char spawned = 0x0;


HANDLE procHandle;

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
PROCESS_INFORMATION pi;
STARTUPINFO si;

char path[0x200];
char dumpPath[0x200];
char iniPath[0x200];
char modPath[0x200];
wchar_t filePath[0x200];

char line[0x100];
char instr[0x60];
int last_eip = 0x0;

const wchar_t* target_lib = L"ntdll.dll";
DWORD target_lib_off = 0x0;
const char* target_call = "NtCreateFile";
DWORD target_call_off = 0x44a10;
DWORD target_off = 0x0;
CREATE_PROCESS_DEBUG_INFO cpdi;
int myPID = 0x0;
int myTID = 0x0;

DWORD nt1_off;
DWORD nt2_off;
DWORD nt3_off;
DWORD nt4_off;
DWORD sysenter_off;
DWORD sysret_off;

#define LOCATION_CONST          0x0
#define LOCATION_STACK          0x1
#define LOCATION_ADDR_STACK     0x2
#define LOCATION_ADDR_ADDR_STACK     0x3
#define LOCATION_REG            0x4
#define LOCATION_MEM            0x5
#define LOCATION_END            0x6

typedef struct OUT_ARGUMENT_
{
    DWORD off;
    DWORD size;
    char off_location;
    char size_location;
    DWORD eax_val_success;
} OUT_ARGUMENT;

OUT_ARGUMENT last_arg = {0x0, 0x0, LOCATION_END, LOCATION_END, 0x0};

typedef struct OUT_LOCATION_
{
    DWORD off;
    DWORD size;
} OUT_LOCATION;

OUT_LOCATION last_location = {0x0, 0x0};

OUT_ARGUMENT syscall_out_args[MAX_SYSCALL_ENTRIES][MAX_SYSCALL_OUT_ARGS];
OUT_LOCATION syscall_out_args_dump_list[MAX_SYSCALL_OUT_ARGS];

DWORD buffer_addr;
DWORD size_addr;

typedef struct _CREATE_THREAD_DEBUG_INFO2
{
    CREATE_THREAD_DEBUG_INFO u;
    DWORD tid;
} CREATE_THREAD_DEBUG_INFO2;

typedef struct _THREAD_ENTRY
{
    char alive;
    char open;
    char created;
    HANDLE handle;
} THREAD_ENTRY;

CREATE_THREAD_DEBUG_INFO2 threads[0x100000000];

THREAD_ENTRY threads2[0x100000000];
DWORD thread_list[0x1000];
DWORD thread_count = 0x0;

#define MAX_NAME 0x200
#define MAX_LIBS 0x200

typedef struct _LIB_ENTRY
{
    char alive;
    DWORD lib_addr;
    char lib_name[MAX_NAME];
} LIB_ENTRY;

DWORD lib_count = 0x0;
LIB_ENTRY libs[MAX_LIBS];

typedef void (*handler_routine)(void*);
int add_breakpoint(DWORD addr, handler_routine handler);
DWORD find_lib(char* name);
BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);
SIZE_T dump_mem(FILE*, void*, SIZE_T);

typedef struct _bpt
{
    DWORD addr;
    char saved_byte;
    handler_routine handler;    
    char enabled;
    char isHook;
} bpt;

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

int unset_ss(DWORD);
int set_ss(DWORD);

int add_to_buffer(char* line)
{
    int written, size;

    size = strlen(line);
    if((index + size) >= buf_size)
    {
        SetEvent(eventLock);
        WaitForSingleObject(eventUnlock, INFINITE);
        index = 0;
    }

    written = sprintf(buffer + index, "%s\n", line);

    if(written>0)
        index += written;

}

void print_context(CONTEXT* ctx)
{
    printf("Context:\n");
    printf("EAX:\t%x\n", ctx->Eax);
    printf("EBX:\t%x\n", ctx->Ebx);
    printf("ECX:\t%x\n", ctx->Ecx);
    printf("EDX:\t%x\n", ctx->Edx);
    printf("ESI:\t%x\n", ctx->Esi);
    printf("EDI:\t%x\n", ctx->Edi);
    printf("EBP:\t%x\n", ctx->Ebp);
    printf("ESP:\t%x\n", ctx->Esp);
    printf("EFLAGS:\t%x\n", ctx->EFlags);
    printf("EIP:\t%x\n", ctx->Eip);
}


int dec_eip(DWORD id)
{
    int i;
    HANDLE myHandle = (HANDLE)-0x1;
/*
    for(i=0x0; i< thread_count; i++)
        if(threads[i].tid == id)
        {
            myHandle = threads[i].u.hThread;
            break;
        }

    if(myHandle == (HANDLE)-0x1) return 0x1;

    printf("Found, decreasing\n");
*/

    //myHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, id);
    myHandle = threads2[id].handle;
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_CONTROL;
    if(GetThreadContext(myHandle, &ctx) == 0x0)
    {
        printf("Failed to get context, error: 0x%08x\n", GetLastError());
    }
    ctx.Eip -= 0x1;
    SetThreadContext(myHandle, &ctx);

    //CloseHandle(myHandle);
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
    char line2[0x100];

    printf("Registering Exception: code 0x%08x at: 0x%08x\n", er.ExceptionCode, er.ExceptionAddress);

    serialize_exception(er, line2);
    sprintf(line, "EX,0x%08x,%s", tid, line2);

    return;
}

void getSelectorEntries(HANDLE handle, CONTEXT ctx, LDT_ENTRY* ldt)
{
    if(GetThreadSelectorEntry(handle, ctx.SegGs, &ldt[0]) == 0x0) printf("Failed: 0x%08x\n", GetLastError());
    if(GetThreadSelectorEntry(handle, ctx.SegFs, &ldt[1]) == 0x0) printf("Failed\n");
    if(GetThreadSelectorEntry(handle, ctx.SegEs, &ldt[2]) == 0x0) printf("Failed\n");
    if(GetThreadSelectorEntry(handle, ctx.SegDs, &ldt[3]) == 0x0) printf("Failed\n");
    if(GetThreadSelectorEntry(handle, ctx.SegCs, &ldt[4]) == 0x0) printf("Failed\n");
    if(GetThreadSelectorEntry(handle, ctx.SegSs, &ldt[5]) == 0x0) printf("Failed\n");
    return;
}

void register_thread(DWORD tid, HANDLE handle)
{
    printf("Registering: TID 0x%08x, handle 0x%08x\n", tid, handle);
    char line2[0x200];
    CONTEXT ctx;
    LDT_ENTRY ldt[0x6];

    if(handle == 0x0) 
        handle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);

    if(threads2[tid].created)
    {
        // do not create new, update this one
        printf("Updating: TID 0x%08x, handle 0x%08x\n", tid, handle);
        threads2[tid].alive = 0x1;
        threads2[tid].handle = handle;
    }
    else
    {
        threads2[tid].alive = 0x1;
        threads2[tid].handle = handle;
        threads2[tid].open = 0x1;
        threads2[tid].created = 0x1;
    
        //if(threads2[tid].handle == 0x0) 
        printf("Registering: TID 0x%08x, handle 0x%08x\n", tid, handle);

        thread_list[thread_count] = tid;
        thread_count ++;
    }

    printf("Registering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", tid, threads2[tid].handle, thread_count);

    ctx.ContextFlags = CONTEXT_FULL;

    //write info about thread registration
    GetThreadContext(threads2[tid].handle, &ctx);

    getSelectorEntries(handle, ctx, ldt);
/*
    printf("GS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegGs, ldt[0].LimitLow, ldt[0].BaseLow, ldt[0].HighWord);
    printf("FS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegFs, ldt[1].LimitLow, ldt[1].BaseLow, ldt[1].HighWord);
    printf("ES: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegEs, ldt[2].LimitLow, ldt[2].BaseLow, ldt[2].HighWord);
    printf("DS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegDs, ldt[3].LimitLow, ldt[3].BaseLow, ldt[3].HighWord);
    printf("CS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegCs, ldt[4].LimitLow, ldt[4].BaseLow, ldt[4].HighWord);
    printf("SS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegSs, ldt[5].LimitLow, ldt[5].BaseLow, ldt[5].HighWord);
*/
    //print_context(&ctx);

    serialize_context(ctx, ldt, line2);
    sprintf(line, "RT,0x%08x,%s", tid, line2);

    add_to_buffer(line);

    return;
}

void deregister_thread(DWORD tid, HANDLE handle)
{
    printf("Deregistering: TID 0x%08x, handle 0x%08x\n", tid, handle);
    char line2[0x200];
    CONTEXT ctx;
    LDT_ENTRY ldt[0x6];

    if(handle == 0x0) 
        handle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, tid);

    threads2[tid].alive = 0x0;
    threads2[tid].handle = handle;
    threads2[tid].open = 0x1;

    //if(threads2[tid].handle == 0x0) 
    printf("Deregistering: TID 0x%08x, handle 0x%08x\n", tid, handle);

    printf("Deregistering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", tid, threads2[tid].handle, thread_count);

    ctx.ContextFlags = CONTEXT_FULL;

    //write info about thread registration
    GetThreadContext(threads2[tid].handle, &ctx);

    getSelectorEntries(handle, ctx, ldt);
    /*
    printf("GS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegGs, ldt[0].LimitLow, ldt[0].BaseLow, ldt[0].HighWord);
    printf("FS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegFs, ldt[1].LimitLow, ldt[1].BaseLow, ldt[1].HighWord);
    printf("ES: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegEs, ldt[2].LimitLow, ldt[2].BaseLow, ldt[2].HighWord);
    printf("DS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegDs, ldt[3].LimitLow, ldt[3].BaseLow, ldt[3].HighWord);
    printf("CS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegCs, ldt[4].LimitLow, ldt[4].BaseLow, ldt[4].HighWord);
    printf("SS: 0x%08x: 0x%08x, 0x%08x, 0x%08x\n", ctx.SegSs, ldt[5].LimitLow, ldt[5].BaseLow, ldt[5].HighWord);
    */
    //print_context(&ctx);

    serialize_context(ctx, ldt, line2);


    sprintf(line, "DT,0x%08x,%s", tid, line2);

    add_to_buffer(line);

    /* not necessary, could be reused for reregistration */
    /*
    if(threads2[tid].handle != 0x0) 
        CloseHandle(threads2[tid].handle);

    threads2[tid].handle = 0x0;
    threads2[tid].open = 0x0;
    */
    return;
}

void deregister_thread2(DWORD tid)
{
    if(threads2[tid].handle != 0x0) 
        CloseHandle(threads2[tid].handle);

    threads2[tid].handle = 0x0;
    threads2[tid].open = 0x0;

    printf("Deregistering TID: 0x%08x with handle: 0x%08x\n", tid, threads2[tid].handle);
    sprintf(line, "DT,0x%08x", tid);
 
    add_to_buffer(line);
    return;
}


void register_lib(LOAD_DLL_DEBUG_INFO info)
{
    char path[MAX_NAME];

    printf("Handle: %p\n", info.hFile);
 
    GetFinalPathNameByHandleA(info.hFile, libs[lib_count].lib_name, MAX_NAME, VOLUME_NAME_NONE);
    printf("Name pointer: %p, len: 0x%08x\n", libs[lib_count].lib_name, strlen(libs[lib_count].lib_name));

//    strcpy(libs[lib_count].lib_name, buffer);
    libs[lib_count].lib_addr = (DWORD)info.lpBaseOfDll;

    printf("RL,0x%08x,%s\n", libs[lib_count].lib_addr, libs[lib_count].lib_name);
    sprintf(line, "RL,0x%08x,%s", libs[lib_count].lib_addr, libs[lib_count].lib_name);

    libs[lib_count].alive = 0x1;
    lib_count++;

//    sprintf(line, "RL,0x%08x,%s", info.lpBaseOfDll, name);




//    UNICODE_STRING* us;

//    if(GetModuleFileNameA((HMODULE)info.hFile, name, 0x100) == 0x0) 
//    {
//        printf("Failure\n");
//        printf("%d", GetLastError());
//    }
//    int i;
//    for(i=0x0; i<0x10; i++)
//        printf("%02x\n ", name[i]);

//    printf("\n");
    add_to_buffer(line);
    return;
}

void deregister_lib(DWORD i)
{
    sprintf(line, "DL,0x%08x,%s", libs[i].lib_addr, libs[i].lib_name);
    libs[i].alive = 0x0;

    //printf(line);
    //printf("\n");
    add_to_buffer(line);

}


void deregister_lib(UNLOAD_DLL_DEBUG_INFO info)
{
    unsigned i;

    for (i = 0x0; i< lib_count; i++)
    {
        if(libs[i].lib_addr == (DWORD)info.lpBaseOfDll) break;
    }

    sprintf(line, "DL,0x%08x,%s", libs[i].lib_addr, libs[i].lib_name);

    libs[i].alive = 0x0;
    //printf(line);
    //printf("\n");
    add_to_buffer(line);

    return;
}

void ss_callback(void* data);
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


    printf("Thread: %08x\n", de->dwThreadId);

    int i = 0x0;

    for(i = 0x0; i<thread_count; i++)
    {
        if(threads2[thread_list[i]].alive)
        printf("TID: 0x%08x, handle: 0x%08x\n", thread_list[i], threads2[thread_list[i]].handle);

    }

    HANDLE myHandle = (HANDLE)-0x1;
    DWORD esp = 0x0;
//    myHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, de->dwThreadId);
    myHandle = threads2[de->dwThreadId].handle;
    printf("Handle: %x\n", myHandle);
    printf("Handle in threads2: %x\n", threads2[de->dwThreadId]);
//    myHandle = threads2[de->dwThreadId].handle;
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    if(GetThreadContext(myHandle, &ctx) == 0x0) 
    {
        printf("Failed to get context, error: 0x%08x\n", GetLastError());
        return;
    }
    printf("ESP: %p\n", ctx.Esp);

    tmp = ctx.Esp + (0x3 * 0x4);
    ReadProcessMemory(cpdi.hProcess, (void*)(tmp), (void*)&buffer_addr, 0x4, &read);

    tmp = ctx.Esp + (0x7 * 0x4);
    ReadProcessMemory(cpdi.hProcess, (void*)(tmp), (void*)&size_addr, 0x4, &read);

    printf("off: 0x%08x, size: 0x%08x\n", buffer_addr, size_addr);

    add_breakpoint(nt2_off, ntmap_2_callback);
//    CloseHandle(myHandle);
}

void ntmap_2_callback(void* data)
{
    printf("ntmap_2\n");

    DWORD offset, size, size_wrote, read;

    ReadProcessMemory(cpdi.hProcess, (void*)(buffer_addr), (void*)&offset, 0x4, &read);
    ReadProcessMemory(cpdi.hProcess, (void*)(size_addr), (void*)&size, 0x4, &read);

    printf("Block 0x%08x - 0x%08x, 0x%08x bytes\n", offset, offset + size, size);

    size_wrote = dump_mem(modifications, (void*)offset, size);
    if(size_wrote == size)
    {
        sprintf(line, "UP,0x%08x,0x%08x", offset, size);
        add_to_buffer(line);
    }

    add_breakpoint(nt1_off, ntmap_1_callback);
}

void ntread_1_callback(void* data)
{
    printf("ntread_1\n");
    add_breakpoint(nt4_off, ntread_2_callback);
}

void ntread_2_callback(void* data)
{
    printf("ntread_2\n");
    add_breakpoint(nt3_off, ntread_1_callback);
}

void sysenter_callback(void* data)
{
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;
    unsigned i;

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    if(GetThreadContext(threads2[tid].handle, &ctx) == 0x0) 
    {
        printf("Failed to get context, error: 0x%08x\n", GetLastError());
        return;
    }
    printf("ESP: %p\n", ctx.Esp);
    sysenter_no = ctx.Eax;
    sysenter_esp = ctx.Esp;

    printf("Deregister thread @ SYSENTER: %08x\n", tid);
    deregister_thread(tid, threads2[tid].handle);
    add_breakpoint(sysret_off, sysret_callback);
    unset_ss(tid);
}

void sysret_callback(void* data)
{
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;
    DWORD arg_addr, arg_val;
    DWORD size, read, off, size_wrote;
    unsigned i;

    printf("Register thread @ SYSRET: %08x\n", tid);
    register_thread(tid, threads2[tid].handle);

    // dump 0x50 bytes from stack
    /*
    printf("Block 0x%08x - 0x%08x, 0x%08x bytes\n", sysenter_esp, sysenter_esp + 0x50, 0x50);
    size = dump_mem(modifications, (void*)sysenter_esp, 0x50);
    sprintf(line, "UP,0x%08x,0x%08x", sysenter_esp, 0x50);
    add_to_buffer(line);
*/

    // prepare dump list
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;
    if(GetThreadContext(threads2[tid].handle, &ctx) == 0x0) 
    {
        printf("Failed to get context, error: 0x%08x\n", GetLastError());
        return;
    }
    printf("EAX: 0x%08x\n", ctx.Eax);

    /*
    for(i = 0x0; i<MAX_SYSCALL_OUT_ARGS; i++)
            syscall_out_args_dump_list[i].off = last_location.off;
    */

    printf("[[Syscall: 0x%08x @ 0x%08x]]\n", sysenter_no, ctx.Eip);
    for(i = 0x0; i<MAX_SYSCALL_OUT_ARGS; i++)
    {
        printf("Arg no: 0x%02x\n", i);
        if(syscall_out_args[sysenter_no][i].off_location == last_arg.off_location) 
        {
            printf("Last arg, finishing\n");
            break;
        }
        if(syscall_out_args[sysenter_no][i].eax_val_success != STATUS_ANY)
        {
            printf("Arg not any\n");
            if(syscall_out_args[sysenter_no][i].eax_val_success != ctx.Eax) 
            {
                printf("Wrong EAX\n");
                continue;
            }
        }
        if(1)
        {
            printf("Locating buffer\n");
            arg_val = 0x0;
            arg_addr = 0x0;
            off = 0x0;
            size = 0x0;

            printf("ESP: 0x%08x\n", sysenter_esp);

            // decide offset
            switch(syscall_out_args[sysenter_no][i].off_location)
            {
                case LOCATION_CONST:
                    syscall_out_args_dump_list[i].off = syscall_out_args[sysenter_no][i].off;
                    arg_val = syscall_out_args_dump_list[i].off;
                    printf("Arg off: 0x%08x\n", arg_val);
                    break;
                case LOCATION_MEM:
                    arg_addr = syscall_out_args[sysenter_no][i].off;
                    ReadProcessMemory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    printf("Arg off: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].off * 0x4;
                    arg_val = arg_addr;
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    printf("Arg off: 0x%08x\n", arg_val);
                    if(arg_val == 0x0) break;
                    syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_ADDR_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].off * 0x4;
                    ReadProcessMemory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    printf("Arg off: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].off = arg_val;
                    break;
                case LOCATION_ADDR_ADDR_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].off * 0x4;
                    ReadProcessMemory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    arg_addr = arg_val;
                    ReadProcessMemory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    if(arg_val == 0x0) break;
                    printf("Arg off: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].off = arg_val;
                    break;
            }

            if(arg_val == 0x0) break;
            off = arg_val;
            arg_val = 0x0;
            arg_addr = 0x0;
            printf("Locating size\n");

            // decide size
            switch(syscall_out_args[sysenter_no][i].size_location)
            {
                case LOCATION_CONST:
                    syscall_out_args_dump_list[i].size = syscall_out_args[sysenter_no][i].size;
                    arg_val = syscall_out_args_dump_list[i].size;
                    printf("Arg size: 0x%08x\n", arg_val);
                    break;
                case LOCATION_MEM:
                    arg_addr = syscall_out_args[sysenter_no][i].size;
                    ReadProcessMemory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    printf("Arg size: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].size = arg_val;
                    break;
                case LOCATION_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].size * 0x4;
                    ReadProcessMemory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    printf("Arg size: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].size = arg_val;
                    break;
                case LOCATION_ADDR_STACK:
                    arg_addr = sysenter_esp + 0x8;
                    arg_addr += syscall_out_args[sysenter_no][i].size * 0x4;
                    ReadProcessMemory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    arg_addr = arg_val;
                    ReadProcessMemory(cpdi.hProcess, (void*)arg_addr, (void*)&arg_val, 0x4, &read);
                    printf("0x%08x: 0x%08x\n", arg_addr, arg_val);
                    printf("Arg size: 0x%08x\n", arg_val);
                    syscall_out_args_dump_list[i].size = arg_val;
                    break;
            }

            size = arg_val;

            size_wrote = dump_mem(modifications, (void*)off, size);
            if(size_wrote == size)
            {
                printf("[Out argument: 0x%08x, size: 0x%08x]\n", off, size);
                sprintf(line, "UP,0x%08x,0x%08x", off, size);
                add_to_buffer(line);
            }
        }
    }

    set_ss(tid);
    ss_callback(data);

    add_breakpoint(sysenter_off, sysenter_callback);
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
            printf("Thread32First\n");  // Show cause of failure
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
                    threads2[te32.th32ThreadID].alive = 0x1;
                    threads2[te32.th32ThreadID].handle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, te32.th32ThreadID);
                    threads2[te32.th32ThreadID].open = 0x1;

                    thread_list[thread_count] = te32.th32ThreadID;
                    thread_count ++;
                    printf("Registering TID: 0x%08x with handle: 0x%08x, new thread count: 0x%x\n", te32.th32ThreadID, threads2[thread_count].handle, thread_count);
 
                }
 
        } while( Thread32Next(hThreadSnap, &te32 ) );

    //  Don't forget to clean up the snapshot object.
        CloseHandle( hThreadSnap );
    

    return 0x0;
}

int unset_ss(DWORD id)
{
    CONTEXT ctx;
    int i;
    DWORD tid;

    if(id == 0x0)
        for(i = 0x0; i< thread_count; i++)
        {
            tid = thread_list[i];
            if(threads2[tid].alive)
                unset_ss(tid);
        }
    else
    {
        HANDLE tHandle;
        ctx.ContextFlags = CONTEXT_CONTROL;
        tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, id);
//        printf("Enabling tracing on thread ID: 0x%x\n", id);

        GetThreadContext(tHandle, &ctx);
        ctx.EFlags &= CLEAR_SS_FLAGS;
        SetThreadContext(tHandle, &ctx);
    }
    return 0x0;
}

int set_ss(DWORD id)
{
    CONTEXT ctx;
    int i;
    DWORD tid;

    if(id == 0x0)
        for(i = 0x0; i< thread_count; i++)
        {
            tid = thread_list[i];
            if(threads2[tid].alive)
                set_ss(tid);
        }
    else
    {
        HANDLE tHandle;
        ctx.ContextFlags = CONTEXT_CONTROL;
        tHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, id);
//        printf("Enabling tracing on thread ID: 0x%x\n", id);

        GetThreadContext(tHandle, &ctx);
        ctx.EFlags |= SS_FLAGS;
        SetThreadContext(tHandle, &ctx);
    }
    return 0x0;
}

void ss_callback(void* data)
{
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD eip;
    DWORD bytes_written;
    int size = 0x0;
    char* disRet;
    int written;

    char bytes[0x2];

    eip = (DWORD)(de->u.Exception.ExceptionRecord.ExceptionAddress);

    //printf("0x%x 0x%08x\n", de->dwThreadId, eip);

    sprintf(line, "0x%x 0x%08x", de->dwThreadId, eip);

    add_to_buffer(line);
/*
    //sprintf(line, "%x\n", eip);
    size = strlen(line);
    if((index + size) >= buf_size)
    {
        SetEvent(eventLock);
        WaitForSingleObject(eventUnlock, INFINITE);
        index = 0;
    }

    written = sprintf(buffer + index, "%x\n", eip);

    if(written>0)
        index += written;
*/
    set_ss(de->dwThreadId);

    return;


    //eip = GetContextData(UE_EIP);

    if(eip == target_off)
        printf("Here\n");

    //eliminate rep
    if(eip == last_eip) 
    {
//        SingleStep(0x1, (LPVOID)&ss_callback);
        set_ss(0x0);
        return;
    }
    last_eip = eip;


    sprintf(line, "%x\n", eip);
    size = strlen(line);
    if((index + size) >= buf_size)
    {
        SetEvent(eventLock);
        WaitForSingleObject(eventUnlock, INFINITE);
        index = 0;
    }

    written = sprintf(buffer + index, "%x\n", eip);

    if(written>0)
        index += written;

//    SingleStep(0x1, (LPVOID)&ss_callback);
    set_ss(0x0);
    //SetCustomHandler(UE_CH_SINGLESTEP, (LPVOID)&ss_callback2);
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

    printf("Position before: 0x%08x\n", ftell(f));
    for(i=0x0; i<whole; i+= buf_size)
    {
        ReadProcessMemory(cpdi.hProcess, (void*)(from+i), (void*)mem_buf, buf_size, &read);
        printf("Read: 0x%08x\n", read);
        if(read == 0x0)
        {
            printf("Failed to read from %p to %p\nError: 0x%08x\n", from,(from+part), GetLastError());
            break;
        }

        fwrite(mem_buf, read, 0x1, f);
        wrote_total += read;
    }

    for(j=0x0; j<0x5; j++)
        printf("0x%02x ", mem_buf[j]);
    printf("\n");

    if(part > 0x0)
    {
        ReadProcessMemory(cpdi.hProcess, (void*)(from+i), (void*)mem_buf, part, &read);
        printf("Read: 0x%08x\n", read);
        if(read == 0x0)
        {
            printf("Failed to read from %p to %p\nError: 0x%08x\n", from,(from+part), GetLastError());
        }

        fwrite(mem_buf, read, 0x1, f);
        wrote_total += read;
    }

    for(j=0x0; j<0x5; j++)
        printf("0x%02x ", mem_buf[j]);
    printf("\n");

    printf("Position after: 0x%08x\n", ftell(f));

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

    printf("dumping mem start\n");
#ifdef MEM_DUMP
    f = fopen(dumpPath, "wb");

    MEMORY_BASIC_INFORMATION mbi;
    
    for(addr = 0x0; addr<0xffffffff; addr += mbi.RegionSize)
    {
        VirtualQueryEx(cpdi.hProcess, (void*)addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
        if(GetLastError() == ERROR_INVALID_PARAMETER)
            break;
        printf("Block 0x%08x - 0x%08x, 0x%08x bytes\n", mbi.BaseAddress, mbi.RegionSize + mbi.BaseAddress, mbi.RegionSize);
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
    printf("dumping mem end\n");
}

void dump_contexts()
{
    printf("Dumping contexts\n");

}

void end_callback(void* data)
{
    ss_callback(data);
    
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD eip;
    eip = (DWORD)de->u.Exception.ExceptionRecord.ExceptionAddress;

    printf("Finishing @ 0x%08x, detaching\n", eip);
    sprintf(line, "FI,0x%08x", eip);
    add_to_buffer(line);

    HandlerRoutine(0x0);

}

void bp_callback(void* data)
{
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;

    DWORD eip;
    eip = (DWORD)de->u.Exception.ExceptionRecord.ExceptionAddress;

    printf("Starting @ 0x%08x\n", eip);
    sprintf(line, "ST,0x%08x", eip);
    add_to_buffer(line);

    DWORD tid = de->dwThreadId;

    deregister_thread(tid, threads2[tid].handle);
    register_thread(tid, threads2[tid].handle);

    dump_memory();

    //add_breakpoint(nt1_off, ntmap_1_callback);
    //add_breakpoint(nt3_off, ntread_1_callback);
    add_breakpoint(sysenter_off, sysenter_callback);

    ss_callback(data);

}

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType)
{
    unsigned int i;
    DWORD tid;

    printf("Detected ctrl-c\n");
    
    printf("Deregistering threads\n");

    for(i=0x0; i<thread_count; i++)
    {
        tid = thread_list[i];
        if(threads2[tid].alive == 0x1) 
        {
            printf("Deregistering: 0x%08x\n", tid);
            deregister_thread(tid, threads2[tid].handle);
        }
    }

    printf("Deregistering libs\n");
    for(i=0x0; i<lib_count; i++)
    {
        if(libs[i].alive == 0x1) 
        {
            deregister_lib(i);
        }
    }


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
        printf("Flushed\n");
        SetEvent(eventUnlock);
    }
}

void default_handler(void *data)
{
    printf("Default handler!\n");
    return;
}

int del_breakpoint(DWORD addr)
{
    printf("Deleting breakpoint at: 0x%08x\n", addr);

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

    int i;
    int my_bpt_idx = -0x1;

    for(i = 0x0; i<my_bpt_count; i++)
        if(my_bpt[i].addr == addr)
        {
            my_bpt_idx = i;
            break;
        }

    if(my_bpt_idx == -0x1)
    {
        default_handler(0x0);
        return -0x1;
    }

    if(my_bpt[my_bpt_idx].enabled == 0x0)
    {
        return -0x1;
    }


    dec_eip(de->dwThreadId);
    printf("Hit breakpoint @ 0x%08x\n", addr);
    del_breakpoint(addr);

    my_bpt[my_bpt_idx].handler(data);


    //enable ss
    //set_ss(de->dwThreadId);
    return 0x0;
}

int add_breakpoint(DWORD addr, handler_routine handler)
{
    int i;
    int my_bpt_index;

    my_bpt_index = 0x0;

    for(i = 0x0; i<my_bpt_count; i++)
        if(my_bpt[i].addr == addr)
        {
            my_bpt_index = i;
        }
    if(my_bpt_index == 0x0) 
    {
        my_bpt_index = my_bpt_count;
        my_bpt_count ++;
    }

    DWORD oldProt;
    char bpt_char = '\xcc';
    printf("Adding breakpoint @ 0x%08x\n", addr);

    my_bpt[my_bpt_index].addr = addr;
    my_bpt[my_bpt_index].handler = handler;
   
    VirtualProtectEx(cpdi.hProcess, (void*)my_bpt[my_bpt_index].addr, 0x1, PAGE_EXECUTE_READWRITE, &oldProt);

    ReadProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_index].addr), (void*)(&my_bpt[my_bpt_index].saved_byte), 0x1, 0x0);
    WriteProcessMemory(cpdi.hProcess, (void*)my_bpt[my_bpt_index].addr, &bpt_char, 0x1, 0x0);

    VirtualProtectEx(cpdi.hProcess, (void*)my_bpt[my_bpt_index].addr, 0x1, oldProt, 0x0);
    my_bpt[my_bpt_index].enabled = 0x1;

    return 0x0;
}
FILE* configure_file()
{
    FILE* f;
    
    printf("Writing instructions to %s\n", path);
    f = fopen(path, "w+");
    if(f == 0x0)
    {
        printf("Error opening file %s: 0x%08x\n", path, GetLastError());
    }
    return f;
} 

void start_trace_fname()
{
    printf("Creating process: %s\n", process_fname);

    CreateProcess(process_fname, 0x0, 0x0, 0x0, 0x0, DEBUG_ONLY_THIS_PROCESS, 0x0, 0x0, &si, &pi);

    myPID = pi.dwProcessId;
}

void start_trace_pid()
{
    printf("Attaching debugger\n");

    if(DebugActiveProcess(myPID) != 0x0)
        printf("Successfully attached to PID: 0x%x, handle: 0x%x\n", myPID);
    else 
        printf("Attach failed\n");

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

int main(int argc, char** argv)
{

	if(argc < 8)
	{
		printf("You need to provide PID, start module & addr, end module & addr, out path and a prefix\n");
		printf("Or: filename, start module & addr & end module & addr, out path and a prefix\n");
		return -1;
	}

    eventLock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    eventUnlock = CreateEvent(0x0, 0x0, 0x0, 0x0);

    strcpy(mod_st, argv[2]);
    addr_st = strtol(argv[3], 0x0, 0x10);
    strcpy(mod_end, argv[4]);
	addr_end = strtol(argv[5], 0x0, 0x10);
    prefix = argv[7];

    sprintf(path, "%s\\%s_%s.txt", argv[6], prefix, "instr");
    sprintf(iniPath, "%s\\%s_%s.ini", argv[6], prefix, "ini");
    sprintf(dumpPath, "%s\\%s_%s.bin", argv[6], prefix, "dump");
    sprintf(modPath, "%s\\%s_%s.bin", argv[6], prefix, "mods");

    SetConsoleCtrlHandler(HandlerRoutine, TRUE);

    file = configure_file();
    modifications = fopen(modPath, "wb+");

    DWORD threadId = 0x0;
    DWORD main_addr = 0x0;
    char main_addr_str[0x20];

    WritePrivateProfileString("general", "dumpPath", dumpPath, iniPath);
    WritePrivateProfileString("general", "instrPath", path, iniPath);

    /* syscall out args table */
    unsigned i, j;

    for(i=0x0; i<MAX_SYSCALL_ENTRIES; i++)
        for(j=0x0;j<MAX_SYSCALL_OUT_ARGS; j++)
            syscall_out_args[i][j].off_location = LOCATION_END;

    /* instructions on handling memory modifications made by sysenter */
    
    // ZwAllocateVirtualMemory
    syscall_out_args[0x13][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY}; 
    syscall_out_args[0x13][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x13][2] = {0x1, 0x3, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0}; 
    syscall_out_args[0x13][3] = last_arg;
   
    // ZwClose
    syscall_out_args[0x32][0] = last_arg;

    // ZwCreateFile
    syscall_out_args[0x42][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x42][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x42][2] = last_arg;

    // ZwCreateSection 
    syscall_out_args[0x54][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x54][1] = last_arg;
   
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

    // NtOpenFile
    syscall_out_args[0xb3][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb3][1] = {0x3, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb3][2] = last_arg;

    // ZwOpenKey
    syscall_out_args[0xb6][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xb6][1] = last_arg;

    // ZwOpenProcessToken
    syscall_out_args[0xbf][0] = {0x2, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xbf][1] = last_arg;

    // ZwOpenSection
    syscall_out_args[0xc2][0] = {0x0, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xc2][1] = last_arg;

    // ZwProtectVirtualMemory
    syscall_out_args[0xd7][0] = {0x1, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xd7][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xd7][2] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xd7][3] = last_arg;

    // NtQueryAttributesFile
    syscall_out_args[0xd9][0] = {0x0, sizeof(FILE_BASIC_INFORMATION), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0xd9][1] = last_arg;

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

    // ZwQueryValueKey
    syscall_out_args[0x10a][0] = {0x3, 0x5, LOCATION_ADDR_STACK, LOCATION_ADDR_STACK, 0};
    syscall_out_args[0x10a][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x10a][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x10a][3] = last_arg;

    // ZwReadFile
    syscall_out_args[0x111][0] = {0x4, sizeof(IO_STATUS_BLOCK), LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x111][1] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, STATUS_ANY};
    syscall_out_args[0x111][2] = {0x5, 0x6, LOCATION_ADDR_STACK, LOCATION_STACK, 0};
    syscall_out_args[0x111][3] = last_arg;

    // ZwRequestWaitReplyPort
    syscall_out_args[0x12b][0] = {0x2, 0x148, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x12b][1] = last_arg;

    // ZwUnmapViewOfSection
    syscall_out_args[0x181][0] = last_arg;

    // ZwTraceControl
    syscall_out_args[0x177][0] = {0x3, 0x4, LOCATION_ADDR_ADDR_STACK, LOCATION_ADDR_STACK, 0x0};
    syscall_out_args[0x177][1] = {0x3, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x177][2] = {0x5, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x177][3] = last_arg;

    // ZwWriteVirtualMemory
    syscall_out_args[0x18f][0] = {0x4, 0x4, LOCATION_ADDR_STACK, LOCATION_CONST, 0x0};
    syscall_out_args[0x18f][1] = last_arg;

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

    thread_count = 0x0;
    while(1)
    {
        status = DBG_CONTINUE;

        WaitForDebugEvent(&de, INFINITE);
        
        //printf("dwDebugEventCode: 0x%08x\n", de.dwDebugEventCode);
        
        switch(de.dwDebugEventCode)
        {
            case CREATE_PROCESS_DEBUG_EVENT:
                cpdi = de.u.CreateProcessInfo;

                // register main thread
                register_thread(de.dwThreadId, de.u.CreateProcessInfo.hThread);

                //configure breakpoints
                if(strstr(mod_st, "0x0"))
                {
                    add_breakpoint(addr_st + (DWORD)de.u.CreateProcessInfo.lpBaseOfImage, bp_callback);
                }
                if(strstr(mod_end, "0x0"))
                {
                    add_breakpoint(addr_end + (DWORD)de.u.CreateProcessInfo.lpBaseOfImage, end_callback);
                }

                break;

            case EXCEPTION_DEBUG_EVENT:
                EXCEPTION_RECORD er;
                status = DBG_EXCEPTION_NOT_HANDLED;
                er = de.u.Exception.ExceptionRecord;
//                printf("Exception: 0x%08x\n", er.ExceptionCode);
//                printf("at: 0x%08x\n", er.ExceptionAddress);

                switch(er.ExceptionCode)
                {
                    case EXCEPTION_SINGLE_STEP:
                        ss_callback((void*)&de);
                        status = DBG_CONTINUE;
                        break;
                
                    case EXCEPTION_BREAKPOINT:
                        handle_breakpoint((DWORD)er.ExceptionAddress, &de);
                        status = DBG_CONTINUE;
                        break;
                    default:
                        register_exception(de.dwThreadId, er);
                        break;
                }

                break;

            case CREATE_THREAD_DEBUG_EVENT: 
                register_thread(de.dwThreadId, de.u.CreateThread.hThread);
                break;

            case LOAD_DLL_DEBUG_EVENT:
                printf("Handle: 0x%08x\n", de.u.LoadDll.hFile);
                register_lib(de.u.LoadDll);
                if(strstr(libs[lib_count].lib_name, mod_st))
                {
                    add_breakpoint(addr_st + find_lib(mod_st), bp_callback);
                }
                if(strstr(libs[lib_count-1].lib_name, mod_end))
                {
                    add_breakpoint(addr_end + find_lib(mod_end), end_callback);
                }
                if(strstr(libs[lib_count-1].lib_name, "ntdll.dll"))
                {
                    printf("ntdll loaded\n");
                    nt1_off = find_lib("ntdll.dll") + NTMAPVIEWOFSECTION_1;
                    nt2_off = find_lib("ntdll.dll") + NTMAPVIEWOFSECTION_2;
                    nt3_off = find_lib("ntdll.dll") + NTREADFILE_OFF_1;
                    nt4_off = find_lib("ntdll.dll") + NTREADFILE_OFF_2;
                    sysenter_off = find_lib("ntdll.dll") + NTSYSENTER_OFF;
                    sysret_off = find_lib("ntdll.dll") + NTSYSRET_OFF;
                }
                break;

            case UNLOAD_DLL_DEBUG_EVENT:
                deregister_lib(de.u.UnloadDll);
                break;

            case EXIT_THREAD_DEBUG_EVENT:
                deregister_thread(de.dwThreadId, de.u.CreateThread.hThread);
                threads2[de.dwThreadId].alive = 0x0;

                if(threads2[de.dwThreadId].handle != 0x0) 
                    CloseHandle(threads2[de.dwThreadId].handle);

                threads2[de.dwThreadId].handle = 0x0;
                threads2[de.dwThreadId].open = 0x0;
                break;
        }

        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, status);
    }


    CloseHandle(file);
    CloseHandle(modifications);

    return 0;
}
