from script import Script

script = Script()

cmds = list()

cmds += ["_load ready", "_sleep 80"]
cmds += ["_comment Ready state loaded"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]

