import statemachine
import usualparts.default_map as dm
import usualparts.binner_parts
import usualparts.globs as globs
import usualparts.tracing_parts
import usualparts.taint_parts
import common

Start = statemachine.State()

ShutdownKillHost = statemachine.State()
RefreshKillHost = statemachine.State()
PlugTimeHole = statemachine.State()

TraceConfigurePort = statemachine.State()
TraceStart = statemachine.State()
TraceEnd = statemachine.State()
DetachDebugger = statemachine.State()
PID = statemachine.State()
GetAdditionalOptions = statemachine.State()
RevertClean = statemachine.State()
WaitForever = statemachine.State()
WaitABit = statemachine.State()
OpenSample= statemachine.State()

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

WaitForever.name = "Waiting forever"
WaitForever.consequence = dm.ShutdownSequence
WaitForever.executing_routine = usualparts.other_parts.wait_for_keypress

OpenSample.name = "Starting execution of sample"
OpenSample.consequence = WaitForever
OpenSample.executing_routine = usualparts.tracing_parts.open_sample

WaitABit.name = "Waiting a bit"
WaitABit.consequence = OpenSample
WaitABit.executing_routine = usualparts.other_parts.wait_10_seconds

TraceStart.name = "Starting execution of sample"
TraceStart.consequence = WaitABit
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

DetachDebugger.name = "detaching debuggers"
DetachDebugger.consequence = TraceStart
DetachDebugger.executing_routine = usualparts.tracing_parts.detach_debugger

RevertClean.name = "Reverting to clean disk"
RevertClean.consequence = dm.EnableLogging
RevertClean.executing_routine = usualparts.qemu_parts.offline_revert

GetAdditionalOptions.name = "Getting additional options"
GetAdditionalOptions.consequence = RevertClean
GetAdditionalOptions.executing_routine = usualparts.tracing_parts.get_additional_options

PID.name = "GetPID"
PID.consequence = DetachDebugger
PID.executing_routine = usualparts.taint_parts.find_pid


#rerouting

dm.ShutdownSequence.consequence = ShutdownKillHost
dm.RefreshSequence.consequence = RefreshKillHost
dm.ScriptRun.consequence = PlugTimeHole
dm.GetOptions.consequence = GetAdditionalOptions

dm.Cooldown.consequence = PID

dm.QemuConnectDevSocket.executing_routine = usualparts.qemu_parts.qemu_connect_dev_socket_infinite

