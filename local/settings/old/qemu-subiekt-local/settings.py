from default import *
import generators.changer as changer
from sys import path
from time import sleep

try:
    from script import rs, rss
except Exception:
    pass

use_snapshots = True

def react1(dbg):
    if(dbg.check_counters(dbg.exception_address)):
        dbg.reqScript("SB")
        dbg.signal_ma()
    return DBG_CONTINUE

def react2(dbg):
    dbg.reqScript("FO")
    return DBG_CONTINUE

script_codes["SB"] = "subiekt_4"
script_codes["FO"] = "foobar2"

ma_st_rvas = [("Archiwizacja.dll", 0x28867, 0)]
ma_end_rvas = [("Kameleon.dll", 0x63e14, 0)]
ma_react_rvas = [("Archiwizacja.dll", 0x2a4e8, (0, react1, []))]
ma_rd_rvas = [("Kameleon.dll", 0x63e1b, 0)]

app_path = "C:\\Program Files\\InsERT\\InsERT GT\\Archiw.exe"
app_module = "Archiw.exe"
log_name = "HS:Subiekt"
fuzzbox_timeout = wait_sleep*4
closing_plugin_name = "nop"
revert_scripts = ["load_ready"]
mutator = "changer.Changer"

def runner_0(options, args=None):
    rs("lclick", options.m)
    rs("subiekt_2", options.m, args=args)
    rs("subiekt_3", options.m, args=args)

def specific_preperations_1(options, args=None):
    rs("lclick", options.m)
    rs("python_server_spawn_args", options.m, args=args)

def specific_preperations_3(options, args=None):
    rss(["maximize", "subiekt_mouse_1"], options.m, options.slowdown)

scripts_3 = ["maximize", "subiekt_mouse_1"]

scripts_5 = ["nop"]

