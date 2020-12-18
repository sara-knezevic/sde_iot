# Main smart home file!
# This file contains the main endless loop for the execution of the program.
# Includes all other files which contain the necessary functions for the house to function.

import ReceiveEmail
import Communication
import Commander
import GenerateReport
import serial, time

serialPort = serial.Serial("COM2", baudrate=9600)

start_time = time.time()
interval = 600 # 10 minutes

# Main infinite loop for the smart home.
while True:
    current_time = time.time()
    elapsed_time = current_time - start_time

    # Check e-mail for commands and forward them to Arduino.
    ReceiveEmail.translateCommandsAndSend(serialPort)

    # Check if there are messages over the serial port.
    if (serialPort.in_waiting > 0):
        # Fetch sensor data and upload them to ThingSpeak.
        Communication.getSensorData(serialPort)
    else:
        print ("No data at serial.")

    # Check if 10 minutes has passed to send a report.
    if int(elapsed_time) % interval < 3:
        start_time = time.time()
        GenerateReport.sendReport(serialPort)

    serialPort.flush()
    time.sleep(1)

serialPort.close()