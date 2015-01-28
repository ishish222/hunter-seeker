#global options
#global status
#global reqScript
#global stats

#options = None
#status = "RD"
#reqScript = ""
#stats = None
#state = None

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

