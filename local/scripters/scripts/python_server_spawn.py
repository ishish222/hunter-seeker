from script import Script

script = Script()

cmds = list()

cmds += ["_comment Spawning fuzzing server"]
cmds += "\\\\10.0.2.4\\qemu\\server\\binner-generic.py"
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


