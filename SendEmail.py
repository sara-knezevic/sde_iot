import smtplib

def send_email(source, destination, message):
    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.starttls()
    if (source == "lost.func@gmail.com"):
        server.login(source, "rgzekyhzyfryrdbe")
    else:
        print ("Login failed.")
        return

    subject = "Subject: {}".format(message.split("\r\n", 1)[0])
    body = ' '.join(message.split("\r\n", 2)[1:])

    message = subject + "\r\n" + body

    server.sendmail(source, destination, message.encode('utf-8'))

    server.quit()