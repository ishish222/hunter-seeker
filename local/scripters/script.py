import time
import importlib
#from generic import slowdown

def runscriptq(sc, p, slowdown=1):
    scriptmod = importlib.import_module("scripts."+sc)
    script = scriptmod.script
    script.run(p, slowdown)

def runscript(sc, p, slowdown=1):
    print("[Executing: " + sc + "]")
    scriptmod = importlib.import_module("scripts."+sc)
    script = scriptmod.script
    print("[ETA: " + script.eta_str + "]")
    script.run(p, slowdown)
    print("[Executing: " + sc + " finished]")

def runscripts(sclist, p, slowdown=1):
    for sc in sclist:
        runscript(sc, p, slowdown)

def rs(a, p, sl=1):
    runscript(a, p, sl)

def rss(a, p, sl=1):
    runscripts(a, p, sl)

class ScriptException(Exception):
    pass

def read_monitor(pipe):
    data = ''
    while(True):
        data += pipe.stdout.read(1)
        if(data[-6:] == "(qemu)"): 
            pipe.stdout.flush()
            break

def write_monitor(pipe, data):
    if(pipe == None):
        print("Monitor not ready")
        return
#    print("m> " + str(data[:-1]))
    pipe.stdin.write(data + "\n")
    read_monitor(pipe)
    read_monitor(pipe)

class Script:
    def __init__(self):
        self.prev_list = list()
        self.steps = list()
        self.schedule = list()
        self.eta_str = "unknown"
        self.slowdown = 1

    def delay(self, to):
        eto = float(to) * float(self.slowdown)
        print("sleeping " + str(eto) + " seconds")
        time.sleep(eto)

    def reg_script(self, script):
        self.prev_list += script

    def run(self, pipe, slowdown=1):
        self.slowdown = slowdown
        for script in self.prev_list:
            script.run(pipe)
        self.run_self(pipe)

    def run_self(self, pipe):
        for (steps_idx, interval, timeout) in self.schedule:
            for k in self.steps[steps_idx]:
                if(k[0] == "_"):
                #internal command

                    if(k[1:].find("sleep") == 0):
                    #sleep request
                        secs = int(k[7:], 10)
                        self.delay(secs)
                        continue

                    if(k[1:].find("load") == 0):
                        tag = k[6:]
                        print("loading state with tag: " + tag)
                        write_monitor(pipe, "loadvm " + tag + "\n")
                        continue

                    if(k[1:].find("save") == 0):
                        tag = k[6:]
                        print("saving state with tag: " + tag)
                        write_monitor(pipe, "savevm " + tag + "\n")
                        continue

                    if(k[1:].find("delete") == 0):
                        tag = k[8:]
                        print("deleting state with tag: " + tag)
                        write_monitor(pipe, "delvm " + tag + "\n")
                        continue

                    if(k[1:].find("screendump") == 0):
                        sd = k[12:]
                        print("screendump to file " + sd)
                        write_monitor(pipe, "screendump " + sd + "\n")
                        continue

                    if(k[1:].find("comment") == 0):
                        comment = k[9:]
                        print("[" + comment + "]")
                        continue

                    if(k[1:].find("quit") == 0):
                        write_monitor(pipe, "quit\n")
                        continue

                if(k == "*"):
                    k = "asterisk"
                if(k == "_"):
                    k = "shift-minus"
                if(k == "-"):
                    k = "minus"
                if(k == " "):
                    k = "spc"
                if(k == "\\"):
                    k = "backslash"
                if(k == "/"):
                    k = "slash"
                if(k == "."):
                    k = "dot"
#                print("sendkey " + k)
                write_monitor(pipe, "sendkey " + k + "\n")
                time.sleep(interval)
            time.sleep(timeout)


