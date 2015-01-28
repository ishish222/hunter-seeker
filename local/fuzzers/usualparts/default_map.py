import statemachine
import usualparts.hs_logging
import usualparts.other_parts 
import usualparts.diagnostic
import usualparts.qemu_parts
import usualparts.disk_fs_parts
import usualparts.binner_parts
import usualparts.testing_parts
import generators
import globs
from statemachine import MachineError, Exit

Sleep = statemachine.State()
Sleep.name = "Sleeping"
Sleep.consequence = statemachine.Exit
Sleep.executing_routine = usualparts.other_parts.wait_10_seconds

#for recursion

#paths

# no samples refreshing
StopLogNoRefresh = statemachine.State()
PoweroffNoRevertNoRefresh = statemachine.State()

# shutdown path
ShutdownSequence = statemachine.State()
RefreshSequence = statemachine.State()

Shutdown = statemachine.State()
CloseSample = statemachine.State()
LogResult = statemachine.State()
BinnerCheckReady = statemachine.State()
PreFuzzingActions= statemachine.State()
StopLog = statemachine.State()
TestPerform = statemachine.State()
ScriptRun = statemachine.State()
UpdateStats = statemachine.State()
HandleCrash = statemachine.State()

PreparePipes = statemachine.State()
RefreshSamples = statemachine.State()
PoweroffNoRevert = statemachine.State()

# execution loop
GetOutput = statemachine.State()
Decision = statemachine.State()

# disk generation
GlobMethod = statemachine.State()
DiskGlob = statemachine.State()
AutogeneratedMethod = statemachine.State()
AutogeneratedDisk = statemachine.State()

# regular setup
BinnerStartLogs = statemachine.State()
FuzzerReady = statemachine.State()
BinnerConfigure = statemachine.State()
Cooldown = statemachine.State()
BinnerSpawnApp = statemachine.State()
BinnerSpawn = statemachine.State()
BinnerKillExplorer = statemachine.State()
IsSocketConnected = statemachine.State()
QemuConnectDevSocket = statemachine.State()
BinnerSpawnPythonServer = statemachine.State()
QemuMountDisks = statemachine.State()
StartQemuFull = statemachine.State()
ChooseSavedMethod = statemachine.State()
PrepareStats = statemachine.State()
RegisterSignals = statemachine.State()
EnableLogging = statemachine.State()
GetOptions = statemachine.State()
PrintLogo = statemachine.State()


def make_after_test_decision():
    options = globs.state.options
    status = globs.state.status
    state = globs.state

    if(state == None): 
        return ShutdownSequence

    if(status == ""):
        if(state.timeout):
            return PoweroffNoRevertNoRefresh
        else:
            return GetOutput
    
    if(state.samples_exhausted):
        return RefreshSequence
    if(status =="PT"):
        # co to znaczy?
        return TestPerform
    if(status == "STTO"):
        return TestPerform
    if(status == "RDTO"):
        return TestPerform
    if(status == "SR"):
        return ScriptRun
    if(status == "RD"):
        return TestPerform
    if(status == "MA"):
        return UpdateStats
    if(status == "TO"):
        return UpdateStats
    if(status == "ST"):
        return GetOutput
    if(status == "CR"):
        return HandleCrash

    print "Unable to handle state: %s, shutting down" % status
    return ShutdownSequence

# Main decision vertex
Decision.name = "Deciding on next step"
Decision.consequence = None
Decision.choosing_consequence = make_after_test_decision

ShutdownSequence.name = "Initiating shutdown sequence"
ShutdownSequence.consequence = Shutdown
#ShutdownSequence.executing_routine = usualparts.other_parts.wait_100_seconds

RefreshSequence.name = "Initiating refresh sequence"
RefreshSequence.consequence = RefreshSamples

Shutdown.name = "Shutting down"
Shutdown.consequence = Exit
Shutdown.executing_routine = usualparts.qemu_parts.poweroff_no_revert

