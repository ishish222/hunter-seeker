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

def cooldown():
    options = globs.state.options
    print "Cooling down"
    write_socket(options.s, "cooldown")
    read_socket(options.s)

Start = statemachine.State()
Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

Cooldown = statemachine.State()
Cooldown.name = "Cooling down"
Cooldown.consequence = dm.Sleep
Cooldown.executing_routine = cooldown

CreateEmpty = statemachine.State()
CreateEmpty.name = "Create empty disk"
CreateEmpty.consequence = dm.StartQemuFull
CreateEmpty.executing_routine = usualparts.disk_fs_parts.prepare_disk_empty

#reroutes
dm.PreparePipes.consequence = CreateEmpty
dm.BinnerSpawnApp.consequence = Cooldown
