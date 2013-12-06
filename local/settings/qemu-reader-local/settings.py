import generators.changer as changer

DBG_CONTINUE = 0x00010002
HIT_COUNT = 1
PASS_COUNT = 0

visible = True
testing = False
breaking = True
debug = True

machines = {
    'hs2-1': {'disk' : 'hs2-1.qcow2', 'ip' : '127.0.0.1', 'port' : 12345},
    'hs2-2': {'disk' : 'hs2-2.qcow2', 'ip' : '127.0.0.1', 'port' : 12346}
}

ff = True

def react1(dbg):
    dbg.dlog("SR marker reached")
    dbg.reqScript("enter3")
    dbg.ok()
    return DBG_CONTINUE

ma_addrs = []
ma_rvas = []
ma_st_addrs = []
ma_st_rvas = [("AcroRd32.exe", 0x4cae0, 0)]
ma_end_addrs = []
ma_end_rvas = [("user32.dll", 0x18fe9, 7)]
ma_react_addrs = []
#ma_react_rvas = [("AcroRd32.dll", 0x3664da, (0, react1, ["space"]))]
ma_react_rvas = [("AcroRd32.dll", 0x3664df, (0, react1, ["space"]))]
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
start_sleep = 10
revert_sleep = 40
settle_sleep = 3
restart_count = 100000
closing_plugin_name = "close_sample_opera"
revert_script = "load_ready"
slowdown = 1
extension = "pdf"
mutations = 3
mutator = changer.Changer

qemu_machines = "/home/ish/machines/qemu"
qemu_m = "4G"
qemu_shared_folder = "/home/ish/projects/2012-08-02-korrino/hs2-current-deploy/qemu-reader-local"
qemu_additional =  ['-enable-kvm']
qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options):
#    from shutil import copyfile
#    copyfile(options.shared_folder + "/server/index2.html", options.samples_shared + "/index.html")
    pass

scripts_1 = ["beep2"]
log_level = 1

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
        dbg.ok()
    return DBG_CONTINUE

def end_marker_test(dbg):
    if(check_counters(dbg.exception_address)):
        dbg.dlog("END marker test")
        dbg.ok()
    return DBG_CONTINUE

#st_marker_handler = st_marker_test
#end_marker_handler = end_marker_test

def specific_preperations_2(options):
    pass

scripts_2 = []

def specific_preperations_3(options):
    pass

scripts_3 = []

def specific_preperations_4(options):
    pass

scripts_4 = []

from time import sleep

def specific_preperations_5(options):
    pass

scripts_5 = ["enter3", "close_sample_reader"]

def prepare_sample(sample_path):
    return sample_path
