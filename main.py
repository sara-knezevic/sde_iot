import ReceiveEmail
import Communication
import Commander
import serial, time

serialPort = serial.Serial("COM2", baudrate=9600)

while True:
    Communication.getSensorData(serialPort)
    ReceiveEmail.translateCommandsAndSend(serialPort)

    time.sleep(2)

serialPort.close()