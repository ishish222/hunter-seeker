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
#DumpMemory
#TracerStartTrace
EnableReaction(C1)
EnableReaction(C3)
TracerDebugContinueInf

### First CreateProcess

TracerDebugContinueInf
TracerDebugContinueInf

### Second CreateProcess

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
#EIP
#Adjust(0xb8)
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
#TracerRegisterReactions(0x00401de0,ST,0x0)
ManualST
EnableReaction(ST)
TracerPrev
TracerDebugContinueInf
TracerNext
TracerDebugContinueInf

#ST
EnableBuiltin
DumpMemory
TracerStartTrace
TracerDebugContinueInf

#RX
