from script import Script

script = Script()

cmds = list()

#06_WindowsXP-KB936929-SP3-x86-ENU.exe
cmds += ["_comment 06_WindowsXP-KB936929-SP3-x86-ENU.exe"]

#select 
cmds += ["down", "down", "down", "down", "down", "ret", "_sleep 120"]

#confirm, wait for unpack
cmds += ["left", "ret", "_sleep 600"]

#1st screen
cmds += ["ret"]

#agree
cmds += ["tab", "tab", "spc", "tab", "tab", "tab", "ret"]

#wait for installer
cmds += ["_sleep 600"]

#do not restart, just finish
cmds += ["spc", "tab", "ret"]

cmds += ["_comment All done"]

script.steps.append(cmds)
script.schedule.append((0, 0.1, 1))


