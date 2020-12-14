import imaplib
import time
import serial
import struct
import http.client, urllib

WRITE_KEY = 'GKS2XLJCBA5X2D95'

serialPort = serial.Serial("COM1", baudrate=9600)

while True:
    if (serialPort.in_waiting > 0):
        receivedMessage = serialPort.read(16)
        t1, l1, t2, l2 = struct.unpack('ffff', receivedMessage)
        # print ("T1: {:.2f} L1: {:.2f}\nT2: {:.2f} L2: {:.2f}\n".format(t1, l1, t2, l2))
        
        # connection = http.client.HTTPSConnection("api.thingspeak.com")
        # url = "/update?api_key={}&field1={:.2f}&field2={:.2f}&field3={:.2f}&field4={:.2f}".format(WRITE_KEY, t1, l1, t2, l2)
        # connection.request("POST", url)
        # connection.close

        pass

    # message = struct.pack(">BBHI", ARDUINO_ID, SOURCE_ID, temp)
    # serialPort.write(message)

serialPort.close()