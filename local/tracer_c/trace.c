// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#define _WIN32_WINNT 0x0500
#include <windows.h>
//#include <dbghelp.h>
#include <winbase.h>
#include <tlhelp32.h>
#include <stdio.h>

#define CONTEXT_ALL                   (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER | CONTEXT_DEBUG | CONTEXT_IA32_CONTROL)
#define CONTEXT_IA32_CONTROL          (CONTEXT_IA64 | 0x00000020)
#define CONTEXT_IA64                  0x00080000
#define CONTEXT_DEBUG                 (CONTEXT_IA64 | 0x00000010)

typedef int (*routine)(DWORD);

typedef struct bp_
{
	DWORD active;
	DWORD offset;
	DWORD org_byte;
	routine function;
} bp;

bp my_bps[0x100];
HANDLE myHandle;
int myPID;
int cur_bp_idx = 0x0;
char cc_byte = '\xcc';
DWORD access = PROCESS_ALL_ACCESS;
DWORD taccess = THREAD_ALL_ACCESS;
DWORD faccess = GENERIC_ALL;
HANDLE serialH;
FILE* f;
char path[] = "c:\\trace.txt";

DWORD readCurAddr(DWORD th)
{
	HANDLE curTh = 0x0;
	DWORD res = -1;
	CONTEXT ctx;
	DWORD cur_instr = 0x00;

	ctx.ContextFlags = CONTEXT_FULL;

	curTh = OpenThread(taccess, 0x0, th);

	res = GetThreadContext(curTh, &ctx);
	//ReadProcessMemory(myHandle, (LPCVOID)ctx.Eip, &cur_instr, 0x1, 0x0);

	fprintf(f, "0x%08x\n", ctx.Eip);
	char line[0x10];
	sprintf(line, "0x%08x\n", ctx.Eip);
	DWORD bytes_written;
	res = WriteFile(serialH, line, strlen(line), &bytes_written, 0x0);
	return cur_instr;
}

DWORD readCurInstr(DWORD th)
{
	HANDLE curTh = 0x0;
	DWORD res = -1;
	CONTEXT ctx;
	DWORD cur_instr = 0x00;

	ctx.ContextFlags = CONTEXT_FULL;

	curTh = OpenThread(taccess, 0x0, th);
	//res = SuspendThread(curTh);
	res = GetThreadContext(curTh, &ctx);
	ReadProcessMemory(myHandle, (LPCVOID)ctx.Eip, &cur_instr, 0x1, 0x0);
	//res = ResumeThread(curTh);
	printf("Current instruction: 0x%02x\n", cur_instr);
	return cur_instr;
}

int lookup_offset(DWORD offset)
{
	int my_i = -0x1;
    int i;
	for(i=0; i<cur_bp_idx; i++)
	{
		if(my_bps[i].offset == offset)
			my_i = i;
			break;
	}
	return my_i;
}

int activate_bp(DWORD offset)
{
	int idx;
	char org_byte;

	idx = lookup_offset(offset);

	ReadProcessMemory(myHandle, (LPCVOID)offset, &org_byte, 0x1, 0x0);
	my_bps[idx].org_byte = org_byte;

	WriteProcessMemory(myHandle, (LPVOID)offset, &cc_byte, 0x1, 0x0);
	my_bps[idx].active = 0x1;

	return 0;
}

int deactivate_bp(DWORD offset)
{
	int idx;
	idx = lookup_offset(offset);
	
	WriteProcessMemory(myHandle, (LPVOID)offset, &my_bps[idx].org_byte, 0x1, 0x0);
	my_bps[cur_bp_idx].active = 0x0;

	return 0;
}

int install_bp(DWORD offset, routine function)
{
	char org_byte;
	ReadProcessMemory(myHandle, (LPCVOID)offset, &org_byte, 0x1, 0x0);

	my_bps[cur_bp_idx].offset = offset;
	my_bps[cur_bp_idx].org_byte = org_byte;
	my_bps[cur_bp_idx].function = function;

	cur_bp_idx++;
	return 0;
}

int deinstall_bp(DWORD offset)
{
	//get entry for offset
	//not useful
	return 0;
}


/* Routines */

