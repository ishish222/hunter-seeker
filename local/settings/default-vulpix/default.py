# important ones

# new options

# external

external_paths_machines = '/home/ish/machines/qemu-hs2'
external_paths_src = '/home/ish/malware_malware_malware'
external_paths_final_all_output = '/home/ish/hs2-data/output'
external_paths_tmp_all_input_output = '/home/ish/hs2-data/research'
external_paths_log = '/home/ish/hs2-data/log'
external_paths_images = '/home/ish/hs2-data/images'
external_paths_link_tmp_output_final_output = True

external_qemu_socket_timeout_step = 10
external_qemu_socket_timeout_mult = 1
external_qemu_binary_path = 'qemu-system-i386'
external_qemu_memory_options = '3G'
external_qemu_drive_options = 'if=virtio'
external_qemu_use_vnc = True
external_qemu_use_taskset = False
external_qemu_additional_options = '-enable-kvm'
external_qemu_machines = {
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
    'r1-01': {'disk' : 'r1-01.qcow2', 'vnc' : ':1', 'taskset' : '1,2,3',     'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'r1-02': {'disk' : 'r1-02.qcow2', 'vnc' : ':2', 'taskset' : '4,5,6',     'monitor' : '/tmp/monitor-hs2', 'serial' : '/tmp/serial-hs2'},
    'r1-03': {'disk' : 'r1-03.qcow2', 'vnc' : ':3', 'taskset' : '7,8,9',     'monitor' : '/tmp/monitor-hs3', 'serial' : '/tmp/serial-hs3'}
}

# for creating drive
internal_paths_root = '\\\\10.0.2.4\\qemu'
internal_paths_input = internal_paths_root + '\\input'
internal_paths_output = internal_paths_root + '\\output'
internal_paths_log = internal_paths_root + '\\logs'

internal_sys_reactions_anchors = "ExitProcess_start:GetModuleHandleA_start:LoadLibraryA_start:LoadLibraryW_start:CreateFileA_start:CreateFileW_start:WriteFile_start:ReadFile_start:GetModuleFileNameA_start:GetModuleFileNameW_start:GetModuleHandleW_start:GetThreadContext_start:SetThreadContext_start:OpenThread_start:VirtualAllocEx_start:WriteProcessMemory_start:SetUnhandledExceptionFilter_start:SHGetFolderPathA_start:SHGetFolderPathW_start:SHGetSpecialFolderPathA_start:SHGetSpecialFolderPathW_start:LoadStringA_start:LoadStringW_start:DeleteFileA_start:DeleteFileW_start:RegCreateKeyA_start:RegCreateKeyW_start:RegCreateKeyExA_start:RegCreateKeyExW_start:RegSetKeyValueA_start:RegSetKeyValueW_start:RegSetValueA_start:RegSetValueW_start:RegSetValueExA_start:RegSetValueExW_start:RegQueryValueA_start:RegQueryValueW_start:RegQueryValueExA_start:RegQueryValueExW_start:RegOpenKeyA_start:RegOpenKeyW_start:RegOpenKeyExA_start:RegOpenKeyExW_start:RegEnumValueA_start:RegEnumValueW_start:CreateProcessA_start:CreateProcessW_start:ShellExecuteA_start:ShellExecuteW_start:InternetConnectA_start:InternetConnectW_start:HttpSendRequestA_start:HttpSendRequestW_start:InternetSetOptionA_start:InternetSetOptionW_start:InternetReadFile_start:HttpOpenRequestA_start:HttpOpenRequestW_start:GetLastError_start:CreateMutexA_start:CreateMutexW_start:OpenMutexW_start:WideCharToMultiByte_start:WaitForSingleObject_start:WaitForSingleObjectEx_start:WaitForMultipleObjects_start:WaitForMultipleObjectsEx_start:OpenMutexA_start:KiFastSystemCall_start:strstr_start:strcat_start:strcmp_start:strcpy_start:strncat_start:strncmp_start:LdrGetProcedureAddress_start:LdrGetProcedureAddressEx_start"

internal_sys_reactions = """
kernel32.dll+0x0005214f,ExitProcess_start,0x0;
kernel32.dll+0x00090193,SetThreadContext_start,0x0;
kernel32.dll+0x00002082,CreateProcessA_start:CreateProcessA_end,0x0;
kernel32.dll+0x000020af,CreateProcessA_end:CreateProcessA_start,0x0;
kernel32.dll+0x0000204d,CreateProcessW_start:CreateProcessW_end,0x0;
kernel32.dll+0x0000207a,CreateProcessW_end:CreateProcessW_start,0x0;
kernel32.dll+0x0008f33b,CreateRemoteThread_start:CreateRemoteThread_end,0x0;
kernel32.dll+0x0008f35d,CreateRemoteThread_end:CreateRemoteThread_start,0x0;
user32.dll+0x00036d0c,SetWindowsHookExA_start:SetWindowsHookExA_end,0x0;
user32.dll+0x00036d25,SetWindowsHookExA_end:SetWindowsHookExA_start,0x0;
user32.dll+0x0000e30c,SetWindowsHookExW_start:SetWindowsHookExW_end,0x0;
user32.dll+0x0000e325,SetWindowsHookExW_end:SetWindowsHookExW_start,0x0;
kernelbase.dll+0x000226b3,QueueUserAPC_start:QueueUserAPC_end,0x0;
kernelbase.dll+0x00022713,QueueUserAPC_end:QueueUserAPC_start,0x0;
kernelbase.dll+0x0000b65f,ResumeThread_start:ResumeThread_end,0x0;
kernelbase.dll+0x0000b67d,ResumeThread_end:ResumeThread_start,0x0;
kernel32.dll+0x0004ba46,Sleep_start:Sleep_end,0x0;
kernel32.dll+0x0004ba4c,Sleep_end:Sleep_start,0x100;
kernel32.dll+0x00040296,SleepEx_start:SleepEx_end,0x0;
kernel32.dll+0x0004029c,SleepEx_end:SleepEx_start,0x100;
kernel32.dll+0x000533d3,GetProcAddress_start:GetProcAddress_end,0x301:0x312;
kernel32.dll+0x000533d9,GetProcAddress_end:GetProcAddress_start,0x100;
kernel32.dll+0x0004cf41,GetModuleHandleA_start:GetModuleHandleA_end,0x311;
kernel32.dll+0x0004cf47,GetModuleHandleA_end:GetModuleHandleA_start,0x100;
kernel32.dll+0x0005395c,LoadLibraryA_start:LoadLibraryA_end,0x311;
kernel32.dll+0x00053992,LoadLibraryA_end:LoadLibraryA_start,0x100;
kernel32.dll+0x00053c01,LoadLibraryW_start:LoadLibraryW_end,0x321;
kernel32.dll+0x00053c13,LoadLibraryW_end:LoadLibraryW_start,0x100;
kernel32.dll+0x0004cee8,CreateFileA_start:CreateFileA_end,0x311;
kernel32.dll+0x0004cf2e,CreateFileA_end:CreateFileA_start,0x330;
kernel32.dll+0x0004cc56,CreateFileW_start:CreateFileW_end,0x321;
kernel32.dll+0x0004cca1,CreateFileW_end:CreateFileW_start,0x330;
kernel32.dll+0x00051400,WriteFile_start:WriteFile_end,0x301;
kernel32.dll+0x0005144f,WriteFile_end:WriteFile_start,0x100;
kernel32.dll+0x000496fb,ReadFile_start:ReadFile_end,0x301;
kernel32.dll+0x00049754,ReadFile_end:ReadFile_start,0x100;
kernel32.dll+0x000533f6,GetModuleFileNameA_start:GetModuleFileNameA_end,0x301;
kernel32.dll+0x00053403,GetModuleFileNameA_end:GetModuleFileNameA_start,0x100;
kernel32.dll+0x00053c26,GetModuleFileNameW_start:GetModuleFileNameW_end,0x301;
kernel32.dll+0x00053c2c,GetModuleFileNameW_end:GetModuleFileNameW_start,0x100;
kernel32.dll+0x0005374d,GetModuleHandleW_start:GetModuleHandleW_end,0x321;
kernel32.dll+0x00053753,GetModuleHandleW_end:GetModuleHandleW_start,0x100;
kernel32.dll+0x00060cc1,GetThreadContext_start:GetThreadContext_end,0x301;
kernel32.dll+0x00060cde,GetThreadContext_end:GetThreadContext_start,0x100;
kernel32.dll+0x00090193,SetThreadContext_start:SetThreadContext_end,0x301;
kernel32.dll+0x000901b6,SetThreadContext_end:SetThreadContext_start,0x100;
kernel32.dll+0x00056733,OpenThread_start:OpenThread_end,0x303;
kernel32.dll+0x00056740,OpenThread_end:OpenThread_start,0x100;
kernel32.dll+0x0003c1b6,VirtualAllocEx_start:VirtualAllocEx_end,0x302;
kernel32.dll+0x0003c1bc,VirtualAllocEx_end:VirtualAllocEx_start,0x100;
kernel32.dll+0x0003c1de,WriteProcessMemory_start:WriteProcessMemory_end,0x301:0x302;
kernel32.dll+0x0003c1e4,WriteProcessMemory_end:WriteProcessMemory_start,0x100;
kernel32.dll+0x00053d01,SetUnhandledExceptionFilter_start:SetUnhandledExceptionFilter_end,0x301;
kernel32.dll+0x00053dbe,SetUnhandledExceptionFilter_end:SetUnhandledExceptionFilter_start,0x100;
shell32.dll+0x00117804,SHGetFolderPathA_start:SHGetFolderPathA_end,0x302;
shell32.dll+0x0011786f,SHGetFolderPathA_end:SHGetFolderPathA_start,0x100;
shell32.dll+0x00085708,SHGetFolderPathW_start:SHGetFolderPathW_end,0x302;
shell32.dll+0x00085810,SHGetFolderPathW_end:SHGetFolderPathW_start,0x100;
shell32.dll+0x0024fb26,SHGetSpecialFolderPathA_start:SHGetSpecialFolderPathA_end,0x303;
shell32.dll+0x0024fb4f,SHGetSpecialFolderPathA_end:SHGetSpecialFolderPathA_start,0x100;
shell32.dll+0x00020468,SHGetSpecialFolderPathW_start:SHGetSpecialFolderPathW_end,0x303;
shell32.dll+0x00020491,SHGetSpecialFolderPathW_end:SHGetSpecialFolderPathW_start,0x100;
user32.dll+0x000066a7,LoadStringA_start:LoadStringA_end,0x302;
user32.dll+0x000066f1,LoadStringA_end:LoadStringA_start,0x100;
user32.dll+0x0000dfba,LoadStringW_start:LoadStringW_end,0x302;
user32.dll+0x0000dfd4,LoadStringW_end:LoadStringW_start,0x100;
kernelbase.dll+0x0000c8a1,DeleteFileA_start:DeleteFileA_end,0x311;
kernelbase.dll+0x0000c8d1,DeleteFileA_end:DeleteFileA_start,0x100;
kernelbase.dll+0x000109b5,DeleteFileW_start:DeleteFileW_end,0x321;
kernelbase.dll+0x00010ac9,DeleteFileW_end:DeleteFileW_start,0x100;
advapi32.dll+0x0000cd01,RegCreateKeyA_start:RegCreateKeyA_end,0x301:0x312;
advapi32.dll+0x0000cd3b,RegCreateKeyA_end:RegCreateKeyA_start,0x343;
advapi32.dll+0x00011514,RegCreateKeyW_start:RegCreateKeyW_end,0x301:0x322;
advapi32.dll+0x0001154f,RegCreateKeyW_end:RegCreateKeyW_start,0x343;
advapi32.dll+0x00011469,RegCreateKeyExA_start:RegCreateKeyExA_end,0x301:0x312;
advapi32.dll+0x0001146f,RegCreateKeyExA_end:RegCreateKeyExA_start,0x348;
advapi32.dll+0x000140fe,RegCreateKeyExW_start:RegCreateKeyExW_end,0x301:0x322;
advapi32.dll+0x00014104,RegCreateKeyExW_end:RegCreateKeyExW_start,0x348;
advapi32.dll+0x0005f88b,RegSetKeyValueA_start:RegSetKeyValueA_end,0x301:0x312:0x313:0x304:0x305;
advapi32.dll+0x0005f8ed,RegSetKeyValueA_end:RegSetKeyValueA_start,0x100;
advapi32.dll+0x00027180,RegSetKeyValueW_start:RegSetKeyValueW_end,0x301:0x322:0x323:0x304:0x305;
advapi32.dll+0x000271c8,RegSetKeyValueW_end:RegSetKeyValueW_start,0x100;
advapi32.dll+0x00060e41,RegSetValueA_start:RegSetValueA_end,0x301:0x312:0x314;
advapi32.dll+0x00060f15,RegSetValueA_end:RegSetValueA_start,0x100;
advapi32.dll+0x0002a68a,RegSetValueW_start:RegSetValueW_end,0x301:0x322:0x324;
advapi32.dll+0x0002a73c,RegSetValueW_end:RegSetValueW_start,0x100;
advapi32.dll+0x000114b3,RegSetValueExA_start:RegSetValueExA_end,0x301:0x312:0x304:0x305;
advapi32.dll+0x000114b9,RegSetValueExA_end:RegSetValueExA_start,0x100;
advapi32.dll+0x000114d6,RegSetValueExW_start:RegSetValueExW_end,0x301:0x322:0x304:0x305;
advapi32.dll+0x000114dc,RegSetValueExW_end:RegSetValueExW_start,0x100;
advapi32.dll+0x0000ce12,RegQueryValueA_start:RegQueryValueA_end,0x301:0x312;
advapi32.dll+0x0000cf07,RegQueryValueA_end:RegQueryValueA_start,0x303;
advapi32.dll+0x000144b4,RegQueryValueW_start:RegQueryValueW_end,0x301:0x322;
advapi32.dll+0x000145c6,RegQueryValueW_end:RegQueryValueW_start,0x303;
kernel32.dll+0x000535b3,RegQueryValueExA_start:RegQueryValueExA_end,0x301:0x312;
kernel32.dll+0x000536e9,RegQueryValueExA_end:RegQueryValueExA_start,0x305;
kernel32.dll+0x00053218,RegQueryValueExW_start:RegQueryValueExW_end,0x301:0x322;
kernel32.dll+0x00053327,RegQueryValueExW_end:RegQueryValueExW_start,0x305;
advapi32.dll+0x0000cc15,RegOpenKeyA_start:RegOpenKeyA_end,0x301:0x312;
advapi32.dll+0x0000cc52,RegOpenKeyA_end:RegOpenKeyA_start,0x343;
advapi32.dll+0x00012459,RegOpenKeyW_start:RegOpenKeyW_end,0x301:0x322;
advapi32.dll+0x00012497,RegOpenKeyW_end:RegOpenKeyW_start,0x343;
kernel32.dll+0x0005340e,RegOpenKeyExA_start:RegOpenKeyExA_end,0x301:0x312;
kernel32.dll+0x00053430,RegOpenKeyExA_end:RegOpenKeyExA_start,0x345;
kernel32.dll+0x0004c189,RegOpenKeyExW_start:RegOpenKeyExW_end,0x301:0x322;
kernel32.dll+0x0004c1ab,RegOpenKeyExW_end:RegOpenKeyExW_start,0x345;
advapi32.dll+0x0000cf49,RegEnumValueA_start:RegEnumValueA_end,0x301:0x307;
advapi32.dll+0x0000cf56,RegEnumValueA_end:RegEnumValueA_start,0x100;
advapi32.dll+0x000148cc,RegEnumValueW_start:RegEnumValueW_end,0x301:0x307;
advapi32.dll+0x000148d2,RegEnumValueW_end:RegEnumValueW_start,0x100;
kernel32.dll+0x00002082,CreateProcessA_start:CreateProcessA_end,0x301;
kernel32.dll+0x000020af,CreateProcessA_end:CreateProcessA_start,0x100;
kernel32.dll+0x0000204d,CreateProcessW_start:CreateProcessW_end,0x311;
kernel32.dll+0x0000207a,CreateProcessW_end:CreateProcessW_start,0x100;
shell32.dll+0x00247078,ShellExecuteA_start:ShellExecuteA_end,0x302:0x303;
shell32.dll+0x002470fb,ShellExecuteA_end:ShellExecuteA_start,0x100;
shell32.dll+0x00013c71,ShellExecuteW_start:ShellExecuteW_end,0x312:0x313;
shell32.dll+0x00013cf8,ShellExecuteW_end:ShellExecuteW_start,0x100;
wininet.dll+0x000249e9,InternetConnectA_start:InternetConnectA_end,0x301;
wininet.dll+0x00024a3a,InternetConnectA_end:InternetConnectA_start,0x100;
wininet.dll+0x0002492c,InternetConnectW_start:InternetConnectW_end,0x301;
wininet.dll+0x000249e1,InternetConnectW_end:InternetConnectW_start,0x100;
wininet.dll+0x000918f8,HttpSendRequestA_start:HttpSendRequestA_end,0x301;
wininet.dll+0x0009193d,HttpSendRequestA_end:HttpSendRequestA_start,0x100;
wininet.dll+0x0002ba12,HttpSendRequestW_start:HttpSendRequestW_end,0x301;
wininet.dll+0x0002ba8c,HttpSendRequestW_end:HttpSendRequestW_start,0x100;
wininet.dll+0x000175e8,InternetSetOptionA_start:InternetSetOptionA_end,0x100; # is this correct?
wininet.dll+0x00017718,InternetSetOptionA_end:InternetSetOptionA_start,0x100;
wininet.dll+0x00017741,InternetSetOptionW_start:InternetSetOptionW_end,0x100; # is this correct?
wininet.dll+0x0001780d,InternetSetOptionW_end:InternetSetOptionW_start,0x100;
wininet.dll+0x0001b406,InternetReadFile_start:InternetReadFile_end,0x100; #is this correct?
wininet.dll+0x0001b448,InternetReadFile_end:InternetReadFile_start,0x100;
wininet.dll+0x00024c7d,HttpOpenRequestA_start:HttpOpenRequestA_end,0x313;
wininet.dll+0x00024ccd,HttpOpenRequestA_end:HttpOpenRequestA_start,0x100;
wininet.dll+0x00024a42,HttpOpenRequestW_start:HttpOpenRequestW_end,0x323;
wininet.dll+0x00024c75,HttpOpenRequestW_end:HttpOpenRequestW_start,0x100;
kernelbase.dll+0x00006806,GetLastError_start:GetLastError_end,0x100;
kernelbase.dll+0x0000680f,GetLastError_end:GetLastError_start,0x330;
kernelbase.dll+0x0000c354,CreateMutexA_start:CreateMutexA_end,0x313;
kernelbase.dll+0x0000c372,CreateMutexA_end:CreateMutexA_start,0x330;
kernelbase.dll+0x0000adfd,CreateMutexW_start:CreateMutexW_end,0x323;
kernelbase.dll+0x0000ae1f,CreateMutexW_end:CreateMutexW_start,0x330;
kernelbase.dll+0x00008ca9,OpenMutexW_start:OpenMutexW_end,0x323;
kernelbase.dll+0x00008d30,OpenMutexW_end:OpenMutexW_start,0x330;
kernel32.dll+0x0005450e,WideCharToMultiByte_start:WideCharToMultiByte_end,0x323;
kernel32.dll+0x00054514,WideCharToMultiByte_end:WideCharToMultiByte_start,0x100;
kernel32.dll+0x0004ba90,WaitForSingleObject_start:WaitForSingleObject_end,0x301;
kernel32.dll+0x0004baa3,WaitForSingleObject_end:WaitForSingleObject_start,0x100;
kernel32.dll+0x0004bab0,WaitForSingleObjectEx_start:WaitForSingleObjectEx_end,0x301;
kernel32.dll+0x0004baf5,WaitForSingleObjectEx_end:WaitForSingleObjectEx_start,0x100;
kernel32.dll+0x0004bce4,WaitForMultipleObjects_start:WaitForMultipleObjects_end,0x301;
kernel32.dll+0x0004bcfd,WaitForMultipleObjects_end:WaitForMultipleObjects_start,0x100;
kernel32.dll+0x0004bc00,WaitForMultipleObjectsEx_start:WaitForMultipleObjectsEx_end,0x301;
kernel32.dll+0x0004bca1,WaitForMultipleObjectsEx_end:WaitForMultipleObjectsEx_start,0x100;
kernel32.dll+0x0003fe1a,OpenMutexA_start:OpenMutexA_end,0x313;
kernel32.dll+0x0003fe5a,OpenMutexA_end:OpenMutexA_start,0x330;
ntdll.dll+0x000470b0,KiFastSystemCall_start:KiFastSystemCallRet_start,0x1;
ntdll.dll+0x000470b4,KiFastSystemCallRet_start:KiFastSystemCall_start,0x2;
ntdll.dll+0x000475c0,strstr_start:strstr_end,0x311:0x312;
ntdll.dll+0x00047644,strstr_end:strstr_start,0x100;
ntdll.dll+0x000353d0,strcat_start:strcat_end,0x311:0x312;
ntdll.dll+0x000354bd,strcat_end:strcat_start,0x100;
ntdll.dll+0x000354c0,strcmp_start:strcmp_end,0x311:0x312;
ntdll.dll+0x00035546,strcmp_end:strcmp_start,0x100;
ntdll.dll+0x000353c0,strcpy_start:strcpy_end,0x311:0x312;
ntdll.dll+0x000353c5,strcpy_end:strcpy_start,0x100;
ntdll.dll+0x00035650,strncat_start:strncat_end,0x311:0x312;
ntdll.dll+0x00035786,strncat_end:strncat_start,0x100;
ntdll.dll+0x000725ec,strncmp_start:strncmp_end,0x311:0x312;
ntdll.dll+0x00072653,strncmp_end:strncmp_start,0x100;
ntdll.dll+0x0006228d,LdrGetProcedureAddress_start:LdrGetProcedureAddress_end,0x301:0x318;
ntdll.dll+0x000622a6,LdrGetProcedureAddress_end:LdrGetProcedureAddress_start,0x100;
ntdll.dll+0x0005f881,LdrGetProcedureAddressEx_start:LdrGetProcedureAddressEx_end,0x301:0x318;
ntdll.dll+0x0005f9e8,LdrGetProcedureAddressEx_end:LdrGetProcedureAddressEx_start,0x100;
"""

