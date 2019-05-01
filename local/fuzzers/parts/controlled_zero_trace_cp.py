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

TracerConfigureOutDir2= statemachine.State()
TracerConfigureOutDir2.name = "Configuring out dir 2"
TracerConfigureOutDir2.executing_routine = usualparts.tracer_parts.tracer_configure_out_dir

TracerConfigureOutPrefix2= statemachine.State()
TracerConfigureOutPrefix2.name = "Configuring out prefix 2"
TracerConfigureOutPrefix2.executing_routine = usualparts.tracer_parts.tracer_configure_out_prefix

TracerConfigureInDir2= statemachine.State()
TracerConfigureInDir2.name = "Configuring in dir 2"
TracerConfigureInDir2.executing_routine = usualparts.tracer_parts.tracer_configure_in_dir

TracerPrepareTrace2= statemachine.State()
TracerPrepareTrace2.name = "Prepare trace output 2"
TracerPrepareTrace2.executing_routine = usualparts.tracer_parts.tracer_prepare_trace

TracerConfigureMarkers2= statemachine.State()
TracerConfigureMarkers2.name = "Configuring markers 2"
TracerConfigureMarkers2.executing_routine = usualparts.tracer_parts.tracer_configure_markers


TracerRegisterRegions2= statemachine.State()
TracerRegisterRegions2.name = "Registering regions 2"
TracerRegisterRegions2.executing_routine = usualparts.tracer_parts.tracer_register_regions

TracerRegisterReactions2= statemachine.State()
TracerRegisterReactions2.name = "Registering reactions 2"
TracerRegisterReactions2.executing_routine = usualparts.tracer_parts.tracer_register_reactions

DisableReactions2= statemachine.State()
DisableReactions2.name = "Disabling all reactions 2"
DisableReactions2.executing_routine = usualparts.tracer_parts.tracer_disable_all_reactions

TracerRelease = statemachine.State()
TracerRelease.name = "Releasing"
TracerRelease.executing_routine = usualparts.tracer_parts.tracer_release_thread

SetST2= statemachine.State()
SetST2.name = "Setting ST marker 2"
SetST2.args = "self+0x7363,A2,0x0"
#SetST2.executing_routine = usualparts.tracer_parts.tracer_auto_st
SetST2.executing_routine = usualparts.tracer_parts.tracer_add_reaction

SelectPrev = statemachine.State()
SelectPrev.name = "Selecting previous tracer"
SelectPrev.executing_routine = usualparts.tracer_parts.trace_controller_activate_prev_tracer

ReadESP3 = statemachine.State()
ReadESP3.name = "Reading ESP"
ReadESP3.args = "ESP"
ReadESP3.executing_routine = usualparts.tracer_parts.tracer_read_register

Adjust4 = statemachine.State()
Adjust4.name = "Adjust to arg -1"
Adjust4.args = 0x4 * -1
Adjust4.executing_routine = usualparts.other_parts.adjust

ReadArg_1 = statemachine.State()
ReadArg_1.name = "Read argument -1"
ReadArg_1.args = None
ReadArg_1.executing_routine = usualparts.tracer_parts.tracer_read_dword

Adjust5 = statemachine.State()
Adjust5.name = "Adjust to arg 3"
Adjust5.args = 0x4 * 3
Adjust5.executing_routine = usualparts.other_parts.adjust

ReadTID = statemachine.State()
ReadTID.name = "Reading TID"
ReadTID.args = None
ReadTID.executing_routine = usualparts.tracer_parts.tracer_read_dword

TracerRelease = statemachine.State()
TracerRelease.name = "Releasing thread"
TracerRelease.args = None
TracerRelease.executing_routine = usualparts.tracer_parts.tracer_release_thread

SelectNext = statemachine.State()
SelectNext.name = "Selecting next"
SelectNext.executing_routine = usualparts.tracer_parts.trace_controller_activate_next_tracer

TracerAttach2 = statemachine.State()
TracerAttach2.name = "Attaching sample 2"
TracerAttach2.executing_routine = usualparts.tracer_parts.tracer_attach_sample

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


ListTebs = statemachine.State()
ListTebs.name = "Listing tebs"
ListTebs.executing_routine = usualparts.tracer_parts.tracer_list_tebs


#ReadEAX = statemachine.State()

GetPID = statemachine.State()
GetPID.name = "Getting PID"

ReadESP10 = statemachine.State()
ReadESP10.name = "Reading ESP 10"
ReadESP10.args = "ESP"
ReadESP10.executing_routine = usualparts.tracer_parts.tracer_read_register

Adjust10 = statemachine.State()
Adjust10.name = "Adjust 10"
Adjust10.args = 0x4 * 2
Adjust10.executing_routine = usualparts.other_parts.adjust

