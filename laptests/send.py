import sys
import socket
import signal
import time
from scapy.all import bytes_encode, hexdump, raw

ETH_P_ALL=0x88DC
s=socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(ETH_P_ALL))
s.bind(("wlp1s0",0))
destination = "ffffffffffff"
source = "7ce9351866e"
destType = "0x88DC"
packet = destination + source + str(destType) + "test"
packet2 = 'FFFF'
encoded = packet.encode(encoding = 'UTF-8')
hexdump(encoded)
print(packet2)

while True:
    s.send(encoded)
    hexdump(encoded)
    time.sleep(1)
    print("done\n")

