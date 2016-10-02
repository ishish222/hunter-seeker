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
#define CLEAR_SF_FLAGS 0xffffff7f
#define SET_ZF_FLAGS   0x00000040
//#define MEM_DUMP
#define MAX_NAME 0x200
#define MAX_LINE 0x100
#define MAX_HANDLERS 0x100000
#define MAX_SYSCALL_OUT_ARGS 0x10
#define MAX_SYSCALL_ENTRIES  0x10000
#define MAX_HOOKS 0x50
#define MAX_FUNCTIONS 0x300
#define MAX_E_REACTIONS 0x50
#define MAX_THREADS 0x100
#define MAX_LIBS 0x50
#define MAX_BREAKPOINTS 0x50
#define MAX_REGIONS 0x20

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
#define REPORT_EXCEPTION_NH     0x8
#define REPORT_NOTHING          0x99

/* tracer commands */

#define CMD_SET_NAME            "SN"
#define CMD_SET_IN_DIRECTORY    "SD"
#define CMD_SET_OUT_DIRECTORY   "Sd"
#define CMD_SET_OUT_PREFIX      "Sp"
#define CMD_PREPARE_TRACE       "PT"
#define CMD_SET_E_REACTION_1        "M1"
#define CMD_SET_E_REACTION_2        "M2"
#define CMD_START_DEBUG         "sd"
#define CMD_LIST_TEBS           "lt"
#define CMD_LIST_LIBS           "ll"
#define CMD_LIST_E_REACTIONS        "lm"
#define CMD_LIST_BPTS           "lb"
#define CMD_READ_MEMORY         "RM"
#define CMD_WRITE_MEMORY        "WM"
#define CMD_READ_REGISTER       "RR"
#define CMD_WRITE_REGISTER      "WR"
#define CMD_READ_STACK          "RS"
#define CMD_CONTINUE            "cn"
#define CMD_CONTINUE_TIME       "cN"
#define CMD_ACTIVATE_E_REACTIONS    "AM"
#define CMD_SET_LIMIT           "SL"
#define CMD_SET_TRACE_NAME      "ST"
#define CMD_SET_DUMP_NAME       "sD"
#define CMD_INFO_NAME           "SI"
#define CMD_ENABLE_TRACE        "ET"
#define CMD_ENABLE_DBG_TRACE    "ED"
#define CMD_DISABLE_TRACE       "DT"
#define CMD_DUMP_MEMORY         "DM"
#define CMD_CONFIGURE_E_REACTIONS   "cm"
#define CMD_CONFIGURE_I_REACTIONS "cR"
#define CMD_CONFIGURE_REGIONS   "cr"
#define CMD_ENABLE_I_REACTION     "eR"
#define CMD_ENABLE_ALL_I_REACTIONS "eA"
#define CMD_DISABLE_I_REACTION    "dR"
#define CMD_ACTIVATE_E_REACTIONS    "am"
#define CMD_AUTO_ST             "AS"

#define CMD_ROUTINE_1           "R1"
#define CMD_ROUTINE_2           "R2"
#define CMD_ROUTINE_3           "R3"
#define CMD_ROUTINE_4           "R4"

/* tracer configuration */

/*
typedef struct FUNCTION_
{
    char function_code[4];
    i_reaction_routine routine;    
} FUNCTION;
*/

typedef void (*handler_routine)(void*);
typedef void (*reaction_routine)(void*);

typedef struct LOCATION_DESCRIPTOR_NEW_
{
    struct LOCATION_DESCRIPTOR_NEW_* a1;
    struct LOCATION_DESCRIPTOR_NEW_* a2;
    char op[MAX_NAME];

} LOCATION_DESCRIPTOR_NEW;

typedef struct _HANDLER
{
    handler_routine handler;
    char enabled;
} HANDLER;

typedef struct BREAKPOINT_
{
    LOCATION_DESCRIPTOR_NEW* location;
    char location_str[MAX_NAME];
    OFFSET resolved_location;
    char loaded;
    char enabled;
    char written;

    HANDLER handlers[MAX_HANDLERS];
    unsigned handler_count;

    /* deprecated */
    DWORD addr;
    char saved_byte;
//    handler_routine handler;    
    char isHook;
} BREAKPOINT;

typedef struct E_REACTION_
{
    char id[3];
//    char enabled;
    BREAKPOINT* bp;

    /* deprecated */
    char lib_name[MAX_NAME];
    OFFSET lib_offset;
    OFFSET real_offset;
    OFFSET offset;
} E_REACTION;

typedef struct I_REACTION_
{
    unsigned id;
//    char enabled;
    BREAKPOINT* bp;

    /* deprecated */
    char lib_name[MAX_NAME];
    OFFSET lib_offset;
    OFFSET real_offset;
    OFFSET offset;
    char pending_enable;
} I_REACTION;