ReadPID2 = statemachine.State()
ReadPID2.name = "Reading PID2"
ReadPID2.args = None
ReadPID2.executing_routine = usualparts.tracer_parts.tracer_read_dword

SpawnTracer3 = statemachine.State()
SpawnTracer3.name = "Spawning 3rd tracer"
SpawnTracer3.executing_routine = usualparts.tracing_parts.start_tracer_log

TracerConfigureSample3 = statemachine.State()
TracerConfigureSample3.name = "Configuring sample PID"
TracerConfigureSample3.args = None
TracerConfigureSample3.executing_routine = usualparts.tracer_parts.tracer_configure_sample_pid

TracerConfigureOutDir3 = statemachine.State()
TracerConfigureOutDir3.name = "Configuring out dir 3"
TracerConfigureOutDir3.executing_routine = usualparts.tracer_parts.tracer_configure_out_dir

TracerConfigureOutPrefix3 = statemachine.State()
TracerConfigureOutPrefix3.name = "Configuring out prefix 3"
TracerConfigureOutPrefix3.executing_routine = usualparts.tracer_parts.tracer_configure_out_prefix

TracerConfigureInDir3 = statemachine.State()
TracerConfigureInDir3.name = "Configuring in dir 3"
TracerConfigureInDir3.executing_routine = usualparts.tracer_parts.tracer_configure_in_dir

TracerPrepareTrace3 = statemachine.State()
TracerPrepareTrace3.name = "Prepare trace output 3"
TracerPrepareTrace3.executing_routine = usualparts.tracer_parts.tracer_prepare_trace

TracerConfigureMarkers3 = statemachine.State()
TracerConfigureMarkers3.name = "Configuring markers 3"
TracerConfigureMarkers3.executing_routine = usualparts.tracer_parts.tracer_configure_markers

TracerRegisterRegions3 = statemachine.State()
TracerRegisterRegions3.name = "Registering regions 3"
TracerRegisterRegions3.executing_routine = usualparts.tracer_parts.tracer_register_regions

TracerRegisterReactions3 = statemachine.State()
TracerRegisterReactions3.name = "Registering reactions 3"
TracerRegisterReactions3.executing_routine = usualparts.tracer_parts.tracer_register_reactions

DisableReactions3 = statemachine.State()
DisableReactions3.name = "Disabling all reactions 3"
DisableReactions3.executing_routine = usualparts.tracer_parts.tracer_disable_all_reactions

TracerAttach3 = statemachine.State()
TracerAttach3.name = "Attaching sample 3"
TracerAttach3.executing_routine = usualparts.tracer_parts.tracer_attach_sample

SelectPrev3 = statemachine.State()
SelectPrev3.name = "Selecting previous tracer 3"
SelectPrev3.executing_routine = usualparts.tracer_parts.trace_controller_activate_prev_tracer

EnableReactionC4 = statemachine.State()
EnableReactionC4.name = "Enabling C4"
EnableReactionC4.args = "C4"
EnableReactionC4.executing_routine = usualparts.tracer_parts.tracer_enable_reaction


CreateSuspendedThread = statemachine.State()
CreateSuspendedThread.name = "Reading ESP"

ReadESP6 = statemachine.State()
ReadESP6.name = "Reading ESP"
ReadESP6.args = "ESP"
ReadESP6.executing_routine = usualparts.tracer_parts.tracer_read_register

Adjust7 = statemachine.State()
Adjust7.name = "Reading ESP"
Adjust7.args = 0x4 * 5
Adjust7.executing_routine = usualparts.other_parts.adjust

WriteThreadCreationFlags = statemachine.State()
WriteThreadCreationFlags.name = "Writing to suspend thread"
WriteThreadCreationFlags.args = (None, 0x00000004)
WriteThreadCreationFlags.executing_routine = usualparts.tracer_parts.tracer_write_dword

EnableReactionC5 = statemachine.State()
EnableReactionC5.name = "Enabling reaction for spawned suspended process"
EnableReactionC5.args = "C5"
EnableReactionC5.executing_routine = usualparts.tracer_parts.tracer_enable_reaction

CreateSuspended = statemachine.State()
CreateSuspended.name = "Creating suspended"

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

EnableReactionC3 = statemachine.State()
EnableReactionC3.name = "Enabling reaction for CreateRemoteThread"
EnableReactionC3.args = "C3"
EnableReactionC3.executing_routine = usualparts.tracer_parts.tracer_enable_reaction

ListAllTebs1 = statemachine.State()
ListAllTebs1.name = "Listing all TEBs"
ListAllTebs1.executing_routine = usualparts.tracer_parts.tracer_list_all_tebs

