#include <Arduino.h>
#line 1 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_logClient\\mesh_logClient.ino"
//************************************************************
// This device uses a mesh to communicate freezer sensor data 
// to a central node. Up to 4 DS18B20 probes can be connected. 
// Device will monitor sensor status and send all data to root
// node with a JSON document.
//************************************************************
#include <OneWire.h>
#include <DallasTemperature.h>
#include <painlessMesh.h>

#define   MESH_PREFIX     "LabIOTMesh"
#define   MESH_PASSWORD   "sayyeshtothemesh"
#define   MESH_PORT       5555

Scheduler     userScheduler;                // to control your personal task
painlessMesh  mesh;

// Temp Sensors
#define ONE_WIRE_BUS 2                      // Data wire is plugged into digital pin 2 on the Arduino
OneWire oneWire(ONE_WIRE_BUS);	            // Setup a oneWire instance to 
DallasTemperature sensors(&oneWire);        // Pass oneWire reference to DallasTemperature library

// Temp sensor vars
const int numSensors = 4;
int deviceCount = 0;
float tempC;
float tempSensors[numSensors] = {0,0,0,0};
int connectedSensor[numSensors] = {0,0,0,0};

// Prototype
void receivedCallback( uint32_t from, String &msg );
size_t logServerId = 0;                     // init server id

// Send message to the logServer every 10 seconds 
Task myLoggingTask(10000, TASK_FOREVER, []() {
  DynamicJsonDocument jsonBuffer(1024);
  JsonArray sensorArray = jsonBuffer.createNestedArray("sensorArray");
  for (int i = 0;  i < numSensors;  i++){
    JsonObject msg = sensorArray.createNestedObject();
    msg["index"] = i;
    msg["FreezerID"] = "F"+String(i+1);
    msg["Connected"] = connectedSensor[i];
    //Serial.println(mesh.getNodeId());
    msg["value"] = tempSensors[i];
    msg["unit"] = "C";
  }
  JsonArray infoArray = jsonBuffer.createNestedArray("nodeID");
  //JsonObject nodeInfo = infoArray.createNestedObject();
  //nodeInfo["nodeId"] = String(mesh.getNodeId());
  infoArray.add(String(mesh.getNodeId()));

  JsonArray indexArray = jsonBuffer.createNestedArray("index");
  indexArray.add("0");

  String str;
  serializeJson(jsonBuffer, str);
  if (logServerId == 0)                     // If we don't know the logServer yet
    mesh.sendBroadcast(str);
  else
    mesh.sendSingle(logServerId, str);
  serializeJson(jsonBuffer, Serial);        // Log to serial
  Serial.printf("\n");
  Serial.println("");
});

// Task to read temp sensors
Task readTemps(1000, TASK_FOREVER, []() {
  sensors.requestTemperatures();            // Get all temps
  for (int i = 0;  i < numSensors;  i++){
    tempC = sensors.getTempCByIndex(i);     // Temp of ith sensor
    tempSensors[i] = tempC;                 // Store temp reading
    //Serial.println(tempSensors[i]);
    if (tempC != -127) {
      connectedSensor[i] = 1;               // Sensor is connected
      } else {
      connectedSensor[i] = 0;               // Sensor is disconnected
    }
  }
});

#line 81 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_logClient\\mesh_logClient.ino"
void setup();
#line 98 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_logClient\\mesh_logClient.ino"
void loop();
#line 81 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_logClient\\mesh_logClient.ino"
void setup() {
  Serial.begin(115200);
  sensors.begin();                          // Start temp sensors
  deviceCount = sensors.getDeviceCount();   // locate devices on the bus

  // Start mesh
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() to see start msgs
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);

  // Add the task to the your scheduler
  userScheduler.addTask(myLoggingTask);
  myLoggingTask.enable();
  userScheduler.addTask(readTemps);
  readTemps.enable();
}

void loop() {
  mesh.update();                            // Renew mesh and tasks
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("logClient: Received from %u msg=%s\n", from, msg.c_str());

  // Saving logServer id
  DynamicJsonDocument jsonBuffer(1024 + msg.length());
  DeserializationError error = deserializeJson(jsonBuffer, msg);
  if (error) {
    Serial.printf("DeserializationError\n");
    return;
  }
  
  JsonObject root = jsonBuffer.as<JsonObject>();
  if (root.containsKey("topic")) {
      if (String("logServer").equals(root["topic"].as<String>())) {
          // check for on: true or false
          logServerId = root["nodeId"];
          Serial.printf("logServer detected!!!\n");
      }
      Serial.printf("Handled from %u msg=%s\n", from, msg.c_str());
  }
}

