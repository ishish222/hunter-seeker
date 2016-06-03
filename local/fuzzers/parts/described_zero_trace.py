import statemachine
import usualparts.default_map as dm
import usualparts.binner_parts
import usualparts.globs as globs
import usualparts.tracing_parts
import usualparts.taint_parts
import common

### declaring 

Start = statemachine.State()

ShutdownKillHost = statemachine.State()

GetAdditionalOptions = statemachine.State()
SpawnTrace = statemachine.State()
RevertClean = statemachine.State()
WaitForever = statemachine.State()

DefaultShutdown = dm.ShutdownSequence

### linking

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

WaitForever.name = "Waiting forever"
WaitForever.consequence = dm.ShutdownSequence
WaitForever.consequence = usualparts.other_parts.wait_for_keypress

SpawnTrace.name = "Trace sample"
SpawnTrace.consequence = WaitForever
SpawnTrace.executing_routine = usualparts.tracing_parts.trace_sample2_2

RevertClean.name = "Reverting to clean disk"
RevertClean.consequence = dm.EnableLogging
RevertClean.executing_routine = usualparts.qemu_parts.offline_revert

GetAdditionalOptions.name = "Getting additional options"
GetAdditionalOptions.consequence = RevertClean
#GetAdditionalOptions.consequence = dm.EnableLogging
GetAdditionalOptions.executing_routine = usualparts.tracing_parts.get_additional_options

### relinking

#dm.AutogeneratedDisk.consequence = dm.StartQemuLoad
#dm.DiskGlob.consequence = dm.StartQemuLoad
dm.GetOptions.consequence = GetAdditionalOptions
dm.BinnerKillExplorer.consequence = dm.Cooldown # skip spawning app


dm.ShutdownSequence.consequence = ShutdownKillHost
dm.Cooldown.consequence = SpawnTrace

dm.QemuConnectDevSocket.executing_routine = usualparts.qemu_parts.qemu_connect_dev_socket_infinite
