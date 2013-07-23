//
// Sample application for remote process injection. Spawns a process and injects itself into it.
//

#include <stdio.h>
#include <windows.h>

#define PIPE_NAME "\\\\.\\pipe\\control"
#define PIPE_SIZE 128
#define LOG_PATH "C:\\log.txt"
#define MAX_TRAMPOLINE_SIZE 0x100
//#define ADDR_OFF 0x2
#define ADDR_OFF 0x1
#define TRAMPOLINE_SIZE 0x6
#define CODE_PEAMBLE_SIZE 0x1

int is_injected = 0;
// this module base. this will be properly set even in the injected process, inject() takes care of that
HMODULE hmodule;

// PID to the original process we were injected from
DWORD parent_pid;

// this is just a buffer to keep an image of the executable at point of entry; before the crt is initialized and before main is called
char*image_mem = 0;
DWORD image_size = 0;

// take an image of the process
// do not use any CRT functions here,f since it's not initialized yet
void take_image() {
	char*p = (char*)hmodule;
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)p;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(p + dos->e_lfanew);

	// find first section...
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
	int n_sections = nt->FileHeader.NumberOfSections;

	// ...to find last section
	DWORD begin = (DWORD)p;
	DWORD end = begin + section[n_sections-1].VirtualAddress + section[n_sections-1].Misc.VirtualSize;
	image_size=end-begin;

	// any memory allocation function that does not use the CRT will do (that excludes malloc)
	image_mem=(char*)VirtualAlloc(0,image_size,MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
	// copy all the memory from the beginning of the module to the end of the last section
	memcpy(image_mem,(void*)begin,image_size);
}

// copy from src in this process to dst in the image
// use to set a variable or memory area in the image before injection

void image_set(const void*dst,const void*src,size_t size) {
	DWORD offset = (DWORD)dst - ((DWORD)hmodule);
	memcpy(image_mem + offset,src,size);
}

// easy way to "copy" a variable or memory area over to the image
void image_copy(const void*p,size_t size) {
	image_set(p,p,size);
}

// this goes through the Import Address Table and loads all the imports
// essentially the same as Windows does upon loading a module
// note that it just silently ignores errors, so if it fails to load a module, the process might crash later
// this function is provided only for completeness, and I do not recommend using it
void do_iat() {
	const char*p = (const char*)hmodule;
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)p;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(p + dos->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR import = (PIMAGE_IMPORT_DESCRIPTOR) (p + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	while (import->Characteristics) {
		HMODULE hm = LoadLibraryA(p+import->Name);
		if (hm) {
			DWORD*dw = (DWORD*)(p+import->OriginalFirstThunk);
			int i;
			for (i=0;*dw;i++) {
				FARPROC proc;
				if (*dw&0x80000000) proc=GetProcAddress(hm,(LPCSTR)(*dw&&0xFFFF)); // load by ordinal
				else proc=GetProcAddress(hm,p+*dw+2); // load by name
				if (proc) {
					*((FARPROC*)(p+import->FirstThunk)+i) = proc; // set the value in the bound IAT
				} else {
					// failed to load proc
				}
				++dw;
			}
		} else {
			// failed to load module
		}
		++import;
	}
}

// just to be compatible with C, we wrap these in ifdefs
// mainCRTStartup and start need to have C symbol names

#ifdef __cplusplus
extern "C"
#endif
void mainCRTStartup();
#ifdef __cplusplus
extern "C"
#endif
void start() {
	hmodule = GetModuleHandle(0);
	// take image before CRT is initialized
	take_image();
	mainCRTStartup();
}

typedef void* (*ACADopen_type)(DWORD, LPTSTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);

typedef int (*sprtype)(const char*, const char*, ...);
typedef FILE* (*fopen_type)(const char*, const char*);
typedef int (*fprintf_type)(FILE*, const char*, ...);
typedef int (*fclose_type)(FILE*);
typedef FILE* (*freopen_type)(const char*, const char*, FILE*);
typedef int (*strcmp_type)(const char*, const char*);
typedef void* (*memset_type)(void*, int, size_t);
typedef void* (*memcpy_type)(void*, void*, size_t);
typedef void (*trampoline_type)();
typedef char* (*strtok_type)(char*, char*);
typedef wchar_t* (*wcscpy_type)(wchar_t*, wchar_t*);

sprtype spr;

fopen_type myfopen;
fprintf_type myfprintf;
fclose_type myfclose;
fclose_type myfflush;
strcmp_type mystrcmp;
freopen_type myfreopen;
memset_type mymemset;
memcpy_type mymemcpy;
strtok_type mystrtok;
wcscpy_type mywcscpy;
ACADopen_type myACADOpen;

char* ACADClass = "Afx:00400000:b:00010011:00000006:00210139";
HANDLE ACADSaved = 0x0;

char class_name[256];
char title[256];
char my_hwnd[10];
char PID[10];
int iPID = 0;
FILE* log;
HANDLE pHandle;
char outbuffer[PIPE_SIZE];

char code1[MAX_TRAMPOLINE_SIZE];
char code2[MAX_TRAMPOLINE_SIZE];

//char trampoline1[0x10] =	"\xb8\x00\x00\x00\x00"	/*far jmp to worker*/
//							"\xff\x60"						/*call eax*/


char trampoline1[0x10] =	"\x68\x00\x00\x00\x00"	/*push addr*/
							"\xc3";					/*ret*/

char preamble[0x10] =		"\x5e";					/* pop esi */

DWORD retaddr1;
DWORD retaddr2;
char path2[0x100];
char installed_path[0x200];

BOOL installPath(char* str)
{
	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str, strlen(str), (LPWSTR)installed_path, 0x200); 
	//MessageBoxW(NULL, (LPCWSTR)installed_path, TEXT("tja"), MB_OK);
	return TRUE;
}

