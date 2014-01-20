from script import Script

script = Script()

script.steps.append(["_sleep 0.1"])
script.steps.append(["ret"])
script.schedule.append((0, 0.1, 0.1))
