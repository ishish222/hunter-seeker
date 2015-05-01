// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#define _WIN32_WINNT 0x0500
#include <stdio.h>
#include <shlwapi.h>
#include <windows.h>
#include "TitanEngine_x86.h"
#define buf_size 0x100000
//#define buf_size 0x100


DWORD addr_st, addr_end;
HANDLE pipe;
FILE* file;
char buffer[buf_size];
int index = 0;
HANDLE eventLock, eventUnlock;

void ss_callback();

char line[0x100];
char instr[0x60];
int last_eip = 0x0;

void exception_callback(CREATE_THREAD_DEBUG_INFO* CreateThread)
{
    DWORD dwThreadId = ((DEBUG_EVENT*)GetDebugData())->dwThreadId;
    printf("New thread: 0x%x\n", dwThreadId);
}


void ss_callback()
{
    DWORD eip;
    DWORD bytes_written;
    int size = 0x0;

    eip = GetContextData(UE_EIP);
    if(eip == last_eip) 
    {
        SingleStep(0x1, (LPVOID)&ss_callback);
        return;
    }
    last_eip = eip;

    sprintf(instr, "%s", (char*)Disassemble((LPVOID)eip));
    if(instr !=0x0)
        sprintf(line, "%x: %s\n", eip, instr);
   else 
        sprintf(line, "%x: [unkn]\n", eip);
    printf("%s", line);
 
//    sprintf(line, "%x: %s", eip, instr);
//    printf("3\n");
//    printf("%s\n", line);
//    printf("4\n");
    size = strlen(line);
//    printf("5\n");
//    WriteFile(pipe, line, strlen(line), &bytes_written, 0x0);
//    bytes_written = fprintf(file, "%x\n", eip);
//        printf("1index: 0x%x\n", index);
    if((index + size) >= buf_size)
    {
//        printf("passed size, index: %d\n", index);
        printf("Locked, need to flush\n");
        SetEvent(eventLock);
        WaitForSingleObject(eventUnlock, INFINITE);
//        printf("Unlocked, continuing\n");
        index = 0;
//        printf("index: %d\n", index);
    }

//    printf("%x\n", eip);
    int written = sprintf(buffer + index, "%s", line);
//    printf("written: %d\n", written);

    if(written>0)
        index += written;

//        printf("2index: 0x%x\n", index);
    SingleStep(0x1, (LPVOID)&ss_callback);
}

DWORD WINAPI writer(LPVOID lpParam)
{
    DWORD bytes_written;
    while(1)
    {
//        printf("Waiting for flush signal\n");
        WaitForSingleObject(eventLock, INFINITE);
        ResetEvent(eventLock);
//        printf("index: %d, buffer: %c\n", index, buffer);
//        printf("Flushing with index: %x\n", index);
        fwrite(buffer, index, 1, file);
        fflush(file);
        //WriteFile(pipe, buffer, index, &bytes_written, 0x0);
        //FlushFileBuffers(pipe);
//        printf("Flushed\n");
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
    char path[] = "c:\\instr.txt";
    FILE* f;
    
    f = fopen(path, "w+");
    return f;
} 

int main(int argc, char** argv)
{
    bool res = 0x0;
    int myPID = 0x0;
    char myPath[0x100];
    char param[0x100];
    char startFolder[0x100];
    DWORD threadId;
/*
    long long ib;
    long long oep;
    long long ebp;
*/
    DWORD ib;
    DWORD oep;
    DWORD ebp;

    PROCESS_INFORMATION* pi;

	//if((argc < 3) || (strlen(argv[1]) >= 0x100))
	if(argc < 3)
	{
		printf("You need to provide path to executable\n");
		return -1;
	}

    file = configure_file();
    strcpy(myPath, argv[1]);
    strcpy(startFolder, argv[1]);
    PathRemoveFileSpec(startFolder);
    param[0x0] = '\0';

    printf("Module: %s\nParams: %s\nCmdline: %s\n", myPath, myPath, startFolder);

//    addr_st = 0x1012475;
    addr_st = strtol(argv[2], 0x0, 0x10);

    //pipe = configure_com();

//    printf("Pipe handle: 0x%x\n", pipe);

    eventLock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    eventUnlock = CreateEvent(0x0, 0x0, 0x0, 0x0);
    CreateThread(0x0, 0x0, (LPTHREAD_START_ROUTINE)&writer, 0x0, 0x0, &threadId);

    SetCustomHandler(UE_CH_CREATETHREAD, (LPVOID)&ss_callback);



    if(InitDebugEx(myPath, myPath, startFolder, (LPVOID)&ss_callback) != 0x0)
        printf("Created process\n");
    else 
    {
        printf("Creation failed\n");
        printf("Module: %s\nParams: %s\nCmdline: %s\n", myPath, myPath, startFolder);
    }



//    myPID = atoi(argv[1]);
//    addr_st = strtol(argv[2], 0x0, 0x10);
//	addr_end = strtol(argv[3], 0x0, 0x10);
/*
    printf("Getting PE data\n");
    ib = GetPE32Data(myPath, 0x0, UE_IMAGEBASE);
    printf("ImageBase: 0x%x\n", ib);
    oep = GetPE32Data(myPath, 0x0, UE_OEP);
    printf("Entry point: 0x%x\n", oep);

    ebp = ib+oep;
    printf("Will start tracing at: 0x%x\n", ebp);

    printf("Creating process\n");
  */  
//    strcpy(myPath, argv[1]);
//    strcpy(startFolder, argv[2]);
//    param[0x0] = '\0';


/*
    if(SetBPX(ebp, UE_BREAKPOINT, (LPVOID)&dbg_callback) == true)
        printf("Bp successful\n");
    else 
        printf("Break failed\n");
*/
/*
    if(AttachDebugger(myPID, 0x1, &pi, (LPVOID)&dbg_callback) == true)
        printf("Successful\n");
    else 
        printf("Attach failed\n");

    printf("Setting breakpoint\n");
*/

    DebugLoop();

    printf("Finished\n");
    CloseHandle(file);

    return 0;
}
