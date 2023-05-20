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
DHTesp dht;
void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
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
      if (tempTaskHandle !=NULL) {
        vTaskResume(tempTaskHandle);
      }
    }
    yield();

    Serial.println("Disconnecting...");
    client.stop();

    delay(10000);
}