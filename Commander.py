import imaplib
import time
import serial
import struct

def sendCommand(device, room, state):
    serialPort = serial.Serial("COM2", baudrate=9600)

    message = struct.pack('>BBB', device, room, state)
    serialPort.write(message)
    serialPort.flush()

    serialPort.close()