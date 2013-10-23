from script import Script

script = Script()

script.steps.append(["ctrl-esc"])
script.schedule += [(0, 0.1, 1)]
