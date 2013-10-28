visible = False
testing = False

ips = { 
'hs2-1':'127.0.0.1',
'hs2-2':'192.168.56.102',
'hs2-3':'192.168.56.103',
'hs2-4':'192.168.56.104',
'hs2-5':'192.168.56.105',
'hs2-6':'192.168.56.106',
'hs2-7':'192.168.56.107',
'hs2-8':'192.168.56.108',
'hs2-9':'192.168.56.109',
'hs2-10':'192.168.56.110',
'hs2-11':'192.168.56.111',
'hs2-12':'192.168.56.112',
'hs2-13':'192.168.56.113',
'hs2-14':'192.168.56.114',
'hs2-15':'192.168.56.115',
'hs2-16':'192.168.56.116',
'hs2-17':'192.168.56.117',
'hs2-18':'192.168.56.118',
'hs2-19':'192.168.56.119',
'hs2-20':'192.168.56.120',
'hs2-21':'192.168.56.121',
'hs2-22':'192.168.56.122',
'hs2-23':'192.168.56.123',
'win7-hs2-01':'192.168.56.141',
'win7-hs2-02':'192.168.56.142',
'win7-hs2-03':'192.168.56.143',
'win7-hs2-04':'192.168.56.144',
'win7-hs2-05':'192.168.56.145',
'win7-hs2-06':'192.168.56.146',
'win7-hs2-07':'192.168.56.147',
'win7-hs2-08':'192.168.56.148',
'win7-hs2-09':'192.168.56.149',
'win7-hs2-10':'192.168.56.150',
'win7-hs2-11':'192.168.56.151',
'win7-hs2-12':'192.168.56.152',
'win7-hs2-13':'192.168.56.153',
'win7-hs2-14':'192.168.56.154',
'win7-hs2-15':'192.168.56.155',
'win7-hs2-16':'192.168.56.156',
'win7-hs2-17':'192.168.56.157',
'win7-hs2-18':'192.168.56.158',
'win7-hs2-19':'192.168.56.159'
}

ports = { 
'hs2-1':12345
}
fuzzbox_port = 12345

second = "hs2-second"

samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
app_path = "C:\\Program Files\\Opera\\16.0.1196.80\\Opera.exe"
corrector = None
buffer_size = 4096
log_name = "HS:Opera"
fuzzbox_timeout = 10.0
start_sleep = 3
revert_sleep = 60
restart_count = 1000

qemu_machines = "/home/ish/machines/qemu"
qemu_m = "4G"
qemu_shared_folder = "/home/ish/projects/2012-08-02-korrino/hs2-current-deploy"
qemu_additional =  ['-enable-kvm']
qemu_additional += ['-net', 'nic,model=rtl8139']
qemu_additional += ['-net', 'user,restrict=n,smb=' + qemu_shared_folder+",hostfwd=tcp:127.0.0.1:12345-:"+str(fuzzbox_port)]
qemu_additional += ['-net', 'nic,model=rtl8139']
#qemu_additional += ['-net', 'tap,ifname=tap1,script=no,downscript=no']
qemu_additional += ['-monitor', 'stdio']

