from script import Script

script = Script()

cmds = list()

cmds += ["_delete qsave", "_sleep 60"]
cmds += ["_comment qsave state deleted"]
cmds += ["_save qsave", "_sleep 60"]
cmds += ["_comment New qsave state saved"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


