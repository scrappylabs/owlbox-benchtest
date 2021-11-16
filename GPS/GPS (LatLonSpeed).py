import serial
import signal
import sys
import time
import threading
import pynmea2
import json


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
vtg=False
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
            msg_heading = pynmea2.parse(nmea_sentence)
            try:
                angle = float(msg_heading.true_course)
                angle = 360+(90-angle)
                if angle > 360:
                    angle = angle - 360
            except:
                angle='-1'
            o+="%s,"%(str(angle))
            rmc=True
        if 'VTG' in nmea_sentence:
            msg_kmh = pynmea2.parse(nmea_sentence)
            try:
                k = float(msg_kmh.spd_over_grnd_kmph)
            except:
                k = 0.0
            o+="%s,"%(k)
            vtg=True
        if gga and rmc and vtg:
            #print(o)
            data = o.split(",")
            OwlBox_BSM["angle"] = float(data[0])
            OwlBox_BSM["speed"] = float(data[1])
            OwlBox_BSM["lat"] = float(data[2])
            OwlBox_BSM["long"] = float(data[3])
            print(data[1]) #kilometers per hour
            #print(OwlBox_BSM)
            print('/n')
            
            o = ''
            gga=False
            rmc=False
            vtg=False

            o=''
    time.sleep(.1)
