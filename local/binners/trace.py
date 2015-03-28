from functions import decode_op1
from threading import Event
from pydbg.defines import *
import time
import utils
import serial

def handle_trace_start(dbg):
    dbg.trace.dlog("Reached trace start")
    dbg.trace.ph2.write("Tracing started\n")
    dbg.trace.ph2.write("--cut-here--\n")

    # handle trace end
#    dbg.trace.trace_end_addr = dbg.trace.trace_addr + dbg.trace.current_instr.length
#    dbg.trace.install_trace_end_bp()
    dbg.signal_ws()
    # dive
    dbg.trace.running = True
    ea = dbg.get_register("EIP")
    dbg.trace.current_ea = ea
    dbg.trace.current_dis = dbg.disasm(ea)
    dbg.trace.current_instr = dbg.get_instruction(ea)

    print("--cut here--")
    if(dbg.trace.running):
        dbg.single_step(True)
    return DBG_CONTINUE

def handle_crash(dbg):
    dbg.trace.ph2.write("--cut-here--\n")
    dbg.trace.ph2.write("Interrupted due to crash\n")
    print("--cut here--")
    dbg.trace.dlog("Walk interrupted due to crash")
    print("[*] Walk interrupted due to crash")
    dbg.single_step(False)
    dbg.trace.running = False

    dbg.trace.uninstall_trace_bp()
    dbg.trace.uninstall_trace_end_bp()
    dbg.signal_cr()
#    dbg.trace.unregister_callbacks()

#    return DBG_CONTINUE

def handle_trace_end(dbg):
    dbg.trace.ph2.write("--cut-here--\n")
    dbg.trace.ph2.write("Interrupted due to finish\n")
    print("--cut here--")
    dbg.trace.dlog("Reached trace end")
    print("current dbg breakpoints:")
    for b in dbg.breakpoints:
        print b
    print("[*] Reached trace end")
    dbg.single_step(False)
    dbg.trace.running = False
    dbg.trace.uninstall_trace_bp()
    dbg.trace.uninstall_trace_end_bp()
    dbg.signal_we()

    dbg.single_step(False)
    return DBG_CONTINUE

def handle_working_bp(dbg):
    ea = dbg.get_register("EIP")
    dbg.trace.current_ea = ea
    dbg.trace.current_dis = dbg.disasm(ea)
#    dbg.trace.current_instr = dbg.get_instruction(ea)
#
##    if(dbg.mnemonic == "int3"):
##        return DBG_CONTINUE
#
#    for addr in dbg.trace.addr_blacklist:
#        if(ea == addr):
#            print("[*] Blacklisted, ignoring")
#            return DBG_CONTINUE
#
## dodaj obslluge listy obserwowanych
#
##        dbg.trace.level -= 1
#
    dbg.trace.ph2.write("%08x %s\n" % (dbg.trace.current_ea, dbg.trace.current_dis))
#
#    if(dbg.mnemonic == "call"):
##        for i in range(0, dbg.trace.level):
##            print(" ", end="")
#        my_module = dbg.addr_to_module(int(ea & 0xffffffff))
#        target_module = dbg.addr_to_module(decode_op1(dbg, dbg.op1))
#
#        if(target_module == None):
#            target_name = "unknown"
#        else:
#            target_name = target_module.szModule
#
#        if(my_module == None):
#            my_name = "unknown"
#        else:
#            my_name = my_module.szModule
#
#        #print(my_name + str(":") + hex(int(ea & 0xffffffff)) + ": " + dbg.trace.current_dis + " (" + target_name + ")")
##        dbg.trace.dive()
##        return DBG_CONTINUE
#
#    if(dbg.trace.running):
#        dbg.single_step(True)
#
#
#    dbg.trace.ph2.write("aaaaaaaaaaaaaaaaaaa\n")
    dbg.single_step(True)

    return DBG_CONTINUE

def handle_ex(dbg, ec):
    ea = dbg.get_register("EIP")
    dbg.trace.current_ea = ea
    dbg.trace.current_dis = dbg.disasm(ea)
    dbg.trace.gf.write(format_empty_node_text("%x: %s generated EXCEPTION %08X" % (ea, dbg.trace.current_dis, ec), color="#ff0000"))
