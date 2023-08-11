from email.mime.text import MIMEText
import urllib.parse
import json
import send
import requests

freezeThresh = -14     # Freezer temperature threshold
global trigger          # Variable accessible by thread functions
subject = "Freezer Alert!"
body = "Lab Freezer Alert! Temperature of freezer has exceeded " + freezeThresh + "degrees! Please investigate"

#API to call freezer data
url = "192.168.0.100/data"     #mac of root node is 0C-DC-7E-57-A7-28
payload = {}
headers = {}
response = requests.request("GET", url, headers=headers, data=payload)
print(response.text)

print("test")