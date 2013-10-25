from script import Script

script = Script()

cmds = list()

cmds += ["_load clean", "_sleep 120"]
cmds += ["_comment Clean state loaded"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


