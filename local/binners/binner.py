import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

from Queue import PriorityQueue
import settings
from threading import Event, Thread
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
        self.loop_lock = Lock()
        self.loop_lock.acquire()
        self.debuggers = {}                 # dictionary of debuggers for processes
        self.sockets = {}
        self.init_dbg = pydbg()
        self.crash_bin = utils.crash_binning.crash_binning()
        self.active = False
        self.last_crashed = None

        self.dbg_event = Event()
        self.dbg_output = None
        self.dbg_line = ""
       
        self.status = PriorityQueue()
        self.reqScript = ""
        self.main_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.main_socket.bind(("127.0.0.1", 12347))
        self.main_socket.listen(3)

        if(defined("settings.log_level") == True):
            self.log_level = settings.log_level
        else:
            self.log_level = 0

        if(defined("settings.debug") == True):
            if(settings.debug == True):
                self.debug = True
                self.last_log_file = open("z:\\logs\\init_log.txt", "w", 0)
        else:
            self.debug = False
            self.last_log_file = None

    def dlog(self, data, level=0):
#        dlog("[binner] %s" % data, level)
        if(self.debug == True):
            if(self.log_level <0):
                return
            if(level > self.log_level):
                return
            self.last_log_file.write("[%s] %s\n" % (timestamp(), data))

    def ddlog(self, data, level=0):
        #pass to regular log facility
        self.dlog("%s" % data, level)

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
        self.dlog("Sending: %s" % cmd, 3)
        for pid in self.debuggers:
#            self.sockets[str(pid)].flush()
            self.write_debugger(self.sockets[str(pid)], cmd)
#            self.ddlog(self.read_debugger(self.sockets[str(pid)]))
            # no read ?
