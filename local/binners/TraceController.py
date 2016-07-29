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
from shutil import copyfile, copytree
import os
from mutex_2 import NamedMutex
from Tracer import Tracer

statusPri = {'SR' : 1, 'SL' : 1, 'CR' : 0, 'TO' : 2, 'MA' : 2, 'RD' : 2, 'ST' : 2, 'WS' : 2, 'WE' : 2, 'EX' : 1}
end = "-=OK=-"

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

class TraceController(object):
    def __init__(self, ext_pipe):
        self.start_mutex = NamedMutex("StartMutex", True)
        self.stop_mutex = NamedMutex("StopMutex", True)
        #self.ext_pipe = ext_pipe
        self.test_lock = Lock()
        self.loop_lock = Lock()
        self.loop_lock.acquire()
        self.debuggers = {}                 # dictionary of debuggers for processes
        self.debuggers_free = {}                 # dictionary of debuggers for processes
        self.dbg_free_count = 0
        # new - tracers
        self.tracers = []
        self.trace_count = 0
        self.trace_sockets = []

        # active stuff
        self.tracer_active_id = 0
        self.tracer_active = None

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
        self.last_report = ""
       
        self.status = PriorityQueue()
        self.reqScript = ""
        self.main_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.main_socket.bind(("127.0.0.1", 12341))
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
#            self.last_log_file.write("[%s] %s\n" % (timestamp(), data))
            print("[TracerController]: %s: %s\n" % (timestamp(), data))

    def ddlog(self, data, level=0):
        #pass to regular log facility
        self.dlog("%s" % data, level)

    def writePipe(self, data):
        return
        self.dlog("Writing to pipe: %s" % data, 3)
#        print("Writing to pipe: %s" % data)
        self.ext_pipe.write(data)

    def readPipe(self):
        data = ""
        while True:
            data += self.ext_pipe.read(1)
            
            if(data[-6:] == "-=OK=-"): 
                break
        return data[:-6]

    def ok(self):
        time.sleep(0.1)
        self.writePipe("-=OK=-")

    def send_command_active(self, cmd):
        self.tracer_active.send_command(cmd)

    def recv_report_active(self):
        self.tracer_active.recv_report()
        return (self.tracer_active.last_report, self.tracer_active.last_answer)
        
    def send_command_active2(self, cmd):
        cmd = cmd + "-=OK=-"

        self.dlog("Sending: %s to tracer no: %d" % (cmd, self.tracer_active_id), 3)
        try:
            self.write_debugger(self.tracer_active.socket, cmd)
        except Exception:
            print("Failed to send %s to tracer no: %d" % (cmd, self.tracer_active_id))
        self.dlog("Sent: %s to tracer no: %d" % (cmd, self.tracer_active_id), 3)

    def recv_report_active2(self):
        try:
            self.read_debugger(self.tracer_active.socket)
        except Exception as inst:
            print("Failed to receive from tracer no: %d" % (self.tracer_active_id))
            print(type(inst))
            print(inst.args)
            print(inst)
        self.dlog("Read: %s from tracer no: %d" % (self.last_answer, self.trace_active), 3)
        return self.last_report

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
        self.last_report = ""
        self.last_answer = ""
        data = ""
        part = ""

        data = dbg_socket.recv(2)
        self.last_report += data

        while True:
            part = ''
            part = dbg_socket.recv(1)
            if(part == False): continue
    
            data += part    

            self.dlog(data)
            if(data[-6:] == '-=OK=-'):
                break

        self.last_answer = data[:-6]
        self.dlog(' Received: %s - %s' % (self.last_report, self.last_answer))
            
        '''
        while True:
            r, _, _ = select((dbg_socket, ), [], [], 0)
            if(r == []): break
            data = dbg_socket.recv(2)
            self.last_report += data
            self.last_answer += data
            if(self.last_report == 'BP' or self.last_report == 'RI'):
                long_data = ""
                while True:
                    long_data += dbg_socket.recv(1)
                    if(long_data[-6:] == end): 
                        self.last_data = long_data[:-6]
                        self.last_answer += self.last_data
                        break
                self.reqScript += long_data
                self.status.put((statusPri[status], status, self.reqScript))
        '''

    def read_debugger2(self, dbg_socket):
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
            if(data == "EX"):
                exception_code = dbg_socket.recv(8)
                print(exception_code)
                self.last_answer += exception_code
                status = "EX"
                self.status.put((statusPri[status], status, exception_code))
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
#        self.send_command("S3")
        self.stop_mutex.release()
        self.stop_mutex.acquire()
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
#        print("Releasing mutex")
        self.start_mutex.release()
#        print("Acquiring mutex")
        self.start_mutex.acquire()
        self.loop_lock.release()
        self.dlog("[UNLOCK] Debug section", 2)
        self.dlog("Waiting for debug event")
        self.dlog("Reason: %s" % reason)
        if(to != None):
            self.send_command("S2%f%s" % (to, end))
        else:
            self.send_command("S1")

    '''
    def list_tebs(self):
        for pid in self.debuggers:
            dlog("Listing TEBs for %s" % pid)
            yield self.debuggers[pid].list_tebs()
    '''

    def detach_all(self):
        print("detaching")
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

