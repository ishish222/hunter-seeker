from script import Script

script = Script()

cmds = list()

cmds += ["_sleep 20"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


