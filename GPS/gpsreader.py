import pynmea2
import serial
import signal
import sys
import threading
import json


ms = pynmea2.parse("")