visible = False
testing = False

machines = {
    'hs2-01': {'disk' : 'hs2-01.qcow2', 'ip' : '127.0.0.1', 'port' : 12301, 'vnc' : ':1'},
    'hs2-02': {'disk' : 'hs2-02.qcow2', 'ip' : '127.0.0.1', 'port' : 12302, 'vnc' : ':2'},
    'hs2-03': {'disk' : 'hs2-03.qcow2', 'ip' : '127.0.0.1', 'port' : 12303, 'vnc' : ':3'},
    'hs2-04': {'disk' : 'hs2-04.qcow2', 'ip' : '127.0.0.1', 'port' : 12304, 'vnc' : ':4'},
    'hs2-05': {'disk' : 'hs2-05.qcow2', 'ip' : '127.0.0.1', 'port' : 12305, 'vnc' : ':5'},
    'hs2-06': {'disk' : 'hs2-06.qcow2', 'ip' : '127.0.0.1', 'port' : 12306, 'vnc' : ':6'},
    'hs2-07': {'disk' : 'hs2-07.qcow2', 'ip' : '127.0.0.1', 'port' : 12307, 'vnc' : ':7'},
    'hs2-08': {'disk' : 'hs2-08.qcow2', 'ip' : '127.0.0.1', 'port' : 12308, 'vnc' : ':8'},
    'hs2-09': {'disk' : 'hs2-09.qcow2', 'ip' : '127.0.0.1', 'port' : 12309, 'vnc' : ':9'},
    'hs2-10': {'disk' : 'hs2-10.qcow2', 'ip' : '127.0.0.1', 'port' : 12310, 'vnc' : ':10'},
    'hs2-11': {'disk' : 'hs2-11.qcow2', 'ip' : '127.0.0.1', 'port' : 12311, 'vnc' : ':11'},
    'hs2-12': {'disk' : 'hs2-12.qcow2', 'ip' : '127.0.0.1', 'port' : 12312, 'vnc' : ':12'}
}

bad_addrs = [0x13518F0]
samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
app_path = "C:\\Program Files\\Opera\\16.0.1196.80\\Opera.exe"
app_module = "opera.exe"
corrector = None
buffer_size = 4096
log_name = "HS:Opera"
fuzzbox_timeout = 10.0
start_sleep = 3
revert_sleep = 40
restart_count = 1000
closing_plugin_name = "close_sample_opera"
revert_script = "load_ready_6"
slowdown = 2

qemu_machines = "/home/hs1/machines/qemu"
qemu_m = "4G"
qemu_shared_folder = "/home/hs1/hs2-current-deploy/qemu-opera"
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
document.location="http://google.pl";
}}
</script>
<video autoplay src="{0}" onended="end()"></video>
</html>
""".format(sample_file)
    f.write(text)
    f.close
    return file_path

