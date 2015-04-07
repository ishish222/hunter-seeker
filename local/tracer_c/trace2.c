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

void ss_callback();

void ss_callback()
{
    DWORD eip;
    DWORD bytes_written;
    char line[0x100];
    int size = 0x0;

    eip = GetContextData(UE_EIP);

//    printf("%x\n", eip);
    sprintf(line, "%x\n", eip);
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

void dbg_callback()
{
    DWORD threadId;

    printf("Attached!\n");

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
    char path[] = "e:\\instr.txt";
    FILE* f;
    
    f = fopen(path, "w+");
    return f;
} 

int main(int argc, char** argv)
{
    bool res = 0x0;
    int myPID = 0x0;
    PROCESS_INFORMATION pi;

	if(argc < 4)
	{
		printf("You need to provide PID, start addr & end addr\n");
		return -1;
	}

//    pipe = configure_com();
    eventLock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    eventUnlock = CreateEvent(0x0, 0x0, 0x0, 0x0);

    file = configure_file();

    myPID = atoi(argv[1]);
    addr_st = strtol(argv[2], 0x0, 0x10);
	addr_end = strtol(argv[3], 0x0, 0x10);

    printf("Attach debugger\n");
    if(AttachDebugger(myPID, 0x1, &pi, (LPVOID)&dbg_callback) == true)
        printf("Successful\n");
    else 
        printf("Attach failed\n");

    printf("Setting breakpoint\n");


    CloseHandle(file);

    return 0;
}
