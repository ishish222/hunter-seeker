from default import *
import generators.changer as changer
from subprocess import Popen

def react1(dbg):
    dbg.reqScript("RE")
    dbg.binner.send("SAMA") # we don't need locks this time
    return DBG_CONTINUE

def react2(dbg):
    print("FO")
    dbg.reqScript("FO")
    return DBG_CONTINUE

debug = True

script_codes["R2"] = "ret2"
script_codes["RE"] = "ret"
script_codes["FO"] = "foobar2"

ma_st_rvas = [("DesignerPro.exe", 0xe30ac, 0)]
ma_end_rvas = [("user32.dll", 0x17f87, 0)]

ma_react_rvas = [
#("DesignerPro.exe", 0xa08ffe, (0, react1, []))
#("DesignerPro.exe", 0xa0d036, (0, react1, []))
("user32.dll", 0x1680b, (0, react1, []))
]
#("foobar2000.exe", 0x58b5b, (0, react2, []))
#]
ma_rd_rvas = [("DesignerPro.exe", 0xa95a26, 0)]

app_path = "C:\\Program Files\\Xara\\Xara Designer Pro X9\\DesignerPro.exe"
app_module = "DesignerPro.exe"
buffer_size = 4096
log_name = "HS:TEMU:XaraDesigner9"
#fuzzbox_timeout = wait_sleep*4
fuzzbox_timeout = 0
closing_plugin_name = "nop"
start_sleep = 30
mutator = "changer.Changer"
qemu_m = "2000"
qemu_command = "/home/hs1/temu-1.0-3/tracecap/temu"
qemu_env = [('LD_LIBRARY_PATH', '/mnt/gentoo32/usr/lib')]
smp = 4
qemu_additional = ['--no-kqemu', '-net', 'none']
qemu_drive_options=""
qemu_secondary="temu-samples-1.img"


def specific_preperations_1(options, args=None):
    from sys import path
    path.append("../client")
    from script import rs
    rs("lclick", options.m, args=args)
    rs("python_server_spawn_args", options.m, args=args)

def runner(args):
    Popen("powershell -command \"& { invoke-expression %s }\"" % args)

scripts_4 = ["lclick", "esc"]

#scripts_5 = ["sleep_05"]

