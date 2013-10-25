from script import Script

script = Script()

script.steps.append(["_sleep 1"])
script.schedule.append((0, 0.1, 1))
