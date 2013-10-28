from script import Script

script = Script()

cmds = list()

cmds += ["_sleep 20"]
cmds += ["ret", "ret", "ret"]
cmds += ["_comment PowerShell configuration 1"]
cmds += "set-executionpolicy unrestricted"
cmds += ["ret"]
cmds += "\\\\10.0.2.4\\qemu\\config_1.ps1"
cmds += ["ret"]
cmds += ["r"]
cmds += ["ret"]

#cmds += "cd ..\\.."
#cmds += ["ret"]
#cmds += "cp python26\\lib\\site-packages\\pywin32_system32\\* python26\\lib\\site-packages\\win32\\"
#cmds += ["ret"]
#cmds += "set-executionpolicy unrestricted"
#cmds += ["ret"]
#cmds += ["_sleep 10"]
#cmds += "reg add hklm\\software\\microsoft\\.netframework /v onlyuselatestclr /t reg_dword /d 1"
#cmds += ["ret"]
#cmds += ["_sleep 10"]
#cmds += "cp \\\\10.0.2.4\\qemu\\powershell.exe.config windows\\system32\\windowspowershell\\v1.0\\"
#cmds += ["ret"]
#cmds += ["_sleep 20"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


