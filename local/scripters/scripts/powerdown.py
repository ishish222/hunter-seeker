from script import Script

script = Script()

cmds = list()

cmds += ["_comment Shutting down"]
cmds += ["_powerdown"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


