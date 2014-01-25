from functions import decode_op1
from threading import Event

def format_empty_node(ea, my_mod, dis, tmod, color = None):
    if(tmod != None):
        target_name = tmod.szModule
    else:
        target_name = "unknown"

    if(my_mod != None):
        my_name = my_mod.szModule
    else:
        my_name = "unknown"


    if(color == None):
        return "<node TEXT=\"" + my_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + target_name + ")" + "\"/>\n"
    else:
        return "<node COLOR=\""+ color +"\" TEXT=\"" + my_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + target_name + ")" + "\"/>\n"
        
def format_node(ea, my_mod, dis, tmod, color = None):
    if(tmod != None):
        target_name = tmod.szModule
    else:
        target_name = "unknown"

    if(my_mod != None):
        my_name = my_mod.szModule
    else:
        my_name = "unknown"

    if(color == None):
        return "<node FOLDED=\"true\" TEXT=\"" + my_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + target_name + ")" + "\">\n"
    else:
        return "<node FOLDED=\"true\" COLOR=\""+ color +"\" TEXT=\"" + my_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + target_name + ")" + "\">\n"

def format_empty_node_text(text, color = None):
    if(color == None):
        return "<node TEXT=\"" + text + "\"/>\n"
    else:
        return "<node COLOR=\""+ color +"\" TEXT=\"" + text + "\"/>\n"

def format_node(ea, my_mod, dis, tmod, color = None):
    if(tmod != None):
        target_name = tmod.szModule
    else:
        target_name = "unknown"

    if(my_mod != None):
        my_name = my_mod.szModule
    else:
        my_name = "unknown"

    if(color == None):
        return "<node FOLDED=\"true\" TEXT=\"" + my_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + target_name + ")" + "\">\n"
    else:
        return "<node FOLDED=\"true\" COLOR=\""+ color +"\" TEXT=\"" + my_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + dis + " (" + target_name + ")" + "\">\n"

def format_empty_node_text(text, color = None):
    if(color == None):
        return "<node TEXT=\"" + text + "\"/>\n"
    else:
        return "<node COLOR=\""+ color +"\" TEXT=\"" + text + "\"/>\n"

def handle_ret(dbg):
    ea = dbg.get_register("EIP")
    dbg.bp_del(ea)
    dbg.single_step(True)
    return DBG_CONTINUE

def handle_ret_surface(dbg):
    ea = dbg.get_register("EIP")
    dbg.bp_del(ea)
    dbg.walk.surface()
    dbg.single_step(True)
    return DBG_CONTINUE
    
def handle_walk_start(dbg):
    dbg.walk.dlog("[*] Reached walk start")

    # handle walk end
#    dbg.walk.walk_end_addr = dbg.walk.walk_addr + dbg.walk.current_instr.length
#    dbg.walk.install_walk_end_bp()
    dbg.walk.level -= 1
    # dive
    dbg.signal_ws()
    dbg.walk.dive()
    print("--cut here--")

def handle_walk_end(dbg):
    print("--cut here--")
    self.dlog("[*] Reached walk end")
    print("[*] Reached walk end")
    dbg.single_step(False)
    dbg.signal_we()
    dbg.walk.unregister_callbacks()
    dbg.walk.running = False
#    dbg.walk.kill()
    return DBG_CONTINUE

def handle_working_bp(dbg):
    ea = dbg.get_register("EIP")
    dbg.walk.current_ea = ea
    dbg.walk.current_dis = dbg.disasm(ea)
    dbg.walk.current_instr = dbg.get_instruction(dbg.get_register("EIP"))

    if(dbg.mnemonic == "int3"):
        return DBG_CONTINUE

    for addr in dbg.walk.addr_blacklist:
        if(ea == addr):
            print("[*] Blacklisted, ignoring")
            return DBG_CONTINUE

        dbg.walk.level -= 1

    if(dbg.mnemonic == "call"):
