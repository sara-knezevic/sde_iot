import imaplib
import time
import serial
import struct

def sendCommand(serialPort, device, room, state):
    message = struct.pack('>BBB', device, room, state)
    serialPort.write(message)
    serialPort.flush()