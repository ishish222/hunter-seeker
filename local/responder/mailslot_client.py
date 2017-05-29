import sys
import win32file

class mailslot_client(object):
    def __init__(self, name="", log_fname="e:\\logs\\mailslot_log.txt"):
        self.name = name
        self.handle = 0x0
        self.next_response = ""        
        self.logf = open(log_fname, 'w', 1)
        print "Mailslot client started\n" 
        self.logf.write("Mailslot client started\n")
        self.connect()

    def load_next_response_from_file(self, fname):
        self.logf.write("Loading reponse from file: %s" % fname)
        
        with open(fname, 'rb') as content_file:
            self.next_response = content_file.read()

    def load_next_response(self, next_response):
        self.logf.write("Loading reponse: %s" % next_response)
        if(next_response.find("file:") == 0x0):
            self.load_next_response_from_file(next_response[5:])
        else:
            self.next_response = next_response
        self.send_mail()

    def connect(self):
        ret = win32file.CreateFile(self.name, win32file.GENERIC_WRITE, win32file.FILE_SHARE_READ, None, win32file.OPEN_EXISTING, 0, None)
        if(ret == win32file.INVALID_HANDLE_VALUE):
            print "Mailslot client failed to connect\n" 
            self.logf.write("Mailslot client failed to connect\n")
            return
        self.handle = ret

    def send_mail(self):
        ret = win32file.WriteFile(self.handle, self.next_response); 
        if(ret == 0x0):
            print "Mailslot client failed to send\n" 
            self.logf.write("Mailslot client failed to send\n")
            return
