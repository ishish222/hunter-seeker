from script import Script

# Prerequsites:
#  part 1 finished with stage1

script = Script()
script.eta_str = "15 min"

cmds = list()

cmds += ["_comment Installing Windows XP (2)"]
cmds += ["_load stage1", "_sleep 60"]

#name & organization
cmds += ["a", "tab", "a", "tab", "tab", "ret"]

#computer name, admin passwd
cmds += ["tab", "tab", "tab", "tab", "ret"]

#time
cmds += ["_sleep 5", "ret"]

#installator continues
cmds += ["_sleep 660"]

cmds += ["alt-tab", "ret", "_sleep 5", "tab", "ret"]

cmds += ["_sleep 60"]

cmds += ["_screendump stage2.jpg"]
cmds += ["_delete stage1", "_sleep 120"]
cmds += ["_save stage2", "_sleep 120"]
cmds += ["_comment Installing Windows XP (2) finished"]

script.steps.append(cmds)
script.schedule += [(0, 0.5, 1)]


