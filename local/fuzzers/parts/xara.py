import statemachine
import usualparts.default_map as dm
import usualparts.binner_parts
import usualparts.globs as globs
import common

Start = statemachine.State()

ShutdownKillHost = statemachine.State()
RefreshKillHost = statemachine.State()
PlugTimeHole = statemachine.State()

DefaultShutdown = dm.ShutdownSequence

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

ShutdownKillHost.name = "Killing host application"
ShutdownKillHost.consequence = dm.Shutdown
ShutdownKillHost.executing_routine = usualparts.binner_parts.binner_kill_host 

RefreshKillHost.name = "Killing host application"
RefreshKillHost.consequence = dm.RefreshSamples
RefreshKillHost.executing_routine = usualparts.binner_parts.binner_kill_host 

def xara_plug_time_hole():
    options = globs.state.options

    common.execute_script(options, "ret")

PlugTimeHole.name = "Executing script for Xara time hole"
PlugTimeHole.consequence = dm.GetOutput
PlugTimeHole.executing_routine = xara_plug_time_hole

#rerouting

dm.ShutdownSequence.consequence = ShutdownKillHost
dm.RefreshSequence.consequence = RefreshKillHost
dm.ScriptRun.consequence = PlugTimeHole
