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





void setup()
{
  Serial.begin(115200);

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

    Serial.println("Disconnecting...");
    client.stop();

    delay(10000);
}
