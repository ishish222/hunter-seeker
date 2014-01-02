from script import Script

script = Script()

cmds = list()

cmds += ["_comment Spawning fuzzing server"]
cmds += "d:\\server\\binner-generic.py"
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


