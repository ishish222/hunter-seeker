from script import Script

script = Script()

cmds = list()

cmds += "e:\\samples\\shared\\"
cmds += ["_arg0"]
cmds += ["tab", "tab", "down", "tab"]
cmds += "test2"
cmds += ["tab", "ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


