import time
import importlib
from subprocess import call

def runscriptq(sc, p, slowdown=1, args=None):
    scriptmod = importlib.import_module("scripts."+sc)
    script = scriptmod.script
    script.run(p, slowdown, args)

def runscript(sc, p, slowdown=1, args=None):
    print("[Executing: " + sc + "]")
    scriptmod = importlib.import_module("scripts."+sc)
    script = scriptmod.script
    print("[ETA: " + script.eta_str + "]")
    script.run(p, slowdown, args)
    print("[Executing: " + sc + " finished]")

def runscripts(sclist, p, slowdown=1, args=None):
    for sc in sclist:
        runscript(sc, p, slowdown, args)

def rs(a, p, sl=1, args=None):
    runscript(a, p, sl, args)

def rss(a, p, sl=1, args=None):
    runscripts(a, p, sl, args)

class ScriptException(Exception):
    pass

def read_monitor(pipe):
    data = ''
    while(True):
#        data += pipe.stdout.read(1)
        data += pipe.recv(1)
        if(data[-6:] == "(qemu)"): 
            break
    return data[:-6]

def write_monitor_only(pipe, data):
    if(pipe == None):
        print("Monitor not ready")
        return
#    print("m> " + str(data[:-1]))
#    pipe.stdin.write(data + "\n")
    pipe.send(data + "\n")

def write_monitor_2(pipe, data):
    if(pipe == None):
        print("Monitor not ready")
        return
#    pipe.stdin.write(data + "\n")
    pipe.send(data + "\n")
    ret = read_monitor(pipe)
#    print(ret)
    return ret

def write_monitor(pipe, data):
    if(pipe == None):
        print("Monitor not ready")
        return
#    print("m> " + str(data[:-1]))
#    pipe.stdin.write(data + "\n")
    pipe.send(data + "\n")
    ret = read_monitor(pipe)
#    print(ret)
    ret = read_monitor(pipe)
#    print(ret)
    return ret

def translate(k):
    if(k == "*"):
        k = "asterisk"
    if(k == "_"):
        k = "shift-minus"
    if(k == "-"):
        k = "minus"
    if(k == " "):
        k = "spc"
    if(k == "\""):
        k = "shift-apostrophe"
    if(k == "\\"):
        k = "backslash"
    if(k == "/"):
        k = "slash"
    if(k == "."):
        k = "dot"
    if(k == "="):
        k = "equal"
    if(k == ":"):
        k = "shift-semicolon"
    return k

class Script:
    def __init__(self):
        self.prev_list = list()
        self.steps = list()
        self.schedule = list()
        self.eta_str = "unknown"
        self.slowdown = 1

    def delay(self, to):
        eto = float(to) * float(self.slowdown)
        #print("sleeping " + str(eto) + " seconds")
        time.sleep(eto)

    def reg_script(self, script):
        self.prev_list += script

    def run(self, pipe, slowdown=1, args=None):
        self.slowdown = slowdown
        for script in self.prev_list:
            script.run(pipe, args)
        self.run_self(pipe, args)

    def run_self(self, pipe, args=None):
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

                    if(k[1:].find("mousemv") == 0):
                        (dx,dy) = k[9:].split(",")
                        print("moving mouse by {0},{1}".format(dx, dy))
                        write_monitor(pipe, "mouse_move {0} {1}\n".format(dx, dy))
                        continue

                    if(k[1:].find("mousebt") == 0):
                        bt = k[9:]
                        print("Changing mouse button state ({0})".format(bt))
                        write_monitor(pipe, "mouse_button {0}\n".format(bt))
                        continue

                    if(k[1:].find("screendump") == 0):
                        sd = k[12:]
                        print("screendump to file " + sd)
                        write_monitor(pipe, "screendump " + sd + "\n")
                        continue

                    if(k[1:].find("syscmd") == 0):
                        cmd = k[8:]
                        print("[syscmd: %s]" % cmd)
                        call(cmd.split(" "))
                        continue

                    if(k[1:].find("comment") == 0):
                        comment = k[9:]
                        print("[" + comment + "]")
                        continue

                    if(k[1:].find("quit") == 0):
                        write_monitor_only(pipe, "quit\n")
                        continue

                    if(k[1:].find("powerdown") == 0):
                        write_monitor_only(pipe, "system_powerdown\n")
                        continue

                    if(k[1:].find("readline") == 0):
                        read_monitor(pipe)
                        continue

                    if(k[1:].find("arg0") == 0):
                        if(args != None):
                            for l in args[0]:
                                l = translate(l)
                                write_monitor(pipe, "sendkey " + l + "\n")
                        continue

                    if(k[1:].find("arg1") == 0):
                        if(args != None):
                            for l in args[1]:
                                l = translate(l)
                                write_monitor(pipe, "sendkey " + l + "\n")
                        continue

                k = translate(k)
                write_monitor(pipe, "sendkey " + k + "\n")
                time.sleep(interval)
            time.sleep(timeout)


