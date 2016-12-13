from socket import AF_INET, SOCK_STREAM, socket
import threading


#default_header="HTTP/1.1 200 OK\x0d\x0aDate: Wed, 29 Sep 2010 20:19:05 GMT\x0d\x0aServer: Testserver\x0d\x0aConnection: Keep-Alive\x0d\x0aContent-Type: text/html; charset=UTF-8\x0d\x0aContent-Length: %d\x0d\x0a%s\x0d\x0a\x0d\x0a"
#default_header="HTTP/1.1 200 OK\x0d\x0aDate: Wed, 29 Sep 2010 20:19:05 GMT\x0d\x0aServer: Testserver\x0d\x0aConnection: Keep-Alive\x0d\x0aContent-Type: text/html; charset=UTF-8\x0d\x0aContent-Length: 291\x0d\x0a"

default_header = """
HTTP/1.0 301 Moved Permanently
Location: http://www.google.pl/
Content-Type: text/html; charset=UTF-8
Date: Tue, 13 Dec 2016 10:24:50 GMT
Expires: Thu, 12 Jan 2017 10:24:50 GMT
Cache-Control: public, max-age=2592000
Server: gws
Content-Length: 218
X-XSS-Protection: 1; mode=block
X-Frame-Options: SAMEORIGIN

<HTML><HEAD><meta http-equiv="content-type" content="text/html;charset=utf-8">
<TITLE>301 Moved</TITLE></HEAD><BODY>
<H1>301 Moved</H1>
The document has moved
<A HREF="http://www.google.pl/">here</A>.
</BODY></HTML>
\x0d\x0a\x0d\x0a"""

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

        if(self.logf is not None):
            self.logf.write("Accepted connection\n")

        while(self.working):
            self.logf.write("In loop\n")
            working_socket, addr = self.socket.accept()
            request  = working_socket.recv(4096)
            self.logf.write("Received: %s\n" % request)
#            response = self.header+self.next_response+"\x0d\x0a\x0d\x0a"
#            response = self.header % (len(self.header) + len(self.next_response), self.next_response)

#            response_body = [
#                '<html><body><h1>Hello, world!</h1>',
#                '<p>This page is in location (request_uri)r, was requested ',
#                'using (request_method)r, and with (request_proto)r.</p>',
#                '<p>Request body is (request_body)r</p>',
#                '<p>Actual set of headers received:</p>',
#                '<ul>',
#            ]
#
#            response_body.append('</ul></body></html>')
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
            self.logf.write("Responded: %s\n")

