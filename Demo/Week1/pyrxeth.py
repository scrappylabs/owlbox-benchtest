import sys
import socket
import signal
ETH_P_ALL=0x88DC # not defined in socket module, sadly...
s=socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(ETH_P_ALL))
s1= socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(ETH_P_ALL))
s.bind(("wlp1s0", 0))
s1.bind(("lan0",0))
pktnum = 1

#sys.stdout.write("<%s>\n"%repr(r))
while True:
    r=s.recv(2000)
    print("Received packet %i" % pktnum)
    s1.send(bytes(r))
    #print(r)
    pktnum = pktnum+1
    #print("%s" % r)


def signal_handler(sig, frame):
	#print('\r\nNumber Rx: ')
	print('\r\n'+len(pkt))
	sys.exit(0)
signal.signal(signal.SIGINT,signal_handler)
signal.pause()