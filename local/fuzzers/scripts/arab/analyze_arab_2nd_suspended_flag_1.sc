PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(arab.exe)
SetResearchDir(e:\samples\shared)
GlobPattern(/home/hs1/malware_samples/arab.exe)
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
EnableReaction(C5)
EnableReaction(C7)
EnableReaction(C9)
TracerDebugContinueInf

### First CreateProcess

TracerDebugContinueInf
TracerDebugContinueInf

### Second CreateProcess

TracerDebugContinueInf
TracerDebugContinueInf

### Third CreateProcess

# pass function prologue
TracerDebugContinueInf
# get PID and TID
ReadRegister(ESP)
Adjust(0x28)
ReadDword
Adjust(0x8)
ReadPID
ReadRegister(ESP)
Adjust(0x28)
ReadDword
Adjust(0xc)
ReadTID
EnableReaction(T1)
TracerDebugContinueInf

# first set context
ReadRegister(ESP)
Adjust(0x8)
ReadDword
# extract EIP
#EAX
Adjust(0xb0)
ReadDword
SaveEP
EnableReaction(R1)
TracerDebugContinueInf

# We have everything we need and attempt to resume
SpawnTracerLog
TracerConfigureSamplePID
TracerConfigureOutDir
TracerConfigurePIDPrefix
TracerConfigureInDir
TracerPrepareTrace
TracerRegisterBuiltin
DisableReactions
TracerAttachSample

# RR
LoadEP
ManualST
EnableReaction(ST)
TracerPrev
TracerDebugContinueInf
TracerNext
TracerDebugContinueInf

#ST
TracerRegisterReactions(self+0x35a3,Z1,0x103)
EnableBuiltin
EnableReaction(Z1)
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

#RX
