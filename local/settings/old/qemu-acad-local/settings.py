from default import *
import generators.changer as changer
from subprocess import Popen

def react1(dbg):
    print("ES")
    dbg.reqScript("ES")
    return DBG_CONTINUE

def react2(dbg):
    print("FO")
    dbg.reqScript("FO")
    return DBG_CONTINUE

script_codes["ES"] = "foobar3"
script_codes["FO"] = "foobar2"

ma_st_rvas = [("foobar2000.exe", 0xa5d3e, 0)]
ma_end_rvas = [("foobar2000.exe", 0x7b9fb, 0)]
ma_react_rvas = [
("foobar2000.exe", 0x7ceca, (0, react1, []))
]
ma_rd_rvas = [("foobar2000.exe", 0x7f1bd, 0)]

app_path = "C:\\Program Files\\Autodesk\\AutoCAD 2014\\acad.exe"
app_module = "acad.exe"
buffer_size = 4096
log_name = "HS:ACAD"
fuzzbox_timeout = wait_sleep*4
closing_plugin_name = "nop"
mutator = "changer.Changer"

def specific_preperations_1(options, args=None):
    from sys import path
    path.append("../client")
    from script import rs
    rs("lclick", options.m, args=args)
    rs("python_server_spawn_args", options.m, args=args)

def runner(args):
    Popen("powershell -command \"& { invoke-expression %s }\"" % args)

scripts_3 = ["foobar_mouse_2", "lclick"]

scripts_5 = ["nop"]

