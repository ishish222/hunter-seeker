from script import Script
from scripts.run import script as run

# ETA: 15 min
# Prerequisites:
#  open_install_dir

script = Script()
script.eta_str = "15 min"

cmds = list()

#01_WindowsInstaller-KB893803-v2-x86.exe
cmds += ["_comment 01_WindowsInstaller-KB893803-v2-x86.exe"]
cmds += ["down", "up"]
cmds += ["ret", "_sleep 20"]
#confirm & wait for unpacking
cmds += ["left", "ret", "_sleep 30"]
#1st screen
cmds += ["ret"]
#agree
cmds += ["tab", "right", "tab", "tab", "tab", "ret"]
#wait until install finishes
cmds += ["_sleep 60"]
#do not restart
cmds += ["spc", "ret"]

#02_wic_x86_enu.exe
cmds += ["_comment 02_wic_x86_enu.exe"]
#reset pos
cmds += ["up", "up", "up", "up", "up", "up", "up", "up", "up"]
cmds += ["down"]
cmds += ["ret", "_sleep 20"]
#confirm & wait for unpacking
cmds += ["left", "ret", "_sleep 30"]
#1st screen
cmds += ["ret"]
#agree
cmds += ["tab", "right", "tab", "tab", "tab", "ret"]
#wait until install finishes
cmds += ["_sleep 120"]
#do not restart
cmds += ["ret"]

#03_dotnetFx40_Full_x86_x64.exe
cmds += ["_comment 03_dotnetFx40_Full_x86_x64.exe"]
cmds += ["up", "up", "up", "up", "up", "up", "up", "up", "up"]
cmds += ["down", "down"]
cmds += ["ret", "_sleep 20"]
#confirm & wait for unpacking (this one takes long)
cmds += ["left", "ret", "_sleep 180"]
#agree
cmds += ["tab", "tab", "tab", "spc", "ret"]
#wait looong
cmds += ["_sleep 600"]
#finished
cmds += ["tab", "ret"]

cmds += ["_screendump dotnet_installed.jpg"]

#close install window

#Done
cmds += ["_comment All done"]

script.steps.append(cmds)
script.schedule.append((0, 0.1, 1))
