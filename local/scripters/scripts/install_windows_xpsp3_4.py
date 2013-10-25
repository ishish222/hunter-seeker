from script import Script
from scripts.run import script as run

# Prerequsites:
#  part 1 finished with stage1

script = Script()
script.eta_str = "15 min"

cmds = list()

cmds += ["_comment Configuring Windows XP (4)"]
cmds += ["_load stage3", "_sleep 60"]

#layout (& possibly res)
cmds += ["esc", "alt-tab", "menu", "up", "ret"]

#desktop menu, change layout to classic
cmds += ["down", "ret", "_sleep 2"]

#start menu layout
cmds += ["ctrl-esc", "menu", "down", "ret"]

#include control panel opts in start menu
cmds += ["down", "tab", "ret"]
cmds += ["tab", "tab", "tab", "tab", "tab"]
#administrative tools
cmds += ["spc"]
#control panel
cmds += ["down", "down", "down", "down", "spc"]
#network connections
cmds += ["down", "down", "down", "spc"]
#small icons
cmds += ["down", "down", "down", "spc"]
cmds += ["ret"]
cmds += ["tab", "tab", "tab", "ret", "esc", "_sleep 2"]

#turn off firewall
#start
cmds += ["ctrl-esc"]
#fw in ctrlpanel
cmds += ["up", "up", "up", "up", "up", "up", "right", "right", "up", "up", "ret", "_sleep 2"]
#turn off
cmds += ["down", "tab", "tab", "ret", "_sleep 2"]

#folder options
#cmds += run.steps[0]
cmds += ["ctrl-esc", "up", "up", "up", "ret"]
cmds += "\\\\10.0.2.4"
cmds += ["ret", "_sleep 5"]

#arrange: detailed view
cmds += ["menu", "down", "right", "down", "down", "down", "down", "ret"]

#folder options
cmds += ["alt", "right", "right", "right", "right", "down", "down", "down", "down", "ret", "down"]
#next tab
cmds += ["ctrl-tab", "tab", "tab"]
cmds += ["down", "down", "down", "down", "spc"]
cmds += ["down", "down", "down", "down", "down", "down", "spc"]
cmds += ["down", "spc"]
cmds += ["down", "spc", "tab", "ret"]

#for all folders
cmds += ["ctrl-tab", "ctrl-tab", "spc", "spc"]

#apply and esc
cmds += ["tab", "tab", "tab", "tab", "tab", "tab", "ret", "esc"]
cmds += ["alt-f4"]

#finalize
cmds += ["_screendump stage4.jpg"]
cmds += ["_delete stage3", "_sleep 60"]
cmds += ["_save clean", "_sleep 60"]
cmds += ["_comment Configuring Windows XP (4) finished"]
cmds += ["_quit"]

#Installer start

script.steps.append(cmds)
script.schedule += [(0, 0.2, 1)]


