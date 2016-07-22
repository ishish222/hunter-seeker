class Tracer(object):
    def __init__(self):
        self.socket = None
        self.addr = None
        self.my_id = 0
        self.active_tid_id = 0
        self.tid_id_count = 0
        self.controller = None
        self.last_answer = ''
        self.last_report = ''

    def dlog(self, cmd, level=0):
        self.controller.dlog('[Tracer 0x%02x]: %s' % (self.my_id, cmd), level)

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

            if(data[-6:] == '-=OK=-'):
                break
        self.last_answer = data[:-6]

    def write_debugger(self, dbg_socket, data):
        self.dlog("Writing: %s" % data, 2)
        try:
            dbg_socket.send(data) #two chars
        except IOError:
            return
        return

    def send_command(self, cmd):
        cmd = cmd + "-=OK=-"

        self.dlog("Sending: %s" % cmd, 3)
        try:
            self.write_debugger(self.socket, cmd)
        except Exception:
            print("Failed to send %s" % cmd)
        self.dlog("Sent: %s" % cmd, 3)

    def recv_report(self):
        try:
            self.read_debugger(self.socket)
        except Exception as inst:
            print("Failed to receive")
            print(type(inst))
            print(inst.args)
            print(inst)
        self.dlog("Read: %s" % self.last_answer, 3)
        return (self.last_report, self.last_answer)

    def read_register(self, reg):
        self.send_command('RR %d %s' % (self.active_tid_id, reg))

    def read_dword(self, args):
        self.send_command('RM %s' % (args))

    def write_dword(self, args):
        self.send_command('WM %s' % (args))
