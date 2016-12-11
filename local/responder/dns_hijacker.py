server_ip="12.154.254.33"
client_ip="10.154.254.15"
server_mac="00:0B:CD:AE:9F:C6"
client_mac="00:02:a5:ea:54:20"
subnet_mask="255.255.255.192"
gateway="12.154.254.10"

import scapy
from scapy.all import DNS, DNSQR, DNSRR, Ether, IP, UDP
from scapy.sendrecv import sniff, sendp

def detect_dns(pkt):
    if(DNS in pkt):
        print '---'
        #pkt.show()
        try:
            dns = pkt.getlayer(DNS)
            assert dns.opcode == 0
#            assert dnsqtypes[dns[DNSQR].qtype] == 'A'
            query = dns[DNSQR].qname.decode('ascii')  # test.1.2.3.4.example.com.
            if(query == 'test.12.34.56.78.example.com.'):
                pkt.show()
                
            head, domain, tld, tail = query.rsplit('.', 3)
            assert domain == 'example' and tld == 'com' and tail == ''
            head = head.split('.', 1)[-1]  # drop leading "prefix." part

            response = DNS(id=dns.id, ancount=1, qr=1, an=DNSRR(rrname=str(query), type='A', rdata=str('127.0.0.1'), ttl=1234))

            print 'response'
            print response

            e_src = pkt.getlayer(Ether).src
            e_dst = pkt.getlayer(Ether).dst

            print 'response3'
            i_src = pkt[IP].src
            i_dst = pkt[IP].dst

            print 'response4'

            u_src = pkt[UDP].sport
            u_dst = pkt[UDP].dport

            print 'RESPONSE'
            p = Ether(src = e_dst, dst = e_src) / IP(src = i_dst, dst = i_src, id=pkt[IP].id+1)  / UDP(sport = u_dst, dport = u_src) / response
            p.show()
            send(p)
            print 'SUCCESS'

        except Exception as e:
            pass
            #print e

def start():
    sniff(filter = "udp and port 53", prn = detect_dns, store = 0) 
