import statemachine
import usualparts.default_map as dm
import usualparts.other_parts
import usualparts.disk_fs_parts
from common import write_socket, read_socket
import usualparts.globs as globs


def ask_for_cpu_usage():
    options = globs.state.options
    print "Checking usage"
    write_socket(options.s, "getCPUUsage")
    read_socket(options.s)

def cpu_monitor_enable():
    options = globs.state.options
    write_socket(options.s, "startCPUMon")

def cpu_monitor_disable():
    options = globs.state.options
    write_socket(options.s, "startCPUMon")
 
Sleep4 = statemachine.State()
Sleep4.name = "Sleeping 4th"
Sleep4.consequence = statemachine.Exit
Sleep4.executing_routine = usualparts.other_parts.wait_10_seconds

CPUCheck2 = statemachine.State()
CPUCheck2.name = "Check CPU 2nd"
CPUCheck2.consequence = Sleep4
CPUCheck2.executing_routine = ask_for_cpu_usage

Sleep3 = statemachine.State()
Sleep3.name = "Sleeping 3nd"
Sleep3.consequence = CPUCheck2
Sleep3.executing_routine = usualparts.other_parts.wait_10_seconds

CPUCheck1 = statemachine.State()
CPUCheck1.name = "Check CPU 1st"
CPUCheck1.consequence = Sleep3
CPUCheck1.executing_routine = ask_for_cpu_usage

Sleep2 = statemachine.State()
Sleep2.name = "Sleeping 2nd"
Sleep2.consequence = CPUCheck1
Sleep2.executing_routine = usualparts.other_parts.wait_10_seconds

CPUEnable = statemachine.State()
CPUEnable.name = "Enable CPU monitoring"
CPUEnable.consequence = Sleep2
CPUEnable.executing_routine = ask_for_cpu_usage

CPUDisable = statemachine.State()
CPUDisable.name = "Disable CPU monitoring"
CPUDisable.consequence = Sleep2
CPUDisable.executing_routine = ask_for_cpu_usage

Start = statemachine.State()
Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = dm.GetOptions

CreateEmpty = statemachine.State()
CreateEmpty.name = "Create empty disk"
CreateEmpty.consequence = dm.StartQemuFull
CreateEmpty.executing_routine = usualparts.disk_fs_parts.create_disk_empty

# reroutes
dm.PreparePipes.consequence = CreateEmpty
dm.BinnerSpawn.consequence = dm.BinnerStartLogs
dm.BinnerCheckReady.consequence = CPUEnable

