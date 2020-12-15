import imaplib
import time
import serial
import struct
import http.client, urllib
import Commander

WRITE_KEY = 'GKS2XLJCBA5X2D95'

def readSensorData(serialPort):
    receivedMessage = serialPort.read(16)
    t1, l1, t2, l2 = struct.unpack('ffff', receivedMessage)
    serialPort.flush()

    return (t1, l1, t2, l2)

def sendDataThingspeak(t1, l1, t2, l2):
    connection = http.client.HTTPSConnection("api.thingspeak.com")
    url = "/update?api_key={}&field1={:.2f}&field2={:.2f}&field3={:.2f}&field4={:.2f}".format(WRITE_KEY, t1, l1, t2, l2)
    connection.request("GET", url)
    connection.close()

def getSensorData(serialPort):
    if (serialPort.in_waiting > 0):
        t1, l1, t2, l2 = readSensorData(serialPort)
        print ("{} {} {} {}".format(t1, l1, t2, l2))

        sendDataThingspeak(t1, l1, t2, l2)