from default import *
import generators.changer as changer
from subprocess import Popen

needs_ready = True

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

script_codes["E3"] = "enter3"

ma_st_rvas = [("AcroRd32.exe", 0x4cae0, 0)]
ma_end_rvas = [("user32.dll", 0x18fe9, 7)]
ma_react_rvas = [("AcroRd32.dll", 0x3664da, (0, react1, ["space"]))]
ma_rd_rvas = [("user32.dll", 0x18fe9, 2)]

app_path = "C:\\Program Files\\Adobe\\Reader 11.0\\Reader\\AcroRd32.exe"
app_module = "AcroRd32.exe"
log_name = "HS:Reader"
wait_sleep = 15
fuzzbox_timeout = wait_sleep*4
mutator = changer.Changer
to_mult_factor = 100
boot_wait = 60
shutdown_wait = 60

def runner(args):
    Popen("powershell -command \"& { invoke-expression %s }\"" % args)

scripts_1 = ["lclick", "python_server_spawn_e"]

scripts_5 = ["enter3", "close_sample_reader"]