ListAllTebs2 = statemachine.State()
ListAllTebs2.name = "Listing all TEBs 2"
ListAllTebs2.executing_routine = usualparts.tracer_parts.tracer_list_all_tebs

ReadRemoteThreadInfo  = statemachine.State()
ReadRemoteThreadInfo.name = "Reading remote thread info"

ReadESP7 = statemachine.State()
ReadESP7.name = "Reading ESP"
ReadESP7.args = "ESP"
ReadESP7.executing_routine = usualparts.tracer_parts.tracer_read_register

Adjust8 = statemachine.State()
Adjust8.name = "Reading ESP"
Adjust8.args = 0x4 * 3
Adjust8.executing_routine = usualparts.other_parts.adjust

ReadEP = statemachine.State()
ReadEP.name = "Reading EntryPoint"
ReadEP.args = None
ReadEP.executing_routine = usualparts.tracer_parts.tracer_read_dword

Shift1 = statemachine.State()
Shift1.name = "Shifting bottom arg to top"
Shift1.executing_routine = usualparts.tracer_parts.tracer_read_dword

SelectNext4 = statemachine.State()
SelectNext4.name = "Selecting next 4"
SelectNext4.executing_routine = usualparts.tracer_parts.trace_controller_activate_next_tracer

SetST3 = statemachine.State()
SetST3.name = "Setting ST"
SetST3.args = None
SetST3.executing_routine = usualparts.tracer_parts.tracer_set_st

SelectPrev4 = statemachine.State()
SelectPrev4.name = "Selecting previous tracer 4"
SelectPrev4.executing_routine = usualparts.tracer_parts.trace_controller_activate_prev_tracer

ReadESP8 = statemachine.State()
ReadESP8.name = "Reading ESP"
ReadESP8.args = "ESP"
ReadESP8.executing_routine = usualparts.tracer_parts.tracer_read_register

Adjust9 = statemachine.State()
Adjust9.name = "Adjusting for argument ?"
Adjust9.args = 0x4 * 1
Adjust9.executing_routine = usualparts.other_parts.adjust

ReadTID3 = statemachine.State()
ReadTID3.name = "Reading TID"
ReadTID3.args = None
ReadTID3.executing_routine = usualparts.tracer_parts.tracer_read_dword

TracerRelease2 = statemachine.State()
TracerRelease2.name = "Releasing thread"
TracerRelease2.args = None
TracerRelease2.executing_routine = usualparts.tracer_parts.tracer_release_thread

SelectNext3 = statemachine.State()
SelectNext3.name = "Selecting next"
SelectNext3.executing_routine = usualparts.tracer_parts.trace_controller_activate_next_tracer

ReadProcessInfo = statemachine.State()
ReadProcessInfo.name = "Reading process info"
#ReadProcessInfo.args = "ESP" #TODO: chyba
#ReadProcessInfo.executing_routine = usualparts.tracer_parts.tracer_read_register

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
SpawnTracer2.executing_routine = usualparts.tracing_parts.start_tracer_log

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
#TracerPrepareTrace.consequence              = TracerConfigureMarkers
TracerPrepareTrace.consequence              = TracerRegisterRegions
TracerConfigureMarkers.consequence          = TracerRegisterRegions
TracerRegisterRegions.consequence           = TracerRegisterReactions
TracerRegisterReactions.consequence         = DisableReactions
DisableReactions.consequence                = TracerDebugSample
TracerSetLimit.consequence                  = TracerDebugSample
TracerDebugSample.consequence               = TracerDebugContinueInf

### Decision making

def decision():
    options = globs.state.options

    print("received signal")
    print(globs.state.ret[1:3])

    if(globs.state.ret[1:3] == "RR"):
        print("Decision is: Continuing")
        ### RR ###
        return TracerAutoSt

    if(globs.state.ret[1:3] == "RE"):
        print("Decision is: Continuing UH")
        ### RE ###
        return TracerDebugContinueUnhandledInf

    if(globs.state.ret[1:3] == "RX"):
        print("Process is exiting, finishing up")
        ### RX ###
        return TracerEndTrace

    if(globs.state.ret[1:3] == "RB"):
        if(globs.state.ret[3:5] == "ST"):
            print("Decision is: StartTrace")
            ### RB - ST ###
            return TracerEnableSysenter
        if(globs.state.ret[3:5] == "A2"):
            print("Decision is: StartTrace")
            ### RB - ST ###
            return TracerEnableSysenter

        if(globs.state.ret[3:5] == "EN"):
            print("Decision is: EndTrace")
            ### RB - EN ###
            return TracerEndTrace

        if(globs.state.ret[3:5] == "C1"):
            print("Decision is: Modifying process creation")
            ### C1 ###
            return CreateSuspended

        if(globs.state.ret[3:5] == "C2"):
            print("Decision is: Reading spawned process information")
            ### C2 ###
            return ReadProcessInfo

        if(globs.state.ret[3:5] == "C3"):
            print("Decision is: Reading spawned process information")
            ### C3 ###
            return GetPID

        if(globs.state.ret[3:5] == "C4"):
            print("Decision is: Reading spawned process information")
            ### C4 ###
            return CreateSuspendedThread

        if(globs.state.ret[3:5] == "C5"):
            print("Decision is: Reading spawned process information")
            ### C5 ###
            return CreateSuspendedThread

    print("Decision is: Continuing")
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
# We need second pipeline in order to make release, after that we can use it in recurrence

