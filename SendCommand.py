import imaplib
import time
import serial
import struct


serialPort = serial.Serial("COM2", baudrate=19200)

message = struct.pack('>BBB', 2, 2, 0)
serialPort.write(message)
time.sleep(1)
serialPort.flush()

serialPort.close()