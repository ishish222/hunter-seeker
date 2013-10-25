from script import Script

script = Script()

cmds = list()

cmds += ["_delete qsave", "_sleep 120"]
cmds += ["_comment qsave state deleted"]
cmds += ["_save ready", "_sleep 120"]
cmds += ["_comment ready state saved"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


