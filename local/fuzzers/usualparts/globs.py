#global options
#global status
#global reqScript
#global stats

#options = None
#status = "RD"
#reqScript = ""
#stats = None
#state = None

DEBUG_CONTINUE = 0x00010002L
DBG_EXCEPTION_NOT_HANDLED = 0x80010001L

class state_class(object):
    options = None
    status = None
    stats = None
    reqScript = ""
    initialized = False
    timeout = False
    shutting_down = False
    

def init():
    global state
    state = state_class()

