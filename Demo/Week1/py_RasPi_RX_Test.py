import sys
import socket
import signal
import time
from rpi_ws281x import *
import argparse

import threading


velocity = 0
pktdata = "5554004"


LED_COUNT      = 16      # Number of LED pixels.
LED_PIN        = 18      # GPIO pin connected to the pixels (18 uses PWM!).
#LED_PIN        = 10     # GPIO pin connected to the pixels (10 uses SPI /dev/spidev0.0).
LED_FREQ_HZ    = 800000  # LED signal frequency in hertz (usually 800khz)
LED_DMA        = 10      # DMA channel to use for generating signal (try 10)
LED_BRIGHTNESS = 255     # Set to 0 for darkest and 255 for brightest
LED_INVERT     = False   # True to invert the signal (when using NPN transistor level shift)
LED_CHANNEL    = 0       # set to '1' for GPIOs 13, 19, 41, 45 or 53


ETH_P_ALL=0x88DC # not defined in socket module, sadly...
s=socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(ETH_P_ALL))
s.bind(("eth0", 0))
pktnum = 1


#Strip Initialization
strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, LED_FREQ_HZ, LED_DMA, LED_INVERT, LED_BRIGHTNESS, LED_CHANNEL)
strip.begin()

def loop_pkt():
    pktnumrcv = 0
    global velocity
    global pktdata
    while True:
        global data
        global velocity
        #print(velocity)
        r=s.recv(2000)
        print("Received packet %i" % pktnumrcv)
        pktnumrcv = pktnumrcv + 1
        velocity = int(str(r).strip()[-2:-1])
        print(velocity)
        #print(str(r[0]).strip()[-2:])
        pktdata = r
        #print("%s" % r)

def parse_args():
    global velocity
    global pktdata
    while True:
        internalvelocity = int(pkt.strip()[-2:-1])
        velocity = internalvelocity
        print("internal velocity: ", internalvelocity)


def update_LED():
    global velocity
    global pktdata
    while True:
        #if (velocity > 200):
            #velocity = 0
        for i in range(strip.numPixels()):
            strip.setPixelColor(i, Color(velocity*20, 0, 0))
            strip.show()
            time.sleep(2/1000)
        #velocity = velocity + 1
        print("velocity ", velocity)
        



if __name__ == '__main__':
    t1 = threading.Thread(target=loop_pkt)
    t2 = threading.Thread(target=parse_args)
    t3 = threading.Thread(target=update_LED)

t1.start()
#t2.start()
t3.start()


def signal_handler(sig, frame):
	#print('\r\nNumber Rx: ')
	#print('\r\n'+len(pktdata))
	sys.exit(0)
signal.signal(signal.SIGINT,signal_handler)
signal.pause()