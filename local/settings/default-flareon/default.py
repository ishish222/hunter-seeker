DBG_CONTINUE = 0x00010002
DBG_EXCEPTION_NOT_HANDLED      = 0x80010001
HIT_COUNT = 1
PASS_COUNT = 0
MIN_DISK_SIZE = 4000000
MAX_DISK_SIZE = 2000000000
SERVER_SIZE = 50000000

visible = False
testing = False
breaking = False
debug = False
vnc = True
profiling = False
use_taskset = True
use_snapshots = False
dry_run = False
needs_ready = False
ff = True
save_disks = False
save_shared = False
cooldown_level = 20

machines = {
    'hs2-01': {'disk' : 'hs2-01-win7.qcow2', 'vnc' : ':1', 'taskset' : '1,2,3',     'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'hs2-02': {'disk' : 'hs2-02-win7.qcow2', 'vnc' : ':2', 'taskset' : '4,5,6',     'monitor' : '/tmp/monitor-hs2', 'serial' : '/tmp/serial-hs2'},
    'hs2-03': {'disk' : 'hs2-03-win7.qcow2', 'vnc' : ':3', 'taskset' : '7,8,9',     'monitor' : '/tmp/monitor-hs3', 'serial' : '/tmp/serial-hs3'},
    'hs2-04': {'disk' : 'hs2-04-win7.qcow2', 'vnc' : ':4', 'taskset' : '10,11,12',  'monitor' : '/tmp/monitor-hs4', 'serial' : '/tmp/serial-hs4'},
    'hs2-05': {'disk' : 'hs2-05-win7.qcow2', 'vnc' : ':5', 'taskset' : '13,14,15',  'monitor' : '/tmp/monitor-hs5', 'serial' : '/tmp/serial-hs5'},
    'hs2-06': {'disk' : 'hs2-06-win7.qcow2', 'vnc' : ':6', 'taskset' : '16,17,18',  'monitor' : '/tmp/monitor-hs6', 'serial' : '/tmp/serial-hs6'},
    'hs2-07': {'disk' : 'hs2-07-win7.qcow2', 'vnc' : ':7', 'taskset' : '19,20,21',  'monitor' : '/tmp/monitor-hs7', 'serial' : '/tmp/serial-hs7'},
    'hs2-08': {'disk' : 'hs2-08-win7.qcow2', 'vnc' : ':8', 'taskset' : '22,23,24',  'monitor' : '/tmp/monitor-hs8', 'serial' : '/tmp/serial-hs8'},
    'hs2-09': {'disk' : 'hs2-09-win7.qcow2', 'vnc' : ':9', 'taskset' : '1,2,3',     'monitor' : '/tmp/monitor-hs9', 'serial' : '/tmp/serial-hs9'},
    'hs2-10': {'disk' : 'hs2-10-win7.qcow2', 'vnc' : ':10', 'taskset' : '4,5,6',    'monitor' : '/tmp/monitor-hs10', 'serial' : '/tmp/serial-hs10'},
    'hs2-11': {'disk' : 'hs2-11-win7.qcow2', 'vnc' : ':11', 'taskset' : '7,8,9',    'monitor' : '/tmp/monitor-hs11', 'serial' : '/tmp/serial-hs11'},
    'hs2-12': {'disk' : 'hs2-12-win7.qcow2', 'vnc' : ':12', 'taskset' : '10,11,12', 'monitor' : '/tmp/monitor-hs12', 'serial' : '/tmp/serial-hs12'},
    'hs3-01': {'disk' : 'hs3-01.qcow2', 'vnc' : ':1', 'taskset' : '1,2,3',     'monitor' : '/tmp/monitor-hs31', 'serial' : '/tmp/serial-hs31'},
    'hs3-01': {'disk' : 'hs3-02.qcow2', 'vnc' : ':2', 'taskset' : '4,5,6',     'monitor' : '/tmp/monitor-hs32', 'serial' : '/tmp/serial-hs32'}
}

script_codes = {}

ma_addrs = []
ma_rvas = []
ma_st_addrs = []
ma_st_rvas = []
ma_end_addrs = []
ma_end_rvas = []
ma_react_addrs = []
ma_react_rvas = []
ma_rd_addrs = []
ma_rd_rvas = []

samples_shared_path = "e:\\samples\\shared"
samples_shared_subdir = "samples/shared"
samples_saved = "e:\\samples\\saved"
samples_binned = "f:\\"
samples_original = "d:\\samples\\original"
samples_size_margin = 0.3
log_path = "../logs"
corrector = None
buffer_size = 4096
log_name = "HS:Unknown"
extension = "sample"
wait_sleep = 5
fuzzbox_timeout = 20
start_sleep = 60
revert_sleep = 40
settle_sleep = 3
restart_count = 100000
closing_plugin_name = "close_sample_opera"
revert_scripts = ["load_ready"]
slowdown = 1
mutations = 3
mutator = None
generator = "generatorCorrected.Generator"
metric_res = 100
to_mult_factor = 30
boot_wait = 25
revert_wait = 20
shutdown_wait = 25
init_timeout = 10 
log_level = 4
#smp = 2
smp = 4 # in order to precisely measure usage
large_results_dir = '/mnt/2/output'
revert_snapshot = 'clean'
qemu_machines = "/home/hs1/machines/qemu"
qemu_m = "3G"
qemu_shared_folder = "/mnt/1/output"
qemu_additional =  ['-enable-kvm']
qemu_command = 'qemu-system-i386'
qemu_drive_options = 'cache=none,if=virtio'
qemu_env = None
qemu_drives_dir="/home/hs1/images"
qemu_secondary=""
qemu_drive_format = 'mkfs.ntfs'
host_mount_options="loop,umask=0000"
saved_dir="/mnt/1/raw/"
log_dir="/mnt/1/log/"
builtin_reactions_anchors="EN:a0:a2:a4:a6:a8:b0:b2:b4:b6:b8:c0:c2:c4:c6:c8:d0:d2:d4:d6:d8:e0:e2:e4:e6:e8:f0:f2:f4:f6:f8:g0:g2:g4:g6:g8:h0:h2:h6:h8:i0:i2:i4:i6:i8:j0:j2:j4:j6:j8:k0:k2:k4:k6:k8:l0:l2:l4:l6:l8:m0:m2:m4:m6:m8:n0:n2:n4:n6:n8:o0:o2:o4:o6:o8:p0:p2:p4:p6:p8:q0:q2:q4:q6:q8:r0:r2:r4:r6:r8"
builtin_reactions="""
kernel32.dll+0x2acf,EN,0x0;                 # ExitProcess
kernel32.dll+0x2064,CREATEPROCESSA+:CREATEPROCESSA-,0x0;              # CreateProcessA start
kernel32.dll+0x208e,CREATEPROCESSA-:CREATEPROCESSA+,0x0;              # CreateProcessA end
kernel32.dll+0x202f,CREATEPROCESSW+:CREATEPROCESSW-,0x0;              # CreateProcessW start
kernel32.dll+0x2059,CREATEPROCESSW-:CREATEPROCESSW+,0x0;              # CreateProcessW end
kernel32.dll+0x8f405,CREATEREMOTETHREAD+:CREATEREMOTETHREAD-,0x0;             # CreateRemoteThread start
kernel32.dll+0x8f424,CREATEREMOTETHREAD-:CREATEREMOTETHREAD+,0x0;             # CreateRemoteThread end
KERNELBASE.dll+0x916f,OPENPROCESS+:OPENPROCESS-,0x0;            # OpenProcess start
KERNELBASE.dll+0x91c2,OPENPROCESS-:OPENPROCESS+,0x0;            # OpenProcess end
WININET.dll+0x20494,INTERNETCONNECTW+1:INTERNETCONNECTW-1,0x0;
WININET.dll+0x20546,INTERNETCONNECTW-1:INTERNETCONNECTW+1,0x0;
WININET.dll+0x20551,INTERNETCONNECTA+1:INTERNETCONNECTA-1,0x0;
WININET.dll+0x2059f,INTERNETCONNECTA-1:INTERNETCONNECTA+1,0x0;
WININET.dll+0x1eef5,HTTPSENDREQUESTW+1:HTTPSENDREQUESTW-1,0x0;
WININET.dll+0x1ef6c,HTTPSENDREQUESTW-1:HTTPSENDREQUESTW+1,0x0;
WININET.dll+0x900fe,HTTPSENDREQUESTA+1:HTTPSENDREQUESTA-1,0x0;
WININET.dll+0x90140,HTTPSENDREQUESTA-1:HTTPSENDREQUESTA+1,0x0;
WININET.dll+0x14ea3,INTERNETSETOPTION+1:INTERNETSETOPTION-1,0x0;
WININET.dll+0x14f6c,INTERNETSETOPTION-1:INTERNETSETOPTION+1,0x0;
WININET.dll+0x20615,HTTPOPENREQUESTW+1:HTTPOPENREQUESTW-1,0x0;
WININET.dll+0x20845,HTTPOPENREQUESTW-1:HTTPOPENREQUESTW+1,0x0;
WININET.dll+0x2043c,HTTPOPENREQUESTA+1:HTTPOPENREQUESTA-1,0x0;
WININET.dll+0x20489,HTTPOPENREQUESTA-1:HTTPOPENREQUESTA+1,0x0;
WININET.dll+0x22d7d,HTTPQUERYINFOW+1:HTTPQUERYINFOW-1,0x0;
WININET.dll+0x22e74,HTTPQUERYINFOW-1:HTTPQUERYINFOW+1,0x0;
WININET.dll+0x1cc04,HTTPQUERYINFOA+1:HTTPQUERYINFOA-1,0x0;
WININET.dll+0x1cdb5,HTTPQUERYINFOA-1:HTTPQUERYINFOA+1,0x0;
WININET.dll+0x1e2a6,INTERNETREADFILE+1:INTERNETREADFILE-1,0x0;
WININET.dll+0x1e2e5,INTERNETREADFILE-1:INTERNETREADFILE+1,0x0;
KERNELBASE.dll+0x6811,GETLASTERROR+1:GETLASTERROR-1,0x0;
KERNELBASE.dll+0x6817,GETLASTERROR-1:GETLASTERROR+1,0x0;
user32.dll+0x36dfa,C7:C8,0x0;               # SetWindowsHookExA start
user32.dll+0x36e13,C8:C7,0x0;               # SetWindowsHookExA end
user32.dll+0x1210a,C9:C0,0x0;               # SetWindowsHookExW start
user32.dll+0x12123,C0:C9,0x0;               # SetWindowsHookExW end
kernel32.dll+0x901cd,SETTHREADCONTEXT+,0x0;                # SetThreadContext start
kernel32.dll+0x901ed,SETTHREADCONTEXT-,0x0;                # SetThreadContext start
KERNELBASE.dll+0x21cc3,D1:D2,0x0;           # QueueUserAPC start
KERNELBASE.dll+0x21d23,D2:D1,0x0;           # QueueUserAPC end
KERNELBASE.dll+0xc3cb,RESUMETHREAD+:RESUMETHREAD-,0x0;            # ResumeThread start
KERNELBASE.dll+0xc3e6,RESUMETHREAD-:RESUMETHREAD+,0x0;            # ResumeThread end
kernel32.dll+0x4ef66,W9:W0,0x0;             # Sleep start
kernel32.dll+0x4ef6c,W0:W9,0x100;           # Sleep end
kernel32.dll+0x45f70,V1:V2,0x0;             # SleepEx start
kernel32.dll+0x45f76,V2:V1,0x100;           # SleepEx end
ntdll.dll+0x464f0,s0:s1,0x1;                # KiFastSystemCall
ntdll.dll+0x464f4,s1:s0,0x2;                # KiFastSystemRet
kernel32.dll+0x51837,a0:a1,0x301:0x312;     # GetProcAddr start
kernel32.dll+0x5183c,a1:a0,0x100;           # GetProcAddr end
kernel32.dll+0x528d7,a2:a3,0x311;           # GetModuleHandleA start
kernel32.dll+0x528dc,a3:a2,0x100;           # GetModuleHandleA end
kernel32.dll+0x52864,a4:a5,0x311;           # LoadLibraryA start
kernel32.dll+0x5289a,a5:a4,0x100;           # LoadLibraryA end
kernel32.dll+0x528b2,a6:a7,0x321;           # LoadLibraryW start
kernel32.dll+0x528c4,a7:a6,0x100;           # LoadLibraryW end
kernel32.dll+0x528fc,a8:a9,0x311;           # CreateFileA start
kernel32.dll+0x52941,a9:a8,0x330;           # CreateFileA end
kernel32.dll+0x50b5d,b0:b1,0x321;           # CreateFileW start
kernel32.dll+0x50ba8,b1:b0,0x330;           # CreateFileW end
kernel32.dll+0x511cc,b2:b3,0x301;           # WriteFile start
kernel32.dll+0x5121b,b3:b2,0x100;           # WriteFile end
kernel32.dll+0x4daa9,b4:b5,0x301;           # ReadFile start
kernel32.dll+0x4db02,b5:b4,0x100;           # ReadFile end
kernel32.dll+0x51074,b6:b7,0x301;           # GetModuleFileNameA start
kernel32.dll+0x5107a,b7:b6,0x100;           # GetModuleFileNameA end
kernel32.dll+0x529f4,b8:b9,0x301;           # GetModuleFileNameW start
kernel32.dll+0x529fa,b9:b8,0x100;           # GetModuleFileNameW end
kernel32.dll+0x519a1,c0:c1,0x321;           # GetModuleHandleW start
kernel32.dll+0x519a7,c1:c0,0x100;           # GetModuleHandleW end
kernel32.dll+0x6962f,c2:c3,0x301;           # GetThreadContext start
kernel32.dll+0x6964c,c3:c2,0x100;           # GetThreadContext end
kernel32.dll+0x901cb,c4:c5,0x301;           # SetThreadContext start
kernel32.dll+0x901ee,c5:c4,0x100;           # SetThreadContext end
kernel32.dll+0x50d8f,c6:c7,0x303;           # OpenThread start
kernel32.dll+0x50d95,c7:c6,0x100;           # OpenThread end
kernel32.dll+0x3b42c,c8:c9,0x301:0x302:0x303;           # VirtualAllocEx start
kernel32.dll+0x3b432,c9:c8,0x100;           # VirtualAllocEx end
kernel32.dll+0x6859f,d0:d1,0x301:0x302;     # WriteProcessMemory start
kernel32.dll+0x685a5,d1:d0,0x100;           # WriteProcessMemory end
kernel32.dll+0x53142,d2:d3,0x301;           # SetUnhandledExceptionFilter start
kernel32.dll+0x531ff,d3:d2,0x100;           # SetUnhandledExceptionFilter end
SHELL32.dll+0x110654,d4:d5,0x302;           # SHGetFolderPathA start
SHELL32.dll+0x1106bf,d5:d4,0x100;           # SHGetFolderPathA end
SHELL32.dll+0x07ca26,d6:d7,0x302;           # SHGetFolderPathW start
SHELL32.dll+0x07cb29,d7:d6,0x100;           # SHGetFolderPathW end
SHELL32.dll+0x2526ae,d8:d9,0x303;           # SHGetSpecialFolderPathA start
SHELL32.dll+0x2526d7,d9:d8,0x100;           # SHGetSpecialFolderPathA end
SHELL32.dll+0x01fe3d,e0:e1,0x303;           # SHGetSpecialFolderPathW start
SHELL32.dll+0x01fe66,e1:e0,0x100;           # SHGetSpecialFolderPathW end
user32.dll+0x0006563,e2:e3,0x302;           # LoadStringA start
user32.dll+0x00065ad,e3:e2,0x100;           # LoadStringA end
user32.dll+0x0015533,e4:e5,0x302;           # LoadStringW start
user32.dll+0x001554d,e5:e4,0x100;           # LoadStringW end
KERNELBASE.dll+0xfb98,e6:e7,0x311;          # DeleteFileA start
KERNELBASE.dll+0xfbc8,e7:e6,0x100;          # DeleteFileA end
KERNELBASE.dll+0xf6fd,e8:e9,0x321;          # DeleteFileW start
KERNELBASE.dll+0xf811,e9:e8,0x100;          # DeleteFileW end
ADVAPI32.dll+0x0d3c1,f0:f1,0x301:0x312;                     # RegCreateKeyA start
ADVAPI32.dll+0x0d3fb,f1:f0,0x343;                           # RegCreateKeyA end
ADVAPI32.dll+0x11cc0,f2:f3,0x301:0x322;                     # RegCreateKeyW start
ADVAPI32.dll+0x11cfb,f3:f2,0x343;                           # RegCreateKeyW end
kernel32.dll+0x4670b,f4:f5,0x301:0x312;                     # RegCreateKeyExA start
kernel32.dll+0x46739,f5:f4,0x348;                           # RegCreateKeyExA end
kernel32.dll+0x43be1,f6:f7,0x301:0x322;                     # RegCreateKeyExW start
kernel32.dll+0x43c0f,f7:f6,0x348;                           # RegCreateKeyExW end
ADVAPI32.dll+0x5df73,f8:f9,0x301:0x312:0x313:0x304:0x305;   # RegSetKeyValueA start
ADVAPI32.dll+0x5dfd5,f9:f8,0x100;                           # RegSetKeyValueA end
ADVAPI32.dll+0x2daf2,g0:g1,0x301:0x322:0x323:0x304:0x305;   # RegSetKeyValueW start
ADVAPI32.dll+0x2db4d,g1:g0,0x100;                           # RegSetKeyValueW end
ADVAPI32.dll+0x5f529,g2:g3,0x301:0x312:0x314;               # RegSetValueA start
ADVAPI32.dll+0x5f5f5,g3:g2,0x100;                           # RegSetValueA end
ADVAPI32.dll+0x2fa72,g4:g5,0x301:0x322:0x324;               # RegSetValueW start
ADVAPI32.dll+0x2fb24,g5:g4,0x100;                           # RegSetValueW end
kernel32.dll+0x46889,g6:g7,0x301:0x312:0x304:0x305;         # RegSetValueExA start
kernel32.dll+0x469fc,g7:g6,0x100;                           # RegSetValueExA end
kernel32.dll+0x4c902,g8:g9,0x301:0x322:0x304:0x305;         # RegSetValueExW start
kernel32.dll+0x4ca2a,g9:g8,0x100;                           # RegSetValueExW end
ADVAPI32.dll+0x0d403,h0:h1,0x301:0x312;                     # RegQueryValueA start
ADVAPI32.dll+0x0d4f8,h1:h0,0x303;                           # RegQueryValueA end
ADVAPI32.dll+0x1b96b,h2:h3,0x301:0x322;                     # RegQueryValueW start
ADVAPI32.dll+0x1ba32,h3:h2,0x303;                           # RegQueryValueW end
kernel32.dll+0x52677,h4:h5,0x301:0x312;                     # RegQueryValueExA start
kernel32.dll+0x527c7,h5:h4,0x305;                           # RegQueryValueExA end
kernel32.dll+0x4fcf1,h6:h7,0x301:0x322;                     # RegQueryValueExW start
kernel32.dll+0x4fdfa,h7:h6,0x305;                           # RegQueryValueExW end
ADVAPI32.dll+0x0d2ed,h8:h9,0x301:0x312;                     # RegOpenKeyA start
ADVAPI32.dll+0x0d31c,h9:h8,0x343;                           # RegOpenKeyA end
ADVAPI32.dll+0x13129,i0:i1,0x301:0x322;                     # RegOpenKeyW start
ADVAPI32.dll+0x13159,i1:i0,0x343;                           # RegOpenKeyW end
kernel32.dll+0x515bd,i2:i3,0x301:0x312;                     # RegOpenKeyExA start
kernel32.dll+0x515df,i3:i2,0x345;                           # RegOpenKeyExA end
kernel32.dll+0x4f729,i4:i5,0x301:0x322;                     # RegOpenKeyExW start
kernel32.dll+0x4f74b,i5:i4,0x345;                           # RegOpenKeyExW end
ADVAPI32.dll+0x0d539,i6:i7,0x301:0x307;                     # RegEnumValueA start
ADVAPI32.dll+0x0d53f,i7:i6,0x100;                           # RegEnumValueA end
ADVAPI32.dll+0x1bb72,i8:i9,0x301:0x307;                     # RegEnumValueW start
ADVAPI32.dll+0x1bb78,i9:i8,0x100;                           # RegEnumValueW end
ntdll.dll+0x000469d0,j0:j1,0x311:0x312;        # strstr start
ntdll.dll+0x00046a0d,j1:j0,0x100;              # strstr end
ntdll.dll+0x000347f0,j2:j3,0x311:0x312;        # strcat start
ntdll.dll+0x000348dd,j3:j2,0x100;              # strcat end
ntdll.dll+0x000348e0,j4:j5,0x311:0x312;        # strcmp start
ntdll.dll+0x0003492b,j5:j4,0x100;              # strcmp end
ntdll.dll+0x000347e0,j6:j7,0x311:0x312;        # strcpy start
ntdll.dll+0x000347e5,j7:j6,0x100;              # strcpy end
ntdll.dll+0x00034a80,j8:j9,0x311:0x312;        # strncat start
ntdll.dll+0x00034a85,j9:j8,0x100;              # strncat end
ntdll.dll+0x0007191c,k0:k1,0x311:0x312;        # strncmp start
ntdll.dll+0x00071983,k1:k0,0x100;              # strncmp end
ntdll.dll+0x0005ee27,k2:k3,0x301:0x318;        # LdrGetProcedureAddress start
ntdll.dll+0x0005ee40,k3:k2,0x100;              # LdrGetProcedureAddress end
ntdll.dll+0x0005eb05,k4:k5,0x301:0x318;        # LdrGetProcedureAddressEx start invalid
ntdll.dll+0x0005ec50,k5:k4,0x100;              # LdrGetProcedureAddressEx end
kernel32.dll+0x02062,k6:k7,0x301;              # CreateProcessA start
kernel32.dll+0x0208f,k7:k6,0x100;              # CreateProcessA end
kernel32.dll+0x0202d,k8:k9,0x311;              # CreateProcessW start
kernel32.dll+0x0205a,k9:k8,0x100;              # CreateProcessW end
SHELL32.dll+0x249ba5,l0:l1,0x312:0x313:0x314;  # ShellExecuteA start
SHELL32.dll+0x249c28,l1:l0,0x100;              # ShellExecuteA end
SHELL32.dll+0x0141f0,l2:l3,0x322:0x323:0x324;  # ShellExecuteW start
SHELL32.dll+0x014277,l3:l2,0x100;              # ShellExecuteW end
WININET.dll+0x02054f,l4:l5,0x301;              # InternetConnectA start
WININET.dll+0x0205a0,l5:l4,0x100;              # InternetConnectA end 
WININET.dll+0x020492,l6:l7,0x301;              # InternetConnectW start
WININET.dll+0x020547,l7:l6,0x100;              # InternetConnectW end 
WININET.dll+0x0900fc,l8:l9,0x301;              # HttpSendRequestA start
WININET.dll+0x090141,l9:l8,0x100;              # HttpSendRequestA end 
WININET.dll+0x01eef3,m0:m1,0x301;              # HttpSendRequestW start
WININET.dll+0x01ef6d,m1:m0,0x100;              # HttpSendRequestW end 
KERNELBASE.dll+0x6811,m2:m3,0x100;             # GetLastError start
KERNELBASE.dll+0x681a,m3:m2,0x330;             # GetLastError end
WININET.dll+0x014d49,m4:m5,0x301:0x302:0x303:0x304; # InternetSetOptionA start
WININET.dll+0x014e79,m5:m4,0x330;              # InternetSetOptionA end
WININET.dll+0x014ea1,m6:m7,0x301:0x302:0x303:0x304; # InternetSetOptionW start
WININET.dll+0x014f6d,m7:m6,0x330;            # InternetSetOptionW end
kernel32.dll+0x50f86,m8:m9,0x323;           # WideCharToMultiByte start
kernel32.dll+0x50f8c,m9:m8,0x100;           # WideCharToMultiByte end
WININET.dll+0x01e2a4,n0:n1,0x301:0x302:0x303; # InternetReadFile start
WININET.dll+0x01e2e6,n1:n0,0x330;            # InternetReadFile end
WININET.dll+0x02043a,n2:n3,0x313;            # HttpOpenRequestA start
WININET.dll+0x02048a,n3:n2,0x100;            # HttpOpenRequestA end
WININET.dll+0x020613,n4:n5,0x323;            # HttpOpenRequestW start
WININET.dll+0x020846,n5:n4,0x100;            # HttpOpenRequestW end
KERNELBASE.dll+0xba73,n6:n7,0x313;          # CreateMutexA start
KERNELBASE.dll+0xba91,n7:n6,0x330;          # CreateMutexA end
KERNELBASE.dll+0x9271,n8:n9,0x323;          # CreateMutexW start
KERNELBASE.dll+0x9293,n9:n8,0x330;          # CreateMutexW end
KERNELBASE.dll+0xe3ba,o0:o1,0x313;          # OpenMutexA start
KERNELBASE.dll+0xe3fc,o1:o0,0x330;          # OpenMutexA end
KERNELBASE.dll+0xed96,o2:o3,0x323;          # OpenMutexW start
KERNELBASE.dll+0xee1d,o3:o2,0x330;          # OpenMutexW end
kernel32.dll+0x04efa0,o4:o5,0x301;           # WaitForSingleObject start
kernel32.dll+0x04efb3,o5:o4,0x100;           # WaitForSingleObject end
kernel32.dll+0x04efc0,o6:o7,0x301;           # WaitForSingleObjectEx start
kernel32.dll+0x04f005,o7:o6,0x100;           # WaitForSingleObjectEx end
kernel32.dll+0x04f286,o8:o9,0x301;           # WaitForMultipleObjects start
kernel32.dll+0x04f29f,o9:o8,0x100;           # WaitForMultipleObjects end
kernel32.dll+0x04f09c,p0:p1,0x301;           # WaitForMultipleObjectsEx start
kernel32.dll+0x04f13d,p1:p0,0x100;           # WaitForMultipleObjectsEx end
kernel32.dll+0x0527fd,p2:p3,0x303:0x304;     # CreateThread start
kernel32.dll+0x05281e,p3:p2,0x100;           # CreateThread end
WININET.dll+0x27e1c,p4:p5,0x311:0x302:0x313:0x314:0x305; # InternetOpenA start
WININET.dll+0x27f86,p5:p4,0x330;             # InternetOpenA end
WININET.dll+0x29da0,p6:p7,0x321:0x302:0x323:0x324:0x305; # InternetOpenW start
WININET.dll+0x29ea0,p7:p6,0x330;             # InternetOpenW end
WININET.dll+0x2dc18,p8:p9,0x301:0x312:0x313:0x304:0x305:0x346; # InternetOpenUrlA start
WININET.dll+0x2dc60,p9:p8,0x330;             # InternetOpenUrlA end
WININET.dll+0x7dc14,q0:q1,0x301:0x322:0x323:0x304:0x305:0x346; # InternetOpenUrlW start
WININET.dll+0x7dcc4,q1:q0,0x330;             # InternetOpenUrlW end 
WININET.dll+0x1cc02,q2:q3,0x301:0x302:0x303:0x344:0x345; # HttpQueryInfoA start
WININET.dll+0x1cdb6,q3:q2,0x330;             # HttpQueryInfoA end
WININET.dll+0x22d7b,q4:q5,0x301:0x302:0x303:0x344:0x345; # HttpQueryInfoA start
WININET.dll+0x22e75,q5:q4,0x330;             # HttpQueryInfoA end
kernel32.dll+0x8f403,q6:q7,0x301:0x302:0x303:0x304:0x305:0x306;             # CreateRemoteThread start
kernel32.dll+0x8f425,q7:q6,0x100;             # CreateRemoteThread end
KERNELBASE.dll+0xdada,q8:q9,0x100;          # GetCommandLineA start
KERNELBASE.dll+0xdadf,q9:q8,0x350;          # GetCommandLineA end
KERNELBASE.dll+0x12f2f,r0:r1,0x100;         # GetCommandLineW start
KERNELBASE.dll+0x12f34,r1:r0,0x360;         # GetCommandLineW end
kernel32.dll+0x376ab,r2:r3,0x311:0x302:0x303;         # CreateMailslotA start
kernel32.dll+0x376e8,r3:r2,0x330;         # CreateMailslotA end
kernel32.dll+0x376f0,r4:r5,0x311:0x302:0x303;         # CreateMailslotW start
kernel32.dll+0x377c8,r5:r4,0x330;         # CreateMailslotW end
WININET.dll+0x90016,r6:r7,0x301:0x302:0x303:0x304:0x309;              # HttpSendRequestExA start
WININET.dll+0x90091,r7:r6,0x330;              # HttpSendRequestExA end 
WININET.dll+0x38e8c,r8:r9,0x301:0x302:0x303:0x304:0x305;              # HttpSendRequestExW start
WININET.dll+0x38f31,r9:r8,0x330;              # HttpSendRequestExW end 
"""

hs_user="hs1"

def runner_0(options, args=None):
    pass

def runner(args=None):
    pass

def specific_preperations_1(options, args=None):
    pass

def specific_preperations_2(options, args=None):
    pass

def specific_preperations_3(options, args=None):
    pass

def specific_preperations_4(options, args=None):
    pass

def specific_preperations_5(options, args=None):
    pass

scripts_1 = []
scripts_2 = []
scripts_3 = []
scripts_4 = []
scripts_5 = []

def prepare_sample(sample_path):
    return sample_path
