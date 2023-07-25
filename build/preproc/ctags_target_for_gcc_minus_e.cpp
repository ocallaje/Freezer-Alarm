# 1 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino"
# 2 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino" 2
# 3 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino" 2
# 4 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino" 2
# 5 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino" 2
# 6 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino" 2
# 7 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino" 2
# 8 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino" 2


// Mesh Parameters



painlessMesh mesh;
Scheduler userScheduler; // to control your personal task

// Network parameters



WebServer server(80);
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

// Initialise vars
void receivedCallback( uint32_t from, String &msg );
StaticJsonDocument<1024> SensorHub;
int nidx;
const int arraySize = 3;
String dataArray[arraySize] = {"a", "b", "c"};
String idtable[arraySize] = {"0","0","0"};

//temporary
int temperature = 0;
String tempstr = "test";
String C = "c";

//***
// Begin Tasks

// Send my ID every 60 seconds to inform others
Task logServerTask(60000, (-1), []() {
  DynamicJsonDocument jsonBuffer(1024);
  JsonObject msg = jsonBuffer.to<JsonObject>();
  msg["topic"] = "logServer";
  msg["nodeId"] = mesh.getNodeId();
  String str;
  serializeJson(msg, str);
  mesh.sendBroadcast(str);
});

// Create JSON
Task compileJSON(10000, (-1), []() {
  Serial.println("Starting compile task");
  SensorHub.clear(); // Clear JSON file
  JsonArray SensorDevice = SensorHub.createNestedArray("FreezerDevices"); // Create array for Freezers
  StaticJsonDocument<1024> objdoc; // Create temp JSON doc 
  for (int i =0; i< arraySize; i++) { // for every freezer
    deserializeJson(objdoc, dataArray[i]); // convert str array to json
    JsonObject obj = SensorDevice.createNestedObject(); // Create nested freezer object
    //obj["index"] = objdoc["index"];
    //obj["FreezerID"] = objdoc["FreezerID"];                               // build json object from temp
    //obj["Connected"] = objdoc["Connected"];
    //obj["value"] = objdoc["value"];
    //obj["unit"] = objdoc["unit"];
    obj["sensorArray"] = objdoc["sensorArray"];
  }
  // log to serial
  serializeJson(SensorHub, Serial);
  Serial.printf("\n");
});

//*** End Tasks

//***
// Begin API Functions

// Function to set up API routing
void setup_routing() {
  server.on("/data", getData);
  server.begin();
}
// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];
void create_json(char *tag, float value, char *unit) {
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}
void add_json_object(char *tag, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit;
}
// API Implementation
void getData() {
  Serial.println("Get All Sensor Data");
  jsonDocument.clear();
  // add_json_object(tempstr, temperature, c);
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

// Get local sensor data
void read_sensor_data(void * parameter) {
  Serial.println("Reading sensor data...");
  // Read Temp
  //sensors.requestTemperatures();
  //temperature = sensors.getTempCByIndex(0); //index corresponds to each sensor 0 = 1st sensor
  //Serial.print(temperature);
  //Serial.println("C");
}

void api_setup() {
  Serial.println("Initialising API...");
  read_sensor_data(
# 120 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino" 3 4
                  __null
# 120 "D:\\Projects\\Github\\Freezer-Alarm\\mesh\\mesh_root\\mesh_root.ino"
                      );
}
//*** End API


void setup() {
  Serial.begin(115200);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | DEBUG ); // all types on
  mesh.setDebugMsgTypes( ERROR | CONNECTION ); // set before init() so that you can see startup messages
  mesh.init( "LabIOTMesh", "sayyeshtothemesh", &userScheduler, 5555, WIFI_MODE_APSTA, 6, 1 );
  mesh.stationManual("CampbellIOT", "Claudin5"); // set AP info for mesh
  mesh.onReceive(&receivedCallback);
  //Newly Connected Node
  mesh.onNewConnection([](size_t nodeId) {
    Serial.printf("New Connection %u\n", nodeId);
    int nodes = mesh.getNodeList().size();
    Serial.println("Total number of nodes");
    Serial.println(nodes);
  });
  //Disconnected Node
  mesh.onDroppedConnection([](size_t nodeId) {
    Serial.printf("Dropped Connection %u\n", nodeId);
    for (int i = 0; i < arraySize; i++) {
      //Serial.println(idtable[i]);
      if (idtable[i] == String(nodeId)) { // Find index matching node ID
        dataArray[i].replace("\"Connected\":\"1\"", "\"Connected\":\"0\""); // set connection to 0
      }
    }
  });

  // Add the tasks to the scheduler
  userScheduler.addTask(logServerTask);
  logServerTask.enable();
  userScheduler.addTask(compileJSON);
  compileJSON.enable();

  // Begin API	 	 
  setup_routing();
}

void loop() {
  mesh.update(); // Renew mesh and tasks
  server.handleClient(); // Listen for API events
}

void receivedCallback( uint32_t from, String &msg ) {
  //Serial.printf("logServer: Received from %u msg=%s\n", from, msg.c_str());
  const char* newdata = msg.c_str(); // get data from node
  //Serial.println(newdata);
  StaticJsonDocument<200> tempdoc; // create temp json doc
  deserializeJson(tempdoc, newdata); // save node string to json doc
  nidx = tempdoc["index"]; // get index from json key
  dataArray[nidx] = newdata; // save string data to string array in correct index position
  const char* nodename = tempdoc["nodeID"]; // get node id from json
  idtable[nidx] = nodename; // save node name in idtable in correct index position
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}
