server_ip="12.154.254.33"
client_ip="10.154.254.15"
server_mac="00:0B:CD:AE:9F:C6"
client_mac="00:02:a5:ea:54:20"
subnet_mask="255.255.255.192"
gateway="12.154.254.10"

import scapy
from scapy.all import DHCP
from scapy.sendrecv import sniff

def detect_dhcp(pkt):
    if((DHCP in pkt) and (pkt[DHCP].options[0][1] == 1)):
        print "DHCP discover detected\n"
        sendp(Ether(src = server_mac, dst = "ff:ff:ff:ff:ff:ff") / 
            IP(src = server_ip, dst = "255.255.255.255") / 
            UDP(sport = 67, dport = 68) / 
            BOOTP(op = 2, yiaddr = client_ip, siaddr = server_ip, giaddr = gateway, chaddr = client_mac, xid = pkt[BOOTP].xid) / 
            DHCP(options = [('message-type', 'offer')]) / 
            DHCP(options = [('subnet-mask', subnet_mask)]) / 
            DHCP(options = [('server_id', server_ip), ('end')]))

    if((DHCP in pkt) and (pkt[DHCP].options[0][1] == 3)):
        print "DHCP request detected\n"
        sendp(Ether(src = server_mac, dst = "ff:ff:ff:ff:ff:ff") / 
            IP(src = server_ip, dst = "255.255.255.255") / 
            UDP(sport = 67, dport = 68) / 
            BOOTP(op = 2, yiaddr = client_ip, siaddr = server_ip, giaddr = gateway, chaddr = client_mac, xid = pkt[BOOTP].xid) / 
            DHCP(options = [('message-type', 'ack')]) / 
            DHCP(options = [('subnet-mask', subnet_mask)]) / 
            DHCP(options = [('server_id', server_ip), ('end')]))

def start():
    sniff(filter = "arp or (udp and (port 67 or 68))", prn = detect_dhcp, store = 0) 
