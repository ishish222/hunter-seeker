import time

class ScriptException(Exception):
    pass

class Script:
    def __init__(self):
        self.prev_list = list()
        self.steps = list()
        self.schedule = list()
        self.eta_str = "unknown"

    def reg_script(self, script):
        self.prev_list += script

    def run(self, pipe):
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
                        print("sleeping " + str(secs) + " seconds")
                        time.sleep(secs)
                        continue

                    if(k[1:].find("load") == 0):
                        tag = k[6:]
                        print("loading state with tag: " + tag)
                        pipe.stdin.write("loadvm " + tag + "\n")

                    if(k[1:].find("save") == 0):
                        tag = k[6:]
                        print("saving state with tag: " + tag)
                        pipe.stdin.write("savevm " + tag + "\n")

                    if(k[1:].find("delete") == 0):
                        tag = k[8:]
                        print("deleting state with tag: " + tag)
                        pipe.stdin.write("delvm " + tag + "\n")

                    if(k[1:].find("screendump") == 0):
                        sd = k[12:]
                        print("screendump to file " + sd)
                        pipe.stdin.write("screendump " + sd + "\n")

                    if(k[1:].find("comment") == 0):
                        comment = k[9:]
                        print("[" + comment + "]")

                    if(k[1:].find("quit") == 0):
                        pipe.stdin.write("quit\n")

                if(k == "\\"):
                    k = "backslash"
                if(k == "/"):
                    k = "slash"
                if(k == "."):
                    k = "dot"
#                print("sendkey " + k)
                pipe.stdin.write("sendkey " + k + "\n")
                time.sleep(interval)
            time.sleep(timeout)


