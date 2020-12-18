# Functions used for reading data from thingspeak
# and generating a report and sending it over e-mail
# each 10 minutes (from main.py) or on command.

import http.client, urllib
import time, json, struct
import serial, SendEmail
import Communication

# ThingSpeak read API key.
READ_KEY = 'SXQT97OOEOE60HB0'

deviceStatusSheet = {
    0 : "OFF",
    1 : "ON"
}

# Fetch data from thingspeak!
def readSensorData(numOfResults):
    connection = http.client.HTTPSConnection("api.thingspeak.com")

    # Read entire channel.
    urlChannel = "/channels/1250625/feeds.json?api_key={}&results={}".format(READ_KEY, numOfResults)

    connection.request("GET", urlChannel)
    response = connection.getresponse()
    data = response.read()
    # Take "feeds" only as it contains the sensor data.
    channelData = json.loads(data.decode('utf-8'))["feeds"]

    # Initialize average sensor values.
    roomOneTempAvg = roomOneIlluminanceAvg = 0.0
    roomTwoTempAvg = roomTwoIlluminanceAvg = 0.0

    # Add up the data from thingspeak (numOfResults).
    for feed in channelData:
        roomOneTempAvg += float(feed["field1"])
        roomOneIlluminanceAvg += float(feed["field2"])
        roomTwoTempAvg += float(feed["field3"])
        roomTwoIlluminanceAvg += float(feed["field4"])

    dataExtractedNum = len(channelData)

    # Calculate average sensor values.
    if (dataExtractedNum):
        roomOneTempAvg /= dataExtractedNum
        roomOneIlluminanceAvg /= dataExtractedNum
        roomTwoTempAvg /= dataExtractedNum
        roomTwoIlluminanceAvg /= dataExtractedNum
    else:
        print ("No data!")

    connection.close()

    return roomOneTempAvg, roomOneIlluminanceAvg, roomTwoTempAvg, roomTwoIlluminanceAvg

# Ask Arduino to send device statuses!
def askDeviceStatus(serialPort):
    message = struct.pack('>BBB', 0, 0, 0)
    serialPort.write(message)
    serialPort.flush()

# Gather and calculate data, get device states, compose message and send report over e-mail.
def sendReport(serialPort):
    roomOneTempAvg, roomOneIlluminanceAvg, roomTwoTempAvg, roomTwoIlluminanceAvg = readSensorData(10)

    askDeviceStatus(serialPort)
    ventOne, lightOne, ventTwo, lightTwo = Communication.readData(serialPort, "devices")

    reportBodyRoomOne = "ROOM ONE\r\n" + "Average temperature: {:.2f}\u00b0C\r\n".format(roomOneTempAvg) + "Average illumination: {:.2f} lux\r\n".format(roomOneIlluminanceAvg) + "Current ventillation status: {}\r\n".format(deviceStatusSheet[ventOne]) + "Current light status: {}\r\n\r\n".format(deviceStatusSheet[lightOne])
    reportBodyRoomTwo = "ROOM TWO\r\n" + "Average temperature: {:.2f}\u00b0C\r\n".format(roomTwoTempAvg) + "Average illumination: {:.2f} lux\r\n".format(roomTwoIlluminanceAvg) + "Current ventillation status: {}\r\n".format(deviceStatusSheet[ventTwo]) + "Current light status: {}\r\n".format(deviceStatusSheet[lightTwo])

    message = "Smart home report" + "\r\n" + reportBodyRoomOne + reportBodyRoomTwo

    SendEmail.send_email("lost.func@gmail.com", "lost.func@gmail.com", message)