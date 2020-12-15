import smtplib

def send_email(source, destination, message):
    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.starttls()
    if (source == "lost.func@gmail.com"):
        server.login(source, "rgzekyhzyfryrdbe")
    else:
        print ("Login failed.")
        return

    subject = "Subject: {}".format(message.split()[0])

    message = subject

    server.sendmail(source, destination, message)

    server.quit()

send_email("lost.func@gmail.com", "lost.func@gmail.com", "LIGHT-ON-ROOM1")