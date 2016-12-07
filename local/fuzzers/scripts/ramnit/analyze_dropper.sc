PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(ramnit.exe)
GlobPattern(/home/hs1/malware_samples/ramnit.exe)
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
ExtractEP(e:\samples\shared\ramnit.exe)
SaveEP
ManualSTwSelf
TracerRegisterBuiltin
DisableReactions
TracerDebugSample
TracerDebugContinueInf

# RR
EnableReaction(ST)
TracerDebugContinueInf

# ST
EnableBuiltin
EnableReaction(C1)
EnableReaction(C3)
EnableReaction(C5)
EnableReaction(C7)
EnableReaction(C9)
EnableReaction(D1)
EnableReaction(R1)
EnableReaction(W1)
EnableReaction(W3)
EnableReaction(W5)
EnableReaction(W7)
DumpMemory
TracerStartTrace
TracerDebugContinueInf

decision:
Decision=(RE:loop_2,RX:finish,EN:finish,C1:read_name_uni,C3:read_name_ansi,default:loop)

loop:
TracerDebugContinueInf
goto(decision)

loop_2:
TracerDebugContinueInf(0x80010001)
goto(decision)

read_name_ansi:
ReadArgAnsi(1)
TracerDebugContinueInf
goto(decision)

read_name_uni:
ReadArgUni(1)
TracerDebugContinueInf
goto(decision)

finish:
