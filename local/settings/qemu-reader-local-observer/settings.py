import generators.changer as changer

EXCEPTION_SINGLE_STEP = 0x80000004
EXCEPTION_BREAKPOINT = 0x80000003
DBG_CONTINUE = 0x00010002
HIT_COUNT = 1
PASS_COUNT = 0

visible = True
testing = False
breaking = False
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
qemu_shared_folder = "/home/ish/projects/2012-08-02-korrino/hs2-current-deploy/qemu-reader-local-observer"
qemu_additional =  ['-enable-kvm']
qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options):
#    from shutil import copyfile
#    copyfile(options.shared_folder + "/server/index2.html", options.samples_shared + "/index.html")
    pass

scripts_1 = ["beep2"]
log_level = 1

def observer_bp_handler(dbg):
    ea = dbg.exception_address
    dbg.bp_del(dbg.exception_address)
    thread = dbg.dbg.dwThreadId
    thread_handle  = dbg.open_thread(thread)
    dbg.write_log("[%x] %s 0x%x" % (thread, dbg.addr_to_module_name(ea), ea)) 
    dbg.single_step(True, thread_handle)
    dbg.close_handle(thread_handle)
    return DBG_CONTINUE

def observer_instr_handler(dbg):
    ea = dbg.exception_address
    thread = dbg.dbg.dwThreadId
    dbg.write_log("[%x] %s 0x%x" % (thread, dbg.addr_to_module_name(ea), ea)) 
    dbg.safe_disasm(ea)
    thread_handle  = dbg.open_thread(thread)
#    if(dbg.check_blacklists(ea)): # its allowed to ret from banned
    if(dbg.mnemonic == "call"):
        target_ea = dbg.decode_op1(dbg.op1)
        dbg.write_log("got call to: 0x%x" % ea)
        if(dbg.check_blacklists(ea)):
            dbg.write_log(", skipping, will resume trace at: 0x%x" % (ea + dbg.get_instruction(ea).length))
            dbg.bp_set(ea + dbg.get_instruction(ea).length, handler = observer_bp_handler)
            dbg.single_step(False, thread_handle)
            return DBG_CONTINUE
    dbg.single_step(True, thread_handle)
    dbg.close_handle(thread_handle)
    return DBG_CONTINUE

def observer_st_marker(dbg):
    if(dbg.check_counters(dbg.exception_address)):
        dbg.preparation_lock.acquire()
        dbg.dlog("reached observer ST marker")
        ea = dbg.exception_address
        thread = dbg.dbg.dwThreadId
        dbg.write_log("[%x] %s 0x%x\n" % (thread, dbg.addr_to_module_name(ea), ea)) 
        dbg.write_log("--- reached ST marker ---")

        dbg.write_log("Modules map:\n") 
        for mod in dbg.enumerate_modules_w_size():
            dbg.write_log("%s 0x%x 0x%x" % (mod[0], mod[1], mod[2]))

        dbg.write_log("Threads:\n") 
        for thread in dbg.enumerate_threads():
            dbg.write_log("Tracking [%x] " % thread)
            dbg.tracked_threads.append(thread)

        dbg.preparation_lock.release()
        dbg.signal_st()
        dbg.ok()
    return DBG_CONTINUE

def observer_end_marker(dbg):
    if(dbg.check_counters(dbg.exception_address)):
        dbg.preparation_lock.acquire()
        dbg.dlog("reached observer END marker")
        ea = dbg.exception_address
        thread = dbg.dbg.dwThreadId
        dbg.write_log("[%x] %s 0x%x\n" % (thread, dbg.addr_to_module_name(ea), ea)) 
        dbg.write_log("--- reached END marker ---")
        dbg.signal_ma()
        dbg.ok()
    return DBG_CONTINUE

st_marker_handler = observer_st_marker
end_marker_handler = observer_end_marker
bp_handler = observer_bp_handler
ss_handler = observer_instr_handler

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