#            self.read_debugger(self.sockets[str(pid)])
#            self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
        self.dlog("Sent: %s" % cmd, 3)
        self.dlog("- - - - - - - - - -", 3)

    def poll_debuggers(self, to = None):
        if(to != None):
            self.dlog("TO = %d seconds" % to, 3)
            readable = self.sockets.values()
            ready, _, _ = select(readable, [], [], to)
        else:
            self.dlog("Polling until readable", 2)
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
            status = data[statusOff+8:statusOff+8+2]
            if(status == "SR"):
                scOff = data.find("Script: ")
                lineEnd = data[scOff+8:].find("\n")
                self.reqScript = data[scOff+8:scOff+8+lineEnd]
                self.status.put((0, status, self.reqScript))
                self.dlog("Received: SR", 1)
            elif(status == "CR"):
                self.last_crashed = dbg_socket
                self.status.put((0, status))
                self.dlog("Received: CR", 1)
            elif(status == "TO"):
                self.status.put((1, status))
                self.dlog("Received: TO", 1)
            elif(status == "MA"):
                self.status.put((1, status))
                self.dlog("Received: MA", 1)
            elif(status == "RD"):
                self.status.put((1, status))
                self.dlog("Received: RD", 1)
            elif(status == "ST"):
                self.status.put((1, status))
                self.dlog("Received: ST", 1)
        self.dlog("Received data: \n%s" % data, 3)
        return data[:-6]

    def write_debugger(self, dbg_socket, data):
        return dbg_socket.send(data + "\n")

    def loop_debuggers(self, to = None, invocation = None):
        self.loop_debuggers_iteration(to, invocation)

        # answers are good, but we're waiting for events
        while(self.status.qsize() == 0):
            self.loop_debuggers_iteration(to)

    def race1(self, event):
        self.poll_debuggers()
        event.set()

    def race2(self, to, event):
        time.sleep(to)
        self.status.put((1, "TO"))
        self.dlog("Received: TO", 1)
        event.set()

    # start and break on event
    def loop_debuggers_iteration(self, to = None, invocation = None):
        if(to == None):
            # will wait for signal
            self.start_debuggers("Loop interation")
            if(invocation != None):
                self.dlog("Invoking: %s" % invocation)
                Popen(invocation)
            self.poll_debuggers()
            self.stop_debuggers("Detected readiness")
        else:
            event = Event()
            # will wait for timeout
            self.start_debuggers("Loop interation")
            if(invocation != None):
                self.dlog("Invoking: %s" % invocation)
                Popen(invocation)
            # Create race
            Thread(target=self.race1, args=(event,)).start()
            Thread(target=self.race2, args=(to,event)).start()
            event.wait()
            self.stop_debuggers("Race finished")

    # start, collect events, but ignore them 
    def stop_debuggers(self, reason="unknown"):
        self.send_command("stop")
        self.loop_lock.acquire()
        self.dlog("[LOCK] Debug section", 2)
        self.dlog("Reason: %s" % reason, 2)
        ready_dbg_sockets = self.poll_debuggers(0.0)
        for dbg in ready_dbg_sockets:
            self.read_debugger(dbg)
        self.dlog("Status queue content (%d items):" % self.status.qsize())
        for i in range(0, self.status.qsize()):
            temp = self.status.get()
            self.dlog(temp, 2)
            self.status.put(temp)

    def start_debuggers(self, reason="unknown", to=None):
        self.loop_lock.release()
        self.dlog("[UNLOCK] Debug section", 2)
        self.dlog("Waiting for debug event")
        self.dlog("Reason: %s" % reason)
        if(to != None):
            self.send_command("start %f" % to)
        else:
            self.send_command("start")

    def list_tebs(self):
        for pid in self.debuggers:
            dlog("Listing TEBs for %s" % pid)
            yield self.debuggers[pid].list_tebs()

    def terminate_processes(self):
         self.send_command("terminate")

    def spawn(self, path):
        proc = subprocess.Popen(path)
        return proc

    def attach(self, pid):
        self.debuggers[str(pid)] = Popen([sys.executable, "-u", "z:\\server\\debugger.py"], shell=True)
        self.sockets[str(pid)], addr = self.main_socket.accept()
        self.dlog("Got connection")
        self.ddlog(self.read_debugger(self.sockets[str(pid)]))
        self.ddlog(self.read_debugger(self.sockets[str(pid)]))

        print("Sending attach")
        self.write_debugger(self.sockets[str(pid)], "attach %s" % pid)
        print("Sending read config")
        self.write_debugger(self.sockets[str(pid)], "read_config")

    def enumerate_processes(self):
        return self.init_dbg.enumerate_processes()

    def attach_all_markers(self):
        self.attach_markers()
        self.attach_st_markers()
        self.attach_end_markers()
        self.attach_react_markers()
        self.attach_rd_markers()

    def attach_av_handler(self):
        self.dlog("Attaching AV handlers")
        self.send_command("attach_av_handler")

    def attach_bp_handler(self):
        self.dlog("Attaching BP handlers")
        self.send_command("attach_bp_handler")

    def attach_ss_handler(self):
        self.dlog("Attaching SS handlers")
        self.send_command("attach_ss_handler")

    def attach_markers(self):
        self.dlog("Attaching markers")
        self.send_command("attach_markers")

    def attach_st_markers(self):
        self.dlog("Attaching ST markers")
        self.send_command("attach_st_markers")

    def attach_end_markers(self):
        self.dlog("Attaching END markers")
        self.send_command("attach_end_markers")

    def attach_react_markers(self):
        self.dlog("Attaching REACT markers")
        self.send_command("attach_react_markers")
        
    def attach_rd_markers(self):
        self.dlog("Attaching RD markers")
        self.send_command("attach_rd_markers")
        
    def detach_all_markers(self):
        self.detach_markers()
        self.detach_st_markers()
        self.detach_end_markers()
        self.detach_react_markers()
        self.detach_rd_markers()

    def detach_markers(self):
        self.dlog("Detaching markers")
        self.send_command("detach_markers")

    def detach_st_markers(self):
        self.dlog("Detaching ST markers")
        self.send_command("detach_st_markers")

    def detach_end_markers(self):
        self.dlog("Detaching END markers")
        self.send_command("detach_end_markers")

    def detach_react_markers(self):
        self.dlog("Detaching REACT markers")
        self.send_command("detach_react_markers")

    def detach_rd_markers(self):
        self.dlog("Detaching RD markers")
        self.send_command("detach_rd_markers")

    def track_all_threads(self):
        self.dlog("Tracking all threads")
        self.send_command("track_all_threads")

    def dump_modules(self):
        self.dlog("Dumping modules")
        self.send_command("dump_modules")

    def dump_threads(self):
        self.dlog("Dumping threads")
        self.send_command("dump_threads")

    def start_tracking_all_threads(self):
        self.dlog("Start tracking all threads")
        self.send_command("start_tracking_all_threads")

    def stop_tracking_all_threads(self):
        self.dlog("Stopping tracking all threads")
        self.send_command("stop_tracking_all_threads")

    def set_callback(self, callback):
        self.dlog("Setting callback: %s" % callback)
        self.send_command("set_callback %s" % callback)

    def start_log(self, name):
        self.debug = True
        print("Binner starting log")
        if(self.last_log_file != None):
            self.last_log_file.close()
        self.last_log_file = open("%s-binner.txt" % name, "w")
        self.send_command("start_log %s" % name)

    def log_write(self, text):
        self.send_command("log_write %s" % text)

    def stop_log(self):
        self.dlog("Stopping log")
        if(self.last_log_file != None):
            self.last_log_file.close()
            self.last_log_file = None
        self.debug = False

        self.send_command("stop_log")

    def get_synopsis(self):
        self.dlog("Retrieveing synopsis")
        self.last_crashed
        self.write_debugger(self.last_crashed, "get_synopsis")
        data = self.read_debugger(self.last_crashed)
        while(data.find("CONTEXT DUMP") == -1):
            data = self.read_debugger(self.last_crashed)
        print(data)
        self.writePipe(data)
        self.ok()
