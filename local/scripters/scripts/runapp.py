from script import Script
from scripts.start import script as start

script = Script()

script.prev_list.append(start)
script.steps.append(["up", "up", "up", "ret"])
script.schedule.append((0, 0.1, 5))
