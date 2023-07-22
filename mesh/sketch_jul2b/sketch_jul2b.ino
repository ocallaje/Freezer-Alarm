#include "painlessMesh.h"
#include <DallasTemperature.h>
#include <OneWire.h>
#include <WiFi.h>
#include <WiFiCredsHome.h>
#include "IPAddress.h"
#include <WebServer.h>
#include <AsyncTCP.h>

// Mesh Parameters
#define   MESH_PREFIX     "LabIOTMesh"
#define   MESH_PASSWORD   "sayyeshtothemesh"
#define   MESH_PORT       5555
painlessMesh  mesh;

// Network parameters
#define   STATION_SSID     "SpringdaleSpringadome"
#define   STATION_PASSWORD "Springadome64"
#define HOSTNAME "HTTP_BRIDGE"
WebServer server(80);
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

// Init Scheduler and tasks
Scheduler userScheduler;  // to control your personal task
void sendMessage() ;      // Initialise function
void api_setup();
Task taskSendMessage( TASK_SECOND * 5 , TASK_FOREVER, &sendMessage );
Task taskAPISetup(TASK_SECOND * 20 , TASK_FOREVER, &api_setup);

// Prototype Functions
void receivedCallback( const uint32_t &from, const String &msg );
IPAddress getlocalIP();

// Temp Sensor Parameters
const int oneWireBus = 2;  // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus);  // setup oneWire
DallasTemperature sensors(&oneWire); //pass oneWire reference to temp sensor
float temperature;
char c[] = "°C";
char tempstr[] = "temperature";

// Function to set up API routing
void setup_routing() {     
  server.on("/temperature", getTemperature);          
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
void getTemperature() {
  Serial.println("Get temperature");
  create_json(tempstr, temperature, c);
  server.send(200, "application/json", buffer);
}
void getData() {
  Serial.println("Get All Sensor Data");
  jsonDocument.clear();
  add_json_object(tempstr, temperature, c);
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

// Get local sensor data
void read_sensor_data(void * parameter) {
  Serial.println("Reading sensor data...");
  // Read Temp
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0); //index corresponds to each sensor 0 = 1st sensor
  //Serial.print(temperature);
  //Serial.println("C");
}


void setup() {
  Serial.begin(115200);   // Begin Serial  
  sensors.begin();        // Start the DS18B20 sensor

  // Begin Mesh
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );              // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6, 1 ); // Channel set to 6. 
  mesh.onReceive(&receivedCallback);                                  // set receive callback function
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);                 // set AP info for mesh
  mesh.setHostname(HOSTNAME);                                         // Give node a hostname
  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);                                                 // set this node as root node
  mesh.setContainsRoot(true);                                         // mesh contains a root node

  // Initiate Tasks
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  userScheduler.addTask( taskAPISetup );
  taskAPISetup.enable();

  // Begin API	 	 
  setup_routing(); 

  //Print node ID
  Serial.print("Node ID: ");
  Serial.println( mesh.getNodeId());
}

void loop() {
  mesh.update();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
  server.handleClient();	// Listen for API events
}


//Receive Mesh Data Callback Function 
void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}


// Tasks
void sendMessage() {
  String msg = "Hi from node1";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( TASK_SECOND * 1 );
  //Serial.println(msg);

  // Read Temp
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0); //index corresponds to each sensor 0 = 1st sensor
  //Serial.print(tempC);
  //Serial.println("C");

}

void api_setup() {
  Serial.println("Initialising API...");
  read_sensor_data(NULL);
}