void ACADworker1()
{
	DWORD pointer;
	LPCWSTR path;
	DWORD bytesWrote;
	//MessageBoxA(NULL, "Worker1", PID, MB_OK);
	//__debugbreak();
	 
	__asm
	{
		mov pointer, esp
	}

	//pointer += 0x14;
	pointer += 0x10;
	path = *((LPCWSTR*)pointer);
	
	//WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, path, -1, path2, 0x100, NULL, NULL);

	mywcscpy((LPWSTR)path, (LPWSTR)installed_path);

	//WriteFile(pHandle, path2, strlen(path2), &bytesWrote, 0);

	((trampoline_type)((PVOID)code1))();

	__asm { 
		push retaddr1
		ret
	}
}

void ACADworker2()
{
	DWORD bytesWrote;
	
	__debugbreak();
		
	//MessageBoxA(NULL, "Worker2", PID, MB_OK);
	//ConnectNamedPipe(pHandle, NULL);
	WriteFile(pHandle, "LF", strlen("LF"), &bytesWrote, 0);
	//DisconnectNamedPipe(pHandle);
	((trampoline_type)((PVOID)code2))();

	__asm { 
		push retaddr2
		ret
	}
}

BOOL installHook(DWORD addr, DWORD size, DWORD worker, DWORD code, DWORD* retaddr)
{
	DWORD dwOld;
	*retaddr = addr+size;

	//	MessageBoxA(NULL, "Installing hook", PID, MB_OK);

	//__debugbreak();

	//set up 1st trampoline

	trampoline1[ADDR_OFF+0] = ((char*)&worker)[0];
	trampoline1[ADDR_OFF+1] = ((char*)&worker)[1];
	trampoline1[ADDR_OFF+2] = ((char*)&worker)[2];
	trampoline1[ADDR_OFF+3] = ((char*)&worker)[3];

	VirtualProtect((PVOID)addr, size, PAGE_READWRITE, &dwOld);
	
	mymemcpy((PVOID)(code), (PVOID)preamble, CODE_PEAMBLE_SIZE);
	mymemcpy((PVOID)(code+CODE_PEAMBLE_SIZE), (PVOID)addr, size);
	mymemcpy((PVOID)addr, trampoline1, 6);

	VirtualProtect((PVOID)addr, size, PAGE_EXECUTE, &dwOld);

	//set up 2nd trampoline

	trampoline1[ADDR_OFF+0] = ((char*)retaddr)[0];
	trampoline1[ADDR_OFF+1] = ((char*)retaddr)[1];
	trampoline1[ADDR_OFF+2] = ((char*)retaddr)[2];
	trampoline1[ADDR_OFF+3] = ((char*)retaddr)[3];

	mymemcpy((PVOID)(code+size+CODE_PEAMBLE_SIZE), (PVOID)trampoline1, TRAMPOLINE_SIZE);

	return TRUE;
}

