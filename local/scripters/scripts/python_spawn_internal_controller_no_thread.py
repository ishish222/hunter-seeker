from script import Script

script = Script()

cmds = list()

cmds += ["_comment Spawning internal controller with args"]
cmds += "e:\\server\\internal_no_threads.py "
cmds += ["_arg0"]
cmds += " "
cmds += ["_arg1"]
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


