import statemachine
import usualparts.hs_logging
import usualparts.other_parts 
import usualparts.diagnostic
import usualparts.qemu_parts
import usualparts.disk_fs_parts
import usualparts.binner_parts
import generators
import globs

#
#
# ?decyzja
#test
#binner_key_wait
#binner_check_ready
#binner_start_logs
#binner_configure
#binner_spawn_app
#binner_spawn

Sleep = statemachine.State()
Sleep.name = "Sleeping"
Sleep.consequence = statemachine.Exit
Sleep.executing_routine = usualparts.other_parts.wait_10_seconds

BinnerCheckReady = statemachine.State()
BinnerCheckReady.name = "Binner starting logs"
BinnerCheckReady.consequence = Sleep
BinnerCheckReady.executing_routine = usualparts.binner_parts.binner_start_logs

BinnerStartLogs = statemachine.State()
BinnerStartLogs.name = "Binner starting logs"
BinnerStartLogs.consequence = BinnerCheckReady
BinnerStartLogs.executing_routine = usualparts.binner_parts.binner_start_logs

BinnerConfigure = statemachine.State()
BinnerConfigure.name = "Configuring binner"
BinnerConfigure.consequence = BinnerStartLogs
BinnerConfigure.executing_routine = usualparts.binner_parts.binner_configure

BinnerSpawnApp = statemachine.State()
BinnerSpawnApp.name = "Binner spawning application"
BinnerSpawnApp.consequence = BinnerConfigure
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
QemuConnectDevSocket.acceptable_error_count = 10

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

GlobMethod = statemachine.State()
GlobMethod.name = "Generating saved using glob"
GlobMethod.consequence = statemachine.Exit
GlobMethod.executing_routine = usualparts.disk_fs_parts.create_saved_disk_glob

def choosing_saved_disk_procedure():
    import globs
    options = globs.state.options

    if(options.samples_source == "autogenerated_batch"):
        return AutogeneratedMethod
    else:
        return GlobMethod

ChooseSavedMethod = statemachine.State()
ChooseSavedMethod.name = "Choosing method"
ChooseSavedMethod.consequence = None
ChooseSavedMethod.choosing_consequence = choosing_saved_disk_procedure
ChooseSavedMethod.executing_routine = usualparts.qemu_parts.qemu_bind_pipes

PreparePipes = statemachine.State()
PreparePipes.name = "Prepare pipes"
PreparePipes.consequence = ChooseSavedMethod
PreparePipes.executing_routine = usualparts.qemu_parts.qemu_bind_pipes

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


