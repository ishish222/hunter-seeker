from script import Script
from scripts.run import script as run

# ETA: 15 min
# Prerequisites:
#  open_install_dir

script = Script()
script.eta_str = "15 min"

cmds = list()

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
cmds += ["_sleep 900"]
#finished
cmds += ["tab", "ret"]

#Done
cmds += ["_comment All done"]

script.steps.append(cmds)
script.schedule.append((0, 0.1, 1))
