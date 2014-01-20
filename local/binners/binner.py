import sys

sys.path.append("e:\\server\\paimei")
sys.path.append("e:\\common")

from Queue import PriorityQueue
import settings
from threading import Event
from multiprocessing import Lock, Process, Pipe
from subprocess import Popen, PIPE
from pydbg import *
from pydbg.defines import *
from functions import *
import utils
import win32pipe, win32file
import time
from select import select
import socket
import os
from shutil import copyfile

statusPri = {'SR' : 1, 'SL' : 1, 'CR' : 0, 'TO' : 2, 'MA' : 2, 'RD' : 2, 'ST' : 2}
end = "=[OK]="

### functions
# unable to move cause settings module is not visible

def testdir(x): 
    if(os.path.isdir(x) == False):
        os.mkdir(x)

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
        self.debuggers_free = {}                 # dictionary of debuggers for processes
        self.dbg_free_count = 0
        self.sockets = {}
        self.init_dbg = pydbg()
        self.crash_bin = utils.crash_binning.crash_binning()
        self.active = False
        self.sockets_crashed = []
        self.race_lock = Lock()
        self.debug = False
        self.last_log_file = None

        self.dbg_event = Event()
        self.dbg_output = None
        self.dbg_line = ""
        self.last_crashed = None
        self.last_answer = ""
       
        self.status = PriorityQueue()
        self.reqScript = ""
        self.main_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.main_socket.bind(("127.0.0.1", 12347))
        self.main_socket.listen(3)
        self.last_data = ""
        self.ea = None
        self.used_pids = []
        self.prev_status = ""
        self.last_file = ""

        if(defined("settings.log_level") == True):
            self.log_level = settings.log_level
        else:
            self.log_level = 0

        if(defined("settings.debug") == True):
            if(settings.debug == True):
                self.debug = True
                self.last_log_file = open("e:\\logs\\init_log.txt", "a", 0)
        else:
            self.debug = False
            self.last_log_file = None

    def dlog(self, data, level=0):
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
        self.dlog("Writing to pipe: %s" % data, 3)
        self.ph.write(data)

    def readPipe(self):
        data = ""
        while True:
            data += self.ph.read(1)
            
            if(data[-6:] == "-=OK=-"): 
                break
        return data[:-6]

    def ok(self):
        time.sleep(0.1)
        self.writePipe("-=OK=-")

    def send_command(self, cmd):
        self.dlog("Sending: %s" % cmd, 3)
        for pid in self.debuggers:
            if(self.sockets[str(pid)] in self.sockets_crashed):
                self.dlog("Skipping...")
                continue
            try:
                self.write_debugger(self.sockets[str(pid)], cmd)
            except Exception:
                print("Failed to send data to debugger")
#                self.dlog("Failed to send data to debugger")
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
            self.read_debugger(dbg)

    def read_debugger(self, dbg_socket):
        self.last_answer = ""
        data = ""
        while True:
            r, _, _ = select((dbg_socket, ), [], [], 0)
            if(r == []): break
            data = dbg_socket.recv(2)
            self.last_answer += data
            if(data == "SA"):
                status = dbg_socket.recv(2)
                self.last_answer += status
                self.status.put((statusPri[status], status))
                if(status == "CR"): 
                    self.last_crashed = dbg_socket
            if(data == "SR"):
                script_code = dbg_socket.recv(2)
                self.last_answer += script_code
                self.reqScript = settings.script_codes[script_code]
                status = "SR"
                self.status.put((statusPri[status], status, self.reqScript))
            if(data == "SL"):
                long_data = ""
                script_code = dbg_socket.recv(2)
                self.last_answer += script_code
                self.reqScript = settings.script_codes[script_code]
                status = "SL"
                while True:
                    long_data += dbg_socket.recv(1)
                    if(long_data[-6:] == end): 
                        self.last_data = long_data[:-6]
                        self.last_answer += self.last_data
                        break
                self.reqScript += long_data
                self.status.put((statusPri[status], status, self.reqScript))
            if(data == "LO"):
                self.dlog("In receiving LO")
                print("here")
                long_data = ""
                while True:
                    long_data += dbg_socket.recv(1)
                    if(long_data[-6:] == end): 
                        self.last_data = long_data[:-6]
                        self.last_answer += self.last_data
                        break

    def write_debugger(self, dbg_socket, data):
        self.dlog("Writing: %s" % data, 2)
        try:
            dbg_socket.send(data) #two chars
        except IOError:
            return
        return

    def loop_debuggers(self, to = None, invocation_args = None):
        self.loop_debuggers_iteration(to, invocation_args)

        # answers are good, but we're waiting for events
