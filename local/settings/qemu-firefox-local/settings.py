from default import *
import generators.changer as changer
from subprocess import Popen

def exception(dbg, ec):
    dbg.dlog("Got exception")
    dbg.signal_ex("%08X" % ec)
    return DBG_EXCEPTION_NOT_HANDLED


ma_st_rvas = [("xul.dll", 0x56d20, 0)]
ma_end_rvas = [("xul.dll", 0x5fdb0, 0)]
ma_rvas = [("xul.dll", 0x5fdb0, 0)]
#need to find other rvas?
# what is ws_rvas?

app_path = "C:\\Program Files\\Mozilla Firefox\\firefox.exe"
app_module = "firefox.exe"
buffer_size = 4096
log_name = "HS:Firefox"
fuzzbox_timeout = wait_sleep*4
closing_plugin_name = "close_sample_opera"
start_sleep = 20
ex_handler = exception
mutator = "changer.Changer"
generator = "generator.DirGenerator"

def prepare_sample(sample_path):
    file_path = sample_path + ".html"
    sample_file = path.basename(sample_path)
    f = open(file_path, "w")
    text = """
<html>
<script>
function end()
{{
document.location="http://{0}.pl";
}}
</script>
<video autoplay src="{0}" onended="end()"></video>
</html>
""".format(sample_file)
    f.write(text)
    f.close
    return file_path

