// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#define _WIN32_WINNT 0x0500
#include <stdio.h>
#include <windows.h>
#include "TitanEngine_x86.h"
#include "winternl.h"
#define buf_size 0x100000

DWORD addr_st, addr_end;
HANDLE pipe;
FILE* file;
char buffer[buf_size];
int index = 0;
HANDLE eventLock, eventUnlock;
PROCESS_INFORMATION pi;

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

void dump_memory()
{
    printf("dumping mem start\n");
    //DumpMemory(pi.hProcess, 0x0, 0xffffffff, dumpPath);
    printf("dumping mem end\n");
}

void dump_contexts()
{
    printf("Dumping contexts\n");

}

void ss_callback2(void* exception_data)
{
  //  PEXCEPTION_RECORD pe;

//    pe = exception_data;

    printf("Got it: 0x%08x!\n", GetCurrentThreadId());
}

void ss_callback()
{
    DWORD eip;
    DWORD bytes_written;
    int size = 0x0;
    char* disRet;

    eip = GetContextData(UE_EIP);

    if(eip == target_off)
        printf("Here\n");

    //eliminate rep
    if(eip == last_eip) 
    {
        SingleStep(0x1, (LPVOID)&ss_callback);
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

    int written = sprintf(buffer + index, "%x\n", eip);

    if(written>0)
        index += written;

    SingleStep(0x1, (LPVOID)&ss_callback);
    SetCustomHandler(UE_CH_SINGLESTEP, (LPVOID)&ss_callback2);
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

void target_callback()
{
    printf("In target callback\n");

    DWORD esp;
    DWORD* arg3p;
    DWORD arg3;
    char buff[0x20];
    char ptr[0x4];
    THREAD_ITEM_DATA tid;
    POBJECT_ATTRIBUTES oa;
    PUNICODE_STRING us;

    esp = GetContextDataEx(pi.hThread, UE_ESP);
    esp += 0xc; //3rd arg

    printf("arg3p: 0x%x\n", esp);
    ReadProcessMemory(pi.hProcess, (void*)esp, ptr, 0x4, 0x0);

    printf("1\n");
    arg3p = (DWORD*)ptr;
    arg3 = *arg3p;
    printf("arg3: 0x%x\n", arg3);
    printf("2\n");
    oa = (POBJECT_ATTRIBUTES)arg3;
    printf("3\n");
    printf("oa: 0x%x\n", &oa->ObjectName);
    us = oa->ObjectName;
    printf("4\n");
    printf("%x\n", us);

    printf("%ls\n", *us);
    printf("5\n");


}

void bp_callback()
{
    printf("Started! Single stepping...\n");
    dump_memory();
    SingleStep(0x1, (LPVOID)&ss_callback);

    SetCustomHandler(UE_CH_SINGLESTEP, (LPVOID)&ss_callback2);
/*
        if(SetBPX(target_lib_off + target_call_off, UE_BREAKPOINT, (LPVOID)&target_callback) == true)
            printf("Bp target at 0x%x\n", target_lib_off + target_call_off);
        else
            printf("Error\n");
*/
}

void lib_callback(LPVOID fLibraryDetail)
{
    LIBRARY_ITEM_DATAW* lid;
    lid = (LIBRARY_ITEM_DATAW*)fLibraryDetail;


    printf("Name: %S, address: %x\n", lid->szLibraryPath, lid->BaseOfDll);

    if(wcsstr(lid->szLibraryPath, target_lib) != 0x0)
    {
        printf("Found target lib\n");
        target_lib_off = (DWORD)lid->BaseOfDll;
        target_off = target_lib_off + target_call_off;

    }
        
        
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

void write_context(DWORD tid, CONTEXT* ctx)
{
    char temp[0x20];
    char tid_s[0x20];

    sprintf(tid_s, "tid_%08x", tid);

    sprintf(temp, "%08x", ctx->Eax);
    WritePrivateProfileString(tid_s, "EAX", temp, iniPath);
    sprintf(temp, "%08x", ctx->Ebx);
    WritePrivateProfileString(tid_s, "EBX", temp, iniPath);
    sprintf(temp, "%08x", ctx->Ecx);
    WritePrivateProfileString(tid_s, "ECX", temp, iniPath);
    sprintf(temp, "%08x", ctx->Edx);
    WritePrivateProfileString(tid_s, "EDX", temp, iniPath);
    sprintf(temp, "%08x", ctx->Esi);
    WritePrivateProfileString(tid_s, "ESI", temp, iniPath);
    sprintf(temp, "%08x", ctx->Edi);
    WritePrivateProfileString(tid_s, "EDI", temp, iniPath);
    sprintf(temp, "%08x", ctx->Ebp);
    WritePrivateProfileString(tid_s, "EBP", temp, iniPath);
    sprintf(temp, "%08x", ctx->Esp);
    WritePrivateProfileString(tid_s, "ESP", temp, iniPath);
    sprintf(temp, "%08x", ctx->EFlags);
    WritePrivateProfileString(tid_s, "EFLAGS", temp, iniPath);
    sprintf(temp, "%08x", ctx->Eip);
    WritePrivateProfileString(tid_s, "EIP", temp, iniPath);

}

void thread_callback(LPVOID fThreadDetail)
{
    DWORD eax;
    CONTEXT ctx;

    THREAD_ITEM_DATA* tid;
    tid = (THREAD_ITEM_DATA*)fThreadDetail;

    //debug regs
    ctx.Dr0 = GetContextDataEx(tid->hThread, UE_DR0);
    ctx.Dr1 = GetContextDataEx(tid->hThread, UE_DR1);
    ctx.Dr2 = GetContextDataEx(tid->hThread, UE_DR2);
    ctx.Dr3 = GetContextDataEx(tid->hThread, UE_DR3);
    ctx.Dr6 = GetContextDataEx(tid->hThread, UE_DR6);
    ctx.Dr7 = GetContextDataEx(tid->hThread, UE_DR7);

    //floating area

    //seg regs
    ctx.SegGs = GetContextDataEx(tid->hThread, UE_SEG_GS);
    ctx.SegFs = GetContextDataEx(tid->hThread, UE_SEG_FS);
    ctx.SegEs = GetContextDataEx(tid->hThread, UE_SEG_ES);
    ctx.SegDs = GetContextDataEx(tid->hThread, UE_SEG_DS);

    //general purpose
    ctx.Edi = GetContextDataEx(tid->hThread, UE_EDI);
    ctx.Esi = GetContextDataEx(tid->hThread, UE_ESI);
    ctx.Ebx = GetContextDataEx(tid->hThread, UE_EBX);
    ctx.Edx = GetContextDataEx(tid->hThread, UE_EDX);
    ctx.Ecx = GetContextDataEx(tid->hThread, UE_ECX);
    ctx.Eax = GetContextDataEx(tid->hThread, UE_EAX);

    //control
    ctx.Ebp = GetContextDataEx(tid->hThread, UE_EBP);
    ctx.Eip = GetContextDataEx(tid->hThread, UE_EIP);
    ctx.SegCs = GetContextDataEx(tid->hThread, UE_SEG_CS);
    ctx.EFlags = GetContextDataEx(tid->hThread, UE_EFLAGS);
    ctx.Esp = GetContextDataEx(tid->hThread, UE_ESP);
    ctx.SegSs = GetContextDataEx(tid->hThread, UE_SEG_SS);

    //extended

    print_context(&ctx);
    write_context(tid->dwThreadId, &ctx);

}

void dump_context()
{

}

void handle_NtCreateFile()
{
    DWORD esp;
    printf("NtCreateFile called\n");
    //printf("Thread: 0x%x:\n", );
    printf("Stack dump:\n");
//    POBJECT_ATTRIBUTES pa;
//    PUNICODE_STRING name;

//    pa = esp + 0x4; //verify
//    name = pa->ObjectName;
    
    //verify OBJECT name (3rd arg)
    //if name is correct:
    //get address of out handle (1st arg)
    //wait until return
    //get handle  

}

void dbg_callback()
{
    DWORD threadId = 0x0;
    DWORD main_addr = 0x0;
    char main_addr_str[0x20];


    printf("Attached!\n");
    printf("Successfully attached to PID: 0x%x, handle: 0x%x\n", pi.dwProcessId, pi.hProcess);

    WritePrivateProfileString("general", "dumpPath", dumpPath, iniPath);
    WritePrivateProfileString("general", "instrPath", path, iniPath);

    //module
    main_addr = GetDebuggedFileBaseAddress();
    printf("Debugee module loaded at: %x\n", main_addr);
    sprintf(main_addr_str, "%08x", main_addr);

    WritePrivateProfileString("modules", "main", main_addr_str, iniPath);

    //libraries
    LibrarianEnumLibraryInfo((LPVOID)&lib_callback);

    //enumerate threads
    ThreaderEnumThreadInfo((LPVOID)&thread_callback);

    //attach hooks on functions that modify memory
    /*

NtEnumerateKey
NtEnumerateValueKey
NtLoadDriver
NtLoadKey
NtLoadKey2
NtLoadKeyEx
NtOpenFile
NtQueryKey
NtQuerySection
NtQueryValueKey

//most important
NtOpenFile
NtReadFile
NtMapViewOfSection


NtReadFileScatter

NtReadFile


*/

    if(SetBPX(addr_st, UE_BREAKPOINT, (LPVOID)&bp_callback) == true)
        printf("Bp successful\n");
    else 
        printf("Break failed\n");

    CreateThread(0x0, 0x0, (LPTHREAD_START_ROUTINE)&writer, 0x0, 0x0, &threadId);
}

HANDLE configure_com()
{
	char com_name[] = "COM1";
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS cTimeouts = {0};
    HANDLE h;

	h = CreateFileA(com_name, GENERIC_READ | GENERIC_WRITE, 0x0, 0x0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0x0);

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	dcbSerialParams.BaudRate = CBR_256000;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	SetCommState(h, &dcbSerialParams);

    return h;
}

FILE* configure_file()
{
    FILE* f;
    
    printf("Writing instructions to %s\n", path);
    f = fopen(path, "w+");
    return f;
} 

int main(int argc, char** argv)
{
    bool res = 0x0;
    int myPID = 0x0;
    char* prefix;

	if(argc < 5)
	{
		printf("You need to provide PID or file path, start module & addr,  end module & addr, output dir and a prefix\n");
		return -1;
	}

//    pipe = configure_com();
    eventLock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    eventUnlock = CreateEvent(0x0, 0x0, 0x0, 0x0);

    myPID = atoi(argv[1]);
    addr_st = strtol(argv[2], 0x0, 0x10);
	addr_end = strtol(argv[3], 0x0, 0x10);
    prefix = argv[5];

    sprintf(path, "c:\\%s_%s.txt", prefix, "instr");
    sprintf(iniPath, "c:\\%s_%s.txt", prefix, "ini");
    sprintf(dumpPath, "c:\\%s_%s.txt", prefix, "dump");
    swprintf(filePath, 100, L"%hs", argv[4]);


    file = configure_file();

    printf("Attach debugger\n");
    if(AttachDebugger(myPID, 0x1, &pi, (LPVOID)&dbg_callback) == true)
        printf("Successfully attached to PID: 0x%x, handle: 0x%x\n", pi.dwProcessId, pi.hProcess);
    else 
        printf("Attach failed\n");

    printf("Setting breakpoint\n");


    CloseHandle(file);

    return 0;
}
