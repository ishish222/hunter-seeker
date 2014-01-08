from script import Script

script = Script()

cmds = list()

cmds += ["_comment Setting MAC"]
cmds += "netsh interface set interface name=\"local area connection 3\" disable"
cmds += ["ret"]
cmds += "reg add hkey_local_machine\system\currentcontrolset\control\class\{4d36e972-e325-11ce-bfc1-08002be10318}\\0001 /v networkaddress /d "
cmds += ["_arg0"]
cmds += " /f"
cmds += ["ret"]
cmds += "netsh interface set interface name=\"local area connection 3\" enable"
cmds += ["ret"]

script.steps.append(cmds)
script.schedule += [(0, 0.01, 1)]


