Execute(scripts/arab/start.sc)
Execute(scripts/arab/debug_sample.sc)

# RR
TracerRegisterRegions(EDI:0x5000)

# exploring reactions
TracerRegisterReactions(self+0x1317,ST,0x0)
DisableReactions
EnableReaction(ST)
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

