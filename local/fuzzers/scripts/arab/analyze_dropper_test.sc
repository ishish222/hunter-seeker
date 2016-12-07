PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(arab.exe)
SetResearchDir(e:\samples\shared)
SetOutDir(\\10.0.2.4\qemu)
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
EnableReaction(C1)
EnableReaction(C3)
##DumpMemory
#TracerStartTrace

loop:
TracerDebugContinueInf
Decision=(RE:loop,RX:finish,EN:finish,C3:read_name_uni,C1:read_name_ansi,C2:loop,C4:loop)

read_name_ansi:
ReadArgAnsi(1)
goto(loop)

read_name_uni:
ReadArgUni(1)
goto(loop)

finish:
