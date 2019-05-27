#!/bin/bash

echo /home/ish/projects/2015-05-25-trace-taint/taint/caller_taint -i $1_instr_all.txt  -d $1_dump.bin -m $1_mods.bin -s $4 -e $5 -I -l$3
/home/ish/projects/2015-05-25-trace-taint/taint/caller_taint -i $1_instr_all.txt  -d $1_dump.bin -m $1_mods.bin -s $4 -e $5 -I -l$3
