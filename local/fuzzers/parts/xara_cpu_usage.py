import statemachine
import usualparts.default_map as dm
import usualparts.default_map
import usualparts.other_parts
import usualparts.disk_fs_parts
from common import write_socket, read_socket
import usualparts.globs as globs


def ask_for_cpu_usage():
    options = globs.state.options
    print "Checking usage"
    write_socket(options.s, "getCPUUsage")
    read_socket(options.s)

Start = statemachine.State()
Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

#redefinition for recursive behavior
CPUCheck = statemachine.State()

Sleep2 = statemachine.State()
Sleep2.name = "Sleeping 2nd"
Sleep2.consequence = CPUCheck
Sleep2.executing_routine = usualparts.other_parts.wait_1_seconds

CPUCheck = statemachine.State()
CPUCheck.name = "Check CPU monitoring"
CPUCheck.consequence = Sleep2
CPUCheck.executing_routine = ask_for_cpu_usage

#for recursion
Sleep2.consequence = CPUCheck

CreateEmpty = statemachine.State()
CreateEmpty.name = "Create empty disk"
CreateEmpty.consequence = dm.StartQemuFull
CreateEmpty.executing_routine = usualparts.disk_fs_parts.prepare_disk_empty

#reroutes
dm.PreparePipes.consequence = CreateEmpty
dm.BinnerSpawnApp.consequence = dm.BinnerCheckReady
dm.BinnerCheckReady.consequence = CPUCheck
