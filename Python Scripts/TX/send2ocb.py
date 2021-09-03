import time
import signal
import sys
from datetime import datetime, timedelta
from socket import socket, AF_PACKET, SOCK_RAW

from scapy.all import *

t_cam = "\xFF\xFF\xFF\xFF\xFF\xFF\x74\xE5\x43\xD5\xFD\xA4\x89\x47\x01\x00\xF1\x01\x20\x50\x02\x74\x00\x2D\x01\x00\x95\xF9\x74\xE5\x43\xD5\xFD\xA4\xA7\x84\x11\x99\x19\x54\x08"

t_cam2 = "\x02\x20\x0f\x01\xac\x10\x01\x0c\x04\x01\x05\x80\x00\x31\x02\x00\x30\x2d\x80\x01\x02\x81\x26\x14\x80\x22\x22\x0e\x8f\xc0\x14\x2a\x28\x27\xcd\x94\xdc\x70\x0a\xb9\xff\xff\xff\xff\xe0\x02\x70\x80\x7f\x00\x08\x07\xd1\x81\x7f\xff\x08\x00\x0c\x80\x32\xa3\x00"

t_cam3 = "\xc3\xbf\xc3\xbf\xcf\xbf\x74\xE5\x43\xD5\xFD\xA4"

t_cam5 = "\x02\x20\x0f\x01\xac\x10\x01\x06\x04\x01\x0e\x80\x00\x31\x02\x00\x30\x2d\x80\x01\x02\x81\x26\x00\x95\xb9\x6b\xfe\xff\xff\x35\xa4\xe9\x01\x6b\x49\xd2\x01\xf0\x00\xff\xff\xff\xff\xff\xff\x70\x80\x7f\x07\xd1\x07\xd1\x81\x7f\xff\x08\x00\x0c\x80\x32\xa3\x00"
#t_cam5 = t_cam5.enc(ASN1_Codecs.BER)

data = t_cam5
pkt = Ether()
pkt[Ether].dst = "ff:ff:ff:ff:ff:ff"
pkt[Ether].src = "7c:e9:3:51:86:6e"
pkt[Ether].type = 0x88dc
pkt = pkt/data

s = socket(AF_PACKET, SOCK_RAW)
s.bind(("wlp1s0", 0))
# We're putting together an ethernet frame here, 
# but you could have anything you want instead
# Have a look at the 'struct' module for more 
# flexible packing/unpacking of binary data
# and 'binascii' for 32 bit CRC
src_addr = "\x01\x02\x03\x04\x05\x06"
dst_addr = "\x01\x02\x03\x04\x05\x06"
payload = ("["*30)+"PAYLOAD"+("]"*30)
checksum = "\x1a\x2b\x3c\x4d"
ethertype = "\x08\x01"
message = dst_addr+src_addr+ethertype+payload+checksum
encoded = message.encode()
packetnum = 0
print("Enter send speed (Hz)")
speed = int(input())
speedHz = (1/speed)
print("Enter duration of test (seconds)")
times = int(input())

start_time = time.ctime()
end_time = datetime.now() + timedelta(microseconds = times*1000*1000)

while datetime.now() < end_time:
 time.sleep(0.001)
 #print(hexdump(raw(encoded)))
 break
 s.send(encoded)
 packetnum = packetnum + 1
 print("%s packets sent" % packetnum)
print("Start Time: %s" % start_time)
print("End Time: %s" % time.ctime())
print("Time between sleep %s" % speedHz)
exit()  
