# 1 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino"
# 2 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino" 2
# 3 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino" 2
# 4 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino" 2
# 5 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino" 2

// Network parameters
const char* ssid = "CampbellIOT";
const char* password = "Claudin5";
const uint16_t port = 8090;
const char* host = "192.168.4.1";

// ESP32 parameters




DHTesp dht;
void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = 
# 23 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino" 3 4
                             __null
# 23 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino"
                                 ;
/** Ticker for temperature reading */
Ticker tempTicker;
/** Comfort profile */
ComfortState cf;
/** Flag if task should run */
bool tasksEnabled = false;
/** Pin number for DHT11 data pin */
int dhtPin = 17;





void setup()
{
  // Begin Serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("DHT ESP32 example with tasks");
  initTemp();

  // Begin Network Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }

  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

 // Signal end of setup() to tasks
  tasksEnabled = true;
}

void loop()
{
    WiFiClient client; //Establish connection

    // error checking for failed connection
    if (!client.connect(host, port)) {

        Serial.println("Connection to host failed");

        delay(1000);
        return;
    }
    // Print serial
    Serial.println("Connected to server successful!");
    // Send to client
    client.print("Hello from ESP32!");

    if (!tasksEnabled) {
      delay(2000)
      tasksEnabled = true;
      if (tempTaskHandle !=
# 79 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino" 3 4
                          __null
# 79 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino"
                              ) {
        vTaskResume(tempTaskHandle);
      }
    }
    yield();

    Serial.println("Disconnecting...");
    client.stop();

    delay(10000);
}
