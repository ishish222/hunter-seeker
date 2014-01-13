from script import Script

script = Script()

script.steps.append(["_mousemv -192,-100"])
script.schedule.append((0, 0.1, 1))
