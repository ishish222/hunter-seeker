PrintLogo
RegisterSignals(exception)
GetOptions
GetSampleOptions
SetSampleFile(arab_560000.exe)
SetResearchDir(e:\samples\shared)
GlobPattern(/home/hs1/malware_samples/arab_560000.exe)
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
KillExplorer
ResetTracers
SpawnTracerController
SpawnTracerFileLog
TracerConfigureSample
TracerConfigureOutDir
TracerConfigureOutPrefix
TracerConfigureInDir
TracerPrepareTrace
TracerRegisterBuiltin
DisableReactions
TracerSetParameters(test1 test2 test3)
TracerDebugSample
TracerDebugContinueInf

# RR
# ExtractEP(e:\samples\shared\arab_560000.exe)
# SaveEP
# ManualSTwSelf
TracerRegisterReactions(self+0x1e47,ST,0x0)
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
TracerRegisterReactions(self+0x1f05,A1,0x0)
TracerRegisterReactions(self+0x1f07,A2,0x330)
EnableReaction(A1)
EnableReaction(A2)

Execute(scripts/arab/enable_context_mod_detection.sc)

# we dont need for now, we pass by first creation
DumpMemory
TracerStartTrace
TracerDebugContinueInf

decision:
Decision=(RE:re,A1:a1,default:loop)

a1:
Push(0x5)
WriteRegister(EAX)
TracerDebugContinueInf
goto(decision)

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
