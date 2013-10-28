from script import Script

script = Script()

cmds = list()

#13_WindowsXP-KB926139-v2-x86-ENU.exe
cmds += ["_comment 13_WindowsXP-KB926139-v2-x86-ENU.exe"]

#select 
cmds += ["f5", "up", "up", "up", "up", "up", "up", "up", "up", "up", "up", "up", "up", "up", "up", "up", "up"]
cmds += ["down", "down", "down", "down", "down", "down", "down", "down", "down", "down", "down", "down", "ret", "_sleep 120"]

#confirm, wait for unpack
cmds += ["left", "ret", "_sleep 60"]

#1st screen
cmds += ["ret"]

#agree
cmds += ["tab", "tab", "spc", "tab", "tab", "tab", "ret"]

#wait for installer
cmds += ["_sleep 300"]

#do not restart, just finish
cmds += ["ret"]

cmds += ["_comment All done"]

script.steps.append(cmds)
script.schedule.append((0, 0.1, 1))