#        while(self.status.qsize() == 0):
#            self.loop_debuggers_iteration(to)

    def race1(self, event):
        self.poll_debuggers()
        self.race_lock.acquire()
        if(not event.is_set()): 
            event.set()
        else:
            self.race_lock.release()

    def race2(self, to, event):
        time.sleep(to)
        self.race_lock.acquire()
        if(not event.is_set()): 
            self.status.put((1, "TO"))
            self.dlog("Received: TO", 1)
            event.set()
        else:
            self.race_lock.release()

    # start and break on event
    def loop_debuggers_iteration(self, to = None, invocation_args = None):
        if(to == None):
            # will wait for signal
            self.start_debuggers("Loop interation")
            if(invocation_args != None):
                self.dlog("Invoking: %s" % invocation_args)
                settings.runner(invocation_args)
            self.poll_debuggers()
            self.stop_debuggers("Detected readiness")
        else:
            event = Event()
            event.clear()
            # will wait for timeout
            self.start_debuggers("Loop interation")
            if(invocation_args != None):
                self.dlog("Invoking: %s" % invocation_args)
                settings.runner(invocation_args)
            # Create race
            T1 = Thread(target=self.race1, args=(event,))
            T2 = Thread(target=self.race2, args=(to,event))
            T1.start()
            T2.start()
            event.wait()
            self.race_lock.release()
            self.stop_debuggers("Race finished")

    # start, collect events, but ignore them 
    def stop_debuggers(self, reason="unknown"):
        self.send_command("S3")
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
            self.send_command("S2%f%s" % (to, end))
        else:
            self.send_command("S1")

    def list_tebs(self):
        for pid in self.debuggers:
            dlog("Listing TEBs for %s" % pid)
            yield self.debuggers[pid].list_tebs()

    def detach_all(self):
        self.send_command("Q1")
        while(len(self.debuggers) > 0):
            dbg = self.debuggers.popitem()
            self.debuggers_free[dbg[0]] = dbg[1]
            self.dbg_free_count += 1

    def clear_status_queue(self):
        for i in range(0, self.status.qsize()):
            self.status.get()

    def terminate_processes(self):
        self.send_command("PT")
        cmd = "terminate"
        self.dlog("Sending: %s" % cmd, 3)
        for pid in self.debuggers:
            try:
                self.write_debugger(self.sockets[str(pid)], cmd)
            except Exception:
                print("Failed to send data to debugger")
#                self.dlog("Failed to send data to debugger")
        self.dlog("Sent: %s" % cmd, 3)
        self.dlog("- - - - - - - - - -", 3)

#        self.detach_all()
#        self.proc.terminate()
        #self.start_debuggers()
#        self.send_command("terminate")
        self.clear_status_queue()
#        for dbg in self.debuggers.values():
#            dbg.terminate()
        # no mem leak?
#        self.debuggers = {}
        #self.proc.terminate()

        #for debugger in self.debuggers.values():
        #    debugger.terminate()

    def spawn(self, path):
        self.proc = Popen(path)

    def attach(self, pid):
        if(pid in self.used_pids):
            return

        if(self.dbg_free_count > 0):
            my_dbg = self.debuggers_free.popitem()
            self.debuggers[my_dbg[0]] = my_dbg[1]
            self.dbg_free_count -= 1
        else:
            print("Spawning dbg")
            pid = str(pid)
            self.debuggers[pid] = Popen([sys.executable, "-u", "e:\\server\\debugger.py"], shell=True)
            self.sockets[pid], addr = self.main_socket.accept()
            self.dlog("Got connection")
            my_dbg = (pid, self.debuggers[pid])

