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
WaitKeypress = statemachine.State()
WaitKeypress2 = statemachine.State()
ListLibs1 = statemachine.State()
ListLibs2 = statemachine.State()
ListLibs3 = statemachine.State()
TracerConfigureMarkers = statemachine.State()
TracerActivateMarkers = statemachine.State()
TracerConfigureInDir = statemachine.State()
TracerConfigureOutDir = statemachine.State()
TracerConfigureOutPrefix = statemachine.State()
TracerConfigureSample = statemachine.State()
TracerDebugSample = statemachine.State()
TracerDebugContinue = statemachine.State()
TracerDebugContinue1s = statemachine.State()
TracerDebugContinue3s = statemachine.State()
TracerDebugContinue1s2 = statemachine.State()
TracerDebugContinue1s3 = statemachine.State()
TracerDebugContinueInf = statemachine.State()
ListTebs = statemachine.State()
ReadEAX = statemachine.State()
ReadESP2 = statemachine.State()
ReadESP3 = statemachine.State()
ReadStack = statemachine.State()
ReadMemory = statemachine.State()
ReadMemory2 = statemachine.State()
WriteMemory = statemachine.State()
WriteEAX = statemachine.State()
ReadEAX2 = statemachine.State()
ReadEAX3 = statemachine.State()
ListMarkers = statemachine.State()
ListBpts = statemachine.State()
Decision = statemachine.State()

DefaultShutdown = dm.ShutdownSequence

### linking

Start.name = "Start"
Start.consequence = dm.PrintLogo

def decision():
    options = globs.state.options

    print "received signal"
    print globs.state.ret

    return TracerDebugContinueInf

Decision.name = "Making decision"
Decision.consequence = None
Decision.choosing_consequence = decision
Decision.executing_routine = usualparts.other_parts.wait_for_keypress

TracerDebugContinueInf.name = "Continuing"
TracerDebugContinueInf.consequence = Decision
TracerDebugContinueInf.executing_routine = usualparts.tracer_parts.tracer_debug_continue

WaitForever.name = "Waiting forever"
WaitForever.consequence = TracerDebugContinueInf
WaitForever.executing_routine = usualparts.other_parts.wait_for_keypress

ListMarkers.name = "Listing markers"
ListMarkers.consequence = WaitForever
ListMarkers.executing_routine = usualparts.tracer_parts.tracer_list_markers

ListBpts.name = "Listing bpts"
ListBpts.consequence = ListMarkers
ListBpts.executing_routine = usualparts.tracer_parts.tracer_list_bpts

ListLibs3.name = "Listing libs"
ListLibs3.consequence = ListBpts
ListLibs3.executing_routine = usualparts.tracer_parts.tracer_list_libs

TracerDebugContinue3s.name = "Wait for all libs"
TracerDebugContinue3s.consequence = ListLibs3
TracerDebugContinue3s.executing_routine = usualparts.tracer_parts.tracer_debug_continue_10_seconds

ReadStack.name = "Reading stack"
#ReadStack.consequence = TracerDebugContinue3s
ReadStack.consequence = ListLibs3
ReadStack.args = 10
ReadStack.executing_routine = usualparts.tracer_parts.tracer_read_stack

ReadEAX2.name = "Reading EAX"
ReadEAX2.consequence = ReadStack
ReadEAX2.args = "EAX"
ReadEAX2.executing_routine = usualparts.tracer_parts.tracer_read_register

WriteEAX.name = "Writing EAX"
WriteEAX.consequence = ReadEAX2
WriteEAX.args = ("EAX", 0x12345678)
WriteEAX.executing_routine = usualparts.tracer_parts.tracer_write_register

ReadMemory2.name = "Reading Memory"
ReadMemory2.consequence = WriteEAX
ReadMemory2.args = None
ReadMemory2.executing_routine = usualparts.tracer_parts.tracer_read_dword

ReadESP3.name = "Reading ESP"
ReadESP3.consequence = ReadMemory2
ReadESP3.args = "ESP"
ReadESP3.executing_routine = usualparts.tracer_parts.tracer_read_register

