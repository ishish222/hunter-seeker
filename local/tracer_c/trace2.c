#define _WIN32_WINNT 0x0700

#include <stdio.h>
#include <windows.h>
#include "WinBase.h"
#include <TlHelp32.h>
#include "winternl.h"

#define buf_size 0x100000
#define SS_FLAGS 0x100;
//#define MEM_DUMP
#define EXIT_PROCESS_OFF 0x52acf
#define MAX_NAME 0x200

char process_fname[0x200];
char spawned = 0x0;


HANDLE procHandle;

DWORD addr_st, addr_end;
char mod_st[MAX_NAME];
char mod_end[MAX_NAME];
HANDLE pipe;
FILE* file;
char buffer[buf_size];
int index = 0;
HANDLE eventLock, eventUnlock;
PROCESS_INFORMATION pi;
STARTUPINFO si;


char path[0x200];
char dumpPath[0x200];
char iniPath[0x200];
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

typedef struct _CREATE_THREAD_DEBUG_INFO2
{
    CREATE_THREAD_DEBUG_INFO u;
    DWORD tid;
} CREATE_THREAD_DEBUG_INFO2;

typedef struct _THREAD_ENTRY
{
    char alive;
    char open;
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

typedef struct _bpt
{
    DWORD addr;
    char saved_byte;
    handler_routine handler;    
    char enabled;
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
    printf("Decreasing eip\n");

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

    myHandle = OpenThread(THREAD_GET_CONTEXT |THREAD_SET_CONTEXT | THREAD_ALL_ACCESS, 0x0, id);
    printf("Thread ID: 0x%x, handle: 0x%x\n", id, myHandle);
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_CONTROL;
    if(GetThreadContext(myHandle, &ctx) == 0x0) printf("Failed to get context\n");
    else printf("Success\n");
    printf("Before: 0x%x\n", ctx.Eip);
    ctx.Eip -= 0x1;
    printf("After: 0x%x\n", ctx.Eip);
    SetThreadContext(myHandle, &ctx);

    CloseHandle(myHandle);
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

    threads2[tid].alive = 0x1;
    threads2[tid].handle = handle;
    threads2[tid].open = 0x1;

    //if(threads2[tid].handle == 0x0) 
    printf("Registering: TID 0x%08x, handle 0x%08x\n", tid, handle);

    thread_list[thread_count] = tid;
    thread_count ++;

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

    if(threads2[tid].handle != 0x0) 
        CloseHandle(threads2[tid].handle);

    threads2[tid].handle = 0x0;
    threads2[tid].open = 0x0;
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

    char* path2;
    if(info.lpImageName != 0x0) path2 = *(char**)info.lpImageName;
    else printf("Zero1\n");
    if(path2 != 0x0)
    {
        if(info.fUnicode)
            printf("%S\n", path2);
        else
            printf("%s\n", path2);
    }
    else printf("Zero2\n");

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

    for(i=0x0; i<whole; i+= buf_size)
    {
        ReadProcessMemory(cpdi.hProcess, (void*)(from+i), (void*)mem_buf, buf_size, &read);
        if(read == 0x0)
        {
            break;
        }
        fwrite(mem_buf, read, 0x1, f);
        wrote_total += read;
    }
    if(part > 0x0)
    {
        ReadProcessMemory(cpdi.hProcess, (void*)(from+i), (void*)mem_buf, part, &read);
        if(read == 0x0)
        {
            printf("Failed to read from %p to %p\nError: 0x%08x\n", from,(from+part), GetLastError());
        }
        fwrite(mem_buf, read, 0x1, f);
        wrote_total += read;
    }

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
    printf("Finished tracing, detaching\n");
    HandlerRoutine(0x0);
    //DebugSetProcessKillOnExit(FALSE);
    //exit(0);
    


}

void bp_callback(void* data)
{
    DEBUG_EVENT* de;
    de = (DEBUG_EVENT*)data;
    DWORD tid = de->dwThreadId;

    printf("Started! Single stepping...\n");

    deregister_thread(tid, threads2[tid].handle);
    register_thread(tid, threads2[tid].handle);

    dump_memory();
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

    printf("Found, deleting\n");

    VirtualProtectEx(cpdi.hProcess, (void*)(my_bpt[my_bpt_idx].addr), 0x1, PAGE_EXECUTE_READWRITE, &oldProt);

    ReadProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_count].addr), (void*)(&bpt_char), 0x1, 0x0);
    WriteProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_idx].addr), &(my_bpt[my_bpt_idx].saved_byte), 0x1, 0x0);

    printf("before: 0x%02x, now: 0x%02x\n", bpt_char, my_bpt[my_bpt_idx].saved_byte); 
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
    del_breakpoint(addr);

    my_bpt[my_bpt_idx].handler(data);


    return 0x0;
}

int add_breakpoint(DWORD addr, handler_routine handler)
{
    DWORD oldProt;
    char bpt_char = '\xcc';
    printf("Adding breakpoint @ 0x%08x\n", addr);

    my_bpt[my_bpt_count].addr = addr;
    my_bpt[my_bpt_count].handler = handler;
   
    VirtualProtectEx(cpdi.hProcess, (void*)my_bpt[my_bpt_count].addr, 0x1, PAGE_EXECUTE_READWRITE, &oldProt);

    ReadProcessMemory(cpdi.hProcess, (void*)(my_bpt[my_bpt_count].addr), (void*)(&my_bpt[my_bpt_count].saved_byte), 0x1, 0x0);
    WriteProcessMemory(cpdi.hProcess, (void*)my_bpt[my_bpt_count].addr, &bpt_char, 0x1, 0x0);

    VirtualProtectEx(cpdi.hProcess, (void*)my_bpt[my_bpt_count].addr, 0x1, oldProt, 0x0);
    printf("before: 0x%02x, now: 0x%02x\n", my_bpt[my_bpt_count].saved_byte, bpt_char); 
    my_bpt[my_bpt_count].enabled = 0x1;

    my_bpt_count ++;

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

    SetConsoleCtrlHandler(HandlerRoutine, TRUE);

    file = configure_file();

    DWORD threadId = 0x0;
    DWORD main_addr = 0x0;
    char main_addr_str[0x20];

    WritePrivateProfileString("general", "dumpPath", dumpPath, iniPath);
    WritePrivateProfileString("general", "instrPath", path, iniPath);

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

    return 0;
}

