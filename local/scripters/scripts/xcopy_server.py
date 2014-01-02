from script import Script

script = Script()

cmds = list()

cmds += ["_comment Copying fuzzing server"]
cmds += "xcopy d:\\*.* c:\\*.* /s /e /c"
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


