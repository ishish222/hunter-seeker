PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(locky.exe)
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
TracerRegisterReactions(self+0x3a61,A1:A2,0x100;EAX-0xde,A2:ST,0x100;EAX,ST:F1,0x0;self+0x1f280,F1,0x103)
TracerRegisterBuiltin
DisableReactions
TracerDebugSample
TracerDebugContinueInf

# RR
EnableReaction(A1)
TracerDebugContinueInf

# ST
EnableBuiltin
EnableReaction(F1)
EnableReaction(C1)
EnableReaction(C3)
EnableReaction(C5)
EnableReaction(C7)
EnableReaction(C9)
EnableReaction(D1)
EnableReaction(T1)
EnableReaction(R1)
##DumpMemory
#TracerStartTraceDebug

loop:
TracerDebugContinueInf
Decision=(RE:loop,RX:finish,EN:finish,C1:read_name_uni,C3:read_name_ansi,default:loop)

read_name_ansi:
ReadArgAnsi(1)
goto(loop)

read_name_uni:
ReadArgUni(1)
goto(loop)

finish:
