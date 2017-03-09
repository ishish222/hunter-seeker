PrintLogo
RegisterSignals(exception)
GetOptions
GetSampleOptions
SetSampleFile(04301B59C6EB71DB2F701086B617A98C6E026872.exe)
SetResearchDir(e:\samples\shared)
GlobPattern(/home/hs1/malware_samples/Sedreco_payload/04301B59C6EB71DB2F701086B617A98C6E026872.exe)
SetOutDir(\\10.0.2.4\qemu)
CheckHostDir
RevertClean
EnableLogging
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
Execute(scripts/common/debug_sample_no_log.sc)

# RR
ExtractEP(e:\samples\shared\04301B59C6EB71DB2F701086B617A98C6E026872.exe)
SaveEP
ManualSTwSelf
#TracerSetParameters(test1 test2)
DisableReactions
EnableReaction(ST)
TracerDebugContinueInf

# A1 -> A2 -> ST
EnableBuiltin
ExclusiveBuiltin
LowerBuiltin
EnableReaction(s0)
RaiseReaction(s0)
RaiseReaction(s1)

# modifications
#TracerRegisterReactions(self+0x1e86,A1,0x105)
#EnableReaction(A1)

Execute(scripts/arab/enable_context_mod_detection.sc)

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

exception:
Execute(scripts/common/interrupt.sc)
