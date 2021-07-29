#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Temboo.h>
#include <TembooAccount.h> // Contains Temboo account information
#include "DHT.h"

WiFiClient client;

// DHT11 sensor pins
#define DHTPIN 14
#define DHTTYPE DHT11

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);
// Update these Information
const char* WIFI_SSID = "TP-Link_8700";                    // WiFi Name
const char* WPA_PASSWORD = "iotapp8266";            // WiFi Password

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 10;   // Maximum number of times the Choreo should be executed

// Setup WiFi Connection
void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WPA_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  // Init DHT 
  dht.begin();

  int wifiStatus = WL_IDLE_STATUS;

//  // Determine if the WiFi Shield is present
//  Serial.print("\n\nShield:");
//  if (WiFi.status() == WL_NO_SHIELD) {
//    Serial.println("FAIL");
//
//    // If there's no WiFi shield, stop here
//    while(true);
//  }
    setup_wifi();

  Serial.println("OK");

//  // Try to connect to the local WiFi network
//  while(wifiStatus != WL_CONNECTED) {
//    Serial.print("WiFi:");
//    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);
//
//    if (wifiStatus == WL_CONNECTED) {
//      Serial.println("OK");
//    } else {
//      Serial.println("FAIL");
//    }
//    delay(5000);
//  }

  Serial.println("Setup complete.\n");
}

void loop() {
  
  if (numRuns <= maxRuns) {
    Serial.println("Running StatusesUpdate - Run #" + String(numRuns++));

    // Read data
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    TembooChoreo StatusesUpdateChoreo(client);

    // Invoke the Temboo client
    StatusesUpdateChoreo.begin();

    // Set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String StatusUpdateValue = "The temperature is " + String(t) + " and the humidity is " + String(h) + ".";
    Serial.println(StatusUpdateValue);
    StatusesUpdateChoreo.addInput("StatusUpdate", StatusUpdateValue);
    String AccessTokenValue = "144185944-6SpHl29v8W3XARPG1zAekOtbumWBLTqpThWWsF7r";
    StatusesUpdateChoreo.addInput("AccessToken", AccessTokenValue);
    String ConsumerKeyValue = "zlwdKs4qnl6cxisx33ZtfDLfy";
    StatusesUpdateChoreo.addInput("ConsumerKey", ConsumerKeyValue);
    String ConsumerSecretValue = "YZJNtLKDItUeEiXt5xT9d6mDBi1YlMpPYqUyBdkb3JDE07RGtr";
    StatusesUpdateChoreo.addInput("ConsumerSecret", ConsumerSecretValue);
    String AccessTokenSecretValue = "jTgzgHZoiRz5bU7o2pIAcq0ijEiqfu5RdhYXMFONb3e7O";
    StatusesUpdateChoreo.addInput("AccessTokenSecret", AccessTokenSecretValue);

    // Identify the Choreo to run
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");

    // Run the Choreo; when results are available, print them to serial
    StatusesUpdateChoreo.run();

    while(StatusesUpdateChoreo.available()) {
      char c = StatusesUpdateChoreo.read();
      Serial.print(c);
    }
    StatusesUpdateChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(90000); // wait 90 seconds between StatusesUpdate calls
}
