import statemachine
import usualparts.default_map as dm
import usualparts.taint_parts 
import usualparts.binner_parts
import time

Start = statemachine.State()
FindPID = statemachine.State()
TemuPoweroffNoRevert = statemachine.State()
TemuUmountDisks = statemachine.State()
TemuMountDisks = statemachine.State()
LongSleep = statemachine.State()
TemuCreateSharedDisk = statemachine.State()

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

TemuCreateSharedDisk.name = "Creating samples disk for Temu"
TemuCreateSharedDisk.consequence  = dm.StartQemuFull
TemuCreateSharedDisk.executing_routine = usualparts.taint_parts.create_temu_samples_disk

TemuPoweroffNoRevert.name = "Powering off Temu"
TemuPoweroffNoRevert.consequence = dm.Sleep
TemuPoweroffNoRevert.executing_routine = usualparts.qemu_parts.temu_poweroff_no_revert

#TemuMountDisks.name = "Mounting Temu disks"
#TemuMountDisks.consequence = dm.QemuConnectDevSocket
#TemuMountDisks.executing_routine = usualparts.qemu_parts.temu_mount_disks

#TemuUmountDisks.name = "Umounting Temu disks"
#TemuUmountDisks.consequence = TemuPoweroffNoRevert
#TemuUmountDisks.executing_routine = usualparts.qemu_parts.temu_umount_disks

FindPID.name = "Searching for PID"
FindPID.consequence = dm.Sleep
FindPID.executing_routine = usualparts.taint_parts.find_pid

def long_sleep():
    time.sleep(600)

LongSleep.name = "Taking a short 10 min nap"
LongSleep.consequence = dm.BinnerSpawnPythonServer
LongSleep.executing_routine = long_sleep

#reroutes
dm.PreparePipes.consequence = TemuCreateSharedDisk
dm.StartQemuFull.consequence = LongSleep
dm.BinnerConfigure.consequence = FindPID
dm.StopLog.consequence = TemuUmountDisks


