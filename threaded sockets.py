import socket
from threading import Thread
import smtplib
from email.mime.text import MIMEText
import urllib.request
import urllib.parse
import json
import os
from dotenv import load_dotenv
import time

# Freezer temperature threshold
freezeThresh = -12

load_dotenv('config.env')
# Email Settings
recipients = os.getenv('email_recipients')
sender = os.getenv('smtp_sender')
password = os.getenv('smtp_pass')
smtp_user = os.getenv('smtp_user')
smtp_host = os.getenv('smtp_host')
subject = "Freezer Alert!"
body = "Freezer not be chill yo"

# SMS settings
smsreceivers = os.getenv('phonenums')  # comma delimited character list of phone numbers
sms_api = os.getenv('sms_api')

# Email function 
def send_email(subject, body, sender, recipients, password):
    msg = MIMEText(body)
    msg['Subject'] = subject
    msg['From'] = sender
    msg['To'] = ', '.join(recipients)
    #context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    smtp_server = smtplib.SMTP(smtp_host, 587)
    smtp_server.ehlo()
    smtp_server.starttls()
    smtp_server.ehlo()
    smtp_server.login(smtp_user, password)
    smtp_server.sendmail(sender, recipients, msg.as_string())
    smtp_server.quit()

# SMS function
def sendSMS(apikey, numbers, sender, message):
    data =  urllib.parse.urlencode({'apikey': apikey, 'numbers': numbers, 'test' : '1',
                                     'message' : message, 'sender': sender})
    data = data.encode('utf-8')
    request = urllib.request.Request("https://api.txtlocal.com/send/?")
    f = urllib.request.urlopen(request, data)
    fr = f.read()
    return(fr)

# Get freezer data
def on_new_client(client_socket, addr):
    while True:
        data = client_socket.recv(1024).decode('utf-8')
        if not data:
            break
        print(f"{addr} >> {data}")
        print("The variable, content is of type:", type(data))

        # process data to extract freezer name and temp, maybe with special delimiter

        if data > freezeThresh:
            # Send SMS
            resp =  sendSMS(sms_api, smsreceivers,'Lab Notifier', body)
            response = json.loads(resp)
            print (response['cost'])
            # Send Email
            send_email(subject, body, sender, recipients, password)
            time.sleep(3600)  # wait an hour before resetting
    client_socket.close()

# Start scoket
s = socket.socket()         # create socket object
s.bind(('0.0.0.0', 8090))   # allow any incoming connection on port 8090
s.listen(5)                 # increase number for more clients
print(recipients)
print('Waiting for connection...')

while True:
    client, addr = s.accept() # receive a connection
    print(f"New connection from: {addr}")
    thread = Thread(target=on_new_client, args = (client, addr)) # create thread
    thread.start() # start thread
    # close client connection
    print("Closing connection")
    client.close()
    thread.join
#s.close()