# 1 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_logClient\\mesh_logClient.ino"
//************************************************************
// This device uses a mesh to communicate freezer sensor data 
// to a central node. Up to 4 DS18B20 probes can be connected. 
// Device will monitor sensor status and send all data to root
// node with a JSON document.
//************************************************************
# 8 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_logClient\\mesh_logClient.ino" 2
# 9 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_logClient\\mesh_logClient.ino" 2
# 10 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_logClient\\mesh_logClient.ino" 2

// Mesh Parameters




Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

// Temp Sensors

OneWire oneWire(2 /* Data wire is plugged into digital pin 2 on the Arduino*/); // Setup a oneWire instance to 
DallasTemperature sensors(&oneWire); // Pass oneWire reference to DallasTemperature library

// Temp sensor vars
const int numSensors = 4;
const int controllerIndex = 0;
int deviceCount = 0;
float tempC;
float tempSensors[numSensors] = {0,0,0,0};
int connectedSensor[numSensors] = {0,0,0,0};

// Prototype
void receivedCallback( uint32_t from, String &msg );
size_t logServerId = 0; // init server id

// Send message to the logServer every 10 seconds 
Task myLoggingTask(20000, (-1), []() {
  DynamicJsonDocument jsonBuffer(1024);
  JsonArray sensorArray = jsonBuffer.createNestedArray("sensorArray");
  for (int i = 0; i < numSensors; i++){
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

  JsonArray indexArray = jsonBuffer.createNestedArray("ctlindex");
  indexArray.add(controllerIndex);

  String str;
  serializeJson(jsonBuffer, str);
  //Serial.println("full str");
  //Serial.println(str);
  if (logServerId == 0) // If we don't know the logServer yet
    mesh.sendBroadcast(str);
  else
    mesh.sendSingle(logServerId, str);
  serializeJson(jsonBuffer, Serial); // Log to serial
  Serial.printf("\n");
  Serial.println("");
});

// Task to read temp sensors
Task readTemps(10000, (-1), []() {
  sensors.requestTemperatures(); // Get all temps
  for (int i = 0; i < numSensors; i++){
    tempC = sensors.getTempCByIndex(i); // Temp of ith sensor
    tempSensors[i] = tempC; // Store temp reading
    //Serial.println(tempSensors[i]);
    if (tempC != -127) {
      connectedSensor[i] = 1; // Sensor is connected
      } else {
      connectedSensor[i] = 0; // Sensor is disconnected
    }
  }
});

void setup() {
  Serial.begin(115200);

  // Start mesh
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION ); // set before init() to see start msgs
  mesh.init( "LabIOTMesh", "sayyeshtothemesh", &userScheduler, 5555, WIFI_MODE_APSTA, 6, 1);
  mesh.setContainsRoot(true);
  mesh.onReceive(&receivedCallback);

  // Add the task to the your scheduler
  userScheduler.addTask(myLoggingTask);
  myLoggingTask.enable();
  userScheduler.addTask(readTemps);
  readTemps.enable();

  sensors.begin(); // Start temp sensors
  deviceCount = sensors.getDeviceCount(); // locate devices on the bus

}

void loop() {
  mesh.update(); // Renew mesh and tasks
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
