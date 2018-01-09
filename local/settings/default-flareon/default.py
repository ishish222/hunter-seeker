# important ones

guest_samples_path = "e:\\samples"
guest_samples_shared_path = guest_samples_path + "\\shared"
guest_samples_saved_path = guest_samples_path + "\\saved"
guest_samples_binned_path = "f:\\"
guest_samples_original_path = "d:\\samples\\original"


host_data_path = "/mnt/1"
host_data_output_path = host_data_path + "/output"
host_data_research_path = host_data_path + "/research"
host_log_dir = host_data_path + "/log"
host_machines_path = "/home/hs1/machines/qemu"
host_images_path = host_data_path + "/images"
host_sample_source = "/home/hs1/malware_samples"
host_output_link = "/mnt/1/output"
host_qemu_command = '/home/hs1/qemu-bin-2.11/qemu-2.11.0/deploy_dir/bin/qemu-system-i386'
host_qemu_drive_options = 'if=virtio'

# for creating drive
host_samples_path = "samples"
host_samples_shared_path = host_samples_path + "/shared"
host_samples_saved = host_data_path + "/raw"


host_machines = {
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
    'sc-01': {'disk' : 'sc-01.qcow2', 'vnc' : ':1', 'taskset' : '1,2,3',     'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'sc-02': {'disk' : 'sc-02.qcow2', 'vnc' : ':2', 'taskset' : '4,5,6',     'monitor' : '/tmp/monitor-hs2', 'serial' : '/tmp/serial-hs2'},
    'sc-03': {'disk' : 'sc-03.qcow2', 'vnc' : ':3', 'taskset' : '7,8,9',     'monitor' : '/tmp/monitor-hs3', 'serial' : '/tmp/serial-hs3'},
    'rzut_1-01': {'disk' : 'rzut_1-01.qcow2', 'vnc' : ':1', 'taskset' : '1,2,3',     'monitor' : '/tmp/rzut-1_01-monitor', 'serial' : '/tmp/rzut-1_01-serial'},
    'rzut_1-02': {'disk' : 'rzut_1-02.qcow2', 'vnc' : ':2', 'taskset' : '4,5,6',     'monitor' : '/tmp/rzut-1_02-monitor', 'serial' : '/tmp/rzut-1_02-serial'},
    'ORAN1701-01': {'disk' : 'ORAN1701-01.qcow2', 'vnc' : ':1', 'taskset' : '1,2,3',     'monitor' : '/tmp/rzut-1_01-monitor', 'serial' : '/tmp/rzut-1_01-serial'},
    'ORAN1701-02': {'disk' : 'ORAN1701-02.qcow2', 'vnc' : ':2', 'taskset' : '4,5,6',     'monitor' : '/tmp/rzut-1_02-monitor', 'serial' : '/tmp/rzut-1_02-serial'}
}

qemu_additional =  ['-enable-kvm']
qemu_drive_format = 'mkfs.ntfs'
qemu_drive_offset = 65536
qemu_env = None
qemu_secondary=""
qemu_mount_options="loop,umask=0000"
qemu_drive_options = host_qemu_drive_options

hs_user="ish"
revert_snapshot = 'clean'

# backward compatibility mapping

samples_shared_path = guest_samples_shared_path
samples_shared_subdir = host_samples_shared_path
samples_saved = guest_samples_saved_path
samples_binned = guest_samples_binned_path
samples_original = guest_samples_original_path

samples_size_margin = 0.3
log_path = "../logs"
qemu_machines = host_machines_path
qemu_drives_dir = host_images_path
qemu_shared_folder = host_data_output_path
host_mount_options = qemu_mount_options
saved_dir = host_samples_saved
log_dir = host_log_dir
machines = host_machines
template_offset = qemu_drive_offset


# probably deprecated

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
qemu_m = "3G"
drive_mount=1
#builtin_reactions_anchors="ExitProcess_start:s1:GetModuleHandleA_start:a1:GetModuleHandleA_start:a5:e6:LoadLibraryA_start:LoadLibraryW_start:CreateFileA_start:CreateFileW_start:WriteFile_start:c2:c6:c8:d0:d2:d4:d6:d8:e0:e1:e3:e5:e7:e9:f1:f3:f5:f7:f9:g1:g3:g5:g7:g9:h1:h3:h4:h6:h7:h9:i2:i3:i5:i7:i9:j1:j3:j5:j6:j8:j9:k0:k1:k3:k4:k6:k7:k9:k0:l2:l6:l8:l0:m2:m3:m5:m6:m8:m0:n2:n4:n0:o2:o4:o6:o8:o0:p2:p4:p6:p8:r0:r2:r4:r6:r8:t1:t3:t5"
builtin_reactions_anchors="ExitProcess_start:GetModuleHandleA_start:LoadLibraryA_start:LoadLibraryW_start:CreateFileA_start:CreateFileW_start:WriteFile_start:ReadFile_start:GetModuleFileNameA_start:GetModuleFileNameW_start:GetModuleHandleW_start:GetThreadContext_start:SetThreadContext_start:OpenThread_start:VirtualAllocEx_start:WriteProcessMemory_start:SetUnhandledExceptionFilter_start:SHGetFolderPathA_start:SHGetFolderPathW_start:SHGetSpecialFolderPathA_start:SHGetSpecialFolderPathW_start:LoadStringA_start:LoadStringW_start:DeleteFileA_start:DeleteFileW_start:RegCreateKeyA_start:RegCreateKeyW_start:RegCreateKeyExA_start:RegCreateKeyExW_start:RegSetKeyValueA_start:RegSetKeyValueW_start:RegSetValueA_start:RegSetValueW_start:RegSetValueExA_start:RegSetValueExW_start:RegQueryValueA_start:RegQueryValueW_start:RegQueryValueExA_start:RegQueryValueExW_start:RegOpenKeyA_start:RegOpenKeyW_start:RegOpenKeyExA_start:RegOpenKeyExW_start:RegEnumValueA_start:RegEnumValueW_start:CreateProcessA_start:CreateProcessW_start:ShellExecuteA_start:ShellExecuteW_start:InternetConnectA_start:InternetConnectW_start:HttpSendRequestA_start:HttpSendRequestW_start:InternetSetOptionA_start:InternetSetOptionW_start:InternetReadFile_start:HttpOpenRequestA_start:HttpOpenRequestW_start:GetLastError_start:CreateMutexA_start:CreateMutexW_start:OpenMutexW_start:WideCharToMultiByte_start:WaitForSingleObject_start:WaitForSingleObjectEx_start:WaitForMultipleObjects_start:WaitForMultipleObjectsEx_start:OpenMutexA_start:KiFastSystemCall_start:strstr_start:strcat_start:strcmp_start:strcpy_start:strncat_start:strncmp_start:LdrGetProcedureAddress_start:LdrGetProcedureAddressEx_start:connect_start:bind_start:closesocket_start:listen_start:InternetOpenA_start:InternetOpenW_start:InternetConnectW_start:InternetConnectA_start:InternetSetOptionA_start:InternetSetOptionW_start:InternetCloseHandle_start:HttpOpenRequestA_start:HttpOpenRequestW_start:HttpAddRequestHeadersA_start:HttpAddRequestHeadersW_start:HttpSendRequestA_start:HttpSendRequestW_start"
builtin_reactions="""
kernel32.dll+0x00052acf,ExitProcess_start,0x0;
kernel32.dll+0x000901cb,SetThreadContext_start,0x0;
kernel32.dll+0x00002062,CreateProcessA_start:CreateProcessA_end,0x0;
kernel32.dll+0x0000208f,CreateProcessA_end:CreateProcessA_start,0x0;
kernel32.dll+0x0000202d,CreateProcessW_start:CreateProcessW_end,0x0;
kernel32.dll+0x0000205a,CreateProcessW_end:CreateProcessW_start,0x0;
kernel32.dll+0x0008f403,CreateRemoteThread_start:CreateRemoteThread_end,0x0;
kernel32.dll+0x0008f425,CreateRemoteThread_end:CreateRemoteThread_start,0x0;
user32.dll+0x0004b669,SetWindowsHookA_start:SetWindowsHookA_end,0x0;
user32.dll+0x0004b67c,SetWindowsHookA_end:SetWindowsHookA_start,0x0;
user32.dll+0x00036dfa,SetWindowsHookExA_start:SetWindowsHookExA_end,0x0;
user32.dll+0x00036e13,SetWindowsHookExA_end:SetWindowsHookExA_start,0x0;
kernelbase.dll+0x00021cc3,QueueUserAPC_start:QueueUserAPC_end,0x0;
kernelbase.dll+0x00021d23,QueueUserAPC_end:QueueUserAPC_start,0x0;
kernelbase.dll+0x0000c3c9,ResumeThread_start:ResumeThread_end,0x0;
kernelbase.dll+0x0000c3e7,ResumeThread_end:ResumeThread_start,0x0;
kernel32.dll+0x0004ef66,Sleep_start:Sleep_end,0x0;
kernel32.dll+0x0004ef6c,Sleep_end:Sleep_start,0x100;
kernel32.dll+0x00045f70,SleepEx_start:SleepEx_end,0x0;
kernel32.dll+0x00045f7d,SleepEx_end:SleepEx_start,0x100;
kernel32.dll+0x00051837,GetProcAddress_start:GetProcAddress_end,0x301:0x312;
kernel32.dll+0x0005183d,GetProcAddress_end:GetProcAddress_start,0x100;
kernel32.dll+0x000528d7,GetModuleHandleA_start:GetModuleHandleA_end,0x311;
kernel32.dll+0x000528dd,GetModuleHandleA_end:GetModuleHandleA_start,0x100;
kernel32.dll+0x00052864,LoadLibraryA_start:LoadLibraryA_end,0x311;
kernel32.dll+0x0005289a,LoadLibraryA_end:LoadLibraryA_start,0x100;
kernel32.dll+0x000528b2,LoadLibraryW_start:LoadLibraryW_end,0x321;
kernel32.dll+0x000528c4,LoadLibraryW_end:LoadLibraryW_start,0x100;
kernel32.dll+0x000528fc,CreateFileA_start:CreateFileA_end,0x311;
kernel32.dll+0x00052942,CreateFileA_end:CreateFileA_start,0x330;
kernel32.dll+0x00050b5d,CreateFileW_start:CreateFileW_end,0x321;
kernel32.dll+0x00050ba8,CreateFileW_end:CreateFileW_start,0x330;
kernel32.dll+0x000511cc,WriteFile_start:WriteFile_end,0x301;
kernel32.dll+0x0005121b,WriteFile_end:WriteFile_start,0x100;
kernel32.dll+0x0004daa9,ReadFile_start:ReadFile_end,0x301;
kernel32.dll+0x0004db02,ReadFile_end:ReadFile_start,0x100;
kernel32.dll+0x00051074,GetModuleFileNameA_start:GetModuleFileNameA_end,0x301;
kernel32.dll+0x0005108c,GetModuleFileNameA_end:GetModuleFileNameA_start,0x100;
kernel32.dll+0x000529f4,GetModuleFileNameW_start:GetModuleFileNameW_end,0x301;
kernel32.dll+0x000529fa,GetModuleFileNameW_end:GetModuleFileNameW_start,0x100;
kernel32.dll+0x000519a1,GetModuleHandleW_start:GetModuleHandleW_end,0x321;
kernel32.dll+0x000519a7,GetModuleHandleW_end:GetModuleHandleW_start,0x100;
kernel32.dll+0x0006962f,GetThreadContext_start:GetThreadContext_end,0x301;
kernel32.dll+0x0006964c,GetThreadContext_end:GetThreadContext_start,0x100;
kernel32.dll+0x000901cb,SetThreadContext_start:SetThreadContext_end,0x301;
kernel32.dll+0x000901ee,SetThreadContext_end:SetThreadContext_start,0x100;
kernel32.dll+0x00050d8f,OpenThread_start:OpenThread_end,0x303;
kernel32.dll+0x00050d95,OpenThread_end:OpenThread_start,0x100;
kernel32.dll+0x0003b42c,VirtualAllocEx_start:VirtualAllocEx_end,0x302;
kernel32.dll+0x0003b432,VirtualAllocEx_end:VirtualAllocEx_start,0x100;
kernel32.dll+0x0006859f,WriteProcessMemory_start:WriteProcessMemory_end,0x301:0x302;
kernel32.dll+0x000685a5,WriteProcessMemory_end:WriteProcessMemory_start,0x100;
kernel32.dll+0x00053142,SetUnhandledExceptionFilter_start:SetUnhandledExceptionFilter_end,0x301;
kernel32.dll+0x000531ff,SetUnhandledExceptionFilter_end:SetUnhandledExceptionFilter_start,0x100;
shell32.dll+0x00110654,SHGetFolderPathA_start:SHGetFolderPathA_end,0x302;
shell32.dll+0x001106bf,SHGetFolderPathA_end:SHGetFolderPathA_start,0x100;
shell32.dll+0x0007ca26,SHGetFolderPathW_start:SHGetFolderPathW_end,0x302;
shell32.dll+0x0007cb29,SHGetFolderPathW_end:SHGetFolderPathW_start,0x100;
shell32.dll+0x002526ae,SHGetSpecialFolderPathA_start:SHGetSpecialFolderPathA_end,0x303;
shell32.dll+0x002526d7,SHGetSpecialFolderPathA_end:SHGetSpecialFolderPathA_start,0x100;
shell32.dll+0x0001fe3d,SHGetSpecialFolderPathW_start:SHGetSpecialFolderPathW_end,0x303;
shell32.dll+0x0001fe66,SHGetSpecialFolderPathW_end:SHGetSpecialFolderPathW_start,0x100;
user32.dll+0x00006563,LoadStringA_start:LoadStringA_end,0x302;
user32.dll+0x000065ad,LoadStringA_end:LoadStringA_start,0x100;
user32.dll+0x00015533,LoadStringW_start:LoadStringW_end,0x302;
user32.dll+0x0001554d,LoadStringW_end:LoadStringW_start,0x100;
kernelbase.dll+0x0000fb98,DeleteFileA_start:DeleteFileA_end,0x311;
kernelbase.dll+0x0000fbc8,DeleteFileA_end:DeleteFileA_start,0x100;
kernel32.dll+0x0004656b,DeleteFileW_start:DeleteFileW_end,0x321;
kernel32.dll+0x00046571,DeleteFileW_end:DeleteFileW_start,0x100;
advapi32.dll+0x0000d3c1,RegCreateKeyA_start:RegCreateKeyA_end,0x301:0x312;
advapi32.dll+0x0000d3fb,RegCreateKeyA_end:RegCreateKeyA_start,0x343;
advapi32.dll+0x00011cc0,RegCreateKeyW_start:RegCreateKeyW_end,0x301:0x322;
advapi32.dll+0x00011cfb,RegCreateKeyW_end:RegCreateKeyW_start,0x343;
advapi32.dll+0x00011b71,RegCreateKeyExA_start:RegCreateKeyExA_end,0x301:0x312;
advapi32.dll+0x00011b77,RegCreateKeyExA_end:RegCreateKeyExA_start,0x348;
advapi32.dll+0x0001b946,RegCreateKeyExW_start:RegCreateKeyExW_end,0x301:0x322;
advapi32.dll+0x0001b94c,RegCreateKeyExW_end:RegCreateKeyExW_start,0x348;
advapi32.dll+0x0005df73,RegSetKeyValueA_start:RegSetKeyValueA_end,0x301:0x312:0x313:0x304:0x305;
advapi32.dll+0x0005dfd5,RegSetKeyValueA_end:RegSetKeyValueA_start,0x100;
advapi32.dll+0x0002daf2,RegSetKeyValueW_start:RegSetKeyValueW_end,0x301:0x322:0x323:0x304:0x305;
advapi32.dll+0x0002db56,RegSetKeyValueW_end:RegSetKeyValueW_start,0x100;
advapi32.dll+0x0005f529,RegSetValueA_start:RegSetValueA_end,0x301:0x312:0x314;
advapi32.dll+0x0005f5fd,RegSetValueA_end:RegSetValueA_start,0x100;
advapi32.dll+0x0002fa72,RegSetValueW_start:RegSetValueW_end,0x301:0x322:0x324;
advapi32.dll+0x0002fb24,RegSetValueW_end:RegSetValueW_start,0x100;
advapi32.dll+0x00011b96,RegSetValueExA_start:RegSetValueExA_end,0x301:0x312:0x304:0x305;
advapi32.dll+0x00011b9c,RegSetValueExA_end:RegSetValueExA_start,0x100;
advapi32.dll+0x00011c82,RegSetValueExW_start:RegSetValueExW_end,0x301:0x322:0x304:0x305;
advapi32.dll+0x00011c88,RegSetValueExW_end:RegSetValueExW_start,0x100;
advapi32.dll+0x0000d403,RegQueryValueA_start:RegQueryValueA_end,0x301:0x312;
advapi32.dll+0x0000d4f8,RegQueryValueA_end:RegQueryValueA_start,0x303;
advapi32.dll+0x0001b96b,RegQueryValueW_start:RegQueryValueW_end,0x301:0x322;
advapi32.dll+0x0001ba36,RegQueryValueW_end:RegQueryValueW_start,0x303;
advapi32.dll+0x0001bc25,RegQueryValueExA_start:RegQueryValueExA_end,0x100;
advapi32.dll+0x0001bc2b,RegQueryValueExA_end:RegQueryValueExA_start,0x305;
advapi32.dll+0x0001bcd5,RegQueryValueExW_start:RegQueryValueExW_end,0x301:0x322;
advapi32.dll+0x0001bcdb,RegQueryValueExW_end:RegQueryValueExW_start,0x305;
advapi32.dll+0x0000d2ed,RegOpenKeyA_start:RegOpenKeyA_end,0x301:0x312;
advapi32.dll+0x0000d32a,RegOpenKeyA_end:RegOpenKeyA_start,0x343;
advapi32.dll+0x00013129,RegOpenKeyW_start:RegOpenKeyW_end,0x301:0x322;
advapi32.dll+0x00013167,RegOpenKeyW_end:RegOpenKeyW_start,0x343;
advapi32.dll+0x0001bc0d,RegOpenKeyExA_start:RegOpenKeyExA_end,0x301:0x312;
advapi32.dll+0x0001bc13,RegOpenKeyExA_end:RegOpenKeyExA_start,0x343;
advapi32.dll+0x0001bec4,RegOpenKeyExW_start:RegOpenKeyExW_end,0x301:0x322;
advapi32.dll+0x0001beca,RegOpenKeyExW_end:RegOpenKeyExW_start,0x343;
advapi32.dll+0x0000d539,RegEnumValueA_start:RegEnumValueA_end,0x301:0x307;
advapi32.dll+0x0000d546,RegEnumValueA_end:RegEnumValueA_start,0x100;
advapi32.dll+0x0001bb72,RegEnumValueW_start:RegEnumValueW_end,0x301:0x307;
advapi32.dll+0x0001bb78,RegEnumValueW_end:RegEnumValueW_start,0x100;
kernel32.dll+0x00002062,CreateProcessA_start:CreateProcessA_end,0x301;
kernel32.dll+0x0000208f,CreateProcessA_end:CreateProcessA_start,0x100;
kernel32.dll+0x0000202d,CreateProcessW_start:CreateProcessW_end,0x311;
kernel32.dll+0x0000205a,CreateProcessW_end:CreateProcessW_start,0x100;
shell32.dll+0x00249ba5,ShellExecuteA_start:ShellExecuteA_end,0x302:0x303;
shell32.dll+0x00249c28,ShellExecuteA_end:ShellExecuteA_start,0x100;
shell32.dll+0x000141f0,ShellExecuteW_start:ShellExecuteW_end,0x312:0x313;
shell32.dll+0x00014277,ShellExecuteW_end:ShellExecuteW_start,0x100;
wininet.dll+0x0002054f,InternetConnectA_start:InternetConnectA_end,0x301;
wininet.dll+0x000205a0,InternetConnectA_end:InternetConnectA_start,0x100;
wininet.dll+0x00020492,InternetConnectW_start:InternetConnectW_end,0x301;
wininet.dll+0x00020547,InternetConnectW_end:InternetConnectW_start,0x100;
wininet.dll+0x000900fc,HttpSendRequestA_start:HttpSendRequestA_end,0x301;
wininet.dll+0x00090141,HttpSendRequestA_end:HttpSendRequestA_start,0x100;
wininet.dll+0x0001eef3,HttpSendRequestW_start:HttpSendRequestW_end,0x301;
wininet.dll+0x0001ef6d,HttpSendRequestW_end:HttpSendRequestW_start,0x100;
wininet.dll+0x00014d49,InternetSetOptionA_start:InternetSetOptionA_end,0x100;
wininet.dll+0x00014e79,InternetSetOptionA_end:InternetSetOptionA_start,0x100;
wininet.dll+0x00014ea1,InternetSetOptionW_start:InternetSetOptionW_end,0x100;
wininet.dll+0x00014f6d,InternetSetOptionW_end:InternetSetOptionW_start,0x100;
wininet.dll+0x0001e2a4,InternetReadFile_start:InternetReadFile_end,0x100;
wininet.dll+0x0001e2e6,InternetReadFile_end:InternetReadFile_start,0x100;
wininet.dll+0x0002043a,HttpOpenRequestA_start:HttpOpenRequestA_end,0x313;
wininet.dll+0x0002048a,HttpOpenRequestA_end:HttpOpenRequestA_start,0x100;
wininet.dll+0x00020613,HttpOpenRequestW_start:HttpOpenRequestW_end,0x323;
wininet.dll+0x00020846,HttpOpenRequestW_end:HttpOpenRequestW_start,0x100;
kernelbase.dll+0x00006811,GetLastError_start:GetLastError_end,0x100;
kernelbase.dll+0x0000681a,GetLastError_end:GetLastError_start,0x330;
kernelbase.dll+0x0000ba73,CreateMutexA_start:CreateMutexA_end,0x313;
kernelbase.dll+0x0000ba91,CreateMutexA_end:CreateMutexA_start,0x330;
kernelbase.dll+0x00009271,CreateMutexW_start:CreateMutexW_end,0x323;
kernelbase.dll+0x00009293,CreateMutexW_end:CreateMutexW_start,0x330;
kernelbase.dll+0x0000ed96,OpenMutexW_start:OpenMutexW_end,0x323;
kernelbase.dll+0x0000ee1d,OpenMutexW_end:OpenMutexW_start,0x330;
kernel32.dll+0x00050f86,WideCharToMultiByte_start:WideCharToMultiByte_end,0x323;
kernel32.dll+0x00050f8c,WideCharToMultiByte_end:WideCharToMultiByte_start,0x100;
kernel32.dll+0x0004efa0,WaitForSingleObject_start:WaitForSingleObject_end,0x301;
kernel32.dll+0x0004efb3,WaitForSingleObject_end:WaitForSingleObject_start,0x100;
kernel32.dll+0x0004efc0,WaitForSingleObjectEx_start:WaitForSingleObjectEx_end,0x301;
kernel32.dll+0x0004f005,WaitForSingleObjectEx_end:WaitForSingleObjectEx_start,0x100;
kernel32.dll+0x0004f286,WaitForMultipleObjects_start:WaitForMultipleObjects_end,0x301;
kernel32.dll+0x0004f29f,WaitForMultipleObjects_end:WaitForMultipleObjects_start,0x100;
kernel32.dll+0x0004f09c,WaitForMultipleObjectsEx_start:WaitForMultipleObjectsEx_end,0x301;
kernel32.dll+0x0004f13d,WaitForMultipleObjectsEx_end:WaitForMultipleObjectsEx_start,0x100;
kernel32.dll+0x00043344,OpenMutexA_start:OpenMutexA_end,0x313;
kernel32.dll+0x00043389,OpenMutexA_end:OpenMutexA_start,0x330;
#kernel32.dll+0x00052a92,OpenMutexW_start:OpenMutexW_end,0x323;
#kernel32.dll+0x00052a98,OpenMutexW_end:OpenMutexW_start,0x330;
ntdll.dll+0x000464f0,KiFastSystemCall_start:KiFastSystemCallRet_start,0x1;
ntdll.dll+0x000464f4,KiFastSystemCallRet_start:KiFastSystemCall_start,0x2;
ntdll.dll+0x000469d0,strstr_start:strstr_end,0x311:0x312;
ntdll.dll+0x00046a54,strstr_end:strstr_start,0x100;
ntdll.dll+0x000347f0,strcat_start:strcat_end,0x311:0x312;
ntdll.dll+0x000348dd,strcat_end:strcat_start,0x100;
ntdll.dll+0x000348e0,strcmp_start:strcmp_end,0x311:0x312;
ntdll.dll+0x00034966,strcmp_end:strcmp_start,0x100;
ntdll.dll+0x000347e0,strcpy_start:strcpy_end,0x311:0x312;
ntdll.dll+0x000347e5,strcpy_end:strcpy_start,0x100;
ntdll.dll+0x00034a80,strncat_start:strncat_end,0x311:0x312;
ntdll.dll+0x00034bb6,strncat_end:strncat_start,0x100;
ntdll.dll+0x0007191c,strncmp_start:strncmp_end,0x311:0x312;
ntdll.dll+0x00071983,strncmp_end:strncmp_start,0x100;
ntdll.dll+0x0005ee27,LdrGetProcedureAddress_start:LdrGetProcedureAddress_end,0x318;
ntdll.dll+0x0005ee40,LdrGetProcedureAddress_end:LdrGetProcedureAddress_start,0x100;
ntdll.dll+0x0005eb05,LdrGetProcedureAddressEx_start:LdrGetProcedureAddressEx_end,0x318;
ntdll.dll+0x0005ec50,LdrGetProcedureAddressEx_end:LdrGetProcedureAddressEx_start,0x100;
ws2_32.dll+0x000048be,connect_start:connect_end,0x301;
ws2_32.dll+0x00004967,connect_end:connect_start,0x100;
ws2_32.dll+0x000046bc,bind_start:bind_end,0x301;
ws2_32.dll+0x0000471f,bind_end:bind_start,0x100;
ws2_32.dll+0x00003bed,closesocket_start:closesocket_end,0x301;
ws2_32.dll+0x00003c69,closesocket_end:closesocket_start,0x100;
ws2_32.dll+0x0000a6ea,listen_start:listen_end,0x300;
ws2_32.dll+0x0000a74b,listen_end:listen_start,0x100;
wininet.dll+0x00027e1c,InternetOpenA_start:InternetOpenA_end,0x305;
wininet.dll+0x00027f86,InternetOpenA_end:InternetOpenA_start,0x330;
wininet.dll+0x00029da0,InternetOpenW_start:InternetOpenW_end,0x305;
wininet.dll+0x00029eb6,InternetOpenW_end:InternetOpenW_start,0x330;
wininet.dll+0x00020492,InternetConnectW_start:InternetConnectW_end,0x301:0x321:0x302:0x307;
wininet.dll+0x00020547,InternetConnectW_end:InternetConnectW_start,0x100;
wininet.dll+0x0002054f,InternetConnectA_start:InternetConnectA_end,0x301:0x311:0x302:0x307;
wininet.dll+0x000205a0,InternetConnectA_end:InternetConnectA_start,0x100;
wininet.dll+0x00014d49,InternetSetOptionA_start:InternetSetOptionA_end,0x301:0x302;
wininet.dll+0x00014e79,InternetSetOptionA_end:InternetSetOptionA_start,0x100;
wininet.dll+0x00014ea1,InternetSetOptionW_start:InternetSetOptionW_end,0x301:0x302;
wininet.dll+0x00014f6d,InternetSetOptionW_end:InternetSetOptionW_start,0x100;
wininet.dll+0x0001c87e,InternetCloseHandle_start:InternetCloseHandle_end,0x301;
wininet.dll+0x0001c8b7,InternetCloseHandle_end:InternetCloseHandle_start,0x100;
wininet.dll+0x0002043a,HttpOpenRequestA_start:HttpOpenRequestA_end,0x301:0x312:0x313:0x314:0x315:0x307;
wininet.dll+0x0002048a,HttpOpenRequestA_end:HttpOpenRequestA_start,0x100;
wininet.dll+0x00020613,HttpOpenRequestW_start:HttpOpenRequestW_end,0x301:0x322:0x323:0x324:0x325:0x307;
wininet.dll+0x00020846,HttpOpenRequestW_end:HttpOpenRequestW_start,0x100;
wininet.dll+0x00020888,HttpAddRequestHeadersW_start:HttpAddRequestHeadersW_end,0x301:0x312;
wininet.dll+0x000208f9,HttpAddRequestHeadersW_end:HttpAddRequestHeadersW_start,0x100;
wininet.dll+0x00019afa,HttpAddRequestHeadersA_start:HttpAddRequestHeadersA_end,0x301:0x322;
wininet.dll+0x00019c21,HttpAddRequestHeadersA_end:HttpAddRequestHeadersA_start,0x100;
wininet.dll+0x0001eef3,HttpSendRequestW_start:HttpSendRequestW_end,0x301:0x322:0;
wininet.dll+0x0001ef6d,HttpSendRequestW_end:HttpSendRequestW_start,0x100;
wininet.dll+0x000900fc,HttpSendRequestA_start:HttpSendRequestA_end,0x301:0x312;
wininet.dll+0x00090141,HttpSendRequestA_end:HttpSendRequestA_start,0x100;
"""
