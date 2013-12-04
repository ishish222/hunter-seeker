import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

import settings
from threading import Event
from multiprocessing import Lock, Process, Pipe
from subprocess import Popen, PIPE
from pydbg import *
from pydbg.defines import *
from functions import *
from subprocess import Popen
import utils
import win32pipe, win32file
import time
from select import select
import socket

### functions
# unable to move cause settings module is not visible

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

### binner class

class binner(object):
    def __init__(self, ph):
#        self.ph = None
        self.ph = ph
        self.test_lock = Lock()
        self.debuggers = {}                 # dictionary of debuggers for processes
        self.sockets = {}
        self.init_dbg = pydbg()
        self.crash_bin = utils.crash_binning.crash_binning()
        self.active = False

        self.dbg_event = Event()
        self.dbg_output = None
        self.dbg_line = ""
       
        self.status = "" 
        self.reqScript = ""
        self.main_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.main_socket.bind(("127.0.0.1", 12347))
        self.main_socket.listen(3)

    def dlog(self, data, level=0):
        dlog("[binner] %s" % data, level)

    def ddlog(self, data, level=0):
        dlog("%s" % data, level)

    def writePipe(self, data):
        self.ph.send(data)

    def readPipe(self):
        data = ""
        while True:
            data += self.ph.recv(1)
            
            if(data[-6:] == "-=OK=-"): 
                break
        return data[:-6]

    def ok(self):
        time.sleep(0.1)
        self.writePipe("-=OK=-")
#        win32file.FlushFileBuffers(self.ph)

    def send_command(self, cmd):
        self.dlog("Sending: %s" % cmd, 1)
        for pid in self.debuggers:
#            self.sockets[str(pid)].flush()
            self.write_debugger(self.sockets[str(pid)], cmd)
            self.ddlog(self.read_debugger(self.sockets[str(pid)]))
#            self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
        self.dlog("Sent: %s" % cmd, 1)
        self.dlog("- - - - - - - - - -", 1)

    def poll_debuggers(self, to = None):
        if(to != None):
            self.dlog("TO = %d seconds" % to, 1)
            readable = self.sockets.values()
            ready, _, _ = select(readable, [], [], to)
        else:
            readable = self.sockets.values()
            ready, _, _ = select(readable, [], [])
        return ready

    def read_debuggers(self):
        for dbg in self.debuggers.values():
            self.dlog(self.read_debugger(dbg))
                
    def read_debugger(self, dbg_socket):
        data = ""
        while True:
            data += dbg_socket.recv(1)
            if(data[-6:] == "=[OK]="):
                break
        statusOff = data.find("Status: ")
        if(statusOff > -1):
            self.status = data[statusOff+8:statusOff+8+2]
            if(self.status == "SR"):
                scOff = data.find("Script: ")
                lineEnd = data[scOff+8:].find("\n")
                self.reqScript = data[scOff+8:scOff+8+lineEnd]
                self.dlog(self.reqScript)
                self.dlog("Taken from data offsets: %d, %d" % (scOff, lineEnd))
        self.dlog("Parsed data: %s" % data, 2)
        return data[:-6]

    def write_debugger(self, dbg_socket, data):
        return dbg_socket.send(data + "\n")

    def loop_debuggers(self, to = None):
        self.dlog("Waiting for debug event")
        self.status = ""
        self.start_debuggers()
        ready_dbg_sockets = self.poll_debuggers(to)
        for dbg in ready_dbg_sockets:
            self.ddlog(self.read_debugger(dbg))
        self.stop_debuggers()
#        for dbg in self.sockets.values():
#            dbg.flush()
            
    def stop_debuggers(self):
        self.send_command("stop")

    def start_debuggers(self, to = None):
        if(to != None):
            self.send_command("start %f" % to)
        else:
            self.send_command("start")

    def list_tebs(self):
        for pid in self.debuggers:
            dlog("Listing TEBs for %s" % pid)
            yield self.debuggers[pid].list_tebs()

    def terminate_processes(self):
        for pid in self.debuggers:
            dlog("Terminating %s" % pid)
            self.debuggers[pid].terminate_process()

    def spawn(self, path):
        proc = subprocess.Popen(path)
        return proc

    def attach(self, pid):
        #self.debuggers[str(pid)], child = Pipe()
        #debugger()

        #Process(target = debugger_routine, args=(child,)).start()
#        self.debuggers[str(pid)] = Popen([sys.executable, "z:\\server\\debugger.py"], shell=False, stdin=PIPE)
#        self.debuggers[str(pid)] = Popen([sys.executable, "-u", "z:\\server\\debugger.py"], shell=False, stdin=PIPE, stdout=PIPE)
        self.debuggers[str(pid)] = Popen([sys.executable, "-u", "z:\\server\\debugger.py"], shell=True)
        self.sockets[str(pid)], addr = self.main_socket.accept()
        self.dlog("Got connection")
        self.ddlog(self.read_debugger(self.sockets[str(pid)]))
        self.ddlog(self.read_debugger(self.sockets[str(pid)]))

        self.write_debugger(self.sockets[str(pid)], "attach %s" % pid)
#        self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
        self.ddlog(self.read_debugger(self.sockets[str(pid)]))
        
        self.write_debugger(self.sockets[str(pid)], "read_config")
        self.ddlog(self.read_debugger(self.sockets[str(pid)]))

    def enumerate_processes(self):
        return self.init_dbg.enumerate_processes()

    def attach_all_markers(self):
        self.attach_markers()
        self.attach_st_markers()
        self.attach_end_markers()
        self.attach_react_markers()
        self.attach_rd_markers()

    def attach_markers(self):
        dlog("Attaching markers")
        self.send_command("attach_markers")

    def attach_st_markers(self):
        dlog("Attaching ST markers")
        self.send_command("attach_st_markers")

    def attach_end_markers(self):
        dlog("Attaching END markers")
        self.send_command("attach_end_markers")

    def attach_react_markers(self):
        dlog("Attaching REACT markers")
        self.send_command("attach_react_markers")
        
    def attach_rd_markers(self):
        dlog("Attaching RD markers")
        self.send_command("attach_rd_markers")
        
    def detach_all_markers(self):
        self.detach_markers()
        self.detach_st_markers()
        self.detach_end_markers()
        self.detach_react_markers()
        self.detach_rd_markers()

    def detach_markers(self):
        dlog("Detaching markers")
        self.send_command("detach_markers")

    def detach_st_markers(self):
        dlog("Detaching ST markers")
        self.send_command("detach_st_markers")

    def detach_end_markers(self):
        dlog("Detaching END markers")
        self.send_command("detach_end_markers")

    def detach_react_markers(self):
        dlog("Detaching REACT markers")
        self.send_command("detach_react_markers")

    def detach_rd_markers(self):
        dlog("Detaching RD markers")
        self.send_command("detach_rd_markers")



