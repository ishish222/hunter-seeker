import statemachine
import usualparts.default_map

Start = statemachine.State()
Start.name = "Start"
Start.consequence = usualparts.default_map.PrintLogo

#other_parts.GetOptions.consequence = statemachine.Exit