#        self.ddlog(self.read_debugger(self.sockets[str(pid)]))
#        self.ddlog(self.read_debugger(self.sockets[str(pid)]))

        print("Sending attach")
        self.write_debugger(self.sockets[my_dbg[0]], "AT%s%s" % (pid, end))
        print("Sending read config")
        self.write_debugger(self.sockets[my_dbg[0]], "RC")

        self.used_pids.append(pid)

    def enumerate_processes(self):
        return self.init_dbg.enumerate_processes()

    def enumerate_processes_custom(self):
        return self.init_dbg.enumerate_processes_custom()

    def attach_all_markers(self):
        self.attach_markers()
        self.attach_st_markers()
        self.attach_end_markers()
        self.attach_react_markers()
        self.attach_rd_markers()

    def attach_av_handler(self):
        self.dlog("Attaching AV handlers")
        self.send_command("A1")

    def attach_bp_handler(self):
        self.dlog("Attaching BP handlers")
        self.send_command("A2")

    def attach_ss_handler(self):
        self.dlog("Attaching SS handlers")
        self.send_command("A3")

    def attach_markers(self):
        self.dlog("Attaching markers")
        self.send_command("A4")

    def attach_st_markers(self):
        self.dlog("Attaching ST markers")
        self.send_command("A5")

    def attach_end_markers(self):
        self.dlog("Attaching END markers")
        self.send_command("A6")

    def attach_react_markers(self):
        self.dlog("Attaching REACT markers")
        self.send_command("A7")
        
    def attach_rd_markers(self):
        self.dlog("Attaching RD markers")
        self.send_command("A8")
        
    def detach_all_markers(self):
        self.detach_markers()
        self.detach_st_markers()
        self.detach_end_markers()
        self.detach_react_markers()
        self.detach_rd_markers()

    def detach_markers(self):
        self.dlog("Detaching markers")
        self.send_command("D4")

    def detach_st_markers(self):
        self.dlog("Detaching ST markers")
        self.send_command("D5")

    def detach_end_markers(self):
        self.dlog("Detaching END markers")
        self.send_command("D6")

    def detach_react_markers(self):
        self.dlog("Detaching REACT markers")
        self.send_command("D7")

    def detach_rd_markers(self):
        self.dlog("Detaching RD markers")
        self.send_command("D8")

    def track_all_threads(self):
        self.dlog("Tracking all threads")
        self.send_command("T1")

    def dump_modules(self):
        self.dlog("Dumping modules")
        self.send_command("DM")

    def dump_threads(self):
        self.dlog("Dumping threads")
        self.send_command("DT")

    def start_tracking_all_threads(self):
        self.dlog("Start tracking all threads")
        self.send_command("T2")

    def stop_tracking_all_threads(self):
        self.dlog("Stopping tracking all threads")
        self.send_command("T3")

    def set_callback(self, callback):
        self.dlog("Setting callback: %s" % callback)
        self.send_command("SC%s%s" % (callback, end))

    def start_log(self, name):
#        self.debug = True
        print("Binner starting log")
        if(self.last_log_file != None):
            self.last_log_file.close()
        self.last_log_file = open("%s-binner.txt" % name, "a", 0)
        self.send_command("LS%s%s" % (name, end))

    def log_write(self, text):
        self.send_command("LW%s%s" % (text, end))

    def stop_log(self):
        self.dlog("Stopping log")
        if(self.last_log_file != None):
            self.last_log_file.close()
            self.last_log_file = None
        self.debug = False

        self.send_command("LQ")

    def get_synopsis(self):
        self.dlog("Retrieveing synopsis")
        self.last_crashed
        self.write_debugger(self.last_crashed, "GS")
        data = self.read_debugger(self.last_crashed)
        while(data.find("CONTEXT") < 0):
            print(data)
            data = self.read_debugger(self.last_crashed)
        self.dlog("Got it")
        self.writePipe(data)
        self.ok()

    def get_ea(self):
        self.write_debugger(self.last_crashed, "EA")
        time.sleep(1)
        self.read_debugger(self.last_crashed)
        self.ea = self.last_data

    def test_bin_dir(self):
        bin_dir = "%s\\%s" % (settings.samples_binned, self.ea)
        testdir(bin_dir)

    def save_synopsis(self, filee):
        fname = filee.split("\\")[-1]
        self.send_command("SS%s\\%s\\%s%s" % (settings.samples_binned, self.ea, fname, end))
        self.ok()

    def save_sample(self, filee):
        fname = filee.split("\\")[-1]
        copyfile(filee, "%s\\%s\\%s" % (settings.samples_binned, self.ea, fname))

    def close_logs(self):
        self.last_log_file.flush()
        self.last_log_file.close()
        self.send_command("CL")

    def start_profiling(self):
        self.send_command("RS")

    def dump_stats(self, fname):
        self.send_command("RD%s%s" % (fname, end))