int trace_start(DWORD thread)
{
	CONTEXT ctx;
	DWORD flags;
	HANDLE snap;
	HANDLE curTh;
	DWORD res = 0x0;

	snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, myPID);
	THREADENTRY32 te;
	te.dwSize = sizeof(te);

	if(thread != 0x0)
	{
		curTh = OpenThread(taccess, 0x0, thread);

		ctx.ContextFlags = CONTEXT_ALL;
		res = GetThreadContext(curTh, &ctx);
		//printf("Tracing thread 0x%08x of process 0x%08x\n", te.th32ThreadID, te.th32OwnerProcessID);
		
		ctx.EFlags |= 0x100; //trap
		res = SetThreadContext(curTh, &ctx);

		CloseHandle(curTh);
		return 0;
	}

	Thread32First(snap, &te);
	do
	{
		if(te.th32OwnerProcessID != myPID) continue;
		curTh = OpenThread(taccess, 0x0, te.th32ThreadID);

		ctx.ContextFlags = CONTEXT_ALL;
		res = GetThreadContext(curTh, &ctx);
		printf("Tracing thread 0x%08x of process 0x%08x\n", te.th32ThreadID, te.th32OwnerProcessID);
		
		ctx.EFlags |= 0x100; //trap
		res = SetThreadContext(curTh, &ctx);

		CloseHandle(curTh);
        f = fopen(path, "w+");
		
	} while(Thread32Next(snap, &te));

	printf("Reached start\n");
	return 0;
}

int trace_ss(DWORD arg)
{
	//printf("ss\n");
	readCurAddr(arg);
	return 0;
}

int trace_end(DWORD arg)
{
	printf("Reached end\n");
    fclose(f);
	return 0;
}

/* Routines end */

int main(int argc, char** argv)
{

	if(argc < 4)
	{
		printf("You need to provide PID, start addr & end addr\n");
		return -1;
	}

	DWORD addr_st, addr_end;
	DWORD continue_debug = 0x1;
	DEBUG_EVENT debug_event = {0};
	DWORD dwConSt = DBG_CONTINUE;
	DWORD res = 0x0;
	DWORD started = 0x0;
	char com_name[] = "COM2";
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS cTimeouts = {0};

	myPID = atoi(argv[1]);
	addr_st = strtol(argv[2], 0x0, 0x10);
	addr_end = strtol(argv[3], 0x0, 0x10);

	serialH = CreateFileA(com_name, GENERIC_READ | GENERIC_WRITE, 0x0, 0x0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0x0);

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	dcbSerialParams.BaudRate = CBR_256000;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	res = SetCommState(serialH, &dcbSerialParams);



	printf("PID: 0x%08x, st: 0x%08x, end: 0x%08x\n", myPID, addr_st, addr_end);
	myHandle = OpenProcess(access, 0x0, myPID);

	res = DebugActiveProcess(myPID);

	install_bp(addr_st, trace_start);
	activate_bp(addr_st);

	install_bp(addr_end, trace_end);
	activate_bp(addr_end);


	DWORD bp_offset;
	while(continue_debug)
	{
		bp_offset = -1;
		memset(&debug_event, 0x0, sizeof(debug_event));
        int idx;

		while(!WaitForDebugEvent(&debug_event, INFINITE));

		switch(debug_event.dwDebugEventCode)
		{
		case LOAD_DLL_DEBUG_EVENT:
			break;

		case UNLOAD_DLL_DEBUG_EVENT:
			break;

		case EXCEPTION_DEBUG_EVENT:
			switch(debug_event.u.Exception.ExceptionRecord.ExceptionCode)
			{
			
			case STATUS_BREAKPOINT:
				idx = -1;

				//readCurInstr(debug_event.dwThreadId);

				bp_offset = (DWORD)debug_event.u.Exception.ExceptionRecord.ExceptionAddress;
				idx = lookup_offset(bp_offset);

				if(idx == -1) break;

				deactivate_bp(bp_offset);
				//readCurInstr(debug_event.dwThreadId);
				my_bps[idx].function(0x0);
				
				break;

			case STATUS_SINGLE_STEP:
				//readCurInstr(debug_event.dwThreadId);
				trace_ss(debug_event.dwThreadId);
				trace_start(debug_event.dwThreadId);
				break;
			}
			break;
		}
		
		ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, dwConSt);
	}
	CloseHandle(myHandle);
	return 0;
}
