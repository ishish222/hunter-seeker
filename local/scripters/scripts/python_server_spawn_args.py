from script import Script

script = Script()

cmds = list()

cmds += ["_comment Spawning fuzzing server with args"]
cmds += "e:\\server\\binner-generic.py "
cmds += ["_arg0"]
cmds += " "
cmds += ["_arg1"]
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


