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
fuzzbox_port = 12345
app_path = "C:\\Program Files\\AutoCAD 2010\\acad.exe"
corrector = "acadCorrector"
buffer_size = 1024
log_name = "HS:ACAD-2010"
fuzzbox_timeout = 20.0
