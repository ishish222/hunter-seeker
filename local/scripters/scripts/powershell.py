from script import Script

script = Script()

cmds = list()

cmds += ["_comment Spawning PowerShell"]
cmds += "powershell"
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


