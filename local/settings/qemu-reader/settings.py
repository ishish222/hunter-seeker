import generators.changer as changer

DBG_CONTINUE = 0x00010002
HIT_COUNT = 1
PASS_COUNT = 0

visible = False
testing = False
breaking = False
debug = True
vnc = True
profiling = False
use_taskset = True

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

def react1(dbg):
    dbg.dlog("SR marker reached")
    dbg.reqScript("E3")
    return DBG_CONTINUE

script_codes = {}
script_codes["E3"] = "enter3"

ma_addrs = []
ma_rvas = []
ma_st_addrs = []
ma_st_rvas = [("AcroRd32.exe", 0x4cae0, 0)]
ma_end_addrs = []
ma_end_rvas = [("user32.dll", 0x18fe9, 7)]
ma_react_addrs = []
ma_react_rvas = [("AcroRd32.dll", 0x3664da, (0, react1, ["space"]))]
#ma_react_rvas = [("AcroRd32.dll", 0x3664df, (0, react1, ["space"]))]
ma_rd_addrs = []
ma_rd_rvas = [("user32.dll", 0x18fe9, 2)]

samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
app_path = "C:\\Program Files\\Adobe\\Reader 11.0\\Reader\\AcroRd32.exe"
app_module = "AcroRd32.exe"
corrector = None
buffer_size = 4096
log_name = "HS:Reader"
wait_sleep = 15
fuzzbox_timeout = wait_sleep*4
start_sleep = 5
revert_sleep = 40
settle_sleep = 3
restart_count = 100000
closing_plugin_name = "close_sample_opera"
revert_scripts = ["load_ready"]
slowdown = 1.5
extension = "pdf"
mutations = 3
mutator = changer.Changer
metric_res = 10
save_disks = False
to_mult_factor = 100
boot_wait = 60
shutdown_wait = 60

qemu_machines = "/home/hs1/machines/qemu"
qemu_m = "3G"
qemu_shared_folder = "/home/hs1/hs2-current-deploy/qemu-reader"
qemu_additional =  ['-enable-kvm', '-cpu', 'host', '-smp', '1']
qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options, args=None):
    pass

scripts_1 = ["lclick", "python_server_spawn_e"]
log_level = 4

def check_counters(ea):
    dbg.counters[ea] = (dbg.counters[ea][PASS_COUNT], dbg.counters[ea][HIT_COUNT]+1)
    dbg.dlog("Current hit no: %d, pass count: %d" % (dbg.counters[ea][HIT_COUNT], dbg.counters[ea][PASS_COUNT]), 1)

    if(dbg.counters[ea][HIT_COUNT] == dbg.counters[ea][PASS_COUNT]+1):
        return True
    else:
        return False

def st_marker_test(dbg):
    if(check_counters(dbg.exception_address)):
        dbg.dlog("ST marker test")
        dbg.signal_st()
    return DBG_CONTINUE

def end_marker_test(dbg):
    if(check_counters(dbg.exception_address)):
        dbg.dlog("END marker test")
        dbg.signal_ma()
    return DBG_CONTINUE

#st_marker_handler = st_marker_test
#end_marker_handler = end_marker_test

def specific_preperations_2(options, args=None):
    pass

scripts_2 = []

def specific_preperations_3(options, args=None):
    pass

scripts_3 = []

def specific_preperations_4(options, args=None):
    pass

scripts_4 = []

from time import sleep

def specific_preperations_5(options):
    pass

scripts_5 = ["enter3", "close_sample_reader"]

def prepare_sample(sample_path):
    return sample_path
