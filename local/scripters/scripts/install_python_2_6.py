from script import Script

script = Script()

cmds = list()

#04_python-2.6.msi
cmds += ["_comment 04_python-2.6.msi"]

#select 
cmds += ["down", "down", "down", "ret", "_sleep 60"]

#confirm
cmds += ["left", "ret", "_sleep 60"]

#1st screen
cmds += ["tab", "ret", "_sleep 2"]

#path screen
cmds += ["ret", "_sleep 2"]

#components
cmds += ["tab", "tab", "ret", "_sleep 2"]

#waiting for install to finish
cmds += ["_sleep 180"]

#finish
cmds += ["ret", "_sleep 5"]

#05_pywin32-218.win32-py2.6.exe
cmds += ["_comment 05_pywin32-218.win32-py2.6.exe"]

#select pywin32
cmds += ["down", "down", "down", "down", "ret", "_sleep 20"]

#confirm
cmds += ["left", "ret", "_sleep 10"]

#1st screean and other too ;)
cmds += ["ret", "_sleep 2", "ret", "_sleep 2", "ret"]

#waiting for install to finish
cmds += ["_sleep 180"]
cmds += ["ret"]

#dlls are not copied, but we're finished

cmds += ["_comment All done"]

script.steps.append(cmds)
script.schedule.append((0, 0.1, 1))


