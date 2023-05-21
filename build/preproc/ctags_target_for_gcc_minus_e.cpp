# 1 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino"
# 2 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino" 2

# 4 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino" 2
# 5 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino" 2

# 7 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino" 2

// Network parameters
const char* ssid = "CampbellIOT";
const char* password = "Claudin5";
const uint16_t port = 8090;
const char* host = "192.168.4.1";

// ESP32 parameters




//https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/
const int oneWireBus = 4; // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus); // setup oneWire
DallasTemperature sensors(&oneWire); //pass oneWire reference to temp sensor


void setup()
{
  // Begin Serial
  Serial.begin(115200);
  sensors.begin(); // Start the DS18B20 sensor

  // Begin Network Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }

  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
    // WIFI
    WiFiClient client; //Establish connection

    // error checking for failed connection
    if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    Serial.println("Connected to server successful!");

    // Read Temp
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0); //index corresponds to each sensor 0 = 1st sensor
    Serial.print(tempC);
    Serial.println("C");

   // Send temperature to client
    client.print(tempC);

    // Close connection
    Serial.println("Disconnecting...");
    client.stop();
    delay(10000);
}