#    dbg.trace.gf.write(format_node_text("%x: %s generated EXCEPTION %08X" % (ea, dbg.trace.current_dis, ec), color="#ff0000"))
#    dbg.trace.gf.write("</node>\n")
    return DBG_EXCEPTION_NOT_HANDLED

class trace():
    def __init__(self, addr, endaddr, app="", imagename="", dbg=None):

        #try:
        if True:
            self.ph2 = serial.Serial(1)
            self.ph2.baudRate = 921600
#            self.ph2.write("test-=OK=-") 

            self.app = app
            self.imagename = imagename
            self.phase = 0
            self.running = True
            if(dbg == None):
                self.dbg = pydbg()
            else:
                self.dbg = dbg
            self.dbg.trace = self
            self.trace_addr = addr
            self.trace_end_addr = endaddr
            self.addr_blacklist = []
            self.module_blacklist = []
            self.current_ea = 0x0
            self.delete_next_bp = False
            self.finished = Event()
    
            self.install_trace_bp()
            self.install_trace_end_bp()
            self.register_callbacks()
            self.running = False
#        except Exception:
#            self.dlog("Error initializing trace")
        self.dlog("Trace successfully initialized")


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
            if(addr == self.trace_addr):
                return
            if(addr == self.trace_end_addr):
                return
            self.dbg.bp_set(addr, "Working bp", handler = handler)
        except Exception as e:
            print(e)
 
    def register_callbacks(self):
        try:
            print("[*] Registering callbacks")
            self.dlog("Registering callbacks")
#            self.dbg.set_callback(EXCEPTION_BREAKPOINT, handle_bp)
            self.dbg.detach_av_handler()
            self.dbg.av_handler = handle_crash
            self.dbg.attach_av_handler()
            self.dbg.set_callback(EXCEPTION_SINGLE_STEP, handle_working_bp)
            self.dbg.default_exc_callback = handle_ex
#            self.dbg.set_callback(, handle_crash)
        except Exception as e:
            print(e)
       
    def unregister_callbacks(self):
        try:
            print("[*] Unregistering callbacks")
            self.dlog("Unregistering callbacks")
#            self.dbg.set_callback(EXCEPTION_BREAKPOINT, handle_empty)
            self.dbg.set_callback(EXCEPTION_SINGLE_STEP, handle_empty)
        except Exception as e:
            print(e)

    def install_trace_bp(self):
        try:
            print("[*] Installing trace handlers")
            self.dlog("Installing trace handlers")
            self.dbg.bp_set(self.trace_addr, handler=handle_trace_start)
        except Exception as e:
            print(e)

    def uninstall_trace_bp(self):
        try:
            print("[*] Deinstalling trace handler")
            self.dlog("Deinstalling trace handler")
            self.dbg.bp_del(self.trace_addr)
        except Exception as e:
            print(e)

    def install_trace_end_bp(self):
        try:
            print("[*] Installing trace end handler")
            self.dlog("Installing trace end handler")
            self.dbg.bp_set(self.trace_end_addr, handler=handle_trace_end)
        except Exception as e:
            print(e)

    def uninstall_trace_end_bp(self):
        try:
            print("[*] Deinstalling trace end handler")
            self.dlog("Deinstalling trace end handler")
            self.dbg.bp_del(self.trace_end_addr)
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
#        print("Inside diving 1, level: %d" % self.level)
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
#                if(graph == True):
                if(True):
                    self.gf.write(format_empty_node(ea, my_module, self.dbg.trace.current_dis, target_module, "#006699"))
                self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret)
                self.delete_next_bp = True
                self.dbg.single_step(False)
                return
#        print("Inside diving 2, level: %d" % self.level)
        if(self.max_level > 0x0):
            if(self.level > self.max_level): 
#                print("Max level %d reached, not diving" % self.level)
                self.dlog("Max level reached, not diving")
                self.dlog("[d] Until " + hex(self.current_ea + self.current_instr.length))
                if(True):
