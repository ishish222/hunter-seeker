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
    'hs2-12': {'disk' : 'hs2-12-win7.qcow2', 'vnc' : ':12', 'taskset' : '10,11,12', 'monitor' : '/tmp/monitor-hs12', 'serial' : '/tmp/serial-hs12'}
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
builtin_reactions_anchors="EN:s1:a0:a3:a5:e6:a7:b1:b3:b5:b7:c0:c2:c4:c6:c8:d0:d2:d4:d6:d8:d0:e3"
builtin_reactions="""
kernel32.dll+0x2acf,EN,0x0;         # ExitProcess
kernel32.dll+0x2062,C1:C2,0x0;      # CreateProcessA start
kernel32.dll+0x208f,C2:C1,0x0;      # CreateProcessA end
kernel32.dll+0x202d,C3:C4,0x0;      # CreateProcessW start
kernel32.dll+0x205a,C4:C3,0x0;      # CreateProcessW end
kernel32.dll+0x8f403,C5:C6,0x0;     # CreateRemoteThread start
kernel32.dll+0x8f425,C6:C5,0x0;     # CreateRemoteThread end
user32.dll+0x36dfa,C7:C8,0x0;       # SetWindowsHookExA start
user32.dll+0x36e13,C8:C7,0x0;       # SetWindowsHookExA end
user32.dll+0x1210a,C9:C0,0x0;       # SetWindowsHookExW start
user32.dll+0x12123,C0:C9,0x0;       # SetWindowsHookExW end
KERNELBASE.dll+0x21cc3,D1:D2,0x0;   # QueueUserAPC start
KERNELBASE.dll+0x21d23,D2:D1,0x0;   # QueueUserAPC end
KERNELBASE.dll+0xc3c9,R1:R2,0x0;    # ResumeThread start
KERNELBASE.dll+0xc3e7,R2:R1,0x0;    # ResumeThread end
ntdll.dll+0x464f0,s1:s2,0x1;        # KiFastSystemCall
ntdll.dll+0x464f4,s2:s1,0x2;        # KiFastSystemRet
kernel32.dll+0x51837,a0:a2,0x301;   # GetProcAddr start
kernel32.dll+0x51837,a1,0x312;      # GetProcAddr start
kernel32.dll+0x5183c,a2:a0:a1,0x100;# GetProcAddr end
kernel32.dll+0x528d7,a3:a4,0x311;   # GetModuleHandleA start
kernel32.dll+0x528dc,a4:a3,0x100;   # GetModuleHandleA end
kernel32.dll+0x5177e,a5:a6,0x313;   # CreateMutexA start
kernel32.dll+0x51783,a6:a5,0x100;   # CreateMutexA end
kernel32.dll+0x45f40,e6:e7,0x323;   # CreateMutexW start
kernel32.dll+0x45f46,e7:e6,0x100;   # CreateMutexW end
kernel32.dll+0x52864,a7:a8,0x311;   # LoadLibraryA start
kernel32.dll+0x5289a,a8:a7,0x100;   # LoadLibraryA end
kernel32.dll+0x528b2,b1:b2,0x321;   # LoadLibraryW start
kernel32.dll+0x528c4,b2:b1,0x100;   # LoadLibraryW end
kernel32.dll+0x528fc,b3:b4,0x311;   # CreateFileA start
kernel32.dll+0x52941,b4:b3,0x330;   # CreateFileA end
kernel32.dll+0x50b5d,b5:b6,0x321;   # CreateFileW start
kernel32.dll+0x50ba8,b6:b5,0x330;   # CreateFileW end
kernel32.dll+0x511cc,b7:b8,0x301;   # WriteFile start
kernel32.dll+0x5121b,b8:b7,0x100;   # WriteFile end
kernel32.dll+0x4daa9,c2:c3,0x301;   # ReadFile start
kernel32.dll+0x4db02,c3:c2,0x100;   # ReadFile end
kernel32.dll+0x51074,c6:c7,0x301;   # GetModuleFileNameA start
kernel32.dll+0x5107a,c7:c6,0x100;   # GetModuleFileNameA end
kernel32.dll+0x529f4,c8:c9,0x301;   # GetModuleFileNameW start
kernel32.dll+0x529fa,c9:c8,0x100;   # GetModuleFileNameW end
kernel32.dll+0x519a1,d0:d1,0x321;   # GetModuleHandleW start
kernel32.dll+0x519a7,d1:d0,0x100;   # GetModuleHandleW end
kernel32.dll+0x6962f,d2:d3,0x301;   # GetThreadContext start
kernel32.dll+0x6964c,d3:d2,0x100;   # GetThreadContext end
kernel32.dll+0x901cb,d4:d5,0x301;   # SetThreadContext start
kernel32.dll+0x901cb,T1,0x0;        # SetThreadContext start
kernel32.dll+0x901ee,d5:d4,0x100;   # SetThreadContext end
kernel32.dll+0x50d8f,d6:d7,0x303;   # OpenThread start
kernel32.dll+0x50d95,d7:d6,0x100;   # OpenThread end
kernel32.dll+0x3b42c,d8:d9,0x302;   # VirtualAllocEx start
kernel32.dll+0x3b432,d9:d8,0x100;   # VirtualAllocEx end
kernel32.dll+0x6859f,e0:e2,0x301;   # WriteProcessMemory start
kernel32.dll+0x6859f,e1,0x302;      # WriteProcessMemory start
kernel32.dll+0x685a5,e2:e0:e1,0x100;# WriteProcessMemory end
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
