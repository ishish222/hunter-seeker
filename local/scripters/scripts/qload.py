from script import Script

script = Script()

cmds = list()

cmds += ["_load qsave", "_sleep 60"]
cmds += ["_comment qsave state loaded"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


