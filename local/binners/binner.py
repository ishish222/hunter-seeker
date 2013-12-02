import sys

sys.path.append("z:\\server\\paimei")
sys.path.append("z:\\common")

import settings
from threading import Thread, Event
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

### thread routines

def debug_loop_routine(dbg):
    dlog("Entering debugger routine")
    dbg.binner.active = True
    dbg.debugger_active = True
    dbg.debug_event_loop()
    dbg.binner.active = False
    dlog("Leaving debugger routine")

### binner class

class binner(object):
    def __init__(self, ph):
#        self.ph = None
        self.ph = ph
        self.test_lock = Lock()
        self.debuggers = {}                 # dictionary of debuggers for processes
        self.init_dbg = pydbg()
        self.crash_bin = utils.crash_binning.crash_binning()
        self.active = False

        self.dbg_event = Event()
        self.dbg_output = None
        self.dbg_line = ""

#    def __getstate__(self):
#        return (self.test_lock,
#        self.active,
#        self.bl_modules,
#        self.bl_instructions,
#        self.bl_addresses,
#        self.bl_rvas,
#        self.markers,
#        self.st_markers,
#        self.end_markers,
#        self.react_markers,
#        self.marker_handler,
#        self.st_marker_handler,
#        self.end_marker_handler,
#        self.react_marker_handlers,
#        self.samples_dir,
#        self.crashed_dir,
#        self.hanged_dir,
#        self.clean_dir,
#        self.log_file)
#
##        self.debuggers,
##        self.crash_bin,
#    def __setstate__(self, state):
#        (self.test_lock,
#        self.active,
#        self.bl_modules,
#        self.bl_instructions,
#        self.bl_addresses,
#        self.bl_rvas,
#        self.markers,
#        self.st_markers,
#        self.end_markers,
#        self.react_markers,
#        self.marker_handler,
#        self.st_marker_handler,
#        self.end_marker_handler,
#        self.react_marker_handlers,
#        self.samples_dir,
#        self.crashed_dir,
#        self.hanged_dir,
#        self.clean_dir,
#        self.log_file) = state

    def dlog(self, data):
        dlog("[binner] %s" % data)

    def ddlog(self, data):
        dlog("%s" % data)

    def writePipe(self, data):
        win32file.WriteFile(self.ph, data)

    def ok(self):
        time.sleep(0.1)
        self.writePipe("-=OK=-")
        win32file.FlushFileBuffers(self.ph)

    def send_command(self, cmd):
        self.dlog("Sending: %s" % cmd)
        for pid in self.debuggers:
            self.write_debugger(self.debuggers[str(pid)], cmd)
            self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
#            self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
        self.dlog("Sent: %s" % cmd)

    def poll_debugger(self, dbg):
        #self.dbg_line = dbg.stdout.readline()
        self.dbg_line = self.read_debugger(dbg)
        self.dbg_output = dbg
        self.dbg_event.set()

    def poll_debuggers(self):
#        readable = [dbg.stdout for dbg in self.debuggers.values()]
        self.dbg_output = None
        self.dbg_line = ""

#        readable = self.debuggers.values()
        threads = [Thread(target = self.poll_debugger, args=(dbg, )) for dbg in self.debuggers.values()]
        for t in threads:
            t.start()
        self.dbg_event.wait()
        for t in threads:
            t._Thread__stop()
        return self.dbg_output
#        ready = False
#        while not ready:
#            for dbg in readable:
#                ready = dbg.poll(0.5)
#                if(ready == True):
#                    return dbg
#        ready, _, _ = select(readable, [], []) 
#        return ready[0]
#        ready = self.fds.poll()
#        return ready[0][0]
#        pass
                
    def read_debugger(self, dbg):
        data = ""
        while True:
            line = dbg.stdout.readline()
#            print(line)
            if(line[:-1] == "=[OK]="):
                break
            data += line
        return data

    def write_debugger(self, dbg, data):
        return dbg.stdin.write(data + "\n")

    def loop_debuggers(self):
        dlog("Looping debuggers")
        self.start_debuggers()
        cur_dbg = self.poll_debuggers()
        dlog("%d" % cur_dbg.pid)
        self.dlog(self.dbg_line)
        self.stop_debuggers()
#        self.dlog(self.read_debugger(cur_dbg))

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
        self.debuggers[str(pid)] = Popen([sys.executable, "-u", "z:\\server\\debugger.py"], shell=False, stdin=PIPE, stdout=PIPE)
        self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
        self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
        self.dlog("1")

        self.write_debugger(self.debuggers[str(pid)], "attach %s" % pid)
        self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
        self.dlog("2")
        
        self.write_debugger(self.debuggers[str(pid)], "read_config")
        self.ddlog(self.read_debugger(self.debuggers[str(pid)]))
        self.dlog("3")

#        self.fds.register(self.debuggers[str(pid)].stdout)
        print(type(self.debuggers[str(pid)].stdout))

    def enumerate_processes(self):
        return self.init_dbg.enumerate_processes()

    def attach_all_markers(self):
        self.attach_markers()
        self.attach_st_markers()
        self.attach_end_markers()
        self.attach_react_markers()

    def attach_markers(self):
        self.send_command("attach_markers")

    def attach_st_markers(self):
#        for pid in self.debuggers:
#            dlog("Attaching ST markers in %s" % pid)
         dlog("Attaching ST markers")
         self.send_command("attach_st_markers")

    def attach_end_markers(self):
#        for pid in self.debuggers:
#            dlog("Attaching END markers in %s" % pid)
         dlog("Attaching END markers")
         self.send_command("attach_end_markers")

    def attach_react_markers(self):
        for pid in self.debuggers:
            dlog("Attaching REACT markers in %s" % pid)
            self.send_command("attach_react_markers")

    def detach_all_markers(self):
        self.detach_markers()
        self.detach_st_markers()
        self.detach_end_markers()
        self.detach_react_markers()

    def detach_markers(self):
        for pid in self.debuggers:
            dlog("Detaching markers in %s" % pid)
            self.debuggers[pid].send("detach_markers")

    def detach_st_markers(self):
        for pid in self.debuggers:
            dlog("Detaching ST markers in %s" % pid)
            self.debuggers[pid].send("detach_st_markers")

    def detach_end_markers(self):
        for pid in self.debuggers:
            dlog("Detaching END markers in %s" % pid)
            self.debuggers[pid].send("detach_end_markers")

    def detach_react_markers(self):
        for pid in self.debuggers:
            dlog("Detaching REACT markers in %s" % pid)
            self.debuggers[pid].send("detach_react_markers")



