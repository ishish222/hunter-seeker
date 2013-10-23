from script import Script
from scripts.run import script as run

script = Script()

script.prev_list.append(run)
cmds = list()

cmds += "\\\\10.0.2.4"
cmds += ["ret", "_sleep 2"]

#choose proper folder
cmds += ["down", "ret", "down", "up", "ret"]

#how many dows?
cmds += ["down", "up"]
cmds += ["ret", "_sleep 20"]

#confirm
cmds += ["left", "ret"]


script.steps.append(cmds)
script.schedule.append((0, 0.1, 1))
