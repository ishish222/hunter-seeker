PrintLogo
RegisterSignals(exception)
GetOptions
GetSampleOptions
SetSampleFile(0F04DAD5194F97BB4F1808DF19196B04B4AEE1B8.exe)
SetResearchDir(e:\samples\shared)
GlobPattern(/home/hs1/malware_samples/XAgent-WIN/0F04DAD5194F97BB4F1808DF19196B04B4AEE1B8.exe)
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
Execute(scripts/common/debug_sample_no_log.sc)

# RR
ExtractEP(e:\samples\shared\0F04DAD5194F97BB4F1808DF19196B04B4AEE1B8.exe)
SaveEP
ManualSTwSelf
#TracerSetParameters(test1 test2)
DisableReactions
EnableReaction(ST)
TracerDebugContinueInf

# A1 -> A2 -> ST
DumpMemory
SecureAllSections
EnableBuiltin
ExclusiveBuiltin
LowerBuiltin
EnableReaction(s0)
RaiseReaction(s0)
RaiseReaction(s1)

# modifications
#TracerRegisterReactions(self+0x1e86,A1,0x105)
TracerRegisterReactions(
    SHELL32.dll+0x0141f2,SHELLEXEC+2:SHELLEXEC-1,0x0;
    SHELL32.dll+0x014276,SHELLEXEC-1:SHELLEXEC+2,0x100;
    )
EnableReaction(SHELLEXEC+2)

Execute(scripts/arab/enable_context_mod_detection.sc)

# we dont need for now, we pass by first creation
TracerStartTrace
TracerDebugContinueInf

decision:
Decision=(RE:re,SHELLEXEC+2:inspect_shellexecute,default:loop)

inspect_shellexecute:
    DumpFile(C:\ProgramData\netapi64.dll)
    Beep
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
