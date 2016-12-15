from socket import AF_INET, SOCK_STREAM, socket
import threading
import sys

class simple_responder(object):
    def __init__(self, port, logf=sys.stdout):
        self.port = port
        self.socket = socket(AF_INET, SOCK_STREAM)
        self.socket.bind(('127.0.0.1', port))
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
        

        while(self.working):
            self.logf.write("In loop\n")
            working_socket, addr = self.socket.accept()
            self.logf.write("Accepted connection\n")
            request  = working_socket.recv(4096)
            self.logf.write("Received: %s\n" % request)

            response_body = self.next_response
            response_body_raw = ''.join(response_body)

            # Clearly state that connection will be closed after this response,
            # and specify length of response body
            response_headers = {
                'Content-Type': 'text/html; encoding=utf8',
                'Content-Length': len(response_body_raw),
                'Connection': 'close',
            }

            response_headers_raw = ''.join('%s: %s\n' % (k, v) for k, v in response_headers.iteritems())

            # Reply as HTTP/1.1 server, saying "HTTP OK" (code 200).
            response_proto = 'HTTP/1.1'
            response_status = '200'
            response_status_text = 'OK' # this can be random
    
            response = '%s %s %s' % (response_proto, response_status, response_status_text)
            response += response_headers_raw
            response += '\n'
            response += response_body_raw
            working_socket.send(response)

            working_socket.close()
            self.logf.write("Responded: %s\n" % response)

