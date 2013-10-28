from script import Script
from scripts.run import script as run

# ETA: 15 min
# Prerequisites:
#  open_install_dir

script = Script()
script.eta_str = "15 min"

cmds = list()

#12_NetFx20SP2_x86.exe
cmds += ["_comment 12_NetFx20SP2_x86.exe"]
cmds += ["f5"]
cmds += ["up", "up", "up", "up", "up", "up", "up", "up", "up"]
cmds += ["down", "down", "down", "down", "down", "down", "down", "down", "down", "down", "down"]
cmds += ["ret", "_sleep 20"]
#confirm & wait for unpacking (this one takes long)
cmds += ["left", "ret", "_sleep 120"]
#agree
cmds += ["up", "tab", "tab", "tab", "spc"]
#wait looong
cmds += ["_sleep 600"]
#finished
cmds += ["tab", "ret"]

#Done
cmds += ["_comment All done"]

script.steps.append(cmds)
script.schedule.append((0, 0.1, 1))
