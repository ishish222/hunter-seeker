import generators.changer as changer

visible = True
testing = False
breaking = False

machines = {
    'hs2-1': {'disk' : 'hs2-1.qcow2', 'ip' : '127.0.0.1', 'port' : 12345},
    'hs2-2': {'disk' : 'hs2-2.qcow2', 'ip' : '127.0.0.1', 'port' : 12346}
}

ma_addrs = []
ma_rvas = [("xul.dll", 0x5fdb0, 0)]

samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
app_path = "C:\\Program Files\\Mozilla Firefox\\firefox.exe"
app_module = "firefox.exe"
corrector = None
buffer_size = 4096
log_name = "HS:Firefox"
wait_sleep = 15
fuzzbox_timeout = wait_sleep*4
start_sleep = 3
revert_sleep = 40
settle_sleep = 3
restart_count = 100000
closing_plugin_name = "close_sample_opera"
revert_script = "load_ready"
slowdown = 1
extension = ".ogv"
mutations = 3
mutator = changer.Changer

qemu_machines = "/home/ish/machines/qemu"
qemu_m = "4G"
qemu_shared_folder = "/home/ish/projects/2012-08-02-korrino/hs2-current-deploy"
qemu_additional =  ['-enable-kvm']
qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options):
#    from shutil import copyfile
#    copyfile(options.shared_folder + "/server/index2.html", options.samples_shared + "/index.html")
    pass

scripts_1 = []

def specific_preperations_2(options):
    pass

scripts_2 = []

def specific_preperations_3(options):
    pass

scripts_3 = []

def specific_preperations_4(options):
    pass

scripts_4 = ["alttab"]

from os import path

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
