import generators.changer as changer

visible = True
testing = False
breaking = False

machines = {
    'hs2-acad': {'disk' : 'hs2-acad.qcow2', 'ip' : '127.0.0.1', 'port' : 12345}
}

ma_addrs = []
ma_rvas = []

samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
app_path = "C:\\Program Files\\Autodesk\\AutoCAD 2014\\acad.exe"
app_module = "acad.exe"
buffer_size = 4096
log_name = "HS:ACAD"
#wait before declaring test timeout
wait_sleep = 15
#wait before declaring socket timeout
fuzzbox_timeout = wait_sleep*4
#wait after app spawn
start_sleep = 15
#wait after machine state revert
revert_sleep = 40
settle_sleep = 3
restart_count = 100000
closing_plugin_name = "close_sample_opera"
revert_script = "load_ready"
slowdown = 1
extension = ".dwg"
mutations = 3
mutator = changer.Changer
corrector = "acadCorrector"

qemu_machines = "/home/ish/machines/qemu"
qemu_m = "4G"
qemu_shared_folder = "/home/ish/projects/2012-08-02-korrino/hs2-current-deploy"
qemu_additional =  ['-enable-kvm']
qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options):
    pass

scripts_1 = ["beep"]

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
    return sample_path
