from script import Script

script = Script()
script.eta_str = "15 min"

cmds = list()

#Boot from cd!
cmds += ["_sleep 1", "ret", "ret", "ret"]

#Installer start
cmds += ["_comment Installing Windows XP (1)", "_sleep 40"]

#All space
cmds += ["c", "ret", "ret"]

#format filesystem, copying files, sleep length may vary on disk size!!!
#cmds += ["ret", "_sleep 180"]

#quick format
cmds += ["up", "up","ret", "_sleep 80"]

#rebooting, copying more files
cmds += ["_sleep 180"]

cmds += ["_screendump stage1.jpg"]
cmds += ["_save stage1", "_sleep 60"]
cmds += ["_comment Installing Windows XP (1) finished"]

script.steps.append(cmds)
script.schedule += [(0, 0.1, 1)]


