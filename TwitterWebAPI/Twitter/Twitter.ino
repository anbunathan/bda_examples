#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <ArduinoJson.h>                  // https://github.com/bblanchon/ArduinoJson
#include "secret.h"                       // uncomment if using secret.h file with credentials
//#define TWI_TIMEOUT 3000                  // varies depending on network speed (msec), needs to be before TwitterWebAPI.h
#include <TwitterWebAPI.h>
#include "DHT.h"

// DHT11 sensor pins
#define DHTPIN 14
#define DHTTYPE DHT11
// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

#ifndef WIFICONFIG
const char* ssid = "WiFi SSID";           // WiFi SSID
const char* password = " WiFi Password";   // WiFi Password
#endif

std::string search_str = "#dog";          // Default search word for twitter
const char *ntp_server = "pool.ntp.org";  // time1.google.com, time.nist.gov, pool.ntp.org
int timezone = -5;                        // US Eastern timezone -05:00 HRS
unsigned long twi_update_interval = 120;   // (seconds) minimum 5s (180 API calls/15 min). Any value less than 5 is ignored!

#ifndef TWITTERINFO  // Obtain these by creating an app @ https://apps.twitter.com/
  static char const consumer_key[]    = "consumer_key";
  static char const consumer_sec[]    = "consumer_sec";
  static char const accesstoken[]     = "accesstoken";
  static char const accesstoken_sec[] = "accesstoken_sec";
#endif

//   Dont change anything below this line    //
///////////////////////////////////////////////

unsigned long api_mtbs = twi_update_interval * 1000; //mean time between api requests
unsigned long api_lasttime = 0; 
bool twit_update = false;
std::string search_msg = "No Message Yet!";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, timezone*3600, 60000);  // NTP server pool, offset (in seconds), update interval (in milliseconds)
TwitterClient tcr(timeClient, consumer_key, consumer_sec, accesstoken, accesstoken_sec);

void setup(void){
  //Begin Serial
  Serial.begin(115200);
  // WiFi Connection
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected. yay!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(100);
  // Connect to NTP and force-update time
  tcr.startNTP();
  Serial.println("NTP Synced");
  delay(100);
  // Setup internal LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  if (twi_update_interval < 5) api_mtbs = 5000; // Cant update faster than 5s.
}

void loop(void){
  
  if (millis() > api_lasttime + api_mtbs)  {
    
    digitalWrite(LED_BUILTIN, LOW);
//    extractJSON(tcr.searchTwitter(search_str));
//    Serial.print("Search: ");
//    Serial.println(search_str.c_str());
//    Serial.print("MSG: ");
//    Serial.println(search_msg.c_str()); 
    // Read data
    float h ;
    float t ;
    int count=0;
    do{
      h = dht.readHumidity();
      t = dht.readTemperature();
      count++;
      if(count>=25){
        h = 0;
        t = 0;
        String twittermsg = "The temperature is " + String(t) + " and the humidity is " + String(h) + ".";
        break;
      }
      delay(5000);
    }while(isnan(h) || isnan(t));
    String temp = "#temperature " + String(t);
    Serial.println(temp); 
    std::string temp_post;
    temp_post = std::string(temp.c_str(), temp.length());
    tcr.tweet(temp_post);

    String hum = "#humidity " + String(h);
    Serial.println(hum); 
    std::string hum_post;
    hum_post = std::string(hum.c_str(), hum.length());
    tcr.tweet(hum_post);
    api_lasttime = millis();
  }
//  delay(2);
//  yield();
  digitalWrite(LED_BUILTIN, HIGH);
}
