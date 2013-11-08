from script import Script

script = Script()

script.steps.append(["_syscmd mplayer /home/ish/themes/pika/piwav3.wav"])
script.schedule.append((0, 0.1, 1))
