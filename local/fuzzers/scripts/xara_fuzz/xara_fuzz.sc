PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(arab.exe)
SetResearchDir(e:\samples\shared)
SetOutDir(\\10.0.2.4\qemu)
#generation configuration
#this is dummy
GlobPattern(/home/hs1/malware_samples/arab.exe)
CheckHostDir
RevertClean
EnableLogging
RegisterSignals
PrepareStats
PreparePipes
GlobMethod
DiskGlob
StartQemuFull
QemuMountDisks

start_controller:
SpawnInternalController
QemuConnectDevSocket
IsSocketConnected=(Y:success,N:fail)

fail:
Wait10
goto(start_controller)

success:
KillExplorer
ResetTracers
SpawnTracerController
SpawnTracerLog
Spawn(C:\Program Files\Xara\Xara Designer Pro X10\DesignerPro.exe)
Cooldown
TracerConfigureSample
TracerConfigureOutDir
TracerConfigureOutPrefix
TracerConfigureInDir
TracerPrepareTrace
TracerRegisterRegions(EDI:0x5000)
TracerRegisterReactions(self+0x1317,A1:A2,0x100;EAX+0x9ab,A2:ST:U1,0x100;EDI,ST,0x0;EDI,U1,0x201)
TracerRegisterBuiltin
DisableReactions
TracerDebugSample
TracerDebugContinueInf

# RR
EnableReaction(A1)
TracerDebugContinueInf

# ST
EnableBuiltin
DumpMemory
TracerStartTrace

loop:
TracerDebugContinueInf
Decision=(RE:loop,RX:finish,EN:finish)

finish:
