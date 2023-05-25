#include <Arduino.h>
#line 1 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino"
#include <Ticker.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <WiFi.h>

// Network parameters
const char* ssid = "SpringdaleSpringadome";
const char* password = "Springadome64";
const uint16_t port = 8090;
const char* host = "192.168.1.27";
IPAddress local_IP(192, 168, 1, 240);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

// ESP32 parameters
#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif
//https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/
const int oneWireBus = 4;  // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus);  // setup oneWire
DallasTemperature sensors(&oneWire); //pass oneWire reference to temp sensor


#line 28 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino"
void setup();
#line 48 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino"
void loop();
#line 28 "D:\\Projects\\Github\\Freezer-Alarm\\Freezer-Alarm.ino"
void setup()
{
  // Begin Serial
  Serial.begin(115200);
  sensors.begin(); // Start the DS18B20 sensor

  // Begin Network Connection
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
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
