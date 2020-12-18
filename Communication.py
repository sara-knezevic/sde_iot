# Functions for the communication with the Arduino.

import imaplib
import time
import serial
import struct
import http.client, urllib
import Commander

# ThingSpeak write API key.
WRITE_KEY = 'GKS2XLJCBA5X2D95'

# Read sensor data sent over serial port.
def readData(serialPort, deviceFlag):
    receivedMessage = serialPort.read(20)
    t1, l1, t2, l2, ventOne, lightOne, ventTwo, lightTwo = struct.unpack('ffffBBBB', receivedMessage)

    if (deviceFlag == "sensors"):
        return (t1, l1, t2, l2)
    elif (deviceFlag == "devices"):
        return (ventOne, lightOne, ventTwo, lightTwo)

# Send gathered data to matching ThingSpeak channels.
def sendDataThingspeak(t1, l1, t2, l2):
    connection = http.client.HTTPSConnection("api.thingspeak.com")
    url = "/update?api_key={}&field1={:.2f}&field2={:.2f}&field3={:.2f}&field4={:.2f}".format(WRITE_KEY, t1, l1, t2, l2)
    connection.request("GET", url)
    response = connection.getresponse()
    print("Status: {} and reason: {}".format(response.status, response.reason))
    connection.close()

# Call function to gather data and send to ThingSpeak.
def getSensorData(serialPort):
    t1, l1, t2, l2 = readData(serialPort, "sensors")
    sendDataThingspeak(t1, l1, t2, l2)