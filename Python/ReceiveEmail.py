# Functions for searching for e-mails and extracting
# the ones aimed at the smart home communication
# and translating the commands before sending them
# to the Arduino.

import imaplib, email
import time
import serial
import struct
import re
import Commander, GenerateReport

mail = imaplib.IMAP4_SSL("imap.gmail.com")
mail.login("lost.func@gmail.com", "rgzekyhzyfryrdbe")

# Mapping sheet from e-mail to values to send to the Arduino!
sheet = {
    "VENTILLATION" : 1,
    "LIGHT" : 2,
    "ROOM1" : 1,
    "ROOM2" : 2,
    "ON" : 1,
    "OFF" : 0
}

# Regex for finding command-shaped e-mails.
# General shape: DEVICE-STATE-PLACE
# eg. LIGHT-ON-ROOM1
# eg. VENTILLATION-OFF-ROOM2
filterRegex = re.compile(r'([A-Z])*-[A-Z]*-[A-Z]*\d+')

# Gather e-mails and extract command ones.
def getCommandEmails():
    commands = []

    mail.select("inbox") # search in inbox
    _, data = mail.uid('search', None, "UNSEEN") # fetch unread e-mails
    all_messages = len(data[0].split()) # count unread messages

    for msg in range(all_messages):
        
        latest_email_uid = data[0].split()[msg] # get e-mail uid
        _, email_data = mail.uid('fetch', latest_email_uid, '(RFC822)') # convert data to readable format
        raw_email = email_data[0][1] # extract content from e-mail
        raw_email_string = raw_email.decode('utf-8') # decode e-mail to string
        email_message = email.message_from_string(raw_email_string) # convert to email object
        subject = str(email.header.make_header(email.header.decode_header(email_message['Subject']))) # convert subject to string

        if filterRegex.match(subject): # check if subject matches command format
            commands.append(subject)
        elif subject == "GET-REPORT": # check if subject is asking for the report
            commands.append("GET-REPORT")
        else: # ignore e-mail and mark as unread
            mail.uid('STORE', latest_email_uid, "-FLAGS", '(\\Seen)')

    return commands

# Send commands from e-mail to Arduino
def translateCommandsAndSend(serialPort):
    commands = getCommandEmails()
    for command in commands:
        if (command == "GET-REPORT"): # if command is asking for a report
            GenerateReport.sendReport(serialPort) # send report and go to next command
            continue
        
        try: # Extract corresponding vales from mapping sheet for commands
            device, state, room = command.split('-')
            convertDevice = sheet[device]
            convertState = sheet[state]
            convertRoom = sheet[room]
            Commander.sendCommand(serialPort, convertDevice, convertRoom, convertState)
        except Exception:
            # Command format is correct, but wrong room/device/state.
            print ("Oops! Key error. Please check device, state, or room number.")
            pass