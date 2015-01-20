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
Shutdown = statemachine.State()
CloseSample = statemachine.State()
BinnerCheckReady = statemachine.State()
PreFuzzingActions= statemachine.State()
StopLog = statemachine.State()
TestPerform = statemachine.State()
ScriptRun = statemachine.State()
UpdateStats = statemachine.State()
HandleCrash = statemachine.State()
PreparePipes = statemachine.State()
RefreshSamples = statemachine.State()
ChooseSavedMethod = statemachine.State()
PoweroffNoRevert = statemachine.State()
WaitForMoar = statemachine.State()
GetResult = statemachine.State()
Decision = statemachine.State()

def make_after_test_decision():
    options = globs.state.options
    status = globs.state.status
    state = globs.state

    if(state == None): 
        return Exit
    
    if(state.samples_exhausted):
        return RefreshSamples
    if(status =="PTO"):
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
        return WaitForMoar
    if(status == "CR"):
        return HandleCrash

    return Exit

# Main decision vertex
Decision.name = "Deciding on next step"
Decision.consequence = None
Decision.choosing_consequence = make_after_test_decision

WaitForMoar.name = "Waiting for moar input"
WaitForMoar.consequence = Decision
WaitForMoar.executing_routine = usualparts.testing_parts.read_output

Shutdown.name = "Shutting down"
Shutdown.consequence = Exit
Shutdown.executing_routine = usualparts.qemu_parts.poweroff_no_revert

PoweroffNoRevert.name = "Powering off w/o reverting"
PoweroffNoRevert.consequence = ChooseSavedMethod
PoweroffNoRevert.executing_routine = usualparts.qemu_parts.poweroff_no_revert

StopLog.name = "Disabling binner logging"
StopLog.consequence = PoweroffNoRevert
StopLog.executing_routine = usualparts.hs_logging.disable_logging

HandleCrash.name = "Handle the crash"
HandleCrash.consequence = StopLog
HandleCrash.executing_routine = usualparts.testing_parts.handle_crash

RefreshSamples.name = "Refresh samples batch"
RefreshSamples.consequence = PoweroffNoRevert
RefreshSamples.executing_routine = usualparts.testing_parts.handle_samples_exhaustion

ScriptRun.name = "Run a script"
ScriptRun.consequence = WaitForMoar
ScriptRun.executing_routine = usualparts.testing_parts.execute_script

CloseSample.name = "Closing sample"
CloseSample.consequence = WaitForMoar
CloseSample.executing_routine = usualparts.binner_parts.binner_close_sample

UpdateStats.name = "Updating stats"
UpdateStats.consequence = CloseSample
UpdateStats.executing_routine = usualparts.testing_parts.update_stats

GetResult.name = "Getting test result"
GetResult.consequence = Decision
GetResult.executing_routine = usualparts.testing_parts.read_last_sample

TestPerform.name = "Perform sample test"
TestPerform.consequence = WaitForMoar
TestPerform.executing_routine = usualparts.testing_parts.test_sample

BinnerCheckReady.name = "Binner starting logs"
BinnerCheckReady.consequence = TestPerform
BinnerCheckReady.executing_routine = usualparts.binner_parts.binner_start_logs

PreFuzzingActions.name = "Performing pre-fuzzing actions"
PreFuzzingActions.consequence = BinnerCheckReady
PreFuzzingActions.executing_routine = usualparts.testing_parts.perform_pre_test

BinnerStartLogs = statemachine.State()
BinnerStartLogs.name = "Binner starting logs"
BinnerStartLogs.consequence = PreFuzzingActions
BinnerStartLogs.executing_routine = usualparts.binner_parts.binner_start_logs

FuzzerReady = statemachine.State()
FuzzerReady.name = "Fuzzer is ready"
FuzzerReady.consequence = BinnerStartLogs
FuzzerReady.executing_routine = usualparts.qemu_parts.qemu_ready

BinnerConfigure = statemachine.State()
BinnerConfigure.name = "Configuring binner"
BinnerConfigure.consequence = FuzzerReady
BinnerConfigure.executing_routine = usualparts.binner_parts.binner_configure

