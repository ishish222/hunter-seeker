from multiprocessing import Lock
from datetime import datetime
import struct 

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

def timestamp():
    d=datetime.now()
    return d.strftime("%Y-%m-%d %H:%M:%S:%f")

regs = ["EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EBP", "ESP", "EIP"]

def decode_op1(dbg, op1):
#    print("op1: " + op1)
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
        for tid, tobj in list(threading._active.items()):
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

# CPU sage measurement

import ctypes
from ctypes import byref
from ctypes import Structure, Union
from ctypes.wintypes import *
import threading

__author__ = 'Shao-chuan Wang'

LONGLONG = ctypes.c_longlong
HQUERY = HCOUNTER = HANDLE
pdh = ctypes.windll.pdh
Error_Success = 0x00000000

class PDH_Counter_Union(Union):
    _fields_ = [('longValue', LONG),
                ('doubleValue', ctypes.c_double),
                ('largeValue', LONGLONG),
                ('AnsiStringValue', LPCSTR),
                ('WideStringValue', LPCWSTR)]

class PDH_FMT_COUNTERVALUE(Structure):
    _fields_ = [('CStatus', DWORD),
                ('union', PDH_Counter_Union),]

g_cpu_usage = 0
class QueryCPUUsageThread(threading.Thread):
    def __init__(self):
        super(QueryCPUUsageThread, self).__init__()
        self.hQuery = HQUERY()
        self.hCounter = HCOUNTER()
        if not pdh.PdhOpenQueryW(None, 
                                 0, 
                                 byref(self.hQuery)) == Error_Success:
            raise Exception
        if not pdh.PdhAddCounterW(self.hQuery,
                                 '''\\Processor(_Total)\\% Processor Time''',
                                 0,
                                 byref(self.hCounter)) == Error_Success:
            raise Exception
        
    def run(self):
        while True:
            global g_cpu_usage
            g_cpu_usage = self.getCPUUsage()
            print('cpu_usage: %d' % g_cpu_usage)
        
    def getCPUUsage(self):
        PDH_FMT_LONG = 0x00000100
        if not pdh.PdhCollectQueryData(self.hQuery) == Error_Success:
            raise Exception
        ctypes.windll.kernel32.Sleep(1000)
        if not pdh.PdhCollectQueryData(self.hQuery) == Error_Success:
            raise Exception

        dwType = DWORD(0)
        value = PDH_FMT_COUNTERVALUE()
        if not pdh.PdhGetFormattedCounterValue(self.hCounter,
                                          PDH_FMT_LONG,
                                          byref(dwType),
                                          byref(value)) == Error_Success:
            raise Exception

        return value.union.longValue

def getCPU():
    LONGLONG = ctypes.c_longlong
    HQUERY = HCOUNTER = HANDLE
    pdh = ctypes.windll.pdh
    Error_Success = 0x00000000

    hQuery = HQUERY()
    hCounter = HCOUNTER()

    if not pdh.PdhOpenQueryW(None, 0, byref(hQuery)) == Error_Success:
        raise Exception

    if not pdh.PdhAddCounterW(hQuery, '''\\Processor(_Total)\\% Processor Time''', 0, byref(hCounter)) == Error_Success:
        raise Exception

    PDH_FMT_LONG = 0x00000100
    if not pdh.PdhCollectQueryData(hQuery) == Error_Success:
        raise Exception

    ctypes.windll.kernel32.Sleep(1000)

    if not pdh.PdhCollectQueryData(hQuery) == Error_Success:
        raise Exception

    dwType = DWORD(0)
    value = PDH_FMT_COUNTERVALUE()

    if not pdh.PdhGetFormattedCounterValue(hCounter, PDH_FMT_LONG, byref(dwType), byref(value)) == Error_Success:
#        raise Exception
        # no idea why this exception occurs, let's try to handle it like overload
        return 100

    return value.union.longValue


if __name__=='__main__':
    thread = QueryCPUUsageThread()
    thread.start()
