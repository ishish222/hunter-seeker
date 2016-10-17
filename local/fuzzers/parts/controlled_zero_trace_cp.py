import statemachine
import usualparts.default_tracer_map as dm
import usualparts.binner_parts
import usualparts.globs as globs
import usualparts.tracing_parts
import usualparts.tracer_parts
import usualparts.taint_parts
import common

### Execution nodes configuration

Start = statemachine.State()
Start.name = "Start"
Start.consequence = dm.PrintLogo

#ShutdownKillHost = statemachine.State()

GetSampleOptions = statemachine.State()
GetSampleOptions.name = "Getting sample options"
GetSampleOptions.executing_routine = usualparts.tracing_parts.get_sample_options

#SpawnTrace = statemachine.State()

SpawnTracerLog = statemachine.State()
SpawnTracerLog.name = "Spawning tracer w/log"
SpawnTracerLog.executing_routine = usualparts.tracing_parts.start_tracer_log

RevertClean = statemachine.State()
RevertClean.name = "Reverting to clean disk"
RevertClean.executing_routine = usualparts.qemu_parts.offline_revert

#WaitForever = statemachine.State()

#WaitKeypress = statemachine.State()
#WaitKeypress.name = "Waiting for ketpress"
#WaitKeypress.consequence = TracerDebugContinueInf
#WaitKeypress.executing_routine = usualparts.other_parts.wait_for_keypress

#WaitKeypress2 = statemachine.State()
#ListLibs1 = statemachine.State()
#ListLibs2 = statemachine.State()
#ListLibs3 = statemachine.State()

TracerConfigureMarkers = statemachine.State()
TracerConfigureMarkers.name = "Configuring markers"
TracerConfigureMarkers.executing_routine = usualparts.tracer_parts.tracer_configure_markers

TracerAutoSt = statemachine.State()
TracerAutoSt.name = "Setting auto ST marker"
TracerAutoSt.executing_routine = usualparts.tracer_parts.tracer_auto_st

DisableReactions = statemachine.State()
DisableReactions.name = "Disabling all reactions"
DisableReactions.executing_routine = usualparts.tracer_parts.tracer_disable_all_reactions

TracerSetLimit = statemachine.State()
TracerSetLimit.name = "Setting limit"
TracerSetLimit.executing_routine = usualparts.tracer_parts.tracer_set_limit

TracerConfigureInDir = statemachine.State()
TracerConfigureInDir.name = "Configuring in dir"
TracerConfigureInDir.executing_routine = usualparts.tracer_parts.tracer_configure_in_dir

TracerConfigureOutDir = statemachine.State()
TracerConfigureOutDir.name = "Configuring out dir"
TracerConfigureOutDir.executing_routine = usualparts.tracer_parts.tracer_configure_out_dir

TracerConfigureOutPrefix = statemachine.State()
TracerConfigureOutPrefix.name = "Configuring out prefix"
TracerConfigureOutPrefix.executing_routine = usualparts.tracer_parts.tracer_configure_out_prefix

TracerConfigureSample = statemachine.State()
TracerConfigureSample.name = "Configuring sample"
TracerConfigureSample.executing_routine = usualparts.tracer_parts.tracer_configure_sample

TracerDebugSample = statemachine.State()
TracerDebugSample.name = "Debugging sample"
TracerDebugSample.executing_routine = usualparts.tracer_parts.tracer_debug_sample

TracerDebugContinue = statemachine.State()
TracerDebugContinue1s = statemachine.State()
TracerDebugContinue3s = statemachine.State()
TracerDebugContinue1s2 = statemachine.State()
TracerDebugContinue1s3 = statemachine.State()

TracerDebugContinueInf = statemachine.State()
TracerDebugContinueInf.name = "Continuing"
TracerDebugContinueInf.executing_routine = usualparts.tracer_parts.tracer_debug_continue

TracerStartTrace = statemachine.State()
TracerStartTrace.name = "Starting trace"
TracerStartTrace.executing_routine = usualparts.tracer_parts.tracer_start_trace

TracerEndTrace = statemachine.State()
TracerEndTrace.name = "Ending trace"
TracerEndTrace.executing_routine = usualparts.tracer_parts.tracer_stop_trace


#ListTebs = statemachine.State()
#ReadEAX = statemachine.State()

CreateSuspended = statemachine.State()

ReadESP1 = statemachine.State()
ReadESP1.name = "Reading ESP"
ReadESP1.args = "ESP"
ReadESP1.executing_routine = usualparts.tracer_parts.tracer_read_register

Adjust1 = statemachine.State()
Adjust1.name = "Adjust to arg 5"
Adjust1.args = 0x4 * 5
Adjust1.executing_routine = usualparts.other_parts.adjust

