#!/usr/bin/env python

import generators.generator as generator
import generators.changer as changer

origin_path = "../origins/acad/test.dwg"
samples_shared_path = "../samples_shared"
samples_saved = "../samples_saved"

my_generator = generator.Generator(origin_path, samples_shared_path, ".dwg", changer.Changer)

my_generator.generate(3)

#setup

#setup client machine

#connect

#setup test mode

#in loop:
# generate sample
# write to pipe
# update stats
# move or remove sample