Cooldown = statemachine.State()
Cooldown.name = "Cooling down"
Cooldown.consequence = BinnerConfigure
Cooldown.executing_routine = usualparts.binner_parts.cooldown

BinnerSpawnApp = statemachine.State()
BinnerSpawnApp.name = "Binner spawning application"
BinnerSpawnApp.consequence = Cooldown
BinnerSpawnApp.executing_routine = usualparts.binner_parts.binner_spawn_app

BinnerSpawn = statemachine.State()
BinnerSpawn.name = "Binner spawning"
BinnerSpawn.consequence = BinnerSpawnApp
BinnerSpawn.executing_routine = usualparts.binner_parts.binner_spawn

BinnerKillExplorer = statemachine.State()
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

IsSocketConnected = statemachine.State()
IsSocketConnected.name = "Checking socket connection"
IsSocketConnected.consequence = None
IsSocketConnected.choosing_consequence = is_socket_connected

QemuConnectDevSocket = statemachine.State()
QemuConnectDevSocket.name = "Connecting socket"
QemuConnectDevSocket.consequence = IsSocketConnected
QemuConnectDevSocket.executing_routine = usualparts.qemu_parts.qemu_connect_dev_socket
QemuConnectDevSocket.trans_error_handler = usualparts.other_parts.wait_10_seconds
QemuConnectDevSocket.acceptable_error_count = 100

BinnerSpawnPythonServer = statemachine.State()
BinnerSpawnPythonServer.name = "Spawning python server"
BinnerSpawnPythonServer.consequence = QemuConnectDevSocket
BinnerSpawnPythonServer.executing_routine = usualparts.binner_parts.binner_spawn_python_server

QemuMountDisks = statemachine.State()
QemuMountDisks.name = "Mounting qemu disks"
QemuMountDisks.consequence = BinnerSpawnPythonServer
QemuMountDisks.executing_routine = usualparts.qemu_parts.qemu_mount_disks

StartQemuFull = statemachine.State()
StartQemuFull.name = "Perform full Qemu start"
StartQemuFull.consequence = QemuMountDisks
StartQemuFull.executing_routine = usualparts.qemu_parts.qemu_start_full

AutogeneratedDisk = statemachine.State()
AutogeneratedDisk.name = "Create disk & autogenerate samples"
AutogeneratedDisk.consequence = StartQemuFull
AutogeneratedDisk.executing_routine = usualparts.disk_fs_parts.prepare_disk_autogenerated

AutogeneratedMethod = statemachine.State()
AutogeneratedMethod.name = "Generating saved using autogenerate"
AutogeneratedMethod.consequence = AutogeneratedDisk
AutogeneratedMethod.executing_routine = usualparts.disk_fs_parts.create_saved_disk_autogenerated

DiskGlob = statemachine.State()
DiskGlob.name = "Create disk & autogenerate samples"
DiskGlob.consequence = StartQemuFull
DiskGlob.executing_routine = usualparts.disk_fs_parts.prepare_disk_glob

GlobMethod = statemachine.State()
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

PreparePipes = statemachine.State()
PreparePipes.name = "Prepare pipes"
PreparePipes.consequence = ChooseSavedMethod
PreparePipes.executing_routine = usualparts.qemu_parts.qemu_prepare_pipes

PrepareStats = statemachine.State()
PrepareStats.name = "Prepare stats"
PrepareStats.consequence = PreparePipes
PrepareStats.executing_routine = usualparts.diagnostic.prepare_stats

RegisterSignals = statemachine.State()
RegisterSignals.name = "Register signals"
RegisterSignals.consequence = PrepareStats
RegisterSignals.executing_routine = usualparts.other_parts.register_signals

EnableLogging = statemachine.State()
EnableLogging.name = "Enabling & testing logging"
EnableLogging.consequence = RegisterSignals
EnableLogging.executing_routine = usualparts.hs_logging.enable_logging

GetOptions = statemachine.State()
GetOptions.name = "Get options"
GetOptions.consequence = EnableLogging
GetOptions.executing_routine = usualparts.other_parts.get_options

PrintLogo = statemachine.State()
PrintLogo.name = "Print logo"
PrintLogo.consequence = GetOptions
PrintLogo.executing_routine = usualparts.other_parts.print_logo


