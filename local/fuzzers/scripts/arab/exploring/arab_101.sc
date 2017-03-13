Execute(scripts/arab/start.sc)
Execute(scripts/arab/debug_sample.sc)

# RR
TracerRegisterRegions(EDI:0x5000)

# exploring reactions
TracerRegisterReactions(EDI+0x18,B1,0x0)
TracerRegisterReactions(EDI+0x3f38,B5,0x0)
TracerRegisterReactions(EDI+0x2df9,B7:B8,0x101;EDI+0x2e14,B8:B7,0x100)
TracerRegisterReactions(self+0x1317,A1:A2,0x100;EAX+0x9ab,A2:ST:B1:B5:B7:U1,0x100;EDI,ST,0x0;EDI,U1,0x201)
DisableReactions
EnableReaction(A1)
TracerDebugContinueInf

# A1 -> A2 -> ST
StartLog(e:\samples\internal.log)
SpawnResponder(11443)
Enqueue(Test)
Dequeue
NextResponse
EnableBuiltin
ExclusiveBuiltin
LowerBuiltin
Execute(scripts/arab/enable_context_mod_detection.sc)

# we dont need for now, we pass by first creation
DumpMemory
TracerStartTrace
TracerDebugContinueInf

decision:
Decision=(RE:re,B1:loop,B5:overwrite2,default:loop)

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

