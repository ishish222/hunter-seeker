DBG_CONTINUE = 0x00010002
HIT_COUNT = 1
PASS_COUNT = 0

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

machines = {
    'hs2-01': {'disk' : 'hs2-01.raw', 'vnc' : ':1', 'taskset' : '1,2,3', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'hs2-02': {'disk' : 'hs2-02.raw', 'vnc' : ':2', 'taskset' : '5,6,7', 'monitor' : '/tmp/monitor-hs2', 'serial' : '/tmp/serial-hs2'},
    'hs2-03': {'disk' : 'hs2-03.raw', 'vnc' : ':3', 'taskset' : '9,10,11', 'monitor' : '/tmp/monitor-hs3', 'serial' : '/tmp/serial-hs3'},
    'hs2-04': {'disk' : 'hs2-04.raw', 'vnc' : ':4', 'taskset' : '13,14,15', 'monitor' : '/tmp/monitor-hs4', 'serial' : '/tmp/serial-hs4'},
    'hs2-05': {'disk' : 'hs2-05.raw', 'vnc' : ':5', 'taskset' : '17,18,19', 'monitor' : '/tmp/monitor-hs5', 'serial' : '/tmp/serial-hs5'},
    'hs2-06': {'disk' : 'hs2-06.raw', 'vnc' : ':6', 'taskset' : '21,22,23', 'monitor' : '/tmp/monitor-hs6', 'serial' : '/tmp/serial-hs6'},
    'hs2-07': {'disk' : 'hs2-07.raw', 'vnc' : ':7', 'taskset' : '14', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'hs2-08': {'disk' : 'hs2-08.raw', 'vnc' : ':8', 'taskset' : '224', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'hs2-09': {'disk' : 'hs2-09.raw', 'vnc' : ':9', 'taskset' : '3584', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'hs2-10': {'disk' : 'hs2-10.raw', 'vnc' : ':10', 'taskset' : '57344', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'hs2-11': {'disk' : 'hs2-11.raw', 'vnc' : ':11', 'taskset' : '917504', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
    'hs2-12': {'disk' : 'hs2-12.raw', 'vnc' : ':12', 'taskset' : '14680064', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'}
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

samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
samples_original = "d:\\samples\\original"
log_path = "../logs"
corrector = None
buffer_size = 4096
log_name = "HS:Unknown"
extension = "sample"
wait_sleep = 5
fuzzbox_timeout = 20
start_sleep = 5
revert_sleep = 40
settle_sleep = 3
restart_count = 100000
closing_plugin_name = "close_sample_opera"
revert_scripts = ["load_ready"]
slowdown = 1
mutations = 3
mutator = None
metric_res = 10
to_mult_factor = 30
boot_wait = 25
revert_wait = 20
shutdown_wait = 25
init_timeout = 10 
log_level = 4


qemu_machines = "/home/hs1/machines/qemu"
qemu_m = "3G"
qemu_shared_folder = "/home/hs1/hs2-current-deploy/qemu-reader"
qemu_additional =  ['-enable-kvm', '-smp', '2']

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
