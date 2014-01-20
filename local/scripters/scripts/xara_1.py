from script import Script

script = Script()

cmds = list()

cmds += ["tab", "tab", "tab", "tab", "ret"]
cmds += ["_sleep 1"]
cmds += ["tab", "tab", "tab", "tab", "tab", "ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


