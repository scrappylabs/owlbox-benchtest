from socket import socket, AF_PACKET, SOCK_RAW
from scapy.all import hexdump, raw, bytes_encode
from scapy.layers.l2 import Ether, Dot1Q
import serial
import signal
import sys
import threading
import pynmea2
import json

import time
from datetime import datetime, timedelta

t_cam = "\xFF\xFF\xFF\xFF\xFF\xFF\x74\xE5\x43\xD5\xFD\xA4\x89\x47\x01\x00\xF1\x01\x20\x50\x02\x74\x00\x2D\x01\x00\x95\xF9\x74\xE5\x43\xD5\xFD\xA4\xA7\x84\x11\x99\x19\x54\x08"

t_cam2 = "\x02\x20\x0f\x01\xac\x10\x01\x0c\x04\x01\x05\x80\x00\x31\x02\x00\x30\x2d\x80\x01\x02\x81\x26\x14\x80\x22\x22\x0e\x8f\xc0\x14\x2a\x28\x27\xcd\x94\xdc\x70\x0a\xb9\xff\xff\xff\xff\xe0\x02\x70\x80\x7f\x00\x08\x07\xd1\x81\x7f\xff\x08\x00\x0c\x80\x32\xa3\x00"

t_cam3 = "\xc3\xbf\xc3\xbf\xcf\xbf\x74\xE5\x43\xD5\xFD\xA4"

t_cam5 = "\x02\x20\x0f\x01\xac\x10\x01\x06\x04\x01\x0e\x80\x00\x31\x02\x00\x30\x2d\x80\x01\x02\x81\x26\x00\x95\xb9\x6b\xfe\xff\xff\x35\xa4\xe9\x01\x6b\x49\xd2\x01\xf0\x00\xff\xff\xff\xff\xff\xff\x70\x80\x7f\x07\xd1\x07\xd1\x81\x7f\xff\x08\x00\x0c\x80\x32\xa3\x00\x70\x72\x65\x6d\x34\x34\x30\x30\x70\x20\x20\x20"
#t_cam5 = t_cam5.enc(ASN1_Codecs.BER)
number = 30

#pkt = #pkt/"\x02\x20\x0f\x01\xac\x10\x01\x06\x04\x01\x0e\x80\x00\x31\x02\x00\x30\x2d\x80\x01\x02\x81\x26\x00\x95\xb9\x6b\xfe\xff\xff\x35\xa4\xe9\x01\x6b\x49\xd2\x01\xf0\x00\xff\xff\xff\xff\xff\xff\x70\x80\x7f\x07\xd1\x07\xd1\x81\x7f\xff\x08\x00\x0c\x80\x32\xa3\x00"
#pkt = pkt/"\x02\x20\x0f\x01\xac\x10\x01\x06\x04\x01\x0e\x80\x00\x31\x02\x00\x30\x2d\x80\x01\x02\x81\x26\x00\x95\xb9\x6b\xfe\xff\xff\x35\xa4\xe9\x01\x6b\x49\xd2\x01\xf0\x00\xff\xff\xff\xff\xff\xff\x70\x80\x7f\x07\xd1\x07\xd1\x81\x7f\xff\x08\x00\x0c\x80\x32\xa3\x00"
def packetcreate():
    global encoded
    data = t_cam5 + str(number)
    pkt = Ether()
    pkt[Ether].dst = "ff:ff:ff:ff:ff:ff"
    pkt[Ether].src = "7c:e9:3:51:86:6e"
    pkt[Ether].type = 0x88dc
    pkt = pkt/data
    encoded = bytes_encode(pkt)

s = socket(AF_PACKET, SOCK_RAW)
s.bind(("wlp1s0", 0))
# We're putting together an ethernet frame here, 
# but you could have anything you want instead
# Have a look at the 'struct' module for more 
# flexible packing/unpacking of binary data
# and 'binascii' for 32 bit CRC
#src_addr = "\x7C\xE9\x03\x51\x86\x6E"
#dst_addr = "\xFF\xFF\xFF\xFF\xFF\xFF"
#payload = ("["*30)+"PAYLOAD"+("]"*30)
#checksum = "\x1a\x2b\x3c\x4d"
#ethertype = "\x88\xDC"
#message = dst_addr+src_addr+ethertype+payload


pktnum = 1
print("Enter send speed (Hz) - Enter '2000' for maximum transfer rate")
speed = int(input())
speedHz = (1/speed)
print("Enter duration of test(s)")
times = int(input())
start_time = time.ctime()
end_time = datetime.now() + timedelta(microseconds=(times*1000000)) 
inttimes = int(times)
print("Sending for %i seconds..." % times)

OwlBox_BSM = {
    "owlboxID": "BusBox1",
    "msgCnt": 54,
    "tempid": 1762,
    "time": 1630714938,
    "lat": 30.012,
    "long": -97.6025,
    "elev": 175,
    "accuracy": 506,
    "transmission": 7,
    "speed": 65,
    "heading": "N",
    "angle": 127,
    "accelSet": 9.19,
    "brakes": 0,
    "size": {
        "length": 191,
        "width": 76,
    },
}
ser = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=1)
while datetime.now() < end_time:
    nmea_sentence = '-------'
    nmea_sentence = ser.readline().decode('utf-8')
    packetcreate()
    if 'VTG' in nmea_sentence:
        msg_kmh = pynmea2.parse(nmea_sentence)
        try:
            number = float(msg_kmh.spd_over_grnd_kmph)
        except:
            number = 0.0
    if(speed == 2000):
     s.send(encoded)
     pktnum = pktnum + 1
    else:
     time.sleep(speedHz)
     s.send(encoded)
     pktnum = pktnum + 1
     #print(hexdump(raw(encoded)))
     #print("sent packet number: " + str(pktnum))

print(">>Start Time: %s" % start_time)
print(">>End Time: %s" % time.ctime())
print(">>Time Spent Running: %s seconds" % times)
print(">>Total packets sent: %s" % pktnum)
print(">>Current speed: %i pkts/sec" % (pktnum/inttimes))
#print(speedHz)
