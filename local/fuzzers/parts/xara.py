import statemachine
import usualparts.other_parts as other_parts

Start = statemachine.State()
Start.name = "Start"
Start.consequence = other_parts.PrintLogo

other_parts.PrintLogo.consequence = "Exit"
