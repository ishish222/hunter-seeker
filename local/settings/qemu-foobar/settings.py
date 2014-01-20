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

#ma_st_rvas = [("foobar2000.exe", 0x3c618, 1)] # check again, but seems it has to pass one
#ma_st_rvas = [("foobar2000.exe", 0x7f8d1, 0)]
ma_st_rvas = [("foobar2000.exe", 0xa5d3e, 0)]
ma_end_rvas = [("foobar2000.exe", 0x7b9fb, 0)]
#ma_end_rvas = [
#("foo_input_std.dll", 0x3e5df, 0), # mp3
#("foo_input_std.dll", 0x1c6e3, 0), # ogg, opus
#("foo_input_std.dll", 0x22107, 0), # flac
#("foo_input_std.dll", 0x5731b, 0), # aac
#("foo_input_std.dll", 0x78731, 0), # wma
#("foo_input_std.dll", 0x3834a, 0)  # wav
#]
ma_react_rvas = [
("foobar2000.exe", 0x7ceca, (0, react1, []))
#("foobar2000.exe", 0x58b5b, (0, react2, []))
]
ma_rd_rvas = [("foobar2000.exe", 0x7f1bd, 0)]

app_path = "C:\\Program Files\\foobar2000\\foobar2000.exe"
app_module = "foobar2000.exe"
log_name = "HS:Foobar2000"
wait_sleep = 5
fuzzbox_timeout = wait_sleep*4
closing_plugin_name = "nop"
mutator = "changer.Changer"
boot_wait = 60
shutdown_wait = 60

def runner(args):
    Popen("powershell -command \"& { invoke-expression %s }\"" % args)

def specific_preperations_1(options, args=None):
    from sys import path
    path.append("../client")
    from script import rs
    rs("lclick", options.m, args=args)
    rs("python_server_spawn_args", options.m, args=args)

scripts_3 = ["foobar_mouse_2", "lclick"]
scripts_5 = ["nop"]

