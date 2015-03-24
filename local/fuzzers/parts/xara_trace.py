import statemachine
import usualparts.default_map as dm
import usualparts.binner_parts
import usualparts.globs as globs
import usualparts.tracing_parts
import common

Start = statemachine.State()

ShutdownKillHost = statemachine.State()
RefreshKillHost = statemachine.State()
PlugTimeHole = statemachine.State()

TraceConfigurePort = statemachine.State()
TraceStart = statemachine.State()
TraceEnd = statemachine.State()

DefaultShutdown = dm.ShutdownSequence

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

#TraceConfigurePort.name = "Configuring trace port"
#TraceConfigurePort.consequence = dm.Shutdown
#TraceConfigurePort.executing_routine = usualparts.tracing_parts.trace_sample

#TraceEnd.name = "Concluding execution of sample"
#TraceEnd.consequence = dm.Shutdown
#TraceEnd.executing_routine = 

TraceStart.name = "Starting execution of sample"
TraceStart.consequence = dm.Shutdown
TraceStart.executing_routine = usualparts.tracing_parts.trace_sample

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

dm.BinnerCheckReady.consequence = TraceStart