#                if(graph == True):
                    self.gf.write(format_empty_node(ea, my_module, self.dbg.trace.current_dis, target_module))
                self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret)
                self.delete_next_bp = True
                if(self.running):
                    self.dbg.single_step(False)
                return
        if(self.running):
            self.dbg.single_step(True)
#        print("Inside diving 3, level: %d" % self.level)
        self.level += 1
#        print("Inside diving 4, level: %d" % self.level)
        self.dlog("[d] Diving, level: %d" % self.level)
#        print("[d] Diving, level: %d" % self.level)
        if(True):
#        if(graph == True):
            self.gf.write(format_node(ea, my_module, self.dbg.trace.current_dis, target_module))
        self.install_bp(self.current_ea + self.current_instr.length, handler = handle_ret_surface)

    def surface(self):
        if(self.running):
            self.dbg.single_step(True)
        self.level -= 1
        self.dlog("Surfacing, level: " + hex(self.level))
#        if(graph == True):
        if(True):
            self.gf.write("</node>\n")
        if(self.level < 0x0):
#        if(self.level < self.level_lowest):
#            self.level_lowest = self.level
            print("[!] Level is signed. Sth went wrong")
            self.dlog("[!] Level is signed. Sth went wrong")
#            self.running = False
#            self.kill()

    def detach(self):
        dbg.detach()

    def kill(self):
        print("[*] Killing")
        try:
            self.dbg.terminate_process()
        except Exception as e:
            print(e)

    def dlog(self, data):
        self.dbg.dlog("[TRACE] %s" % data)

def add_default_blacklists(my_trace):
    my_trace.module_blacklist.append("ntdll.dll")
    my_trace.module_blacklist.append("kernel32.dll")
    my_trace.module_blacklist.append("user32.dll")
    my_trace.module_blacklist.append("xpsp2res.dll")
    my_trace.module_blacklist.append("glu32.dll")
    my_trace.module_blacklist.append("advapi32.dll")
    my_trace.module_blacklist.append("comdlg.dll")
    my_trace.module_blacklist.append("crypt32.dll")
    my_trace.module_blacklist.append("dnsapi.dll")
    my_trace.module_blacklist.append("gdi.dll")
    my_trace.module_blacklist.append("lz32.dll")
    my_trace.module_blacklist.append("msctf.dll")
    my_trace.module_blacklist.append("msvcrt.dll")
    my_trace.module_blacklist.append("ole32.dll")
    my_trace.module_blacklist.append("urlmon.dll")
    my_trace.module_blacklist.append("wininet.dll")
    my_trace.module_blacklist.append("ws2_32.dll")
    my_trace.module_blacklist.append("MSVCP90.dll")
    my_trace.module_blacklist.append("MSVCR90.dll")
    my_trace.module_blacklist.append("mfc90u.dll")
    my_trace.module_blacklist.append("shlwapi.dll")
    my_trace.module_blacklist.append("uxtheme.dll")
    my_trace.module_blacklist.append("shimeng.dll")
    my_trace.module_blacklist.append("AcGeneral.dll")
    my_trace.module_blacklist.append("userenv.dll")
    my_trace.module_blacklist.append("winmm.dll")
    my_trace.module_blacklist.append("oleaut32.dll")
    my_trace.module_blacklist.append("comclt32.dll")
    my_trace.module_blacklist.append("msacm32.dll")
    my_trace.module_blacklist.append("version.dll")
    my_trace.module_blacklist.append("msvcrt.dll")
    my_trace.module_blacklist.append("rpcrt.dll")
    my_trace.module_blacklist.append("rpcrt4.dll")
    my_trace.module_blacklist.append("gdi32.dll")
    my_trace.module_blacklist.append("secur32.dll")
    my_trace.module_blacklist.append("user32.dll")
    my_trace.module_blacklist.append("shell32.dll")
    my_trace.module_blacklist.append("mswsock.dll")
    my_trace.module_blacklist.append("ws2_help.dll")
    my_trace.dlog("Default blacklist loaded")
