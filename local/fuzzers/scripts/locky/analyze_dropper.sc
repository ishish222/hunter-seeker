PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(locky.exe)
GlobPattern(/home/hs1/malware_samples/locky.exe)
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
#ExtractEP(e:\samples\shared\locky.exe)
#SaveEP
#ManualSTwSelf
# for ST
#TracerRegisterReactions(self+0x3a61,A1:A2,0x100;EAX-0xde,A2:ST,0x100;EAX,ST:F1,0x0;self+0x1f280,F1,0x103)
TracerRegisterReactions(self+0x3a61,A1:A2,0x100;EAX-0xde,A2:ST,0x100;EAX,ST:F1,0x0;self+0x1f280,F1,0x102)
# for skipping loops
TracerRegisterReactions(self+0x17bc,G1:G2,0x101;self+0x42e4,G2:G3,0x101;self+0x4342,G3,0x102)
TracerRegisterBuiltin
DisableReactions
TracerDebugSample
TracerDebugContinueInf

# RR
EnableReaction(A1)
TracerDebugContinueInf

# ST
EnableBuiltin
EnableReaction(G1)
EnableReaction(C1)
EnableReaction(C3)
EnableReaction(C5)
EnableReaction(C7)
EnableReaction(C9)
EnableReaction(D1)
EnableReaction(T1)
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
