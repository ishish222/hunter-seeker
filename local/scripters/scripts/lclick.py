from script import Script

script = Script()

script.steps.append(["_mousebt 1", "_mousebt 0"])
script.schedule.append((0, 0.1, 1))
