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

def observer_bp_handler(dbg):
    ea = dbg.exception_address
    dbg.bp_del(dbg.exception_address)
    thread = dbg.dbg.dwThreadId
    thread_handle  = dbg.open_thread(thread)
    dbg.log_write("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
    dbg.single_step(True, thread_handle)
    dbg.close_handle(thread_handle)
    return DBG_CONTINUE

def observer_instr_handler(dbg):
    ea = dbg.exception_address
    thread = dbg.dbg.dwThreadId
    dbg.log_write("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
    dbg.safe_disasm(ea)
    thread_handle  = dbg.open_thread(thread)
    blacklisted = False
    if(dbg.check_blacklist(ea)):
        if(dbg.mnemonic == "call"):
            target_ea = decode_op1(dbg, dbg.op1)
            dbg.log_write("got call to: 0x%x" % ea)
            if(not dbg.check_blacklist(ea)):
                log_write(", skipping, will resume trace at: 0x%x" % (ea + dbg.get_instruction(ea).length))
                dbg.bp_set(ea + dbg.get_instruction(ea).length, handler = observer_bp_handler)
                dbg.single_step(False, thread_handle)
            else:
                dbg.single_step(True, thread_handle)
    dbg.close_handle(thread_handle)
    return DBG_CONTINUE

def observer_st_marker(dbg):
    if(check_counters(dbg.exception_address)):
        dbg.preparation_lock.acquire()
        dbg.binner.dlog("reached observer ST marker")
        ea = dbg.exception_address
        thread = dbg.dbg.dwThreadId
        dbg.log_write("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
        dbg.log_write("--- reached ST marker ---")

        dbg.set_callback(EXCEPTION_BREAKPOINT, observer_bp_handler)
        dbg.set_callback(EXCEPTION_SINGLE_STEP, observer_instr_handler)

        for thread_id in dbg.enumerate_threads():
            dbg.dlog("Tracking [%x]: " % thread_id)
            thread_handle  = dbg.open_thread(thread_id)
            if(thread_handle == dbg.h_thread):
                continue
            dbg.single_step(True, thread_handle)
            dbg.close_handle(thread_handle)
        dbg.attach_end_markers(dbg)
        dbg.detach_st_markers(dbg)
        dbg.preparation_lock.release()
    return DBG_CONTINUE

def observer_end_marker(dbg):
    if(check_counters(dbg.exception_address)):
        dbg.preparation_lock.acquire()
        dbg.dlog("reached observer END marker")
        ea = dbg.exception_address
        thread = dbg.dbg.dwThreadId
        dbg.log_write("[%x] %s 0x%x\n" % (thread, get_module(dbg, ea), ea)) 
        dbg.log_write("--- reached END marker ---")
        for thread_id in dbg.enumerate_threads():
            dbg.dlog("Stop tracking [%x]: " % thread_id)
            thread_handle  = dbg.open_thread(thread_id)
            if(thread_handle == dbg.h_thread):
                continue
            dbg.single_step(False, thread_handle)
            dbg.close_handle(thread_handle)
        dbg.signal_ma()
        dbg.ok()
    return DBG_CONTINUE

st_marker_handler = observer_st_marker
end_marker_handler = observer_end_marker

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
