from script import Script

script = Script()

cmds = list()

cmds += ["_comment Exiting shell"]
cmds += "exit"
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


