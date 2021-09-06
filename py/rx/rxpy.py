import sys
import socket
ETH_P_ALL=0x88DC # not defined in socket module, sadly...
s=socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(ETH_P_ALL))
s.bind(("wlp1s0", 0))
pktnum = 1

#sys.stdout.write("<%s>\n"%repr(r))
while True:
    r=s.recv(2000)
    print("Received packet %i" % pktnum)
    pktnum = pktnum+1
    #print("%s" % r)