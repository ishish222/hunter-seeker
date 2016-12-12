from socket import AF_INET, SOCK_STREAM, socket
import threading


default_header="HTTP/1.1 200 OK\x0d\x0aDate: Wed, 29 Sep 2010 20:19:05 GMT\x0d\x0aServer: Testserver\x0d\x0aConnection: Keep-Alive\x0d\x0aContent-Type: text/html; charset=UTF-8\x0d\x0aContent-Length: 291\x0d\x0a\x0d\x0a"


class simple_responder(object):
    def __init__(self, port, logf=None):
        self.port = port
        self.socket = socket(AF_INET, SOCK_STREAM)
        self.socket.bind(('127.0.0.1', port))
        self.header = default_header
        self.next_response = ""        
        self.thread = threading.Thread(target = self.worker)
        self.working = True
        self.thread.start()
        self.logf = logf

    def load_next_response(self, next_response):
        self.next_response = next_response

    def load_header(self, header):
        self.header = header

    def close(self):
        self.working = False

    def worker(self):
        self.socket.listen(5)
        working_socket, addr = self.socket.accept()

        if(self.logf is not None):
            self.logf.write("Accepted connection")

        while(self.working):
            self.logf.write("In loop")
            request  = working_socket.recv(4096)
            self.logf.write("Received: %s" % request)
            response = self.header+self.next_response+"\x0d\x0a\x0d\x0a"
            working_socket.send(response)
            self.logf.write("Responded: %s" % response)