WriteCreationFlags = statemachine.State()
WriteCreationFlags.name = "Writing CREATE_SUSPENDED flag"
WriteCreationFlags.args = (None, 0x00000004)
WriteCreationFlags.executing_routine = usualparts.tracer_parts.tracer_write_dword

EnableReactionC2 = statemachine.State()
EnableReactionC2.name = "Enabling reaction for spawned suspended process"
EnableReactionC2.args = "C2"
EnableReactionC2.executing_routine = usualparts.tracer_parts.tracer_enable_reaction


ReadProcessInfo = statemachine.State()
ReadProcessInfo.name = "Reading ESP"
ReadProcessInfo.args = "ESP"
ReadProcessInfo.executing_routine = usualparts.tracer_parts.tracer_read_register

ReadESP2 = statemachine.State()
ReadESP2.name = "Reading ESP"
ReadESP2.args = "ESP"
ReadESP2.executing_routine = usualparts.tracer_parts.tracer_read_register

Adjust2 = statemachine.State()
Adjust2.name = "Adjust to arg 9"
Adjust2.args = 0x4 * -1
Adjust2.executing_routine = usualparts.other_parts.adjust

ReadArg9 = statemachine.State()
ReadArg9.name = "Adjust to arg 9"
ReadArg9.args = None
ReadArg9.executing_routine = usualparts.tracer_parts.tracer_read_dword

Adjust3 = statemachine.State()
Adjust3.name = "Adjust to arg 2"
Adjust3.args = 0x4 * 2
Adjust3.executing_routine = usualparts.other_parts.adjust

ReadPID = statemachine.State()
ReadPID.name = "Reading PID"
ReadPID.args = None
ReadPID.executing_routine = usualparts.tracer_parts.tracer_read_dword

SpawnTracer2 = statemachine.State()
SpawnTracer2.name = "Spawning second tracer"
SpawnTracer2.executing_routine = usualparts.tracing_parts.start_tracer

TracerConfigureSample2 = statemachine.State()
TracerConfigureSample2.name = "Configuring sample PID"
TracerConfigureSample2.args = None
TracerConfigureSample2.executing_routine = usualparts.tracer_parts.tracer_configure_sample_pid

TracerDebugContinueUnhandledInf = statemachine.State()
TracerDebugContinueUnhandledInf.name = "Continuing UH"
TracerDebugContinueUnhandledInf.args = globs.DBG_EXCEPTION_NOT_HANDLED
TracerDebugContinueUnhandledInf.executing_routine = usualparts.tracer_parts.tracer_debug_continue

TracerPrepareTrace = statemachine.State()
TracerPrepareTrace.name = "Prepare trace output"
TracerPrepareTrace.executing_routine = usualparts.tracer_parts.tracer_prepare_trace

DumpMemory = statemachine.State()
DumpMemory.name = "Dumping memory"
DumpMemory.executing_routine = usualparts.tracer_parts.tracer_dump_memory

TracerRegisterReactions = statemachine.State()
TracerRegisterReactions.name = "Registering reactions"
TracerRegisterReactions.executing_routine = usualparts.tracer_parts.tracer_register_reactions

TracerRegisterRegions = statemachine.State()
TracerRegisterRegions.name = "Registering regions"
TracerRegisterRegions.executing_routine = usualparts.tracer_parts.tracer_register_regions

TracerEnableSysenter = statemachine.State()
TracerEnableSysenter.name = "Enabling SYSENTER reaction"
TracerEnableSysenter.args = "S1"
TracerEnableSysenter.executing_routine = usualparts.tracer_parts.tracer_enable_reaction

TracerEnableDebug1 = statemachine.State()
TracerEnableDebug1.name = "Enabling reaction"
TracerEnableDebug1.args = "S1"
TracerEnableDebug1.executing_routine = usualparts.tracer_parts.tracer_enable_reaction

TracerEnableDebug2 = statemachine.State()
TracerEnableDebug2.name = "Enabling reaction"
TracerEnableDebug2.args = "0x102"
TracerEnableDebug2.executing_routine = usualparts.tracer_parts.tracer_enable_reaction

TracerEnableDebug3 = statemachine.State()
TracerEnableDebug3.name = "Enabling reaction"
TracerEnableDebug3.args = "C1"
TracerEnableDebug3.executing_routine = usualparts.tracer_parts.tracer_enable_reaction

Decision = statemachine.State()
Decision.name = "Making decision"
Decision.consequence = None
Decision.executing_routine = usualparts.other_parts.wait_for_keypress

DefaultShutdown = dm.ShutdownSequence

### linking


### Starting Trace

