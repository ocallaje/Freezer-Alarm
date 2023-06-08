# Freezer-Alarm
ESP32 temperature sensor for monitoring freezer temperature. 
When freezer temperature goes above a defined threshold, an alarm is triggered to send both an SMS and an Email to a list of users. 

## Notifications
### SMS
The Textlocal api is invoked to send an SMS message, all thats needed is an API key.
Textlocal requires than an account be pre-credited to send messages. Test messages can be sent instead with 'test' : '0'.

### Email
Emails are sent using SMTP to a local SMTP server using TLS. Settings can be changed for public SMTP servers e.g. GMAIL. 

## WiFi Connection
The ESP32 connects over WiFi with a websocket to communicate with a PC/Server running the python script to process the data.

## Environment Files
You will need two environment files. One called config.env located in the root directory containing the following parameters (insert strings between each '' or ""):

phonenums = ['']                    # phone numbers e.g. ['012345678']
email_recipients = ["", ""]         # comma delimited list of email addresses
smtp_user = ''                      # smtp server username
smtp_pass = ""                      # smtp server password
smtp_sender = ""                    # email addres of smtp account       
smtp_host = ''                      # smtp server ip address
sms_api = '='                       # textlocal api key


The second environment file called WiFiCredentials.h is located in your arduino/esp32 libraries folder like so:

```bash
├── Arduino
│   ├── Libraries
│   │   ├── WiFiCredentials
│   │   │   ├──WifiCredentials.h
```

This file defines the wifi to connect to for the esp32 and should contain the following with your wifi details within the ""
#define envSSID ""
#define envPASSWORD ""