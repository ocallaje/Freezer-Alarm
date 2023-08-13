#import json
import send
import requests
import sched, time

freezeThresh = -14     # Freezer temperature threshold
subject = "Freezer Alert!"
body = "Lab Freezer Alert! Temperature of freezer has exceeded " + str(freezeThresh) + "degrees! Please investigate"
testmsg = 1
#initialise alert variables

#API to call freezer data
def freezeAPI():
  url = "http://192.168.0.100/data"     #mac of root node is 0C-DC-7E-57-A7-28
  payload = {}
  headers = {}
  try:
    response = requests.request("GET", url, headers=headers, data=payload, timeout=5, verify=True)
    response.raise_for_status()
    print(response.text)
    return response
  except requests.exceptions.HTTPError as errh:
    print("HTTP Error")
    print(errh.args[0])
  except requests.exceptions.ReadTimeout as errrt:
    print("Time out")
  except requests.exceptions.ConnectionError as conerr:
    print("Connection error")
  except requests.exceptions.RequestException as errex:
    print("Exception request")
    

# Parse the JSON data - Iterate through each "FreezerDevice"
def parseJSON(response):
  for idx, freezer_device in enumerate(response.json()["FreezerDevices"]):
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
              if value > freezeThresh:
                #store time in a varaible here and check if its been "time" since 
                send.sendEmail("Freezer Temperature Alert!", body, send.allRecipients)
                send.sendSMS(body, testmsg)
                
    else:
        print("No sensor data available for this FreezerDevice")
        connBody = "Freezer Device " +str(idx) + " is not detected. Please reconnect this device to the mesh."
        send.sendEmail("Freezer Connection Alert!", connBody, send.maintainers)


## Scheduler and Main Loop
def checkFreezers(scheduler):
   # schedule the next call
   scheduler.enter(60, 1, checkFreezers, (scheduler,))
   print("Checking Freezer Data")
   response = freezeAPI()
   parseJSON(response)

my_scheduler = sched.scheduler(time.time, time.sleep)
my_scheduler.enter(60, 1, checkFreezers, (my_scheduler,))
my_scheduler.run()