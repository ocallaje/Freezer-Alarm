"""
Freezer Alarm Python Monitor
"""
import sched
import time
import send
import requests
import datetime

# initialise alert constants
FREEZE_THRESH = -14  # Freezer temperature threshold
SUBJECT = "Freezer Alert!"
BODY = (
    "Lab Freezer Alert! Temperature of freezer has exceeded "
    + str(FREEZE_THRESH)
    + "degrees! Please investigate"
)
TESTMSG = 1
ALERT_INIT = datetime.datetime.now()
TIME_CHANGE = datetime.timedelta(hours=2.0)


# API to call freezer data
def freeze_api():
    """Function to collect freezer data"""
    url = "http://192.168.0.100/data"  # mac of root node is 0C-DC-7E-57-A7-28
    payload = {}
    headers = {}
    try:
        response = requests.request(
            "GET", url, headers=headers, data=payload, timeout=5, verify=True
        )
        response.raise_for_status()
        print(response.text)
        return response
    except requests.exceptions.HTTPError as errh:
        print("HTTP Error")
        print(errh.args[0])
        return None
    except requests.exceptions.ReadTimeout as errrt:
        print("Time out")
        print(errrt.args[0])
        return None
    except requests.exceptions.ConnectionError as conerr:
        print("Connection error")
        print(conerr.args[0])
        return None
    except requests.exceptions.RequestException as errex:
        print("Exception request")
        print(errex.args[0])
        return None


# Parse the JSON data - Iterate through each "FreezerDevice"
def parse_data(response, alerted_time):
    """Function to parse freezer's JSON data"""
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
                print(
                    f"Sensor {index} - FreezerID: {freezer_id}, Connected: {connected}, Value: {value} {unit}"
                )

                # Check if sensor is connected
                if connected == 1:
                    print(f"Connected value: {value}{unit}")
                    if value > FREEZE_THRESH:
                        send.send_email(
                            "Freezer Temperature Alert!", BODY, send.allRecipients
                        )
                        send.send_sms(BODY, TESTMSG)
                        return datetime.datetime.now()
                    return alerted_time
                return alerted_time

        else:
            print("No sensor data available for this FreezerDevice")
            conn_body = (
                "Freezer Device "
                + str(idx)
                + " is not detected. Please reconnect this device to the mesh."
            )
            send.send_email("Freezer Connection Alert!", conn_body, send.maintainers)
            return datetime.datetime.now()


## Scheduler and Main Loop
def check_freezers(scheduler):
    '''Main function to repeatedly process freezer data'''
    global ALERT_INIT
    # schedule the next call
    scheduler.enter(60, 1, check_freezers, (scheduler,))

    alert_time = datetime.datetime.now() # time when functions called
    if alert_time > ALERT_INIT + TIME_CHANGE:
        print("Checking Freezer Data")  
        response = freeze_api()
        ALERT_INIT = parse_data(response, ALERT_INIT)
        print(ALERT_INIT)


my_scheduler = sched.scheduler(time.time, time.sleep)
my_scheduler.enter(60, 1, check_freezers, (my_scheduler,))
my_scheduler.run()
