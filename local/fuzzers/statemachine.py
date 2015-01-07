#!/usr/bin/env python
# author: Tomasz Salacinski (ishish)
# mailto: tomasz.salacinski@korrino.com

def dummy():
    pass

class MachineError(Exception):
    pass

class State:
    def __init__(self):
        self.name = "Generic"
        self.consequence = None
        self.available_consequences = None
        self.choosing_routine = dummy
        self.choosing_consequence = dummy
        self.executing_routine = dummy
        self.trans_routine = dummy
        self.check_trans_routine = dummy
        self.trans_error_handler = dummy
        self.acceptable_error_count = 1
        self.attempts = 0
    
Exit = State()
Exit.name = "Exit"

def _get_subclasses(cls):
    """ Run through subclasses of a particular class

        This returns all classes descended from the main class,
        _including_ the main class itself.  If showall is set to
        False (the default) then classes starting with Abstract 
        will not be returned.
    """
    for i in cls.__subclasses__():
        for c in _get_subclasses(i):
            yield c
    yield cls

