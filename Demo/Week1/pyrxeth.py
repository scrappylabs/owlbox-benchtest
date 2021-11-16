import sys
import socket
import signal
from shapely.geometry import Point
from shapely.geometry.polygon import Polygon

sender = 0
dX = 10              #Easting offset
dY = 10             #Northing offset
center = [0,0]

corners = Polygon([(0, 0), (0, 1), (1, 1), (1, 0)])
#corners = [(center[0] - dX, center[1]-dY), (center[0] - dX, center[1] + dY), (center[0] + dX, center[1] - dY),(center[0] + dX, center[1] + dY)]
print (corners)
ETH_P_ALL=0x88DC # not defined in socket module, sadly...
s=socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(ETH_P_ALL))
#s1= socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(ETH_P_ALL))
s.bind(("lan0", 0))
#s1.bind(("lan0",0))
pktnum = 1

def geofence_calculate

while True: 
    if (sender == 1):
        r=s.recv(2000)
        print("Received packet %i" % pktnum)
        #s1.send(bytes(r))
        #print(r)
        pktnum = pktnum+1
        #print("%s" % r)
    else:
        print("No vehicle within bound")
    


def signal_handler(sig, frame):
	#print('\r\nNumber Rx: ')
	print('\r\n'+len(pkt))
	sys.exit(0)
signal.signal(signal.SIGINT,signal_handler)
signal.pause()