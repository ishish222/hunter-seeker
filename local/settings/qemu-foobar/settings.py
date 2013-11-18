import generators.changer as changer

visible = False
testing = False
breaking = False

machines = {
    'hs2-13': {'disk' : 'hs2-13.qcow2', 'ip' : '127.0.0.1', 'port' : 12313, 'vnc' : ':13'},
    'hs2-14': {'disk' : 'hs2-14.qcow2', 'ip' : '127.0.0.1', 'port' : 12314, 'vnc' : ':14'},
    'hs2-15': {'disk' : 'hs2-15.qcow2', 'ip' : '127.0.0.1', 'port' : 12315, 'vnc' : ':15'},
    'hs2-16': {'disk' : 'hs2-16.qcow2', 'ip' : '127.0.0.1', 'port' : 12316, 'vnc' : ':16'},
    'hs2-17': {'disk' : 'hs2-17.qcow2', 'ip' : '127.0.0.1', 'port' : 12317, 'vnc' : ':17'},
    'hs2-18': {'disk' : 'hs2-18.qcow2', 'ip' : '127.0.0.1', 'port' : 12318, 'vnc' : ':18'}
}

ma_addrs = []
ma_rvas = [("foobar2000.exe", 0x7b9fb, 0)]

samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
app_path = "C:\\Program Files\\foobar2000\\foobar2000.exe"
app_module = "foobar2000.exe"
corrector = None
buffer_size = 4096
log_name = "HS:Foobar2000"
wait_sleep = 15
fuzzbox_timeout = wait_sleep*4
start_sleep = 3
revert_sleep = 40
settle_sleep = 3
restart_count = 100000
closing_plugin_name = "nop"
revert_script = "load_ready"
slowdown = 2
extension = "ogv"
mutations = 3
mutator = changer.Changer

qemu_machines = "/home/hs1/machines/qemu"
qemu_m = "4G"
qemu_shared_folder = "/home/hs1/hs2-current-deploy/qemu-foobar"
qemu_additional =  ['-enable-kvm']
qemu_additional += ['-monitor', 'stdio']

def specific_preperations_1(options):
#    from shutil import copyfile
#    copyfile(options.shared_folder + "/server/index2.html", options.samples_shared + "/index.html")
    pass

scripts_1 = ["foobar_mouse_1"]

def specific_preperations_2(options):
    pass

scripts_2 = []

def specific_preperations_3(options):
    pass

scripts_3 = []

def specific_preperations_4(options):
    pass

scripts_4 = ["lclick"]

#from os import path

def prepare_sample(sample_path):
#    file_path = sample_path + ".html"
#    sample_file = path.basename(sample_path)
#    f = open(file_path, "w")
#    text = """
#<html>
#<script>
#function end()
#{{
#document.location="http://google.pl";
#}}
#</script>
#<video autoplay src="{0}" onended="end()"></video>
#</html>
#""".format(sample_file)
#    f.write(text)
#    f.close
    return sample_path