#        for i in range(0, dbg.walk.level):
#            print(" ", end="")

        my_module = my_walk.dbg.addr_to_module(int(ea & 0xffffffff))
        target_module = my_walk.dbg.addr_to_module(decode_op1(my_walk.dbg, my_walk.dbg.op1))

        if(target_module == None):
            target_name = "unknown"
        else:
            target_name = target_module.szModule

        if(my_module == None):
            my_name = "unknown"
        else:
            my_name = my_module.szModule

        print(my_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + dbg.walk.current_dis + " (" + target_name + ")")
        dbg.walk.dive()
        return DBG_CONTINUE

    dbg.single_step(True)

    return DBG_CONTINUE

class walk():
    def __init__(self, addr, endaddr, app="", imagename="", filee="", max_level=3, dbg=None, gf_path=""):
        self.app = app
        self.imagename = imagename
        self.filee = filee
        self.level = 0
        self.max_level = max_level
        self.phase = 0
        self.running = True
        if(dbg == None):
            self.dbg = pydbg()
        else:
            self.dbg = dbg
        self.dbg.walk = self
        self.walk_addr = addr
        self.walk_end_addr = 0x0
        self.addr_blacklist = []
        self.module_blacklist = []
        self.current_ea = 0x0
        self.delete_next_bp = False
        self.finished = Event()
        self.gf = open(gf_path, "a", 0)

    def attach(self):
        for (pid, name) in self.dbg.enumerate_processes():
            if imagename in name:
                try:
                    print("[*] Attaching to " + str(pid))
                    self.dbg.attach(pid)
                except:
                    print("[!] Problem attaching to " + str(pid))
                    windows_kill(pid)
                    raise AttachFail
        self.pid = pid

    def spawn(self):
        #spawn app & wait to load
        self.proc = subprocess.Popen(self.app)
        time.sleep(3)

    def load_file(self):
        #load file
        try:
            print("[*] Loading file")
            thread = Thread(target = file_run, args = (self.filee, self.dbg, ))
            thread.start()
            return self.dbg
        except Exception as e:
            print(e)

    def install_bp(self, addr, handler = handle_working_bp):
        try:
            self.dlog("[d] Installing bp at: " + hex(addr))
            if(addr == self.walk_addr):
                return
            if(addr == self.walk_end_addr):
                return
            self.dbg.bp_set(addr, "Working bp", handler = handler)
        except Exception as e:
            print(e)
 
    def register_callbacks(self):
        try:
            print("[*] Registering callbacks")
#            self.dbg.set_callback(EXCEPTION_BREAKPOINT, handle_bp)
            self.dbg.set_callback(EXCEPTION_SINGLE_STEP, handle_ss)
        except Exception as e:
            print(e)
       
    def unregister_callbacks(self):
        try:
            print("[*] Unregistering callbacks")
#            self.dbg.set_callback(EXCEPTION_BREAKPOINT, handle_empty)
            self.dbg.set_callback(EXCEPTION_SINGLE_STEP, handle_empty)
        except Exception as e:
            print(e)

    def install_walk_bp(self):
        try:
            print("[*] Installing walk handlers")
            self.dbg.bp_set(self.walk_addr, handle_walk_start, "Walked function")
        except Exception as e:
            print(e)

    def install_walk_end_bp(self):
        try:
            print("[*] Installing walk end handler")
            self.dbg.bp_set(self.walk_end_addr, handle_walk_end, "Walked function finished")
        except Exception as e:
            print(e)

    def run(self):
        try:
            print("[*] Running")
