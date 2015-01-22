from script import Script

script = Script()

script.steps.append(["tab", "tab", "tab", "tab", "ret", "_sleep 2"])
script.steps.append(["tab", "tab", "tab", "tab", "tab", "ret", "_sleep 2"])
script.steps.append(["esc"])
script.schedule.append((0, 0.1, 0.1))
