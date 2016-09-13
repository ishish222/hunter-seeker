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
SpawnTracerLog = statemachine.State()
RevertClean = statemachine.State()
WaitForever = statemachine.State()
WaitKeypress = statemachine.State()
WaitKeypress2 = statemachine.State()
ListLibs1 = statemachine.State()
ListLibs2 = statemachine.State()
ListLibs3 = statemachine.State()
TracerConfigureMarkers = statemachine.State()
TracerAutoSt = statemachine.State()
TracerActivateMarkers = statemachine.State()
TracerActivateMarkers2 = statemachine.State()
TracerSetLimit = statemachine.State()
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
TracerStartTrace = statemachine.State()
TracerEndTrace = statemachine.State()
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
TracerDebugContinueUnhandledInf = statemachine.State()
TracerPrepareTrace = statemachine.State()
DumpMemory = statemachine.State()
TracerRegisterReactions = statemachine.State()
TracerRegisterRegions = statemachine.State()
TracerEnableSysenter = statemachine.State()
TracerEnableDebug1 = statemachine.State()
TracerEnableDebug2 = statemachine.State()
TracerEnableDebug3 = statemachine.State()

DefaultShutdown = dm.ShutdownSequence

### linking

Start.name = "Start"
Start.consequence = dm.PrintLogo

def decision():
    options = globs.state.options

    print "received signal"
    print globs.state.ret[1:3]

    if(globs.state.ret[1:3] == "RR"):
        print "Decision is: Continuing"
        return TracerAutoSt

    if(globs.state.ret[1:3] == "RE"):
        print "Decision is: Continuing UH"
        return TracerDebugContinueUnhandledInf

    if(globs.state.ret[1:3] == "RX"):
        print "Process is exiting, finishing up"
        return TracerEndTrace

    if(globs.state.ret[1:3] == "RB"):
        if(globs.state.ret[3:5] == "ST"):
            print "Decision is: StartTrace"
            return TracerEnableSysenter
        if(globs.state.ret[3:5] == "EN"):
            print "Decision is: EndTrace"
            return TracerEndTrace

    print "Decision is: Continuing"
    return TracerDebugContinueInf

Decision.name = "Making decision"
Decision.consequence = None
Decision.choosing_consequence = decision
Decision.executing_routine = usualparts.other_parts.wait_for_keypress

TracerDebugContinueUnhandledInf.name = "Continuing UH"
TracerDebugContinueUnhandledInf.consequence = Decision
TracerDebugContinueUnhandledInf.args = globs.DBG_EXCEPTION_NOT_HANDLED
TracerDebugContinueUnhandledInf.executing_routine = usualparts.tracer_parts.tracer_debug_continue

TracerDebugContinueInf.name = "Continuing"
TracerDebugContinueInf.consequence = Decision
TracerDebugContinueInf.executing_routine = usualparts.tracer_parts.tracer_debug_continue

TracerStartTrace.name = "Starting trace"
TracerStartTrace.consequence = TracerDebugContinueInf
TracerStartTrace.executing_routine = usualparts.tracer_parts.tracer_start_trace_debug

TracerActivateMarkers2.name = "Activate markers"
TracerActivateMarkers2.consequence = TracerDebugContinueInf
TracerActivateMarkers2.executing_routine = usualparts.tracer_parts.tracer_activate_markers

TracerAutoSt.name = "Setting auto ST marker"
TracerAutoSt.consequence = TracerActivateMarkers2
TracerAutoSt.executing_routine = usualparts.tracer_parts.tracer_auto_st

DumpMemory.name = "Dumping memory"
DumpMemory.consequence = TracerStartTrace
DumpMemory.executing_routine = usualparts.tracer_parts.tracer_dump_memory

TracerEnableDebug3.name = "Enabling all reactions"
TracerEnableDebug3.consequence = DumpMemory
#TracerEnableDebug3.args = "0x201"
TracerEnableDebug3.executing_routine = usualparts.tracer_parts.tracer_enable_all_reactions

TracerEnableDebug2.name = "Enabling reaction"
TracerEnableDebug2.consequence = TracerEnableDebug3
TracerEnableDebug2.args = "0x102"
TracerEnableDebug2.executing_routine = usualparts.tracer_parts.tracer_enable_reaction

TracerEnableDebug1.name = "Enabling reaction"
TracerEnableDebug1.consequence = TracerEnableDebug2
TracerEnableDebug1.args = "0x101"
TracerEnableDebug1.executing_routine = usualparts.tracer_parts.tracer_enable_reaction

TracerEnableSysenter.name = "Enabling SYSENTER reaction"
TracerEnableSysenter.consequence = TracerEnableDebug1
TracerEnableSysenter.executing_routine = usualparts.tracer_parts.tracer_enable_sysenter

TracerEndTrace.name = "Ending trace"
TracerEndTrace.consequence = DefaultShutdown
TracerEndTrace.executing_routine = usualparts.tracer_parts.tracer_stop_trace

WaitKeypress.name = "Waiting for ketpress"
WaitKeypress.consequence = TracerDebugContinueInf
WaitKeypress.executing_routine = usualparts.other_parts.wait_for_keypress

TracerDebugSample.name = "Debugging sample"
TracerDebugSample.consequence = TracerDebugContinueInf
TracerDebugSample.executing_routine = usualparts.tracer_parts.tracer_debug_sample

TracerSetLimit.name = "Setting limit"
TracerSetLimit.consequence = TracerDebugSample
TracerSetLimit.executing_routine = usualparts.tracer_parts.tracer_activate_markers

TracerActivateMarkers.name = "Activate markers"
TracerActivateMarkers.consequence = TracerSetLimit
TracerActivateMarkers.executing_routine = usualparts.tracer_parts.tracer_activate_markers

TracerRegisterReactions.name = "Registering reactions"
TracerRegisterReactions.consequence = TracerActivateMarkers
TracerRegisterReactions.executing_routine = usualparts.tracer_parts.tracer_register_reactions

TracerRegisterRegions.name = "Registering reactions"
TracerRegisterRegions.consequence = TracerRegisterReactions
TracerRegisterRegions.executing_routine = usualparts.tracer_parts.tracer_register_regions

TracerConfigureMarkers.name = "Configuring markers"
TracerConfigureMarkers.consequence = TracerRegisterRegions
TracerConfigureMarkers.executing_routine = usualparts.tracer_parts.tracer_configure_markers

TracerPrepareTrace.name = "Prepare trace output"
TracerPrepareTrace.consequence = TracerConfigureMarkers
TracerPrepareTrace.executing_routine = usualparts.tracer_parts.tracer_prepare_trace

TracerConfigureInDir.name = "Configuring in dir"
TracerConfigureInDir.consequence = TracerPrepareTrace
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

SpawnTracerLog.name = "Spawning tracer wo/log"
SpawnTracerLog.consequence = TracerConfigureSample
SpawnTracerLog.executing_routine = usualparts.tracing_parts.start_tracer

dm.GetOptions.consequence = GetSampleOptions
dm.SpawnTracer.consequence = TracerConfigureSample
dm.QemuConnectDevSocket.executing_routine = usualparts.qemu_parts.qemu_connect_dev_socket_infinite
dm.SpawnTracerController.consequence = SpawnTracerLog
