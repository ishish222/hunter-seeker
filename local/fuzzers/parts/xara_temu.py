import statemachine
import usualparts.default_map as dm
import usualparts.taint_parts 
import usualparts.binner_parts
import usualparts.other_parts
import usualparts.qemu_parts
import time

Start = statemachine.State()
FindPID = statemachine.State()
TemuPoweroffNoRevert = statemachine.State()
TemuUmountDisks = statemachine.State()
TemuMountDisks = statemachine.State()
LongSleep = statemachine.State()
TemuCreateSharedDisk = statemachine.State()
TemuConfigurePlugin = statemachine.State()
TemuTaintBegin = statemachine.State()
TemuTaintWait = statemachine.State()
TemuTaintConclude = statemachine.State()
TemuCooldown = statemachine.State()
WaitForKey = statemachine.State()

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

DefaultShutdown = TemuPoweroffNoRevert

TemuTaintBegin.name = "Beginning tainting"
TemuTaintBegin.consequence = TemuTaintWait
TemuTaintBegin.executing_routine = usualparts.taint_parts.temu_taint_start

TemuTaintWait.name = "Waiting for user command to conclude"
TemuTaintWait.consequence = TemuTaintConclude
TemuTaintWait.executing_routine = usualparts.other_parts.wait_for_keypress

TemuTaintConclude.name = "Concluding tainting"
TemuTaintConclude.consequence = TemuPoweroffNoRevert
TemuTaintConclude.executing_routine = usualparts.taint_parts.temu_taint_conclude

TemuConfigurePlugin.name = "Configuring tracecap"
TemuConfigurePlugin.consequence = TemuTaintBegin
TemuConfigurePlugin.executing_routine = usualparts.taint_parts.temu_configure_tracecap

TemuCreateSharedDisk.name = "Creating samples disk for Temu"
TemuCreateSharedDisk.consequence = dm.StartQemuFull
TemuCreateSharedDisk.executing_routine = usualparts.taint_parts.create_temu_samples_disk

TemuPoweroffNoRevert.name = "Powering off Temu"
TemuPoweroffNoRevert.consequence = statemachine.Exit
TemuPoweroffNoRevert.executing_routine = usualparts.qemu_parts.temu_poweroff_no_revert

TemuCooldown.name = "Cooling down Temu"
TemuCooldown.consequence = FindPID
TemuCooldown.executing_routine = usualparts.taint_parts.cooldown_temu

#TemuUmountDisks.name = "Umounting Temu disks"
#TemuUmountDisks.consequence = TemuPoweroffNoRevert
#TemuUmountDisks.executing_routine = usualparts.qemu_parts.temu_umount_disks

FindPID.name = "Searching for PID"
FindPID.consequence = TemuConfigurePlugin
FindPID.executing_routine = usualparts.taint_parts.find_pid

def long_sleep():
    time.sleep(600)

LongSleep.name = "Taking a short 10 min nap"
LongSleep.consequence = dm.BinnerSpawnPythonServer
LongSleep.executing_routine = long_sleep

#WaitForKey.name = "Wait for user keypress"
#WaitForKey.consequence = dm.BinnerSpawn
#WaitForKey.executing_routine = dm.usualparts.other_parts.wait_for_keypress

#reroutes
dm.PreparePipes.consequence = TemuCreateSharedDisk
dm.StartQemuFull.consequence = LongSleep
#dm.StartQemuFull.consequence = TemuConfigurePlugin
dm.BinnerConfigure.consequence = FindPID
dm.StopLog.consequence = TemuUmountDisks
dm.BinnerSpawnApp.consequence = TemuCooldown
#dm.BinnerKillExplorer.consequence = WaitForKey

# replace socket with timeout with infinite
dm.QemuConnectDevSocket.executing_routine = usualparts.qemu_parts.qemu_connect_dev_socket_infinite

