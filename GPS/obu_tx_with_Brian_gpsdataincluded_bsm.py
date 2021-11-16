import serial
import signal
import sys
import time
import threading
import pynmea2
import json
from socket import socket, AF_PACKET, SOCK_RAW
from scapy.all import hexdump, raw, bytes_encode
from scapy.layers.l2 import Ether, Dot1Q

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

gga=False
rmc=False 
o=''
pktNum = 0
ser = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=1)
while True:
    nmea_sentence = '-------'
    while len(nmea_sentence)>0:
        nmea_sentence = ser.readline().decode('utf-8')
        if 'GGA' in nmea_sentence:
            msg_latlon=pynmea2.parse(nmea_sentence)
            o+="%s,%s"%(msg_latlon.latitude,msg_latlon.longitude)
            gga=True
        if 'RMC' in nmea_sentence:
            msg = pynmea2.parse(nmea_sentence)
            try:
                angle = float(msg.true_course)
                angle = 360+(90-angle)
                if angle > 360:
                    angle = angle - 360
            except:
                angle='-1'
            o+="%s,"%(str(angle))
            rmc=True
                                                 
        if gga and rmc:
            #print(o)
            data = o.split(",")
            OwlBox_BSM["lat"] = float(data[1])
            OwlBox_BSM["long"] = float(data[2])
            OwlBox_BSM["angle"] = float(data[0])
            #print(OwlBox_BSM)
            boxData = json.dumps(OwlBox_BSM).encode('utf8')
            pkt = Ether()
            pkt[Ether].dst = "ff:ff:ff:ff:ff:ff"
            pkt[Ether].src = "44:6d:57:c4:81:69"
            #"7c:e9:3:51:86:6e"
            pkt[Ether].type = 0x88dc
            pkt = pkt/boxData
            encoded = bytes_encode(pkt)
            s.send(encoded)
            print(encoded)
            print('/n')
            pktNum = pktNum + 1
            o = ''
            gga=False
            rmc=False 
            #ss.sendto(o.encode(),(destIP_udp,destPort_udp))
            o=''
    time.sleep(.1)