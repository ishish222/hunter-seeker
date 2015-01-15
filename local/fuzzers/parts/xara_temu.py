import statemachine
import usualparts.default_map as dm
import usualparts.taint_parts

Start = statemachine.State()
FindPID = statemachine.State()

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

FindPID.name = "Searching for PID"
FindPID.consequence = dm.Sleep
FindPID.executing_routine = usualparts.taint_parts.find_pid

#reroutes
dm.PreparePipes.consequence = dm.GlobMethod
dm.BinnerConfigure = FindPID