# Refresh path

PoweroffNoRevert.name = "Powering off w/o reverting"
PoweroffNoRevert.consequence = ChooseSavedMethod
PoweroffNoRevert.executing_routine = usualparts.qemu_parts.poweroff_no_revert

StopLog.name = "Disabling binner logging"
StopLog.consequence = PoweroffNoRevert
StopLog.executing_routine = usualparts.hs_logging.disable_logging

#Crash path

PoweroffNoRevertNoRefresh.name = "Powering off w/o reverting"
PoweroffNoRevertNoRefresh.consequence = StartQemuFull
PoweroffNoRevertNoRefresh.executing_routine = usualparts.qemu_parts.poweroff_no_revert

StopLogNoRefresh.name = "Disabling binner logging"
StopLogNoRefresh.consequence = PoweroffNoRevertNoRefresh
StopLogNoRefresh.executing_routine = usualparts.hs_logging.disable_logging

HandleCrash.name = "Handle the crash"
HandleCrash.consequence = StopLogNoRefresh
HandleCrash.executing_routine = usualparts.testing_parts.handle_crash

RefreshSamples.name = "Refresh samples batch"
RefreshSamples.consequence = PoweroffNoRevert
RefreshSamples.executing_routine = usualparts.testing_parts.handle_samples_exhaustion

ScriptRun.name = "Run a script"
ScriptRun.consequence = GetOutput
ScriptRun.executing_routine = usualparts.testing_parts.execute_script

CloseSample.name = "Closing sample"
CloseSample.consequence = GetOutput
CloseSample.executing_routine = usualparts.binner_parts.binner_close_sample

LogResult.name = "Logging the results"
LogResult.consequence = CloseSample
LogResult.executing_routine = usualparts.testing_parts.log_result

UpdateStats.name = "Updating stats"
UpdateStats.consequence = LogResult
UpdateStats.executing_routine = usualparts.testing_parts.update_stats

GetOutput.name = "Getting test result"
GetOutput.consequence = Decision
GetOutput.executing_routine = usualparts.testing_parts.read_last_sample
GetOutput.acceptable_error_count = 10
GetOutput.trans_error_handler = usualparts.other_parts.noop

TestPerform.name = "Perform sample test"
TestPerform.consequence = GetOutput
TestPerform.executing_routine = usualparts.testing_parts.test_sample

BinnerCheckReady.name = "Binner starting logs"
BinnerCheckReady.consequence = TestPerform
BinnerCheckReady.executing_routine = usualparts.binner_parts.binner_start_logs

PreFuzzingActions.name = "Performing pre-fuzzing actions"
PreFuzzingActions.consequence = BinnerCheckReady
PreFuzzingActions.executing_routine = usualparts.testing_parts.perform_pre_test

BinnerStartLogs.name = "Binner starting logs"
BinnerStartLogs.consequence = PreFuzzingActions
BinnerStartLogs.executing_routine = usualparts.binner_parts.binner_start_logs

FuzzerReady.name = "Fuzzer is ready"
FuzzerReady.consequence = BinnerStartLogs
FuzzerReady.executing_routine = usualparts.qemu_parts.qemu_ready

BinnerConfigure.name = "Configuring binner"
BinnerConfigure.consequence = FuzzerReady
BinnerConfigure.executing_routine = usualparts.binner_parts.binner_configure

Cooldown.name = "Cooling down"
Cooldown.consequence = BinnerConfigure
Cooldown.executing_routine = usualparts.binner_parts.cooldown

BinnerSpawnApp.name = "Binner spawning application"
BinnerSpawnApp.consequence = Cooldown
BinnerSpawnApp.executing_routine = usualparts.binner_parts.binner_spawn_app

BinnerSpawn.name = "Binner spawning"
BinnerSpawn.consequence = BinnerSpawnApp
BinnerSpawn.executing_routine = usualparts.binner_parts.binner_spawn

