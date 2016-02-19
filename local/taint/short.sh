#!/bin/bash

echo /home/ish/projects/2015-05-25-trace-taint/taint/caller_taint -i $1_instr_all.txt  -d $1_dump.bin -s $2  -m $1_mods.bin -I -l$3
/home/ish/projects/2015-05-25-trace-taint/taint/caller_taint -i $1_instr_all.txt  -d $1_dump.bin -s $2  -m $1_mods.bin -I -l$3
