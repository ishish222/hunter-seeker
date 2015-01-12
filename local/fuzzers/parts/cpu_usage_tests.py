import statemachine
import usualparts.default_map as dm
import usualparts.other_parts
import usualparts.disk_fs_parts

def ask_for_cpu_usage():
    print "Checking usage"
    
Sleep3 = statemachine.State()
Sleep3.name = "Sleeping 3nd"
Sleep3.consequence = statemachine.Exit
Sleep3.executing_routine = usualparts.other_parts.wait_10_seconds

CPUCheck2 = statemachine.State()
CPUCheck2.name = "Check CPU 2nd"
CPUCheck2.consequence = Sleep3
CPUCheck2.executing_routine = ask_for_cpu_usage

Sleep2 = statemachine.State()
Sleep2.name = "Sleeping 2nd"
Sleep2.consequence = CPUCheck2
Sleep2.executing_routine = usualparts.other_parts.wait_10_seconds

CPUCheck1 = statemachine.State()
CPUCheck1.name = "Check CPU 1st"
CPUCheck1.consequence = Sleep2
CPUCheck1.executing_routine = ask_for_cpu_usage

Start = statemachine.State()
Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = dm.GetOptions

CreateEmpty = statemachine.State()
CreateEmpty.name = "Create empty disk"
CreateEmpty.consequence = dm.StartQemuFull
CreateEmpty.executing_routine = usualparts.disk_fs_parts.prepare_disk_empty

# reroutes
dm.PreparePipes.consequence = CreateEmpty

dm.Sleep.consequence = CPUCheck1



