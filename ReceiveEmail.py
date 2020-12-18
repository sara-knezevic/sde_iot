import imaplib, email
import time
import serial
import struct
import re
import Commander, GenerateReport

mail = imaplib.IMAP4_SSL("imap.gmail.com")
mail.login("lost.func@gmail.com", "rgzekyhzyfryrdbe")

sheet = {
    "VENTILLATION" : 1,
    "LIGHT" : 2,
    "ROOM1" : 1,
    "ROOM2" : 2,
    "ON" : 1,
    "OFF" : 0
}

filterRegex = re.compile(r'([A-Z])*-[A-Z]*-[A-Z]*\d+')

def getCommandEmails():
    commands = []

    mail.select("inbox")
    _, data = mail.uid('search', None, "UNSEEN")
    all_messages = len(data[0].split())

    for msg in range(all_messages):
        
        latest_email_uid = data[0].split()[msg]
        _, email_data = mail.uid('fetch', latest_email_uid, '(RFC822)')
        raw_email = email_data[0][1]
        raw_email_string = raw_email.decode('utf-8')
        email_message = email.message_from_string(raw_email_string)
        subject = str(email.header.make_header(email.header.decode_header(email_message['Subject'])))

        if filterRegex.match(subject):
            commands.append(subject)
        elif subject == "GET-REPORT":
            commands.append("GET-REPORT")
        else:
            mail.uid('STORE', latest_email_uid, "-FLAGS", '(\\Seen)')

    return commands

def translateCommandsAndSend(serialPort):
    commands = getCommandEmails()
    for command in commands:
        if (command == "GET-REPORT"):
            GenerateReport.sendReport(serialPort)
            continue
        
        try:
            device, state, room = command.split('-')
            convertDevice = sheet[device]
            convertState = sheet[state]
            convertRoom = sheet[room]
            Commander.sendCommand(serialPort, convertDevice, convertRoom, convertState)
        except Exception:
            print ("Oops! Key error. Please check device, state, or room number.")
            pass

    # time.sleep(1)