# Use scapy2.3.1+ from pip (secdev original) or for Python3 use the
# https://github.com/phaethon/scapy Scapy3K version.
#
# Example DNS server that resolves NAME.IPV4.example.com A record
# requests to an A:IPV4 response.
#
# $ dig test.12.34.56.78.example.com -p 1053 @127.0.0.1 +short
# 12.34.56.78

from scapy.all import DNS, DNSQR, DNSRR, dnsqtypes, sr1, IP, UDP, Packet
from socket import AF_INET, SOCK_DGRAM, socket
from traceback import print_exc

sock = socket(AF_INET, SOCK_DGRAM)
sock.bind(('127.0.0.1', 53))

def forwardDNS(orig_pkt):
    response = sr1(IP(dst="8.8.8.8")/UDP() / DNS(rd=1,id=orig_pkt[DNS].id,qd=DNSQR(qname=orig_pkt[DNSQR].qname)),verbose=0)
    text = response[DNS]
    return text

while True:
    request, addr = sock.recvfrom(4096)

    try:
        packet = Packet(request)
        dns = DNS(request)
        assert dns.opcode == 0, dns.opcode  # QUERY
        assert dnsqtypes[dns[DNSQR].qtype] == 'A', dns[DNSQR].qtype
        query = dns[DNSQR].qname.decode('ascii')  # test.1.2.3.4.example.com.
#        head, domain, tld, tail = query.rsplit('.', 3)
        if('korrino.com' in query):
            response = forwardDNS(dns)
            sock.sendto(bytes(response), addr)
        else:
#            head = head.split('.', 1)[-1]  # drop leading "prefix." part    
            response = DNS(
                id=dns.id, ancount=1, qr=1,
                an=DNSRR(rrname=str(query), type='A', rdata=str('127.0.0.1'), ttl=1234))
            print((repr(response)))
            sock.sendto(bytes(response), addr)

    except Exception as e:
        print(e)

