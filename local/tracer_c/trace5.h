#define LIB_VER_W7

#ifdef LIB_VER_W7
#define _WIN32_WINNT 0x0700
#endif 

#include <stdio.h>
#include <windows.h>
#include "WinBase.h"
#include <TlHelp32.h>
#include "winternl.h"

#define buf_size 0x100000
#define BUFF_SIZE 0x100000
#define SS_FLAGS 0x100
#define CLEAR_SS_FLAGS 0xfffffeff
#define MEM_DUMP
#define MAX_NAME 0x200
#define MAX_LINE 0x100
#define MAX_HANDLERS 0x100000
#define MAX_SYSCALL_OUT_ARGS 0x10
#define MAX_SYSCALL_ENTRIES  0x10000
#define MAX_HOOKS 0x50
#define MAX_FUNCTIONS 0x100
#define MAX_MARKERS 0x50
#define MAX_THREADS 0x100

/* functions offsets in respective libs */
#define EXIT_PROCESS_OFF 0x52acf

#ifdef LIB_VER_WXP // WINXPSP2
#define NTDLL_OFF               0x7c900000
#define NTREADFILE_OFF_1        0xe27c
#define NTREADFILE_OFF_2        0xe288
#define NTMAPVIEWOFSECTION_1    0xdc55
#define NTMAPVIEWOFSECTION_2    0xdc61
#define NTSYSENTER_OFF          0xeb8b
#define NTSYSRET_OFF            0xeb94
#endif

#ifdef LIB_VER_W7 // WIN7

/* ntdll addresses */
#define NTDLL_OFF               0x7c900000
#define NTREADFILE_OFF_1        0x45700
#define NTREADFILE_OFF_2        0x4570c
#define NTMAPVIEWOFSECTION_1    0x45070
#define NTMAPVIEWOFSECTION_2    0x4507c
#define NTSYSENTER_OFF          0x464f0
#define NTSYSRET_OFF            0x464f4

/* kernelbase addresses */
#define KERNELBASE_OFF          0xdce0000
#define ISDEBUGGER_OFF_1        0x21f8f
#define ISDEBUGGER_OFF_2        0x21f9c
#define CREATETHREAD_OFF        0x319c7
//#define CREATETHREAD_OFF        0xbb2f
#endif

#define STATUS_ANY 0xff00ff00

#define WATCH_LIMIT 0x100

#define REG_DEBUG 1
#define REG_DEBUG_INTERVAL 4999
#define INSTRUCTION_INTERVAL 10000000
#define INSTRUCTION_SMALL_INTERVAL 10000

#define OFFSET unsigned int

#define LOCATION_CONST          0x0
#define LOCATION_STACK          0x1
#define LOCATION_ADDR_STACK     0x2
#define LOCATION_ADDR_ADDR_STACK     0x3
#define LOCATION_REG            0x4
#define LOCATION_MEM            0x5
#define LOCATION_END            0x6

#define MAX_NAME 0x200
#define MAX_LIBS 0x200

#define STATUS_EMPTY        0x0
#define STATUS_CONFIGURED   0x1
#define STATUS_SPAWNED      0x2
#define STATUS_DBG_STARTED  0x3
#define STATUS_DBG_SCANNED  0x4
#define STATUS_DBG_STOPPED  0x5

/* tracer reports */

#define REPORT_CONTINUE         0x0
#define REPORT_PROCESS_CREATED  0x1
#define REPORT_PROCESS_EXIT     0x2
#define REPORT_ST_BREAKPOINT    0x3
#define REPORT_END_BREAKPOINT   0x4
#define REPORT_BREAKPOINT       0x5
#define REPORT_EXCEPTION        0x6
#define REPORT_INFO             0x7
#define REPORT_NOTHING          0x99

/* tracer configuration */

typedef int (*reaction_routine)(void* args);

typedef struct FUNCTION_
{
    char function_code[4];
    reaction_routine routine;    
} FUNCTION;

typedef struct MARKER_
{
    char lib_name[MAX_NAME];
    OFFSET offset;
} MARKER;

typedef struct REACTION_
{
    MARKER marker;
    char function_code[4];
    reaction_routine routine;    
} REACTION;

typedef struct ROADSIGN_
{
    MARKER marker;
    char sign[4];
} ROADSIGN;

/* other structures */
typedef struct _READ_RECORD
{
    HANDLE handle;
    void* from;
    void* to;
    SIZE_T size;
    SIZE_T* read;
} READ_RECORD;

