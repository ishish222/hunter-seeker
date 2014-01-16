from script import Script

script = Script()

cmds = list()

cmds += ["left", "ret", "esc"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


