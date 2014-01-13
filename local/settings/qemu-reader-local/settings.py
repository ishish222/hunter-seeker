import generators.changer as changer

DBG_CONTINUE = 0x00010002
HIT_COUNT = 1
PASS_COUNT = 0

visible = False
testing = False
breaking = False
debug = True
vnc = True
profiling = True
use_taskset = False

machines = {
    'hs2-1': {'disk' : 'hs2-1.raw', 'ip' : '127.0.0.1', 'port' : 12345, 'vnc' : ':1', 'taskset' : '6', 'monitor' : '/tmp/monitor-hs1', 'serial' : '/tmp/serial-hs1'},
#    'hs2-1': {'disk' : 'hs2-1.raw', 'tap' : 'tap1', 'ip' : '192.168.1.101', 'mac' : '00:00:00:00:00:01', 'server_ip' : '192.168.1.1', 'server_port' : 12301, 'vnc' : ':1'},
    'hs2-2': {'disk' : 'hs2-2.qcow2', 'ip' : '127.0.0.1', 'port' : 12346, 'vnc' : '1'}
}

ff = True

def react1(dbg):
    dbg.dlog("SR marker reached")
    dbg.reqScript("E3")
    print("SR")
    return DBG_CONTINUE

script_codes = {}
script_codes["E3"] = "enter3"
script_codes["CL"] = "close_sample_reader"

ma_addrs = []
ma_rvas = []
ma_st_addrs = []
ma_st_rvas = [("AcroRd32.exe", 0x4cae0, 0)]
ma_end_addrs = []
ma_end_rvas = [("user32.dll", 0x18fe9, 7)]
#ma_end_rvas = [("user32.dll", 0x18fe9, 6)]
ma_react_addrs = []
ma_react_rvas = [("AcroRd32.dll", 0x3664da, (0, react1, ["space"]))]
#ma_react_rvas = [("AcroRd32.dll", 0x3664df, (0, react1, ["space"]))]
ma_rd_addrs = []
ma_rd_rvas = [("user32.dll", 0x18fe9, 2)]
#ma_rd_rvas = [("user32.dll", 0x18fe9, 1)]

samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
samples_original = "d:\\samples\\original"
log_path = "../logs"
app_path = "C:\\Program Files\\Adobe\\Reader 11.0\\Reader\\AcroRd32.exe"
app_module = "AcroRd32.exe"
corrector = None
buffer_size = 4096
log_name = "HS:Reader"
wait_sleep = 5
fuzzbox_timeout = wait_sleep*4
start_sleep = 5
revert_sleep = 40
settle_sleep = 3
restart_count = 100000
closing_plugin_name = "close_sample_opera"
#revert_scripts = ["m_readline", "load_ready"]
revert_scripts = ["load_ready"]
slowdown = 1
extension = "pdf"
mutations = 3
mutator = changer.Changer
metric_res = 10
save_disks = True
to_mult_factor = 30
boot_wait = 25
shutdown_wait = 25


qemu_machines = "/home/ish/machines/qemu"
qemu_m = "3G"
qemu_shared_folder = "/home/ish/projects/2012-08-02-korrino/hs2-current-deploy/qemu-reader-local"
qemu_additional =  ['-enable-kvm', '-smp', '1']
#qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options, args=None):
    from sys import path
    path.append("../client")
    from script import rs
    rs("lclick", options.m)
    rs("python_server_spawn_args", options.m, args=args)

scripts_1 = []
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
        dbg.signal_end()
    return DBG_CONTINUE

#st_marker_handler = st_marker_test
#end_marker_handler = end_marker_test

def specific_preperations_2(options, args=None):
#    options.m.stdin.write("eject ide1-cd0\n")
    pass

scripts_2 = []

def specific_preperations_3(options, args=None):
    pass

scripts_3 = []

def specific_preperations_4(options, args=None):
    pass

scripts_4 = []

from time import sleep

def specific_preperations_5(options, args=None):
    pass

#wtf

scripts_5 = ["enter3", "close_sample_reader"]

def prepare_sample(sample_path):
    return sample_path
