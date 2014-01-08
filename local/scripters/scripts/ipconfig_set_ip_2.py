from script import Script

script = Script()

cmds = list()

cmds += ["_comment Setting IP"]
cmds += "netsh interface ip set address name=\"local area connection 3\" static "
cmds += ["_arg0"]
cmds += " 255.255.255.0 "
cmds += ["_arg1"]
cmds += " 1"
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