#            thread = Thread(target = self.dbg.run)
#            thread.start()
            self.dbg.run()
        except Exception as e:
            print(e)

    def dive(self):
        ea = self.current_ea
        my_module = self.dbg.addr_to_module(int(ea & 0xffffffff))
        target_module = self.dbg.addr_to_module(decode_op1(self.dbg, self.dbg.op1))

        if(target_module != None):
            target_module_name = target_module.szModule
        else:
            target_module_name = "unknown"

        self.dlog("[d] Target module: " + target_module_name)
                
        for mod in self.module_blacklist:
            if(mod.upper() == target_module_name.upper()):
                self.dlog("[d] Target module blacklisted, not diving")
                self.dlog("[d] Until " + hex(self.current_ea + self.current_instr.length))
                if(graph == True):
                    self.gf.write(format_empty_node(ea, my_module, self.dbg.walk.current_dis, target_module, "#006699"))
                self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret)
                self.delete_next_bp = True
                self.dbg.single_step(False)
                return
        if(self.max_level > 0x0):
            if(self.level > self.max_level): 
                self.dlog("Max level reached, not diving")
                self.dlog("[d] Until " + hex(self.current_ea + self.current_instr.length))
                if(graph == True):
                    self.gf.write(format_empty_node(ea, my_module, self.dbg.walk.current_dis, target_module))
                self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret)
                self.delete_next_bp = True
                self.dbg.single_step(False)
                return
        self.dbg.single_step(True)
        self.level += 1
        self.dlog("[d] Diving, level: " + hex(self.level))
        if(graph == True):
            self.gf.write(format_node(ea, my_module, self.dbg.walk.current_dis, target_module))
        self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret_surface)

    def surface(self):
        self.dbg.single_step(True)
        self.level -= 1
        self.dlog("Surfacing, level: " + hex(self.level))
        if(graph == True):
            self.gf.write("</node>\n")
        if(self.level < 0x0):
            print("[!] Level is signed. Sth went wrong")
            self.running = False
            self.kill()

    def detach(self):
        dbg.detach()

    def kill(self):
        print("[*] Killing")
        try:
            self.dbg.terminate_process()
        except Exception as e:
            print(e)

    def dlog(self, data):
        self.dbg.dlog("[WALK] %s" % data)

def add_default_blacklists(my_walk):
    my_walk.module_blacklist.append("ntdll.dll")
    my_walk.module_blacklist.append("kernel32.dll")
    my_walk.module_blacklist.append("user32.dll")
    my_walk.module_blacklist.append("xpsp2res.dll")
    my_walk.module_blacklist.append("glu32.dll")
    my_walk.module_blacklist.append("advapi32.dll")
    my_walk.module_blacklist.append("comdlg.dll")
    my_walk.module_blacklist.append("crypt32.dll")
    my_walk.module_blacklist.append("dnsapi.dll")
    my_walk.module_blacklist.append("gdi.dll")
    my_walk.module_blacklist.append("lz32.dll")
    my_walk.module_blacklist.append("msctf.dll")
    my_walk.module_blacklist.append("msvcrt.dll")
    my_walk.module_blacklist.append("ole32.dll")
    my_walk.module_blacklist.append("urlmon.dll")
    my_walk.module_blacklist.append("wininet.dll")
    my_walk.module_blacklist.append("ws2_32.dll")
    my_walk.module_blacklist.append("MSVCP90.dll")
    my_walk.module_blacklist.append("MSVCR90.dll")
    my_walk.module_blacklist.append("mfc90u.dll")
    my_walk.module_blacklist.append("shlwapi.dll")
    my_walk.module_blacklist.append("uxtheme.dll")
    my_walk.module_blacklist.append("shimeng.dll")
    my_walk.module_blacklist.append("AcGeneral.dll")
    my_walk.module_blacklist.append("userenv.dll")
    my_walk.module_blacklist.append("winmm.dll")
    my_walk.module_blacklist.append("oleaut32.dll")
    my_walk.module_blacklist.append("comclt32.dll")
    my_walk.module_blacklist.append("msacm32.dll")
    my_walk.module_blacklist.append("version.dll")
    my_walk.module_blacklist.append("msvcrt.dll")
    my_walk.module_blacklist.append("rpcrt.dll")
    my_walk.module_blacklist.append("rpcrt4.dll")
    my_walk.module_blacklist.append("gdi32.dll")
    my_walk.module_blacklist.append("secur32.dll")
    my_walk.module_blacklist.append("user32.dll")
    my_walk.module_blacklist.append("shell32.dll")
    my_walk.module_blacklist.append("mswsock.dll")
    my_walk.module_blacklist.append("ws2_help.dll")
    my_walk.dlog("Default blacklist loaded")
