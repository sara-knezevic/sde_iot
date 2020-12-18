Final Project
Internet of Things

In this project you need to simulate a smart home with an Arduino Simulation in Proteus and python script on your PC. Smart home which you need to simulate contains 2 rooms. In each room there is a bulb which operates on 220V AC, it is necessary to have control over both of them with Arduino. Additionally, each room has one DC motor that simulates a ventilation system. While the ventilation system is turned on it has two operation modes: ​supply of fresh air - DC motor rotating in one direction and when rotate in opposite direction that is - ​bad air suction. ​These two modes alternate in an interval of 1 minute.
There are two sensors in both rooms, one sensor for measuring temperature and the other for measuring illuminance. Measurements of each sensor are sent to the PC after every minute. To successfully do this part of the task, it is necessary to select the appropriate control devices and sensors.
For communication with a PC you need to use zigBee(xBee) Protocol and an appropriate device. Define the appearance of the message through which you will be able to send commands but also receive measurements.
Python script. ​In this part of the project, you need to create a script that, in addition to communicating with the Arduino, can send/read email and upload/download data to ThinkSpeak.com. Everytime when scripts receive measurements from an arduino, it uploads them to an appropriate field (one sensor, one field) on ThingSpeak. Arduino control is also sent via e-mail, as well as sending reports. It is necessary to define the structure of the Email Subject through which the command can be determined and which device is switched on in which room.
(​examples: Subject:LIGHT-ON-ROOM2 o​r Subject:VENTILATION-OFF-ROOM1 o​r Subject:GET-REPORT​)
The report consists of the average of the last 10 measurements from each sensor, get this data from thingspeak.com, as well as the current state of the ventilation system and light.
Email Report Example
Subject: Report
Body: Room 1
Average temperature: 23.2°C
Average illumination: 20 lux
Current ventilation status:ON
Current light status:ON
Room 2
Average temperature: 27.3°C
Average illumination: 24 lux
Current ventilation status:OFF
Current light status:ON
Reports should be sent on each 10 minutes or when requested with an email command.