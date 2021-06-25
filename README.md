Final Project <br/>
Internet of Things <br/>
 <br/>
In this project a simulation of a smart home with an Arduino Simulation in Proteus and python script on your PC is created. Smart home which is simulated contains 2 rooms. In each room there is a bulb which operates on 220V AC, it is necessary to have control over both of them with Arduino. Additionally, each room has one DC motor that simulates a ventilation system. While the ventilation system is turned on it has two operation modes:  <br/>
  ​supply of fresh air - DC motor rotating in one direction and when rotate in opposite direction that is -  <br/>
  ​bad air suction.  <br/>
  ​These two modes alternate in an interval of 1 minute. <br/>
There are two sensors in both rooms, one sensor for measuring temperature and the other for measuring illuminance. Measurements of each sensor are sent to the PC after every minute. To successfully do this part of the task, it is necessary to select the appropriate control devices and sensors. <br/>
For communication with a PC you need to use zigBee(xBee) Protocol and an appropriate device. Define the appearance of the message through which you will be able to send commands but also receive measurements. <br/>
Python script. In this part of the project, a script is created that, in addition to communicating with the Arduino, can send/read email and upload/download data to ThingSpeak.com. Everytime when scripts receive measurements from an arduino, it uploads them to an appropriate field (one sensor, one field) on ThingSpeak. Arduino control is also sent via e-mail, as well as sending reports. It is necessary to define the structure of the Email Subject through which the command can be determined and which device is switched on in which room. <br/>
(examples: Subject:LIGHT-ON-ROOM2) <br/>
The report consists of the average of the last 10 measurements from each sensor, get this data from thingspeak.com, as well as the current state of the ventilation system and light. <br/>
Email Report Example <br/>
Subject: Report <br/>
Body: Room 1 <br/>
Average temperature: 23.2°C <br/>
Average illumination: 20 lux <br/>
Current ventilation status:ON <br/>
Current light status:ON <br/>
Room 2 <br/>
Average temperature: 27.3°C <br/>
Average illumination: 24 lux <br/>
Current ventilation status:OFF <br/>
Current light status:ON <br/>
Reports should be sent on each 10 minutes or when requested with an email command.