typedef struct _THREAD_ENTRY
{
    DWORD tid;
    char alive;
    char open;
    char created;
    HANDLE handle;
} THREAD_ENTRY;

typedef struct TRACE_CONFIG_EXTENDED_
{
    FUNCTION functions[MAX_FUNCTIONS];
    unsigned functions_count;

    REACTION reactions[MAX_FUNCTIONS];
    unsigned reactions_count;

    ROADSIGN signs;
    unsigned signs_count;
} TRACE_CONFIG_EXTENDED;

typedef struct TRACE_CONFIG_
{
    /* basic stuff */
    /* traced proces info  */
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    CREATE_PROCESS_DEBUG_INFO cpdi;
    unsigned PID;
    
    /* handles */
    HANDLE procHandle;
    HANDLE file_handle;

    /* threads */

    DWORD thread_map[0x1000000];
    THREAD_ENTRY threads[MAX_THREADS];

    /* synchronization */ 
    HANDLE eventLock, eventUnlock;
    HANDLE mutex;

    /* communication */
    unsigned port;
    char host[MAX_NAME];
    HANDLE pipe;
    DWORD report_code;
    char report_buffer[BUFF_SIZE];

    /* replace this */
    char spawned;
    char started;
    DWORD scan_on;

    /* with this */
    char status;

    /* counters */
    unsigned long long instr_count;
    unsigned long long instr_dbg;
    unsigned long long instr_limit;
    unsigned long long scan_count; /*scan_count*/
    unsigned long long thread_count;

    /* last events */
    READ_RECORD last_read_record;
    DEBUG_EVENT last_event;
    EXCEPTION_RECORD last_exception;
    int last_win_status;

    char verbose; /*full_log*/
    char buffer[BUFF_SIZE];

    /* output streams */
    FILE* log;
    FILE* file;
    FILE* modifications;
    SOCKET socket;

    /* paths */
    char path[0x200];
    char cur_path[0x200];
    int path_i;
    char dumpPath[0x200];
    char iniPath[0x200];
    char modPath[0x200];
    wchar_t filePath[0x200];
    char research_dir[MAX_LINE];
    char sample_path[MAX_LINE];
    char process_fname[MAX_NAME];
   
    /* offsets */
    DWORD img_base;
    DWORD nt1_off;
    DWORD nt2_off;
    DWORD nt3_off;
    DWORD nt4_off;
    DWORD sysenter_off;
    DWORD sysret_off;

    /* markers */
    MARKER st_markers[MAX_MARKERS];
    MARKER end_marker[MAX_MARKERS];

    /* syscall data */
    DWORD sysenter_esp;
    DWORD sysenter_no;

    /* functions & reactions */
    TRACE_CONFIG_EXTENDED* extended;

} TRACE_CONFIG;

/* handling syscalls */
typedef struct OUT_ARGUMENT_
{
    DWORD off;
    DWORD size;
    char off_location;
    char size_location;
    DWORD eax_val_success;
} OUT_ARGUMENT;

typedef struct OUT_LOCATION_
{
    DWORD off;
    DWORD size;
} OUT_LOCATION;

/* debugging structures */
typedef struct _CREATE_THREAD_DEBUG_INFO2
{
    CREATE_THREAD_DEBUG_INFO u;
    DWORD tid;
} CREATE_THREAD_DEBUG_INFO2;

typedef void (*handler_routine)(void*);
int add_breakpoint(DWORD addr, handler_routine handler);
DWORD find_lib(char* name);
BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);
SIZE_T dump_mem(FILE*, void*, SIZE_T);

typedef struct _HANDLER
{
    handler_routine handler;
    char enabled;
} HANDLER;

typedef struct _HOOK
{
    DWORD offset;
    char libname[0x50];
    handler_routine handler;
} HOOK;


typedef struct _bpt
{
    DWORD addr;
    char saved_byte;
//    handler_routine handler;    
    char enabled;
    HANDLER handlers[MAX_HANDLERS];
    unsigned handler_count;
    char isHook;
} bpt;

/* tracking libraries */
typedef struct _LIB_ENTRY
{
    char alive;
    DWORD lib_addr;
    char lib_name[MAX_NAME];
} LIB_ENTRY;

typedef struct _WATCHED
{
    DWORD off;
    int hit;
    int scan;
    int count;
    long long unsigned instr;
} WATCHED;

/* tracing functions */

int unset_ss(DWORD);
int set_ss(DWORD);
int d_print(const char* format, ...);
FILE* configure_file();
