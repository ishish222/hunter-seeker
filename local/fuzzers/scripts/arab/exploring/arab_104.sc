Execute(scripts/arab/start.sc)
Execute(scripts/arab/debug_sample.sc)

# RR
TracerRegisterRegions(EDI:0x5000)

# exploring reactions
TracerRegisterReactions(EDI+0x2fae,B1:B2,0x103;EDI+0x2fb0,B2:B1,0x100)
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
Decision=(default:loop)

loop:
TracerDebugContinueInf
goto(decision)