#TODO: is this necessary?

    def spawn(self, path):
        print("Spawning: %s", path)
        self.proc = Popen(path)

    def close_tracer(self, idd):
        print("Closing tracer")
        self.tracers[idd]

    def spawn_tracer(self):
        print("Spawning tracer")
        Popen(["e:\\server\\b.exe", "127.0.0.1", "12341"], shell=True)
        socket, addr = self.main_socket.accept()
        self.tracers.append(Tracer())
        self.tracer_active_id = self.trace_count
        self.tracer_active = self.tracers[self.tracer_active_id]
        self.tracer_active.socket = socket
        self.tracer_active.addr = addr
        self.tracer_active.active_tid_id = 0
        self.tracer_active.my_id = self.tracer_active_id
        self.tracer_active.controller = self
        self.trace_count += 1
        print("Tracer synced")
        return self.trace_count-1

    def activate_prev_tid(self):
        self.tracer_active.active_tid_id = self.tracer_active.active_tid_id - 1

    def activate_next_tid(self):
        self.tracer_active.active_tid_id = self.tracer_active.active_tid_id + 1

    def activate_prev_tracer(self):
        self.tracer_active_id = (self.trace_active_id + 1) % self.trace_count
        self.tracer_active = self.tracers[self.trace_active_id]

    def activate_next_tracer(self):
        self.tracer_active_id = (self.trace_active_id + 1) % self.trace_count
        self.tracer_active = self.tracers[self.trace_active_id]

# modify
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

#TODO: these should be handled by internal controller
    def enumerate_processes(self):
        return self.init_dbg.enumerate_processes()

    def enumerate_processes_custom(self):
        return self.init_dbg.enumerate_processes_custom()


# markers stuff
    def configure_marker_st(self, mod, addr):
        self.send_command_active("M1 %s %s" % (mod, addr))
        return

    def configure_marker_end(self, mod, addr):
        self.send_command_active("M2 %s %s" % (mod, addr))
        return

    def attach_all_markers(self):
        self.attach_markers()
        self.attach_st_markers()
        self.attach_end_markers()
        self.attach_react_markers()
        self.attach_rd_markers()

    def attach_av_handler(self):
        self.dlog("Attaching AV handlers")
        self.send_command("A1")

    def attach_ex_handler(self):
        self.dlog("Attaching EX handlers")
        self.send_command("A9")

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
        print("Binner starting log: %s-binner.txt" % name)
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

    def take_a_trace(self, args):
        self.send_command("TC%s%s" % (args, end))

    def take_a_trace2(self, pid):
        self.dlog("taking a trace")
        from subprocess import Popen, PIPE
#        st_addr = self.st_markers[0][0]
#        end_addr = self.end_markers[0][0]
        st_addr = 0x4e30ac
        end_addr = 0x7e427f87
   
        self.dlog("e:\\server\\trace.exe %s 0x%x 0x%x" % (pid, st_addr, end_addr))
        Popen("e:\\server\\trace.exe %s 0x%x 0x%x" % (pid, st_addr, end_addr), stdout=PIPE, stderr=PIPE)

    def take_a_walk(self, args):
        self.send_command("WK%s%s" % (args, end))

    def take_a_walk2(self, args):
        self.send_command("WN%s%s" % (args, end))

    def save_synopsis(self, filee):
        fname = filee.split("\\")[-1]
        self.send_command("SS%s%s\\%s%s" % (settings.samples_binned, self.ea, fname, end))
#        self.ok()

    def save_sample(self, filee):
#        fname = filee.split("\\")[-1]
        fname = filee.replace(settings.samples_shared_path, "")
#        if(os.path.isfile(filee)):
#            copyfile(filee, "%s%s\\%s" % (settings.samples_binned, self.ea, fname))
#        else:
#            copytree(filee, "%s%s\\%s" % (settings.samples_binned, self.ea, fname))
        #copyfile(filee, "%s%s\\%s" % (settings.samples_binned, self.ea, fname))
        #copy(filee, "%s%s\\%s" % (settings.samples_binned, self.ea, fname))
        dir1 = filee
        dir2 = "%s%s\\%s" % (settings.samples_binned, self.ea, fname)
        if(dir1[-1:] == "\\"): dir1 = dir1[:-1]
        if(dir2[-1:] == "\\"): dir2 = dir2[:-1]
        #cmd = "xcopy /R /Y /I /E %s %s" % (dir1, dir2)
        cmd = "copy %s %s" % (dir1, dir2)
        print "%s" % cmd
        os.system(cmd)
        #os.system("copy %s %s%s\\%s" % (filee, settings.samples_binned, self.ea, fname))

    def close_logs(self):
        self.last_log_file.flush()
        self.last_log_file.close()
        self.send_command("CL")

    def start_profiling(self):
        self.send_command("RS")

    def dump_stats(self, fname):
        self.send_command("RD%s%s" % (fname, end))

    def set_sample_file(self, filee):
        self.send_command_active("SN %s" % filee)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def configure_in_dir(self, filee):
        self.send_command_active("SD %s" % filee)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def set_sample_pname(self, pname):
        self.send_command_active("SP %s" % pname)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def debug_sample(self):
        self.send_command_active("sd")
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def debug_continue(self):
        self.send_command_active("cn")
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def debug_continue_time(self, time):
        self.send_command_active("cN %s" % time)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def list_tebs(self):
        self.send_command_active("lt")
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def configure_markers(self, markers):
        self.send_command_active("cm %s" % markers)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def activate_markers(self):
        self.send_command_active("am")
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def read_dword(self, args):
        self.tracer_active.read_dword(args)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def write_dword(self, args):
        self.tracer_active.write_dword(args)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def read_register(self, args):
        self.tracer_active.read_register(args)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def write_register(self, args):
        self.tracer_active.write_register(args)
        self.last_report, self.last_answer = self.recv_report_active()
        return 

    def read_stack(self, args):
        self.tracer_active.read_stack(args)
        self.last_report, self.last_answer = self.recv_report_active()
        return 



    def print_sth(self, data):
        self.last_report, self.last_answer = self.recv_report_active()
        return 

