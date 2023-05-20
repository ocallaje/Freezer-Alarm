import socket
import smtplib
import ssl
from email.mime.text import MIMEText
import urllib.request
import urllib.parse
import json

# Freezer temperature threshold
freezeThresh = -12

# Email Settings
recipients = ["ocallaje@tcd.ie", "jeffreyocallaghan@hotmail.com"]
subject = "Freezer Alert!"
body = "Freezer not be chill yo"
sender = "ClusterNotifier@spice.rack"
password = "Claudin5"

# SMS settings
smsreceivers = ['0863612897']   # comma delimited character list of phone numbers

# Email function 
def send_email(subject, body, sender, recipients, password):
    msg = MIMEText(body)
    msg['Subject'] = subject
    msg['From'] = sender
    msg['To'] = ', '.join(recipients)
    #context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    smtp_server = smtplib.SMTP('192.168.5.2', 587)
    smtp_server.ehlo()
    smtp_server.starttls()
    smtp_server.ehlo()

    smtp_server.login('Cluster_Notifier', password)
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


# Start scoket
s = socket.socket()
s.bind(('192.168.4.1', 8090)) 
s.listen(0) # increase number for more clients

while True:
    client, addr = s.accept() # receive a connection

    while True:
        content = client.recv(32) #receive 32 bytes of data

        if len(content) == 0: # 0 if client disconnects
            break
        else:
            print(content)
    
        #debug
        print("The variable, content is of type:", type(content))

        if content > freezeThresh:
            # Send SMS
            resp =  sendSMS('MzE3NzU0NTY1MDY0MzM3Nzc5NmY2OTY2NTE2NDMyNjg=', smsreceivers,'Lab Notifier', body)
            response = json.loads(resp)
            print (response['cost'])

            # Send Email
            send_email(subject, body, sender, recipients, password)

    print("Closing connection")
    client.close()