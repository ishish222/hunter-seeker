from script import Script

script = Script()

script.steps.append(["alt-tab", "esc"])
script.schedule.append((0, 0.1, 0.1))
