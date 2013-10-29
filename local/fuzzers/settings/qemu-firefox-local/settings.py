visible = False
testing = False

machines = {
    'hs2-1': {'disk' : 'hs2-1.qcow2', 'ip' : '127.0.0.1', 'port' : 12345},
    'hs2-2': {'disk' : 'hs2-2.qcow2', 'ip' : '127.0.0.1', 'port' : 12346}
}

bad_addrs = []
samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
app_path = "C:\\Program Files\\Mozilla Firefox\\firefox.exe"
app_module = "firefox.exe"
corrector = None
buffer_size = 4096
log_name = "HS:Firefox"
fuzzbox_timeout = 10.0
start_sleep = 3
revert_sleep = 40
restart_count = 1000
closing_plugin_name = "close_sample_opera"
revert_script = "load_ready"

qemu_machines = "/home/ish/machines/qemu"
qemu_m = "4G"
qemu_shared_folder = "/home/ish/projects/2012-08-02-korrino/hs2-current-deploy"
qemu_additional =  ['-enable-kvm']
qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options):
    from shutil import copyfile
    copyfile(options.shared_folder + "/server/index2.html", options.samples_shared + "/index.html")

scripts_1 = []

def specific_preperations_2(options):
    pass

scripts_2 = []

def specific_preperations_3(options):
    pass

scripts_3 = []
