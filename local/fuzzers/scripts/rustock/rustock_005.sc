PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(rustock.exe)
SetResearchDir(e:\samples\shared)
GlobPattern(/home/hs1/malware_samples/rustock.exe)
SetOutDir(\\10.0.2.4\qemu)
CheckHostDir
RevertClean
EnableLogging
RegisterSignals(exception)
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
TracerRegisterRegions(0x400000:0x5000)
TracerRegisterReactions(self+0x71ad,A1:ST:U1,0x100;EBX+0xe75,ST,0x0;EBX+0xe78,U1,0x201)

DisableReactions
EnableReaction(A1)
TracerDebugContinueInf

# ST
TracerRegisterReactions(self+0x1c8e,A2,0x108:0x103)
EnableReaction(A2)

EnableBuiltin
ExclusiveBuiltin
LowerBuiltin
EnableReaction(s0)
RaiseReaction(s0)
RaiseReaction(s1)
Execute(scripts/common/enable_context_mod_detection.sc)

# we dont need for now, we pass by first creation
DumpMemory
SecureAllSections
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
