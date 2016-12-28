Execute(scripts/arab/start.sc)
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
StartLog(e:\samples\internal.log)
SpawnResponder80
NextResponse(Test)
GetHTTP(http://127.0.0.1)
NextResponse(Test3)
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
TracerRegisterReactions(self+0x35a3,Z1:Z2,0x103;self+0x117e3,Z2,0x104)
TracerRegisterReactions(wininet.dll+0x20492,Z3:Z4,0x0;wininet.dll+0x20547,Z4:Z3,0x0)
TracerRegisterReactions(wininet.dll+0x1eef3,Z5:Z6,0x100;wininet.dll+0x1ef6d,Z6:Z5,0x0)
EnableBuiltin
EnableReaction(Z1)
EnableReaction(Z3)
EnableReaction(Z5)
DisableReaction(m8)
DisableReaction(m9)
DisableReaction(n2)
DisableReaction(n3)
# Modifying CONTEXTS
EnableReaction(C1)
EnableReaction(C3)
EnableReaction(C5)
EnableReaction(C7)
EnableReaction(C9)
EnableReaction(D1)
EnableReaction(T1)
EnableReaction(R1)
# eliminate waiting & sleeping
EnableReaction(W1)
EnableReaction(W3)
EnableReaction(W5)
EnableReaction(W7)
EnableReaction(W9)
EnableReaction(V1)
DumpMemory
TracerStartTrace
TracerDebugContinueInf

decision:
Decision=(RE:re,W1:zero_to_1,W2:zero_eax,W3:zero_to_1,W4:zero_eax,W5:zero_to_2,W6:zero_eax,W7:zero_to_2,W8:zero_eax,W9:zero_to_3,V1:zero_to_3,default:loop,Z3:overwrite,Z4:internetopen,Z6:httpsend)

overwrite:
ReadArgUni(1)
ReadStack
ReadRegister(ESP)
Adjust(0x8)
ReadDword
WriteUnicode(127.0.0.1)
ReadArgUni(1)
TracerDebugContinueInf
goto(decision)

re:
TracerDebugContinueInf(0x80010001)
goto(decision)

zero_to_1:
# zero out timeout
ReadStack
ReadRegister(ESP)
Adjust(0x8)
WriteDword(0x0)
TracerDebugContinueInf
goto(decision)

zero_to_2:
# zero out timeout
ReadStack
ReadRegister(ESP)
Adjust(0x10)
WriteDword(0x0)
TracerDebugContinueInf
goto(decision)

zero_eax:
RunRoutine(0x104)
TracerDebugContinueInf
goto(decision)

zero_to_3:
# zero out timeout
ReadStack
ReadRegister(ESP)
Adjust(0x4)
WriteDword(0x0)
TracerDebugContinueInf
goto(decision)

loop:
TracerDebugContinueInf
goto(decision)

internetopen:
#ReadRegister(EAX)
TracerDebugContinueInf
goto(decision)

httpsend:
ReadRegister(EAX)
TracerDebugContinueInf
goto(decision)

#RX
