// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#define _WIN32_WINNT 0x0500
#include <stdio.h>
#include <windows.h>
#include "TitanEngine_x86.h"
#define buf_size 0x100000

DWORD addr_st, addr_end;
HANDLE pipe;
FILE* file;
char buffer[buf_size];
int index = 0;
HANDLE eventLock, eventUnlock;

char path[0x200];
char dumpPath[0x200];
char iniPath[0x200];


char line[0x100];
char instr[0x60];
int last_eip = 0x0;

void ss_callback();

void ss_callback()
{
    DWORD eip;
    DWORD bytes_written;
    int size = 0x0;
    char* disRet;

    eip = GetContextData(UE_EIP);
 
    //eliminate rep
    if(eip == last_eip) 
    {
        SingleStep(0x1, (LPVOID)&ss_callback);
        return;
    }
    last_eip = eip;


//    printf("%x\n", eip);
    sprintf(line, "%x\n", eip);
//    printf("1\n");
//    disRet = (char*)StaticDisassemble((LPVOID)eip);
//    if(disRet != 0x0)
//    {
//        printf("2\n");
//        sprintf(line, "%x: %s\n", eip, disRet);
//    }
//   else 
//    {
//        printf("3\n");
//        sprintf(line, "%x: [unkn]\n", eip);
//    }
//    printf("4\n");
//    printf("%s", line);


    size = strlen(line);
//    WriteFile(pipe, line, strlen(line), &bytes_written, 0x0);
//    bytes_written = fprintf(file, "%x\n", eip);
//        printf("1index: 0x%x\n", index);
    if((index + size) >= buf_size)
    {
//        printf("passed size, index: %d\n", index);
//        printf("Locked, need to flush\n");
        SetEvent(eventLock);
        WaitForSingleObject(eventUnlock, INFINITE);
//        printf("Unlocked, continuing\n");
        index = 0;
//        printf("index: %d\n", index);
    }

//    printf("%x\n", eip);
    int written = sprintf(buffer + index, "%x\n", eip);
//    printf("written: %d\n", written);

    if(written>0)
        index += written;

//        printf("2index: 0x%x\n", index);
    SingleStep(0x1, (LPVOID)&ss_callback);
}

DWORD WINAPI writer(LPVOID lpParam)
{
    while(1)
    {
//        printf("Waiting for flush signal\n");
        WaitForSingleObject(eventLock, INFINITE);
        ResetEvent(eventLock);
//        printf("index: %d, buffer: %c\n", index, buffer);
//        printf("Flushing with index: %x\n", index);
        fwrite(buffer, index, 1, file);
        fflush(file);
        printf("Flushed\n");
//        printf("Reset & unlock\n");
        SetEvent(eventUnlock);
    }
}

void bp_callback()
{
    printf("Started! Single stepping...\n");
    SingleStep(0x1, (LPVOID)&ss_callback);
}

void lib_callback(LPVOID fLibraryDetail)
{
    LIBRARY_ITEM_DATA* lid;
    lid = (LIBRARY_ITEM_DATA*)fLibraryDetail;

    printf("Name: %S, address: %x\n", lid->szLibraryPath, lid->BaseOfDll);
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

void dump_memory()
{
    printf("dumping mem start\n");
    DumpMemory(0x0, 0x0, 0xffffffff, dumpPath);
    printf("dumping mem end\n");
}

void dump_context()
{

}

void dbg_callback()
{
    DWORD threadId = 0x0;
    DWORD main_addr = 0x0;
    char main_addr_str[0x20];


    printf("Attached!\n");

    WritePrivateProfileString("general", "dumpPath", dumpPath, iniPath);
    WritePrivateProfileString("general", "instrPath", path, iniPath);

    //gather information

    //dump
//    dump_memory();

    //module
    main_addr = GetDebuggedFileBaseAddress();
    printf("Debugee module loaded at: %x\n", main_addr);
    sprintf(main_addr_str, "%08x", main_addr);

    WritePrivateProfileString("modules", "main", main_addr_str, iniPath);

    //libraries
//    LibrarianEnumLibraryInfo((LPVOID)&lib_callback);

    //enumerate threads
//    ThreaderEnumThreadInfo((LPVOID)&thread_callback);

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
    PROCESS_INFORMATION pi;

	if(argc < 5)
	{
		printf("You need to provide PID, start addr & end addr and a prefix\n");
		return -1;
	}

//    pipe = configure_com();
    eventLock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    eventUnlock = CreateEvent(0x0, 0x0, 0x0, 0x0);

    myPID = atoi(argv[1]);
    addr_st = strtol(argv[2], 0x0, 0x10);
	addr_end = strtol(argv[3], 0x0, 0x10);
    prefix = argv[4];

    sprintf(path, "c:\\%s_%s.txt", prefix, "instr");
    sprintf(iniPath, "c:\\%s_%s.txt", prefix, "ini");
    sprintf(dumpPath, "c:\\%s_%s.txt", prefix, "dump");

    file = configure_file();

    printf("Attach debugger\n");
    if(AttachDebugger(myPID, 0x1, &pi, (LPVOID)&dbg_callback) == true)
        printf("Successful\n");
    else 
        printf("Attach failed\n");

    printf("Setting breakpoint\n");


    CloseHandle(file);

    return 0;
}
