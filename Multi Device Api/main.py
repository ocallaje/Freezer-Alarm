from email.mime.text import MIMEText
import urllib.parse
import json
import send
import requests

freezeThresh = -14     # Freezer temperature threshold
global trigger          # Variable accessible by thread functions
subject = "Freezer Alert!"
body = "Lab Freezer Alert! Temperature of freezer has exceeded " + str(freezeThresh) + "degrees! Please investigate"

#API to call freezer data
url = "http://192.168.0.100/data"     #mac of root node is 0C-DC-7E-57-A7-28
payload = {}
headers = {}
response = requests.request("GET", url, headers=headers, data=payload)
print(response.text)


# Parse the JSON data
# Iterate through each "FreezerDevice"
for freezer_device in response.json()["FreezerDevices"]:
    sensor_array = freezer_device["sensorArray"]
    
    # Check if "sensorArray" is not None
    if sensor_array is not None:
        # Iterate through each sensor element in the "sensorArray"
        for sensor in sensor_array:
            index = sensor["index"]
            freezer_id = sensor["FreezerID"]
            connected = sensor["Connected"]
            value = sensor["value"]
            unit = sensor["unit"]
            
            # Print sensor information
            print(f"Sensor {index} - FreezerID: {freezer_id}, Connected: {connected}, Value: {value} {unit}")

            #Check if sensor is connected
            if connected == 1:
                print(f"Connected value: {value}{unit}")
                
    else:
        print("No sensor data available for this FreezerDevice")