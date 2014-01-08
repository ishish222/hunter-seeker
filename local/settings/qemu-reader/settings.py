import generators.changer as changer

DBG_CONTINUE = 0x00010002
HIT_COUNT = 1
PASS_COUNT = 0

visible = False
testing = False
breaking = False
debug = True

machines = {
    'hs2-01': {'disk' : 'hs2-01.qcow2', 'tap' : 'tap1', 'ip' : '192.168.1.101', 'mac' : '00:00:00:00:00:01', 'server_ip' : '192.168.1.1', 'server_port' : 12301, 'vnc' : ':1'},
    'hs2-02': {'disk' : 'hs2-02.qcow2', 'tap' : 'tap2', 'ip' : '192.168.2.102', 'mac' : '00:00:00:00:00:02', 'server_ip' : '192.168.2.1', 'server_port' : 12302, 'vnc' : ':2'},
    'hs2-03': {'disk' : 'hs2-03.qcow2', 'tap' : 'tap3', 'ip' : '192.168.3.103', 'mac' : '00:00:00:00:00:03', 'server_ip' : '192.168.3.1', 'server_port' : 12303, 'vnc' : ':3'},
    'hs2-04': {'disk' : 'hs2-04.qcow2', 'tap' : 'tap4', 'ip' : '192.168.4.104', 'mac' : '00:00:00:00:00:04', 'server_ip' : '192.168.4.1', 'server_port' : 12304, 'vnc' : ':4'},
    'hs2-05': {'disk' : 'hs2-05.qcow2', 'tap' : 'tap5', 'ip' : '192.168.5.105', 'mac' : '00:00:00:00:00:05', 'server_ip' : '192.168.5.1', 'server_port' : 12305, 'vnc' : ':5'},
    'hs2-06': {'disk' : 'hs2-06.qcow2', 'tap' : 'tap6', 'ip' : '192.168.6.106', 'mac' : '00:00:00:00:00:06', 'server_ip' : '192.168.6.1', 'server_port' : 12306, 'vnc' : ':6'},
    'hs2-07': {'disk' : 'hs2-07.qcow2', 'tap' : 'tap7', 'ip' : '192.168.1.107', 'mac' : '00:00:00:00:00:07', 'server_ip' : '192.168.1.1', 'server_port' : 12307, 'vnc' : ':7'},
    'hs2-08': {'disk' : 'hs2-08.qcow2', 'tap' : 'tap8', 'ip' : '192.168.1.108', 'mac' : '00:00:00:00:00:08', 'server_ip' : '192.168.1.1', 'server_port' : 12308, 'vnc' : ':8'},
    'hs2-09': {'disk' : 'hs2-09.qcow2', 'tap' : 'tap9', 'ip' : '192.168.1.109', 'mac' : '00:00:00:00:00:09', 'server_ip' : '192.168.1.1', 'server_port' : 12309, 'vnc' : ':9'},
    'hs2-10': {'disk' : 'hs2-10.qcow2', 'tap' : 'tap10', 'ip' : '192.168.1.110', 'mac' : '00:00:00:00:00:0a', 'server_ip' : '192.168.1.1', 'server_port' : 12310, 'vnc' : ':10'},
    'hs2-11': {'disk' : 'hs2-11.qcow2', 'tap' : 'tap11', 'ip' : '192.168.1.111', 'mac' : '00:00:00:00:00:0b', 'server_ip' : '192.168.1.1', 'server_port' : 12311, 'vnc' : ':11'},
    'hs2-12': {'disk' : 'hs2-12.qcow2', 'tap' : 'tap12', 'ip' : '192.168.1.112', 'mac' : '00:00:00:00:00:0c', 'server_ip' : '192.168.1.1', 'server_port' : 12312, 'vnc' : ':12'},
    'hs2-13': {'disk' : 'hs2-13.qcow2', 'tap' : 'tap13', 'ip' : '192.168.1.113', 'mac' : '00:00:00:00:00:0d', 'server_ip' : '192.168.1.1', 'server_port' : 12313, 'vnc' : ':13'},
    'hs2-14': {'disk' : 'hs2-14.qcow2', 'tap' : 'tap14', 'ip' : '192.168.1.114', 'mac' : '00:00:00:00:00:0e', 'server_ip' : '192.168.1.1', 'server_port' : 12314, 'vnc' : ':14'},
}

def react1(dbg):
    dbg.dlog("SR marker reached")
    dbg.reqScript("enter3")
    return DBG_CONTINUE

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
to_mult_factor = 30

qemu_machines = "/home/hs1/machines/qemu"
qemu_m = "3G"
qemu_shared_folder = "/home/hs1/hs2-current-deploy/qemu-reader"
qemu_additional =  ['-enable-kvm', '-cpu', 'host', '-smp', '2']
qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options, args=None):
    from sys import path
    path.append("../client")
    from script import rs
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