WriteMemory.name = "Writing Memory"
WriteMemory.consequence = ReadESP3
WriteMemory.args = (None, 0xffffffff)
WriteMemory.executing_routine = usualparts.tracer_parts.tracer_write_dword

ReadESP2.name = "Reading ESP"
ReadESP2.consequence = WriteMemory
ReadESP2.args = "ESP"
ReadESP2.executing_routine = usualparts.tracer_parts.tracer_read_register

ReadMemory.name = "Reading Memory"
ReadMemory.consequence = ReadESP2
ReadMemory.args = None
ReadMemory.executing_routine = usualparts.tracer_parts.tracer_read_dword

ReadEAX.name = "Reading ESP"
ReadEAX.consequence = ReadMemory
ReadEAX.args = "ESP"
ReadEAX.executing_routine = usualparts.tracer_parts.tracer_read_register

ListTebs.name = "Listing TEBs"
ListTebs.consequence = ReadEAX
ListTebs.executing_routine = usualparts.tracer_parts.tracer_list_tebs

TracerDebugContinue1s3.name = "Get the rest of events"
TracerDebugContinue1s3.consequence = ListTebs
TracerDebugContinue1s3.executing_routine = usualparts.tracer_parts.tracer_debug_continue_1_second

WaitKeypress2.name = "Waiting for ketpress2"
WaitKeypress2.consequence = ListTebs
WaitKeypress2.executing_routine = usualparts.other_parts.wait_for_keypress

ListLibs2.name = "Listing libs"
#ListLibs2.consequence = WaitKeypress2
ListLibs2.consequence = ListTebs
ListLibs2.executing_routine = usualparts.tracer_parts.tracer_list_libs

TracerDebugContinue1s2.name = "Get the rest of events"
TracerDebugContinue1s2.consequence = ListLibs2
TracerDebugContinue1s2.executing_routine = usualparts.tracer_parts.tracer_debug_continue

WaitKeypress.name = "Waiting for ketpress"
WaitKeypress.consequence = TracerDebugContinue1s2
WaitKeypress.executing_routine = usualparts.other_parts.wait_for_keypress

ListLibs1.name = "Listing libs"
#ListLibs1.consequence = WaitKeypress
ListLibs1.consequence = TracerDebugContinue1s2
ListLibs1.executing_routine = usualparts.tracer_parts.tracer_list_libs

TracerDebugContinue1s.name = "Get PROCESS_CREATED"
TracerDebugContinue1s.consequence = ListLibs1
TracerDebugContinue1s.executing_routine = usualparts.tracer_parts.tracer_debug_continue_1_second

TracerDebugSample.name = "Debugging sample"
TracerDebugSample.consequence = TracerDebugContinue1s
TracerDebugSample.executing_routine = usualparts.tracer_parts.tracer_debug_sample

TracerActivateMarkers.name = "Activate markers"
TracerActivateMarkers.consequence = TracerDebugSample
TracerActivateMarkers.executing_routine = usualparts.tracer_parts.tracer_activate_markers

TracerConfigureMarkers.name = "Configuring markers"
TracerConfigureMarkers.consequence = TracerActivateMarkers
TracerConfigureMarkers.executing_routine = usualparts.tracer_parts.tracer_configure_markers

TracerConfigureInDir.name = "Configuring in dir"
TracerConfigureInDir.consequence = TracerConfigureMarkers
TracerConfigureInDir.executing_routine = usualparts.tracer_parts.tracer_configure_in_dir

TracerConfigureOutPrefix.name = "Configuring out prefix"
TracerConfigureOutPrefix.consequence = TracerConfigureInDir
TracerConfigureOutPrefix.executing_routine = usualparts.tracer_parts.tracer_configure_out_prefix

TracerConfigureOutDir.name = "Configuring out dir"
TracerConfigureOutDir.consequence = TracerConfigureOutPrefix
TracerConfigureOutDir.executing_routine = usualparts.tracer_parts.tracer_configure_out_dir

TracerConfigureSample.name = "Configuring sample"
TracerConfigureSample.consequence = TracerConfigureOutDir
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

