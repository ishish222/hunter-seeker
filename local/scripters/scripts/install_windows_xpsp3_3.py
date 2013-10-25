from script import Script

# Prerequsites:
#  part 1 finished with stage1

script = Script()
script.eta_str = "15 min"

cmds = list()

cmds += ["_comment Installing Windows XP (3)"]
cmds += ["_load stage2", "_sleep 120"]

#loading system
cmds += ["tab", "tab", "spc"]

#loading system
cmds += ["tab", "tab", "down", "tab", "tab", "tab", "tab", "tab",  "spc", "_sleep 30"]

#checking connectivity
cmds += ["tab", "tab", "tab", "tab", "spc"]

#y u no register?
cmds += ["down", "tab", "tab", "tab", "tab", "tab", "spc"]

#users
cmds += ["a", "tab", "tab", "tab", "tab", "tab", "tab", "tab", "spc", "_sleep 1"]

cmds += ["tab", "tab", "spc", "_sleep 60"]

cmds += ["_screendump stage3.jpg"]
cmds += ["_delete stage2", "_sleep 120"]
cmds += ["_save stage3", "_sleep 120"]

cmds += ["_comment Installing Windows XP (3) finished"]

script.steps.append(cmds)
script.schedule += [(0, 1, 1)]


