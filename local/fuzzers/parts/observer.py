import statemachine
import usualparts.testing_parts
import usualparts.default_map as dm
from statemachine import MachineError, Exit
import usualparts.globs as globs

WalkPerform = statemachine.State()
Start = statemachine.State()
WalkDecision = statemachine.State()

def make_after_walk_decision():
    options = globs.state.options
    status = globs.state.status
    state = globs.state

    if(state == None): 
        return Exit
    
    if(state.samples_exhausted):
        print 'Samples exhausted'
        return dm.Shutdown
    if(status =="PTO"):
        # co to znaczy?
        return WalkPerform
    if(status == "STTO"):
        return WalkPerform
    if(status == "RDTO"):
        return WalkPerform
    if(status == "SR"):
        return dm.ScriptRun
    if(status == "RD"):
        return WalkPerform
    if(status == "MA"):
        return dm.CloseSample
    if(status == "TO"):
        return dm.CloseSample
    if(status == "WS"):
        return dm.WaitForMoar
    if(status == "WE"):
        return dm.CloseSample
    if(status == "CR"):
        return dm.HandleCrash

    return Exit

WalkDecision.name = "Deciding on next step"
WalkDecision.consequence = None
WalkDecision.choosing_consequence = make_after_walk_decision

WalkPerform.name = "Performing walk"
WalkPerform.consequence = dm.GetResult
WalkPerform.executing_routine = usualparts.testing_parts.walk_sample

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

#reroutes
dm.GetResult.consequence = WalkDecision
dm.WaitForMoar.consequence = WalkDecision

dm.BinnerCheckReady.consequence = WalkPerform



