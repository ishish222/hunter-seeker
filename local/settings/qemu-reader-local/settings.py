from default import *
import generators.changer as changer
from subprocess import Popen

needs_ready = True

def react1(dbg):
    dbg.dlog("SR marker reached")
    dbg.reqScript("E3")
    print("SR")
    return DBG_CONTINUE

script_codes["E3"] = "enter3"
script_codes["CL"] = "close_sample_reader"

ma_st_rvas = [("AcroRd32.exe", 0x4cae0, 0)]
ma_end_rvas = [("user32.dll", 0x18fe9, 7)]
ma_react_rvas = [("AcroRd32.dll", 0x3664da, (0, react1, ["space"]))]
ma_rd_rvas = [("user32.dll", 0x18fe9, 2)]

app_path = "C:\\Program Files\\Adobe\\Reader 11.0\\Reader\\AcroRd32.exe"
app_module = "AcroRd32.exe"
log_name = "HS:Reader"

wait_sleep = 5
fuzzbox_timeout = wait_sleep*4
mutator = changer.Changer

extension = "pdf"
save_disks = True

def runner(args=None):
    Popen("powershell -command \"& { invoke-expression %s }\"" % args)

def specific_preperations_1(options, args=None):
    from sys import path
    path.append("../client")
    from script import rs
    rs("lclick", options.m)
    rs("python_server_spawn_args", options.m, args=args)

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

scripts_5 = ["enter3", "close_sample_reader"]

