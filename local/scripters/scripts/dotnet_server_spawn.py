from script import Script

script = Script()

cmds = list()

cmds += ["_comment Spawning fuzzing server"]
cmds += "cp \\\\10.0.2.4\\qemu\\server\\runner2.ps1 ."
cmds += ["ret"]
cmds += "\\\\10.0.2.4\\qemu\\server\\server.ps1"
cmds += ["ret"]
cmds += ["r"]
cmds += ["ret"]
cmds += ["_sleep 10"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


