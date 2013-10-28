from script import Script

script = Script()

cmds = list()

cmds += ["_comment Mapping qemu share to Z:"]

cmds += ["ctrl-esc", "up", "up", "up", "ret"]
cmds += "\\\\10.0.2.4"
cmds += ["ret", "_sleep 10"]

cmds += ["down", "menu", "down", "down", "down", "down", "ret", "_sleep 1"]
cmds += ["ret", "_sleep 5"]
cmds += ["alt-f4", "alt-f4"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


