#include <Arduino.h>
#line 1 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino"
#include <Ticker.h>
#include <DallasTemperature.h>
#include <DHTesp.h>
#include <WiFi.h>

// Network parameters
const char* ssid = "CampbellIOT";
const char* password = "Claudin5";
const uint16_t port = 8090;
const char* host = "192.168.4.1";

// ESP32 parameters
#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

#line 18 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino"
void setup();
#line 33 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino"
void loop();
#line 18 "C:\\Users\\jeffr\\Documents\\github\\Freezer-Alarm\\Freezer-Alarm.ino"
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