typedef struct ROADSIGN_
{
    E_REACTION marker;
    char sign[4];
    unsigned reaction;
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
    char skipping;
} THREAD_ENTRY;

typedef struct TRACE_CONFIG_EXTENDED_
{
    /*
    FUNCTION functions[MAX_FUNCTIONS];
    unsigned functions_count;
    */
    //I_REACTION reactions[MAX_FUNCTIONS];
    //unsigned reactions_count;

    ROADSIGN signs;
    unsigned signs_count;
} TRACE_CONFIG_EXTENDED;

/* tracking libraries */
typedef struct _LIB_ENTRY
{
    char loaded;
    DWORD lib_offset;
    char lib_name[MAX_NAME];
    char lib_path[MAX_NAME];
} LIB_ENTRY;

/* handling syscalls */
typedef struct LOCATION_DESCRIPTOR_
{
    DWORD off;
    DWORD size;
    char off_location;
    char size_location;
    DWORD eax_val_success;
} LOCATION_DESCRIPTOR;

typedef struct LOCATION_
{
    DWORD off;
    DWORD size;
} LOCATION;

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

    /* libraries */
    LIB_ENTRY libs[MAX_LIBS];

    /* synchronization */ 
    HANDLE eventLock, eventUnlock;
    HANDLE mutex;

    /* breakpoints */
    BREAKPOINT breakpoints[MAX_BREAKPOINTS];

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
    unsigned long long lib_count;
    unsigned long long bpt_count;

    /* last events */
    READ_RECORD last_read_record;
    DEBUG_EVENT last_event;
    EXCEPTION_RECORD last_exception;
    int last_win_status;
    OFFSET last_eip;
    DWORD last_tid;
    E_REACTION* last_e_reaction;

    char verbose; /*full_log*/
    char buffer[BUFF_SIZE];

    /* other options */

    DWORD skipping_tid;

    /* output streams */
    FILE* log;
    FILE* logg;
    FILE* trace;
    FILE* mods;
    FILE* dump;
    FILE* ini;
    SOCKET socket;

    /* paths */
    char path[MAX_LINE];
    char cur_path[MAX_LINE];
    int path_i;

    char in_research_dir[MAX_LINE];
    char in_sample_path[MAX_LINE];

    char out_dir[MAX_LINE];
    char out_prefix[MAX_LINE];
    unsigned out_postfix;

    char out_trace[MAX_LINE];
    char out_dump[MAX_LINE];
    char out_ini[MAX_LINE];
    char out_mods[MAX_LINE];
    char out_log[MAX_LINE];

    char process_fname[MAX_NAME];
   
    
    /* offsets */
    DWORD img_base;
    DWORD nt1_off;
    DWORD nt2_off;
    DWORD nt3_off;
    DWORD nt4_off;
    DWORD sysenter_off;
    DWORD sysret_off;

    /* e_reactions */
    E_REACTION e_reactions[MAX_E_REACTIONS];
    unsigned e_reaction_count;

    /* i_reactions */
    I_REACTION i_reactions[MAX_E_REACTIONS];
    unsigned i_reaction_count;
    reaction_routine routines[MAX_FUNCTIONS];

    /* regions */
    LOCATION_DESCRIPTOR region_sel[MAX_REGIONS];
    unsigned region_sel_count;

    /* syscall data */
    DWORD sysenter_esp;
    DWORD sysenter_no;

    /* functions & reactions */
    TRACE_CONFIG_EXTENDED* extended;

    /* handling syscalls */
    LOCATION_DESCRIPTOR last_arg = {0x0, 0x0, LOCATION_END, LOCATION_END, 0x0};
    LOCATION last_location = {0x0, 0x0};
    LOCATION_DESCRIPTOR syscall_out_args[MAX_SYSCALL_ENTRIES][MAX_SYSCALL_OUT_ARGS];
    LOCATION syscall_out_args_dump_list[MAX_SYSCALL_OUT_ARGS];

} TRACE_CONFIG;

/* debugging structures */
typedef struct _CREATE_THREAD_DEBUG_INFO2
{
    CREATE_THREAD_DEBUG_INFO u;
    DWORD tid;
} CREATE_THREAD_DEBUG_INFO2;

typedef void (*handler_routine)(void*);
//int add_breakpoint(DWORD addr, handler_routine handler);
int add_breakpoint(DWORD addr, handler_routine handler);
DWORD find_lib(char* name);
BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType);
SIZE_T dump_mem(FILE*, void*, SIZE_T);

typedef struct _HOOK
{
    DWORD offset;
    char libname[0x50];
    handler_routine handler;
} HOOK;

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


