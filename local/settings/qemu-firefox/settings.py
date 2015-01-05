from default import *
import generators.changer as changer
from subprocess import Popen

"""
Machine preparation:
disable restore session
export env:
NSPR_LOG_MODULES = nsSecureBrowserUI:5

"""

#breaking = True
#debug = True

def exception(dbg, ec):
    dbg.dlog("Got exception")
    dbg.signal_ex("%08X" % ec)
    return DBG_EXCEPTION_NOT_HANDLED


ma_st_rvas = [("xul.dll", 0x8dbe8f, 0)]
#ma_end_rvas = [("xul.dll", 0x8dbeaa, 0)]
ma_end_rvas = [("xul.dll", 0x8dbeaa, 0)]
#ma_rd_rvas = [("xul.dll", 0x8dbeaa, 0)]
#need to find other rvas?

app_path = "C:\\Program Files\\Mozilla Firefox\\firefox.exe"
app_module = "firefox.exe"
buffer_size = 4096
log_name = "HS:Firefox"
fuzzbox_timeout = wait_sleep*4
closing_plugin_name = "close_sample_opera"
start_sleep = 10
boot_wait = 7
ex_handler = exception
mutator = "changer.Changer"
generator = "generator.DirGenerator"

def runner(args):
    Popen("powershell -command \"& { invoke-expression \"%s\" }\"" % args)


