from multiprocessing import Lock
from datetime import datetime
import settings

def defined(name):
    if(name in globals()):
        return True
    if(name in locals()):
        return True
    if(name in vars()):
        return True
    names = name.split(".")
    if(len(names)>1):
        if(names[1] in dir(globals()[names[0]])):
            return True
    return False

if(defined("settings.log_level") == True):
    log_level = settings.log_level
else:
    log_level = 0

if(defined("settings.debug") == True):
    if(settings.debug == True):
        last_log_file = open("z:\\server\\last_log.txt", "w")

def timestamp():
    d=datetime.now()
    return d.strftime("%Y-%m-%d %H:%M:%S:%f")

regs = ["EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EBP", "ESP", "EIP"]

def decode_op1(dbg, op1):
    my_op = op1
    if(my_op[0] == '['):
        my_op = decode_op1(dbg, my_op[1:-1])
        my_op = int(struct.unpack("<i", "".join(dbg.read(my_op, 4)))[0]) & 0xffffffff
        return my_op
    for reg in regs:
        if(my_op.upper() == reg):
            my_op = dbg.get_register(reg)
            return my_op & 0xffffffff
    if(len(my_op.split("+")) >1):
        (a,b) = my_op.split("+")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a+b
        return my_op & 0xffffffff
    if(len(my_op.split("-")) >1):
        (a,b) = my_op.split("-")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a-b
        return my_op & 0xffffffff
    if(len(my_op.split("*")) >1):
        (a,b) = my_op.split("*")
        a = decode_op1(dbg, a)
        b = decode_op1(dbg, b)
        my_op = a*b
        return my_op & 0xffffffff
    return int(my_op, 16) & 0xffffffff

import threading
import ctypes
 
def _async_raise(tid, excobj):
    res = ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, ctypes.py_object(excobj))
    if res == 0:
        raise ValueError("nonexistent thread id")
    elif res > 1:
        # """if it returns a number greater than one, you're in trouble, 
        # and you should call it again with exc=NULL to revert the effect"""
        ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, 0)
        raise SystemError("PyThreadState_SetAsyncExc failed")
 
class Thread(threading.Thread):
    def raise_exc(self, excobj):
        assert self.isAlive(), "thread must be started"
        for tid, tobj in threading._active.items():
            if tobj is self:
                _async_raise(tid, excobj)
                return
        
        # the thread was alive when we entered the loop, but was not found 
        # in the dict, hence it must have been already terminated. should we raise
        # an exception here? silently ignore?
    
    def terminate(self):
        # must raise the SystemExit type, instead of a SystemExit() instance
        # due to a bug in PyThreadState_SetAsyncExc
        self.raise_exc(SystemExit)
