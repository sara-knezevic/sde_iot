import ReceiveEmail
import Communication
import Commander
import GenerateReport
import serial, time

serialPort = serial.Serial("COM2", baudrate=9600)

start_time = time.time()
interval = 600 # 10 minutes

while True:
    current_time = time.time()
    elapsed_time = current_time - start_time

    ReceiveEmail.translateCommandsAndSend(serialPort)

    if (serialPort.in_waiting > 0):
        Communication.getSensorData(serialPort)
    else:
        print ("No data at serial.")

    if int(elapsed_time) % interval < 3:
        start_time = time.time()
        GenerateReport.sendReport(serialPort)

    serialPort.flush()
    time.sleep(1)

serialPort.close()