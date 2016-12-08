PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(furtim.exe)
GlobPattern(/home/hs1/malware_samples/furtim.exe)
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
ExtractEP(e:\samples\shared\furtim.exe)
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
Decision=(RE:re,W1:unlock,W3:unlock,W5:unlock,W7:unlock,default:loop)

re:
TracerDebugContinueInf(0x80010001)
goto(decision)

unlock:
RunRoutine(0x104)
TracerDebugContinueInf
goto(decision)

loop:
TracerDebugContinueInf
goto(decision)

finish:
