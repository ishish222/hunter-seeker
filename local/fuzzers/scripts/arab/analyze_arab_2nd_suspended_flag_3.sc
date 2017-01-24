Execute(scripts/arab/start.sc)
Execute(scripts/arab/debug_sample.sc)

# RR
TracerRegisterRegions(EDI:0x5000)
TracerRegisterReactions(self+0x1317,A1:A2,0x100;EAX+0x9ab,A2:ST:U1,0x100;EDI,ST,0x0;EDI,U1,0x201)
DisableReactions
EnableReaction(A1)
TracerDebugContinueInf

# A1 -> A2 -> ST
StartLog(e:\samples\internal.log)
SpawnResponder(11443)
#NextResponse(Test)
#GetHTTP(http://127.0.0.1)
NextResponse(Test3)
EnableBuiltin
ExclusiveBuiltin
LowerBuiltin
Execute(scripts/arab/enable_context_mod_detection.sc)

# we dont need for now, we pass by first creation
DisableReaction(D1)
DisableReaction(T1)
DisableReaction(R1)
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
Execute(scripts/arab/create_process_extract_tid_pid.sc)
EnableReaction(T1)
TracerDebugContinueInf

# extract EIP
Execute(scripts/arab/setthread_extract_eip.sc)
EnableReaction(R1)
TracerDebugContinueInf

# We have everything we need and attempt to resume
Execute(scripts/arab/attach_sample.sc)

# RR
LoadEP
ManualST
EnableReaction(ST)
TracerPrev
TracerDebugContinueInf
TracerNext
TracerRegisterReactions(self+0x35a3,Z1:Z2,0x103;self+0x117e3,Z2,0x104)
TracerRegisterReactions(wininet.dll+0x20494,Z3:Z4,0x0;wininet.dll+0x20546,Z4:Z3,0x0)
TracerRegisterReactions(wininet.dll+0x1eef5,Z5:Z6,0x100;wininet.dll+0x1ef6c,Z6:Z5,0x0)
TracerRegisterReactions(WININET.dll+0x14ea3,Z7:Z8,0x0;WININET.dll+0x14f6c,Z8:Z7,0x100)
TracerRegisterReactions(WININET.dll+0x20615,Z9:Z0,0x0;WININET.dll+0x20846,Z0:Z9,0x100)
TracerRegisterReactions(WININET.dll+0x22d7d,Y1:Y2,0x0;WININET.dll+0x22e74,Y2:Y1,0x0)
TracerRegisterReactions(WININET.dll+0x1e2a6,Y3:Y4,0x0;WININET.dll+0x1e2e5,Y4:Y3,0x0)
TracerRegisterReactions(self+0xd902,A3,0x102)
TracerRegisterReactions(self+0xd95c,A4,0x102)
EnableReaction(Z1)
EnableReaction(A3)
EnableReaction(A4)
TracerDebugContinueInf

decision:
Decision=(ST:started,RE:re,W1:zero_to_1,W2:zero_eax,W3:zero_to_1,W4:zero_eax,W5:zero_to_2,W6:zero_eax,W7:zero_to_2,W8:zero_eax,W9:zero_to_3,V1:zero_to_3,default:loop,Z3:overwrite,Z4:internetopen,Z6:httpsend,Z7:disable_ssl,Z9:disable2,Y1:get_info1,Y2:get_info2,Y3:get_info3,Y4:get_info4,default:loop)

started:
EnableBuiltin
ExclusiveBuiltin
LowerBuiltin
EnableReaction(Z3)
EnableReaction(Z5)
EnableReaction(Z7)
EnableReaction(Y1)
EnableReaction(Y3)
ExclusiveReaction(Z3)
ExclusiveReaction(Z5)
ExclusiveReaction(Z7)
ExclusiveReaction(Y1)
ExclusiveReaction(Y3)
#DisableReaction(m8)
#DisableReaction(m9)
#DisableReaction(n2)
#DisableReaction(n3)

# Modifying CONTEXTS
Execute(scripts/arab/enable_context_mod_detection.sc)

# eliminate waiting & sleeping
#Execute(scripts/arab/enable_wait_reduction.sc)
DumpMemory
TracerStartTrace
TracerDebugContinueInf
goto(decision)

overwrite:
#Execute(scripts/arab/disable_wait_reduction.sc)
ReadArgUni(1)
ReadStack
ReadRegister(ESP)
Adjust(0x8)
ReadDword
WriteUnicode(127.0.0.1)
ReadArgUni(1)
TracerDebugContinueInf
goto(decision)

internetopen:
ReadRegister(EAX)
#Execute(scripts/arab/enable_wait_reduction.sc)
TracerDebugContinueInf
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

httpsend:
ReadRegister(EAX)
TracerDebugContinueInf
goto(decision)

disable_ssl:
ReadRegister(ESP)
Adjust(0x8)
ReadDword
CheckEqual(0x1f)=(Y:disable,N:ignore)

disable:
ReadRegister(ESP)
Adjust(0xc)
ReadDword
WriteDword(0x0)

ignore:
TracerDebugContinueInf
goto(decision)

disable2:
ReadRegister(ESP)
Adjust(0x1c)
WriteDword(0x0)
TracerDebugContinueInf
goto(decision)

get_info1:
ReadStack
ReadRegister(ESP)
Adjust(0xc)
ReadDword
SaveOffset
ReadRegister(ESP)
Adjust(0x10)
ReadDword
ReadDword
SaveSize
TracerDebugContinueInf
goto(decision)

get_info2:
#ReadRegion
OutRegion
TracerDebugContinueInf
goto(decision)

get_info3:
ReadStack
ReadRegister(ESP)
Adjust(0x8)
ReadDword
SaveOffset
ReadRegister(ESP)
Adjust(0xc)
#ReadDword
ReadDword
SaveSize
TracerDebugContinueInf
goto(decision)

get_info4:
#ReadRegion
OutRegion
TracerDebugContinueInf
goto(decision)

loop:
TracerDebugContinueInf
goto(decision)

re:
TracerDebugContinueInf(0x80010001)
goto(decision)

#RX
