import sys
from rpi_ws281x import *
import threading
import time

def sendToLED(r,g,b,strip):
    for i in range(strip.numPixels()):
        strip.setPixelColor(i, Color(r,g,b))
        strip.show()

def getColor(strip):
    print(strip.numPixels)