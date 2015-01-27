import statemachine
import usualparts.default_map as dm
import usualparts.binner_parts

Start = statemachine.State()
ShutdownKillHost = statemachine.State()
RefreshKillHost = statemachine.State()

Start.name = "Start"
#Start.consequence = usualparts.default_map.PrintLogo
Start.consequence = usualparts.default_map.GetOptions

ShutdownKillHost.name = "Killing host application"
ShutdownKillHost.consequence = dm.Shutdown
ShutdownKillHost.executing_routine = usualparts.binner_parts.binner_kill_host 

RefreshKillHost.name = "Killing host application"
RefreshKillHost.consequence = dm.RefreshSamples
RefreshKillHost.executing_routine = usualparts.binner_parts.binner_kill_host 

#rerouting

dm.ShutdownSequence.consequence = ShutdownKillHost
dm.RefreshSequence.consequence = RefreshKillHost
