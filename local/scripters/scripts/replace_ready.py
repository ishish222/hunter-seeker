from script import Script

script = Script()

cmds = list()

cmds += ["_delete ready", "_sleep 120"]
cmds += ["_save ready", "_sleep 120"]
cmds += ["_comment ready state replaced"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


