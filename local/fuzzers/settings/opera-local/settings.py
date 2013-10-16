visible = True
testing = False

ips = { 
'xpsp2-1': '192.168.56.110',
'fuzzbox-acad-2': '192.168.56.111',
'testbox-second' : '192.168.56.112',
'testbox-new' : '192.168.56.113',
'xpsp2-fuzz-assist' : '192.168.56.131',
'xpsp2-fuzzbox1' : '192.168.56.132',
'win7-kAnalysis-1' : '192.168.56.133',
'win7-kTestFuzzbox1' : '192.168.56.134'
}

second = "testbox-second"

samples_shared_path = "../samples/shared"
samples_saved = "../samples/saved"
samples_binned = "../samples/binned"
fuzzbox_port = 12345
app_path = "C:\\Program Files\\Opera\\16.0.1196.80\\Opera.exe"
corrector = None
buffer_size = 4096
log_name = "HS:Opera"
fuzzbox_timeout = 10.0
start_sleep = 3
restart_count = 1000

