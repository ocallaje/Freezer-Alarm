from email.mime.text import MIMEText
import urllib.request
import urllib.parse
import json
import send

freezeThresh = -14     # Freezer temperature threshold
global trigger          # Variable accessible by thread functions
subject = "Freezer Alert!"
body = "Lab Freezer Alert! Temperature of freezer has exceeded " + freezeThresh + "degrees! Please investigate"

#API to call freezer data
