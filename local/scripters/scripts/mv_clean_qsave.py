from script import Script

script = Script()

cmds = list()

cmds += ["_delete clean", "_sleep 120"]
cmds += ["_comment clean state deleted"]
cmds += ["_save qsave", "_sleep 120"]
cmds += ["_comment qsave state saved"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


