# Freezer-Alarm
ESP32 temperature sensor for monitoring freezer temperature. 
When freezer temperature goes above a defined threshold, an alarm is triggered to send both an SMS and an Email to a list of users. 

## Notifications
### SMS
The Textlocal api is invoked to send an SMS message, all thats needed is an API key.
Textlocal requires than an account be pre-credited to send messages. Test messages can be sent instead with 'test' : '0'.

### Email
Emails are sent using SMTP to a local SMTP server using TLS. Settings can be changed for public SMTP servers e.g. GMAIL. 