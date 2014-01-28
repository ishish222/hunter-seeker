from default import *
import generators.changer as changer
from subprocess import Popen

def react1(dbg):
    dbg.dlog("acquire attempt by react1")
    dbg.dbg_lock_acquire(dbg.send_lock)
    dbg.reqScript("RE")
#    dbg.binner_send("SAMA") # we don't need locks this time
    dbg.dlog("release attempt by react1")
    dbg.dbg_lock_release(dbg.send_lock)
    return DBG_CONTINUE

def exception(dbg, ec):
    dbg.dlog("Got exception")
    dbg.signal_ex("%08X" % ec)
    return DBG_EXCEPTION_NOT_HANDLED

def react2(dbg):
    print("FO")
    dbg.reqScript("FO")
    return DBG_CONTINUE

debug = True
#needs_ready = True

script_codes["R2"] = "ret2"
script_codes["RE"] = "ret"
script_codes["FO"] = "foobar2"

ma_st_rvas = [("DesignerPro.exe", 0xe30ac, 0)] # 0x4e30ac
#ma_st_rvas = [("kernel32.dll", 0x107f0, 0)]
ma_end_rvas = [("user32.dll", 0x17f87, 0)] #0x7e427f87
#ma_end_rvas = [("user32.dll", 0x17f87, 0), ("DesignerPro.exe", 0x7bf9f7, 0)]

ma_react_rvas = [
#("DesignerPro.exe", 0xa08ffe, (0, react1, []))
#("DesignerPro.exe", 0xa0d036, (0, react1, []))
("user32.dll", 0x1680b, (0, react1, [])) #0x7e42680b
]
#("foobar2000.exe", 0x58b5b, (0, react2, []))
#]
ma_rd_rvas = [("DesignerPro.exe", 0xa95a26, 0)]
ma_ws_rvas = [("DesignerPro.exe", 0x7c095f, 0)]
ma_we_rvas = [("DesignerPro.exe", 0x7c0961, 0)]
ma_bl_rvas = [("DesignerPro.exe", 0x4B8304, 0), ("DesignerPro.exe", 0x4B83D9, 0)]

app_path = "C:\\Program Files\\Xara\\Xara Designer Pro X9\\DesignerPro.exe"
app_module = "DesignerPro.exe"
buffer_size = 4096
log_name = "HS:XaraDesigner9"
fuzzbox_timeout = wait_sleep*4
closing_plugin_name = "nop"
start_sleep = 30
mutator = "changer.Changer"

def specific_preperations_1(options, args=None):
    from sys import path
    path.append("../client")
    from script import rs
    rs("lclick", options.m, args=args)
    rs("python_server_spawn_args", options.m, args=args)

def runner(args):
    Popen("powershell -command \"& { invoke-expression %s }\"" % args)

scripts_3 = ["lclick", "esc", "space", "lclick", "close_sample_reader", "close_sample_reader", "close_sample_reader", "close_sample_reader", "sleep_1"]

ex_handler = exception
#scripts_5 = ["sleep_05"]