BinnerKillExplorer.name = "Killing explorer"
BinnerKillExplorer.consequence = BinnerSpawn
BinnerKillExplorer.executing_routine = usualparts.binner_parts.binner_kill_explorer

def is_socket_connected():
    options = globs.state.options
    state = globs.state

    if(state.initialized == True):    
        return BinnerKillExplorer
    else:
        return BinnerSpawnPythonServer

IsSocketConnected.name = "Checking socket connection"
IsSocketConnected.consequence = None
IsSocketConnected.choosing_consequence = is_socket_connected

QemuConnectDevSocket.name = "Connecting socket"
QemuConnectDevSocket.consequence = IsSocketConnected
QemuConnectDevSocket.executing_routine = usualparts.qemu_parts.qemu_connect_dev_socket
QemuConnectDevSocket.trans_error_handler = usualparts.other_parts.wait_10_seconds
QemuConnectDevSocket.acceptable_error_count = 100

BinnerSpawnPythonServer.name = "Spawning python server"
BinnerSpawnPythonServer.consequence = QemuConnectDevSocket
BinnerSpawnPythonServer.executing_routine = usualparts.binner_parts.binner_spawn_python_server

QemuMountDisks.name = "Mounting qemu disks"
QemuMountDisks.consequence = BinnerSpawnPythonServer
QemuMountDisks.executing_routine = usualparts.qemu_parts.qemu_mount_disks

StartQemuFull.name = "Perform full Qemu start"
StartQemuFull.consequence = QemuMountDisks
StartQemuFull.executing_routine = usualparts.qemu_parts.qemu_start_full

AutogeneratedDisk.name = "Create disk & autogenerate samples"
AutogeneratedDisk.consequence = StartQemuFull
AutogeneratedDisk.executing_routine = usualparts.disk_fs_parts.create_shared_disk_autogenerated

AutogeneratedMethod.name = "Generating saved using autogenerate"
AutogeneratedMethod.consequence = AutogeneratedDisk
AutogeneratedMethod.executing_routine = usualparts.disk_fs_parts.create_saved_disk_autogenerated

DiskGlob.name = "Create disk & autogenerate samples"
DiskGlob.consequence = StartQemuFull
DiskGlob.executing_routine = usualparts.disk_fs_parts.create_shared_disk_glob

GlobMethod.name = "Generating saved using glob"
GlobMethod.consequence = DiskGlob
GlobMethod.executing_routine = usualparts.disk_fs_parts.create_saved_disk_glob

def choosing_saved_disk_procedure():
    import globs
    options = globs.state.options

    if(options.samples_source == "autogenerated_batch"):
        return AutogeneratedMethod
    else:
        return GlobMethod

ChooseSavedMethod.name = "Choosing method"
ChooseSavedMethod.consequence = None
ChooseSavedMethod.choosing_consequence = choosing_saved_disk_procedure

PreparePipes.name = "Prepare pipes"
PreparePipes.consequence = ChooseSavedMethod
PreparePipes.executing_routine = usualparts.qemu_parts.qemu_prepare_pipes

PrepareStats.name = "Prepare stats"
PrepareStats.consequence = PreparePipes
PrepareStats.executing_routine = usualparts.diagnostic.prepare_stats

RegisterSignals.name = "Register signals"
RegisterSignals.consequence = PrepareStats
RegisterSignals.executing_routine = usualparts.other_parts.register_signals

EnableLogging.name = "Enabling & testing logging"
EnableLogging.consequence = RegisterSignals
EnableLogging.executing_routine = usualparts.hs_logging.enable_logging

GetOptions.name = "Get options"
GetOptions.consequence = EnableLogging
GetOptions.executing_routine = usualparts.other_parts.get_options

PrintLogo.name = "Print logo"
PrintLogo.consequence = GetOptions
PrintLogo.executing_routine = usualparts.other_parts.print_logo


