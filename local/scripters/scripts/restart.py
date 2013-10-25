from script import Script

script = Script()

script.steps.append(["alt-f4", "tab", "tab", "spc", "_sleep 600"])
script.schedule.append((0, 0.1, 1))
