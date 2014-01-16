from script import Script

script = Script()

script.steps.append(["alt", "left", "space", "down", "down", "down", "down", "down", "ret"])
script.schedule.append((0, 0.1, 0.1))
