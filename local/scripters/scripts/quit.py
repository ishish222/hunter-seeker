from script import Script

script = Script()

cmds = list()

cmds += ["_comment Exiting"]
cmds += ["_quit"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