# default start
dm.GetOptions.consequence = GetSampleOptions
GetSampleOptions.consequence = RevertClean
RevertClean.consequence = dm.EnableLogging
# choose option
dm.QemuConnectDevSocket.executing_routine = usualparts.qemu_parts.qemu_connect_dev_socket_infinite
# default spawning tracer
dm.SpawnTracerController.consequence        = SpawnTracerLog
SpawnTracerLog.consequence                  = TracerConfigureSample
TracerConfigureSample.consequence           = TracerConfigureOutDir
TracerConfigureOutDir.consequence           = TracerConfigureOutPrefix
TracerConfigureOutPrefix.consequence        = TracerConfigureInDir
TracerConfigureInDir.consequence            = TracerPrepareTrace
TracerPrepareTrace.consequence              = TracerConfigureMarkers
TracerConfigureMarkers.consequence          = TracerRegisterRegions
TracerRegisterRegions.consequence           = TracerRegisterReactions
TracerRegisterReactions.consequence         = DisableReactions
DisableReactions.consequence                = TracerDebugSample
TracerSetLimit.consequence                  = TracerDebugSample
TracerDebugSample.consequence               = TracerDebugContinueInf

### Decision making

def decision():
    options = globs.state.options

    print "received signal"
    print globs.state.ret[1:3]

    if(globs.state.ret[1:3] == "RR"):
        print "Decision is: Continuing"
        ### RR ###
        return TracerAutoSt

    if(globs.state.ret[1:3] == "RE"):
        print "Decision is: Continuing UH"
        ### RE ###
        return TracerDebugContinueUnhandledInf

    if(globs.state.ret[1:3] == "RX"):
        print "Process is exiting, finishing up"
        ### RX ###
        return TracerEndTrace

    if(globs.state.ret[1:3] == "RB"):
        if(globs.state.ret[3:5] == "ST"):
            print "Decision is: StartTrace"
            ### RB - ST ###
            return TracerEnableSysenter

        if(globs.state.ret[3:5] == "EN"):
            print "Decision is: EndTrace"
            ### RB - EN ###
            return TracerEndTrace

        if(globs.state.ret[3:5] == "C1"):
            print "Decision is: Modifying process creation"
            ### C1 ###
            return CreateSuspended

        if(globs.state.ret[3:5] == "C2"):
            print "Decision is: Reading spawned process information"
            ### C2 ###
            return ReadProcessInfo

    print "Decision is: Continuing"
    return TracerDebugContinueInf


Decision.choosing_consequence               = decision
TracerDebugContinueInf.consequence          = Decision
TracerDebugContinueUnhandledInf.consequence = Decision

### Decision: RR
TracerAutoSt.consequence            = TracerDebugContinueInf

### Decision: RB - ST
TracerEnableSysenter.consequence    = TracerEnableDebug1
#TracerEnableDebug1.consequence      = TracerEnableDebug2
TracerEnableDebug1.consequence      = TracerEnableDebug3
TracerEnableDebug2.consequence      = TracerEnableDebug3
TracerEnableDebug3.consequence      = DumpMemory
DumpMemory.consequence              = TracerStartTrace
TracerStartTrace.consequence        = TracerDebugContinueInf

### Decision: RB - EN
### Decision: RX
TracerEndTrace.consequence = DefaultShutdown

### Decision: RE
# empty, just continue

### Decision: C1
CreateSuspended.consequence     = ReadESP1
ReadESP1.consequence            = Adjust1
Adjust1.consequence             = WriteCreationFlags
WriteCreationFlags.consequence  = EnableReactionC2
EnableReactionC2.consequence    = TracerDebugContinueInf

### Decision: C2

ReadProcessInfo.consequence                 = ReadESP2
ReadESP2.consequence                        = Adjust2
Adjust2.consequence                         = ReadArg9
ReadArg9.consequence                        = Adjust3
Adjust3.consequence                         = ReadPID
ReadPID.consequence                         = SpawnTracer2
SpawnTracer2.consequence                    = TracerConfigureSample2
TracerConfigureSample.consequence           = TracerConfigureOutDir # dalej to samo xD tylko wczesniej trzeba odpauzowac 
'''
TracerConfigureOutDir.consequence           = TracerConfigureOutPrefix 
TracerConfigureOutPrefix.consequence        = TracerConfigureInDir
TracerConfigureInDir.consequence            = TracerPrepareTrace
TracerPrepareTrace.consequence              = TracerConfigureMarkers
TracerConfigureMarkers.consequence          = TracerRegisterRegions
TracerRegisterRegions.consequence           = TracerRegisterReactions
TracerRegisterReactions.consequence         = DisableReactions
DisableReactions.consequence                = TracerDebugSample
TracerDebugSample.consequence               = TracerDebugContinueInf
'''

## relinking 

dm.SpawnTracer.consequence = TracerConfigureSample


