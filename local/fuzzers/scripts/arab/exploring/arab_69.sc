Execute(scripts/arab/start.sc)
Execute(scripts/arab/debug_sample.sc)

# RR
TracerRegisterRegions(EDI:0x5000)

# exploring reactions
TracerRegisterReactions(EDI+0x18,B1,0x0)
#TracerRegisterReactions(EDI+0x3ef3,B3:B4,0x109;EDI+3efb,B4:B3,0x100)
TracerRegisterReactions(self+0x1317,A1:A2,0x100;EAX+0x9ab,A2:ST:B1:U1,0x100;EDI,ST,0x0;EDI,U1,0x201)
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
Decision=(RE:re,B1:overwrite,default:loop)

overwrite:
ReadRegister(ESP)
Adjust(0x4)
WriteDword(0x8)
TracerDebugContinueInf
goto(decision)

loop:
TracerDebugContinueInf
goto(decision)

re:
TracerDebugContinueInf(0x80010001)
goto(decision)

