PrintLogo
RegisterSignals(exception)
GetOptions
GetSampleOptions
SetSampleFile(arab_560000_mod.exe)
SetResearchDir(e:\samples\shared)
GlobPattern(/home/hs1/malware_samples/arab_560000_mod.exe)
SetOutDir(\\10.0.2.4\qemu)
CheckHostDir
RevertClean
EnableLogging
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
SpawnTracerFileLog
TracerConfigureSample
TracerConfigureOutDir
TracerConfigureOutPrefix
TracerConfigureInDir
TracerPrepareTrace
TracerRegisterBuiltin
DisableReactions
TracerSetParameters(test1 test2 test3)

# configure responder
SpawnResponder(11443)
Enqueue(Test)
Enqueue(Test4)
Enqueue(Test3)
Enqueue(Test2)
Enqueue(Test1)
Enqueue(Test0)
Enqueue(Test9)
Enqueue(Test8)
Enqueue(Test7)
Enqueue(Test6)
Enqueue(Test5)
Enqueue(Test4)
Enqueue(Test3)
Enqueue(Test2)
Enqueue(Test1)
Enqueue(Test0)
Enqueue(Test9)
Dequeue
NextResponse

# start debug
TracerDebugSample
TracerDebugContinueInf

# RR
# ExtractEP(e:\samples\shared\arab_560000_mod.exe)
# SaveEP
# ManualSTwSelf
TracerRegisterReactions(self+0xf422,START,0x0)
TracerRegisterReactions(self+0x1f05,SELECTCASE:START,0x0)
TracerRegisterReactions(self+0x1f07,REPORTSELECTED,0x330)

TracerRegisterReactions(WININET.dll+0x20494,INTERNETCONNECTW+1:Z4,0x000;WININET.dll+0x20546,Z4:Z3,0x000)
TracerRegisterReactions(WININET.dll+0x1eef5,Z5:Z6,0x100;WININET.dll+0x1ef6c,Z6:Z5,0x000)
TracerRegisterReactions(WININET.dll+0x14ea3,Z7:Z8,0x000;WININET.dll+0x14f6c,Z8:Z7,0x100)
TracerRegisterReactions(WININET.dll+0x20615,Z9:Z0,0x000;WININET.dll+0x20846,Z0:Z9,0x100)
TracerRegisterReactions(WININET.dll+0x22d7d,Y1:Y2,0x000;WININET.dll+0x22e74,Y2:Y1,0x000)
TracerRegisterReactions(WININET.dll+0x1e2a6,Y3:Y4,0x000;WININET.dll+0x1e2e5,Y4:Y3,0x000)
EnableReaction(Z3)
EnableReaction(Z5)
EnableReaction(Z7)
EnableReaction(Y1)
EnableReaction(Y3)

DisableReactions
EnableReaction(SELECTCASE)
EnableReaction(REPORTSELECTED)

TracerDebugContinueInf

decision:
Decision=(START:Start,RE:re,SELECTCASE:SelectCase,Z3:overwrite,Z4:internetopen,Z6:httpsend,Z7:disable_ssl,Z9:disable2,Y1:get_info1,Y2:get_info2,Y3:get_info3,Y4:get_info4,A5:load_and_continue,A6:zero_eax,default:loop)

Start:
EnableBuiltin
ExclusiveBuiltin
LowerBuiltin
EnableReaction(s0)
RaiseReaction(s0)
RaiseReaction(s1)
# modifications

Execute(scripts/arab/enable_context_mod_detection.sc)

# we dont need for now, we pass by first creation
DumpMemory
TracerStartTrace
TracerDebugContinueInf
goto(decision)

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

internetopen:
ReadRegister(EAX)
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
ReadDword
SaveSize
TracerDebugContinueInf
goto(decision)

get_info4:
OutRegion
TracerDebugContinueInf
goto(decision)

SelectCase:
Push(0x0)
WriteRegister(EAX)
TracerDebugContinueInf
goto(decision)

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

exception:
Execute(scripts/common/interrupt.sc)
