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
    'hs2-01': {'disk' : 'hs2-01.raw', 'vnc' : ':1', 'taskset' : '1,2,3', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'hs2-02': {'disk' : 'hs2-02-win7.qcow', 'vnc' : ':2', 'taskset' : '5,6,7', 'monitor' : '/tmp/monitor-hs2', 'serial' : '/tmp/serial-hs2'},
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
smp = 1 # in order to precisely measure usage
large_results_dir = '/mnt/2/output'
revert_snapshot = 'clean'
qemu_machines = "/home/hs1/machines/qemu"
qemu_m = "3G"
qemu_shared_folder = "/home/hs1/shared"
qemu_additional =  ['-enable-kvm']
qemu_command = 'qemu-system-i386'
qemu_drive_options = 'cache=none,if=virtio'
qemu_env = None
qemu_drives_dir="/home/hs1/images"
qemu_secondary=""
qemu_drive_format = 'mkfs.ntfs'
host_mount_options="loop,umask=0000"
saved_dir="/mnt/3/workspace/large-out"
log_dir="/mnt/3/workspace/large-out"

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