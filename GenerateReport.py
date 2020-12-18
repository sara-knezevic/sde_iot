import http.client, urllib
import time, json, struct
import serial, SendEmail
import Communication

READ_KEY = 'SXQT97OOEOE60HB0'

deviceStatusSheet = {
    0 : "OFF",
    1 : "ON"
}

def readSensorData(numOfResults):
    connection = http.client.HTTPSConnection("api.thingspeak.com")

    urlChannel = "/channels/1250625/feeds.json?api_key={}&results={}".format(READ_KEY, numOfResults)

    connection.request("GET", urlChannel)
    response = connection.getresponse()
    data = response.read()
    channelData = json.loads(data.decode('utf-8'))["feeds"]

    roomOneTempAvg = roomOneIlluminanceAvg = 0.0
    roomTwoTempAvg = roomTwoIlluminanceAvg = 0.0

    for feed in channelData:
        roomOneTempAvg += float(feed["field1"])
        roomOneIlluminanceAvg += float(feed["field2"])
        roomTwoTempAvg += float(feed["field3"])
        roomTwoIlluminanceAvg += float(feed["field4"])

    dataExtractedNum = len(channelData)

    if (dataExtractedNum):
        roomOneTempAvg /= dataExtractedNum
        roomOneIlluminanceAvg /= dataExtractedNum
        roomTwoTempAvg /= dataExtractedNum
        roomTwoIlluminanceAvg /= dataExtractedNum
    else:
        print ("No data!")

    connection.close()

    return roomOneTempAvg, roomOneIlluminanceAvg, roomTwoTempAvg, roomTwoIlluminanceAvg

def askDeviceStatus(serialPort):
    message = struct.pack('>BBB', 0, 0, 0)
    serialPort.write(message)
    serialPort.flush()
    # time.sleep(1)

def sendReport(serialPort):
    roomOneTempAvg, roomOneIlluminanceAvg, roomTwoTempAvg, roomTwoIlluminanceAvg = readSensorData(10)

    askDeviceStatus(serialPort)
    ventOne, lightOne, ventTwo, lightTwo = Communication.readData(serialPort, "devices")

    reportBodyRoomOne = "ROOM ONE\r\n" + "Average temperature: {:.2f}\u00b0C\r\n".format(roomOneTempAvg) + "Average illumination: {:.2f} lux\r\n".format(roomOneIlluminanceAvg) + "Current ventillation status: {}\r\n".format(deviceStatusSheet[ventOne]) + "Current light status: {}\r\n\r\n".format(deviceStatusSheet[lightOne])
    reportBodyRoomTwo = "ROOM TWO\r\n" + "Average temperature: {:.2f}\u00b0C\r\n".format(roomTwoTempAvg) + "Average illumination: {:.2f} lux\r\n".format(roomTwoIlluminanceAvg) + "Current ventillation status: {}\r\n".format(deviceStatusSheet[ventTwo]) + "Current light status: {}\r\n".format(deviceStatusSheet[lightTwo])

    message = "Smart home report" + "\r\n" + reportBodyRoomOne + reportBodyRoomTwo

    SendEmail.send_email("lost.func@gmail.com", "lost.func@gmail.com", message)