import statemachine
import usualparts.testing_parts
import usualparts.default_map as dm
from statemachine import MachineError, Exit
import usualparts.globs as globs

TracePerform = statemachine.State()
Start = statemachine.State()
TraceDecision = statemachine.State()

def make_trace_decision():
    options = globs.state.options
    status = globs.state.status
    state = globs.state

    if(state == None): 
        print "State is None, shutting down"
        return dm.Shutdown
    
    if(state.samples_exhausted):
        print 'Samples exhausted'
        return dm.Shutdown
    if(status == "PT"):
        return TracePerform
    if(status == "PTO"):
        return TracePerform
    if(status == "STTO"):
        return TracePerform
    if(status == "RDTO"):
        return TracePerform
    if(status == "SR"):
        return dm.ScriptRun
    if(status == "RD"):
        return TracePerform
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

    print "Unable to handle status %s, shutting down" %status
    return dm.Shutdown

TraceDecision.name = "Deciding on next step"
TraceDecision.consequence = None
TraceDecision.choosing_consequence = make_trace_decision

TracePerform.name = "Performing trace"
TracePerform.consequence = dm.GetResult
TracePerform.executing_routine = usualparts.tracing_parts.trace_sample

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

#reroutes
dm.GetResult.consequence = TraceDecision
dm.WaitForMoar.consequence = TraceDecision

dm.BinnerCheckReady.consequence = TracePerform



