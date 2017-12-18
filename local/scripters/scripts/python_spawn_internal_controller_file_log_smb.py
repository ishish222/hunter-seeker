from script import Script

script = Script()

cmds = list()

cmds += ["_comment Spawning internal controller with args"]
cmds += "c:\\Python27\\Python.exe -u \\\\10.0.2.4\\qemu\\server\\internal.py > \\\\10.0.2.4\\qemu\\logs\\internal.log"
cmds += ["_arg0"]
cmds += " "
cmds += ["_arg1"]
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


