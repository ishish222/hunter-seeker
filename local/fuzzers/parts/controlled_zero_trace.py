import statemachine
import usualparts.default_tracer_map as dm
import usualparts.binner_parts
import usualparts.globs as globs
import usualparts.tracing_parts
import usualparts.tracer_parts
import usualparts.taint_parts
import common

### declaring 

Start = statemachine.State()

ShutdownKillHost = statemachine.State()

GetSampleOptions = statemachine.State()
SpawnTrace = statemachine.State()
RevertClean = statemachine.State()
WaitForever = statemachine.State()
TracerConfigureMarkers = statemachine.State()
TracerConfigureDir= statemachine.State()
TracerConfigureSample = statemachine.State()
TracerDebugSample = statemachine.State()

DefaultShutdown = dm.ShutdownSequence

### linking

Start.name = "Start"
Start.consequence = dm.PrintLogo

WaitForever.name = "Waiting forever"
WaitForever.consequence = dm.ShutdownSequence
WaitForever.executing_routine = usualparts.other_parts.wait_for_keypress

TracerDebugSample.name = "Debugging sample"
TracerDebugSample.consequence = WaitForever
TracerDebugSample.executing_routine = usualparts.tracer_parts.tracer_debug_sample

TracerConfigureMarkers.name = "Configuring markers"
TracerConfigureMarkers.consequence = TracerDebugSample
TracerConfigureMarkers.executing_routine = usualparts.tracer_parts.tracer_configure_marker_st_end

TracerConfigureDir.name = "Configuring dir"
TracerConfigureDir.consequence = TracerConfigureMarkers
TracerConfigureDir.executing_routine = usualparts.tracer_parts.tracer_configure_dir

TracerConfigureSample.name = "Configuring sample"
TracerConfigureSample.consequence = TracerConfigureDir
TracerConfigureSample.executing_routine = usualparts.tracer_parts.tracer_configure_sample

SpawnTrace.name = "Trace sample"
SpawnTrace.consequence = TracerConfigureSample
SpawnTrace.executing_routine = usualparts.tracing_parts.trace_sample2_2

RevertClean.name = "Reverting to clean disk"
RevertClean.consequence = dm.EnableLogging
RevertClean.executing_routine = usualparts.qemu_parts.offline_revert

GetSampleOptions.name = "Getting sample options"
GetSampleOptions.consequence = RevertClean
GetSampleOptions.executing_routine = usualparts.tracing_parts.get_sample_options

### relinking

dm.GetOptions.consequence = GetSampleOptions
dm.SpawnTracer.consequence = TracerConfigureSample
dm.QemuConnectDevSocket.executing_routine = usualparts.qemu_parts.qemu_connect_dev_socket_infinite

