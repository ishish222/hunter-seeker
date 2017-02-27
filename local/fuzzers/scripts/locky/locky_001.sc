PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(locky.exe)
SetResearchDir(e:\samples\shared)
GlobPattern(/home/hs1/malware_samples/locky.exe)
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
Execute(scripts/common/debug_sample.sc)

# RR
ExtractEP(e:\samples\shared\locky.exe)
SaveEP
ManualSTwSelf
DisableReactions
EnableReaction(ST)
TracerDebugContinueInf

# ST
EnableBuiltin
ExclusiveBuiltin
LowerBuiltin
Execute(scripts/common/enable_context_mod_detection.sc)

# we dont need for now, we pass by first creation
DumpMemory
TracerStartTrace
TracerDebugContinueInf

decision:
Decision=(RE:re,default:loop)

overwrite:
ReadRegister(ESP)
Adjust(0x4)
WriteDword(0x1)
TracerDebugContinueInf
goto(decision)

overwrite2:
ReadRegister(ESP)
Adjust(0x0)
WriteDword(0xa)
TracerDebugContinueInf
goto(decision)

loop:
TracerDebugContinueInf
goto(decision)

re:
TracerDebugContinueInf(0x80010001)
goto(decision)