BOOL AcadOpen(LPTSTR name)
{
	myACADOpen(0x01e4beb0, name, 1, 0, 1, 0, 0, 0, 0);
	return TRUE;
}

BOOL CALLBACK AcaKillCEWProc(HWND hwnd, LPARAM lparam)
{
	GetClassNameA(hwnd, class_name, sizeof(class_name));
	spr(my_hwnd, "%x, class: %s", hwnd, class_name);

	if(!mystrcmp(class_name, ACADClass))
	{
		myfprintf(log, "%x - %s\n", hwnd, class_name);
	}
	return TRUE;
}

BOOL CALLBACK AcaCEWProc(HWND hwnd, LPARAM lparam)
{
	DWORD written;
	GetClassNameA(hwnd, class_name, sizeof(class_name));
	spr(my_hwnd, "%x, class: %s\n", hwnd, class_name);

	if(!mystrcmp(class_name, ACADClass))
	{
		WriteFile(pHandle, my_hwnd, strlen(my_hwnd), &written, 0);
		//myfprintf(log, "%x - %s\n", hwnd, class_name);
	}
	return TRUE;
}

BOOL CALLBACK AllCEWProc(HWND hwnd, LPARAM lparam)
{
	DWORD written;

	GetClassNameA(hwnd, class_name, sizeof(class_name));
	spr(my_hwnd, "%x-%s", hwnd, class_name);
//	spr(my_hwnd, "%x", hwnd);
	//myfprintf(log, "%s - %s\n", class_name, ACADclass);
	//myfprintf(log, "%x - %s\n", hwnd, class_name);
/*	if(hwnd == (HWND)0xdedaeb85)
	{
		myfprintf(log, "%x - %s\n", hwnd, class_name);
	}
*/
//	if(!mystrcmp(class_name, ACADclass))
//	{
		//myfprintf(log, "%x - %s\n", hwnd, class_name);
		//if(hwnd == (HWND)0x53037c)
		//SendMessage(hwnd, 0x10, 0x0, 0x0);
	//}
	//WriteFile(pHandle, my_hwnd, PIPE_SIZE, &written, NULL);
	myfprintf(log, "%x - %s\n", hwnd, class_name);
	WriteFile(pHandle, my_hwnd, strlen(my_hwnd), &written, 0);
	return TRUE;
}

BOOL CALLBACK AcaKillEWProc(HWND hwnd, LPARAM lparam)
{
	char myPID[10];
	DWORD imyPID;
	my_hwnd[0]='\0';
	class_name[0]='\0';
	title[0]='\0';

	GetWindowThreadProcessId(hwnd, &imyPID);
	spr(myPID, "%d", imyPID);
	if(imyPID != iPID) return TRUE;
	EnumChildWindows(hwnd, AcaKillCEWProc, NULL);

	return TRUE;
}

BOOL CALLBACK AcaEWProc(HWND hwnd, LPARAM lparam)
{
	char myPID[10];
	DWORD imyPID;
	my_hwnd[0]='\0';
	class_name[0]='\0';
	title[0]='\0';

	GetWindowThreadProcessId(hwnd, &imyPID);
	spr(myPID, "%d", imyPID);
	if(imyPID != iPID) return TRUE;
	EnumChildWindows(hwnd, AcaCEWProc, NULL);

	return TRUE;
}

BOOL CALLBACK AllEWProc(HWND hwnd, LPARAM lparam)
{
//	int ret = 1;
	char myPID[10];
	DWORD imyPID;
	my_hwnd[0]='\0';
	class_name[0]='\0';
	title[0]='\0';

	GetWindowThreadProcessId(hwnd, &imyPID);
	spr(myPID, "%d", imyPID);
	if(imyPID != iPID) return TRUE;
	//MessageBoxA(NULL, "Matched main window", myPID, MB_OK);

	GetClassNameA(hwnd, class_name, sizeof(class_name));
//	ret = GetWindowTextA(hwnd, title, sizeof(title));
//	if(ret == 0) title[0]='\0';

//	spr(my_hwnd, "%x, class: %s, title: %s", hwnd, class_name, title);
	//spr(my_hwnd, "%x, class: %s", hwnd, class_name);
	//spr(my_hwnd, "%x", hwnd);
	//MessageBoxA(NULL, my_hwnd, "HWND", MB_OK);
	//MessageBoxA(NULL, "Enumerating children", "Get ready", MB_OK);
	myfprintf(log, "[%x]\n", hwnd);
	EnumChildWindows(hwnd, AllCEWProc, NULL);

	return TRUE;
}

BOOL setACADSaved(HANDLE hwnd)
{
	ACADSaved = hwnd;
	MessageBoxA(NULL, "setACADSaved", "Command completed", MB_OK);
	return TRUE;
}

BOOL resetLog()
{
	log = myfreopen(LOG_PATH, "w", log);
	MessageBoxA(NULL, "resetLog", "Command completed", MB_OK);
	return TRUE;
}

BOOL showPID()
{
	DWORD written;
	WriteFile(pHandle, PID, strlen(PID), &written, 0);
	return TRUE;
}

BOOL AcadDumpProjects()
{
	EnumWindows(AcaEWProc, NULL);
	myfprintf(log, "\n\n");
	myfflush(log);
	MessageBoxA(NULL, "AcadDumpProjects", "Command completed", MB_OK);
	return TRUE;
}

BOOL DumpAllHandles()
{
	DWORD written;
	EnumWindows(AllEWProc, NULL);
	myfprintf(log, "\n\n");
	myfflush(log);
	//MessageBoxA(NULL, "DumpAllHandles", "Command completed", MB_OK);
	//WriteFile(pHandle, "OK", strlen("OK"), &written, 0);
	FlushFileBuffers(pHandle);
	return TRUE;
}

BOOL dispatch_command(char* cmd)
{
	DWORD bytesWrote;
	char buf[0x100];
	DWORD written = 0;
	char* arg1;

	arg1 = mystrtok(cmd, " ");
	arg1 = mystrtok(NULL, " ");
	if(arg1 != NULL)
	{
		*(arg1-1) = '\0';
	}

	if(mystrcmp(cmd, "showPID")==0) return showPID();
	if(mystrcmp(cmd, "DumpAllHandles")==0) return DumpAllHandles();
	if(mystrcmp(cmd, "AcadDumpProjects")==0) return AcadDumpProjects();
	if(mystrcmp(cmd, "resetLog")==0) return resetLog();
	if(mystrcmp(cmd, "testOpen")==0) return AcadOpen(L"C:\\test.dwg");
	if(mystrcmp(cmd, "installTestHook")==0) return installHook(0x553f10, 0x8, (DWORD)&ACADworker1, (DWORD)code1, &retaddr1);
	if(mystrcmp(cmd, "installTestHook2")==0) return installHook(0x554315, 0x6, (DWORD)&ACADworker2, (DWORD)code2, &retaddr2);
	if(mystrcmp(cmd, "installPath")==0) return installPath(arg1);
	if(mystrcmp(cmd, "testPipe")==0) 
	{
		WriteFile(pHandle, "TP", strlen("TP"), &bytesWrote, 0);
		return TRUE;
	}
	if(mystrcmp(cmd, "quit")==0) return TRUE;
	if(mystrcmp(cmd, "disconnect")==0) return TRUE;
	
	WriteFile(pHandle, "UC", strlen("UC"), &bytesWrote, 0);
	//MessageBoxA(NULL, cmd, "Unknown command", MB_OK);
	return TRUE;
}

void injected_start() {
	HMODULE hModule;
	BOOL fConnected = FALSE;
	BOOL fSuccess = FALSE;
	char buffer[1024];
//	typedef int (*sprtype)(const char*, const char*, ...);
	typedef DWORD (*anytype)(DWORD, ...);
	typedef DWORD (*anyanytype)();
	DWORD myObj;
//	sprtype spr;
	anytype any, alloc, ctor;
	anyanytype close;
	is_injected = 1;
	DWORD bytesRead;
	DWORD bytesWrote;
	myACADOpen = (ACADopen_type)0x553f10;
	// uncomment next line if you want to be able to inject into further processes
	// take_image()

	// fix up the IAT
	// I would actually recommend not doing this, instead only use functions from kernel32, which is guaranteed to be loaded in every process (and at the same address, to boot)
	// but, for completeness, I've included it here
	//do_iat();
	
	PID[0] = '\0';
	hModule = LoadLibraryA("msvcrt.dll");
	spr = (sprtype)GetProcAddress(hModule, "sprintf");
	myfopen = (fopen_type)GetProcAddress(hModule, "fopen");
	myfclose = (fclose_type)GetProcAddress(hModule, "fclose");
	myfprintf = (fprintf_type)GetProcAddress(hModule, "fprintf");
	mystrcmp = (strcmp_type)GetProcAddress(hModule, "strcmp");
	myfreopen = (freopen_type)GetProcAddress(hModule, "freopen");
	myfflush = (fclose_type) GetProcAddress(hModule, "fflush");
	mymemset = (memset_type) GetProcAddress(hModule, "memset");
	mymemcpy = (memcpy_type) GetProcAddress(hModule, "memcpy");
	mystrtok = (strtok_type) GetProcAddress(hModule, "strtok");
	mywcscpy = (wcscpy_type) GetProcAddress(hModule, "wcscpy");
	

	iPID = GetCurrentProcessId();
	spr(PID, "%d", iPID);

	alloc = (anytype)0x50b230;
	close = (anyanytype)0xb117d4;
	ctor = (anytype)0x4879a0;
//	myObj = alloc(0x408);
//	myObj = ctor(myObj);
	//close();

	log = myfopen(LOG_PATH, "a");
	if(log == NULL) 
	{
		MessageBoxA(NULL, "Log not ready", PID, MB_OK);
		ExitThread(0);
	}
	
	//__debugbreak();

	pHandle = CreateFile(TEXT(PIPE_NAME), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	MessageBoxA(NULL, "Version 0.15", PID, MB_OK);

	mymemset(buffer, 0, PIPE_SIZE-1);
	do
	{
		mymemset(buffer, 0, PIPE_SIZE-1);
		fConnected = ConnectNamedPipe(pHandle, NULL);
		while(mystrcmp(buffer, "quit") && mystrcmp(buffer, "disconnect"))
		{
			mymemset(buffer, 0, PIPE_SIZE-1);
			fSuccess = ReadFile(pHandle, buffer, PIPE_SIZE, &bytesRead, NULL);
			if(fSuccess != TRUE)
			{
				break;
			}
			if(fSuccess)
			{
				dispatch_command(buffer);
				//WriteFile(pHandle, "$", strlen("$"), &bytesWrote, 0);
				//FlushFileBuffers(pHandle);
				WriteFile(pHandle, "OK", strlen("OK"), &bytesWrote, 0);
				//FlushFileBuffers(pHandle);
			}
		}
		DisconnectNamedPipe(pHandle);
	}
	while(mystrcmp(buffer, "quit"));
	CloseHandle(pHandle);
	
	myfclose(log);

	//MessageBoxA(NULL, "Exiting thread", PID, MB_OK);
	ExitThread(0);
	mainCRTStartup();
}

void inject(HANDLE h_proc) {

	char*p = (char*)hmodule;
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)p;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(p + dos->e_lfanew);

	DWORD begin = (DWORD)p;
	DWORD end = begin + image_size;
	int start_offset = (ptrdiff_t)&injected_start - begin; // offset of the entry point for the injected code
	// allocate memory in the target process for the image
	char*mem=(char*)VirtualAllocEx(h_proc,0,image_size,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if (!mem) {
		fprintf(stderr,"VirtualAllocEx failed; error %d",GetLastError());
		return;
	}


	// now we must do base relocation, since we are probably loading the code in a different memory area then where we took the image from :)
	// this is essentially the same thing Windows does whenever loading a module in a different location than it's desired base address
	// the executable must be linked with a relocation section, otherwise it will crash bad after injecting

	{
		// the relocation section is basically a list of IMAGE_BASE_RELOCATION entries
		// each entry has a virtual address and then a list of WORDs
		// the top 4 bits of each WORD specify a relocation type, and the bottom 12 specify an offset
		// (for x86, all base relocations are of type IMAGE_REL_BASED_HIGHLOW)
		// each offset should be added to the virtual address of the IMAGE_BASE_RELOCATION to get the address of a DWORD
		// subtract begin from that DWORD and add mem, and the relocation is done
		PIMAGE_BASE_RELOCATION reloc = (PIMAGE_BASE_RELOCATION) (p + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
		while (reloc->VirtualAddress) {
			DWORD d = (DWORD)(p + reloc->VirtualAddress);
			PIMAGE_BASE_RELOCATION next = (PIMAGE_BASE_RELOCATION)((char*)reloc + reloc->SizeOfBlock);
			WORD*w = (WORD*)(reloc+1);
			while ((char*)w<(char*)next) {
				if (*w>>12 == IMAGE_REL_BASED_HIGHLOW) {
					DWORD d2 = d + (*w&0xFFF);
					if (d2>=begin&&d2<end) {
						DWORD*d = (DWORD*)(d2 - begin + (DWORD)image_mem);
						*d -= begin - (DWORD)mem;
					}
				}
				++w;
			}
			reloc = next;
		}
	}

	// calling GetModuleHandle(0) from the injected code would return the module of the target process,
	// so we set hmodule in the image to the target memory here
	image_set(&hmodule,&mem,sizeof(hmodule));

	// write it into the allocated memory in the target process!
	if (!WriteProcessMemory(h_proc,mem,image_mem,image_size,0)) {
		fprintf(stderr,"WriteProcessMemory failed; %d",GetLastError());
		return;
	}

	{
		// create the remote thread...
		HANDLE h=CreateRemoteThread(h_proc,NULL,0,(LPTHREAD_START_ROUTINE)(mem+start_offset),0,0,0);
		if (!h) {
			fprintf(stderr,"CreateRemoteThread failed; error %d",GetLastError());
			return;
		}
	}

	// ...and the rest is up to fate

}

//#pragma comment(lib,"ws2_32.lib")
//#include <winsock.h>

int main() {

	if (is_injected) {
		// if the process we are injected into is not a console process, then we need to allocate one
		AllocConsole();
		freopen("CONOUT$","w",stdout);

		printf("hello world, my parent process id is %d\n",parent_pid);
		printf("I am loaded at 0x%p, target process is loaded at 0x%p\n",hmodule,GetModuleHandle(0));
		Sleep(1000);

		// uncomment (along with the 2 lines above main) to demonstrate the IAT loading;
		//   this will crash if you comment out do_iat() in injected_start, and inject into a process which does not load ws2_32.dll
		//   if all is well, socket should return -1
		//printf("socket is %d\n",socket(0,0,0));

		// if we return from main, the CRT will call exit, and the target process will terminate
		// so we can either call ExitThread here, or loop forever.
		// naturally, if main doesn't exit properly, the CRT won't exit properly either, so things like destructors for global c++ objects won't be called
		ExitThread(0);
		//while (1) Sleep(100);
	} else {

		// It might be desired to create the process in a suspended state, inject, and then resume the process
		// this is unnecessary for this test

		char* cmdline;
		LPWSTR* szArgList;
		int argCount;
		int i;
		DWORD PID;
		HANDLE hProc;

		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		memset(&si,0,sizeof(si));
		si.cb = sizeof(si);
		
		
		szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
/*		for(i=0; i<argCount; i++)
		{
			MessageBoxW(NULL, szArgList[i], L"Test", MB_OK);
		}
*/		

		PID = _wtoi(szArgList[1]);

		hProc = OpenProcess(PROCESS_VM_WRITE|PROCESS_VM_READ|PROCESS_VM_OPERATION|PROCESS_CREATE_THREAD, FALSE, PID);
		if(NULL == hProc)
		{
			fprintf(stdout, "Error opening process");
		}
		inject(hProc);
/*
		CreateProcessA(0,"notepad",0,0,0,0,0,0,&si,&pi);

		// set parent_pid and copy it over to the image
		parent_pid = GetCurrentProcessId();
		image_copy(&parent_pid,sizeof(parent_pid));
		inject(pi.hProcess);
		*/
	}

	return 0;
}