ReadProcessInfo.consequence                 = ReadESP3
ReadESP3.consequence                        = Adjust4
Adjust4.consequence                         = ReadArg_1
ReadArg_1.consequence                       = Adjust5
Adjust5.consequence                         = ReadTID # save TID
ReadTID.consequence                         = ReadESP2
ReadESP2.consequence                        = Adjust2
Adjust2.consequence                         = ReadArg9
ReadArg9.consequence                        = Adjust3
Adjust3.consequence                         = ReadPID # save PID
ReadPID.consequence                         = SpawnTracer2
SpawnTracer2.consequence                    = TracerConfigureSample2
TracerConfigureSample2.consequence          = TracerConfigureOutDir2
TracerConfigureOutDir2.consequence          = TracerConfigureOutPrefix2
TracerConfigureOutPrefix2.consequence       = TracerConfigureInDir2
TracerConfigureInDir2.consequence           = TracerPrepareTrace2
TracerPrepareTrace2.consequence             = TracerConfigureMarkers2
TracerConfigureMarkers2.consequence         = TracerRegisterRegions2
TracerRegisterRegions2.consequence          = TracerRegisterReactions2
TracerRegisterReactions2.consequence        = DisableReactions2
DisableReactions2.consequence               = TracerAttach2 # load PID
#TracerAttach2.consequence                   = SetST2 
TracerAttach2.consequence                   = EnableReactionC3
EnableReactionC3.consequence                = SelectPrev
#SetST2.consequence                          = SelectPrev
SelectPrev.consequence                      = TracerRelease # load TID
TracerRelease.consequence                   = SelectNext 
SelectNext.consequence                      = TracerDebugContinueInf

### Decision: C3
# After OpenProcess

GetPID.consequence                          = ReadESP10 # save PID
ReadESP10.consequence                       = Adjust10
Adjust10.consequence                        = ReadPID2
ReadPID2.consequence                        = EnableReactionC4
EnableReactionC4.consequence                = TracerDebugContinueInf


### Decision: C4

CreateSuspendedThread.consequence       = ReadESP6
ReadESP6.consequence                    = Adjust7
Adjust7.consequence                     = WriteThreadCreationFlags
WriteThreadCreationFlags.consequence    = EnableReactionC5
EnableReactionC5.consequence            = TracerDebugContinueInf


### Decision: C5

ReadRemoteThreadInfo.consequence            = ReadESP8 
ReadESP8.consequence                        = Adjust9
Adjust9.consequence                         = ReadTID3  # save TID 
ReadTID3.consequence                        = ReadESP7
ReadESP7.consequence                        = Adjust8
Adjust8.consequence                         = ReadEP    # save EP
ReadEP.consequence                          = Shift1    # PID to top
Shift1.consequence                          = SpawnTracer3 
SpawnTracer3.consequence                    = TracerConfigureSample3
TracerConfigureSample3.consequence          = TracerConfigureOutDir3
TracerConfigureOutDir3.consequence          = TracerConfigureOutPrefix3
TracerConfigureOutPrefix3.consequence       = TracerConfigureInDir3
TracerConfigureInDir3.consequence           = TracerPrepareTrace3
TracerPrepareTrace3.consequence             = TracerConfigureMarkers3
TracerConfigureMarkers3.consequence         = TracerRegisterRegions3
TracerRegisterRegions3.consequence          = TracerRegisterReactions3
TracerRegisterReactions3.consequence        = DisableReactions3
DisableReactions3.consequence               = TracerAttach3 # load PID
TracerAttach3.consequence                   = SetST3 # load EP
SetST3.consequence                          = SelectPrev4
SelectPrev4.consequence                     = TracerRelease2 # load TID
TracerRelease2.consequence                  = SelectNext3
SelectNext3.consequence                     = TracerDebugContinueInf

## relinking from default trace program

dm.SpawnTracer.consequence = TracerConfigureSample

