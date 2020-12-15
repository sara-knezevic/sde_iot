import imaplib
import time
import serial
import struct
import http.client, urllib
import Commander

WRITE_KEY = 'GKS2XLJCBA5X2D95'

serialPort = serial.Serial("COM2", baudrate=9600)

def readSensorData():
        receivedMessage = serialPort.read(16)
        t1, l1, t2, l2 = struct.unpack('ffff', receivedMessage)
        serialPort.flush()
    
        return (t1, l1, t2, l2)

def sendDataThingspeak(t1, l1, t2, l2):
    connection = http.client.HTTPSConnection("api.thingspeak.com")
    url = "/update?api_key={}&field1={:.2f}&field2={:.2f}&field3={:.2f}&field4={:.2f}".format(WRITE_KEY, t1, l1, t2, l2)
    connection.request("GET", url)
    response = connection.getresponse()
    print (response)
    connection.close()

while True:
    if (serialPort.in_waiting > 0):
        t1, l1, t2, l2 = readSensorData()
        print ("{} {} {} {}".format(t1, l1, t2, l2))

        sendDataThingspeak(t1, l1, t2, l2)

serialPort.close()