PrintLogo
GetOptions
GetSampleOptions
SetSampleFile(arab.exe)
SetResearchDir(e:\\samples\\shared)
SetOutDir(\\\\10.0.2.4\\qemu\\arab_4)
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

# ST
EnableBuiltin
#DumpMemory
#TracerStartTrace
TracerDebugContinueInf

# First CreateProcess
TracerDebugContinueInf
TracerDebugContinueInf

# Second CreateProcess
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
#Adjust(0xb0)
Adjust(0xb8)
SaveEP

# We have everything we need
SpawnTracerLog
TracerConfigureSamplePID
TracerConfigureOutDir
TracerConfigurePIDPrefix
TracerConfigureInDir
TracerPrepareTrace
TracerRegisterRegions
TracerRegisterReactions
DisableReactions
TracerAttachSample

# RR
LoadEP
ManualST
TracerDebugContinueInf


