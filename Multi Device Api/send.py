'''
Module to send alerts via email and SMS 
'''
import os
import smtplib
from email.mime.text import MIMEText
import urllib.request
import urllib.parse
from dotenv import load_dotenv

load_dotenv('config.env')
# Email Settings
allRecipients = os.getenv('email_recipients')
maintainers = os.getenv('maintainer_recipients')
sender = os.getenv('smtp_sender')
password = os.getenv('smtp_pass')
smtp_user = os.getenv('smtp_user')
smtp_host = os.getenv('smtp_host')

# SMS settings
smsreceivers = os.getenv('phonenums')  # comma delimited character list of phone numbers
sms_api = os.getenv('sms_api')

# Email function 
def send_email(subject, body, recipients):
    '''Function to send alerts via email'''
    msg = MIMEText(body)
    msg['Subject'] = subject
    msg['From'] = sender
    msg['To'] = ', '.join(recipients)
    #context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    try:
        smtp_server = smtplib.SMTP(smtp_host, 587)
        smtp_server.ehlo()
        smtp_server.starttls()
        smtp_server.ehlo()
        smtp_server.login(smtp_user, password)
        smtp_server.sendmail(sender, recipients, msg.as_string())
        smtp_server.quit() 
    except smtplib.SMTPServerDisconnected:
        print("Server unexpectedly disconnected")
    except smtplib.SMTPException:
        print("Failed to Send Email")

# SMS function
def send_sms(message, testmsg):
    '''Function to send alerts via SMS'''
    data =  urllib.parse.urlencode({'apikey': sms_api, 'numbers': smsreceivers, 'test' : testmsg,
                                     'message' : message, 'sender': sender})
    data = data.encode('utf-8')
    request = urllib.request.Request("https://api.txtlocal.com/send/?")
    req_response = urllib.request.urlopen(request, data)
    req_message = req_response.read()
    return req_message
