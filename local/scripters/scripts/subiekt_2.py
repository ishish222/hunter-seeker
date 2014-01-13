from script import Script

script = Script()

script.steps.append(["shift-alt-tab", "down", "down", "ret"])
script.schedule.append((0, 0.1, 0.1))
