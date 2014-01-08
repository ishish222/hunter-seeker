from script import Script
from scripts.run import script as run

script = Script()

script.prev_list.append(run)
cmds = list()

cmds += "cmd"
cmds += ["ret", "_sleep 5"]

script.steps.append(cmds)
script.schedule.append((0, 0.1, 1))
