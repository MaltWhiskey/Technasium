
#include <Arduino.h>
#include "space/Animation.h"
#include <WiFi.h>
#include "time.h"
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
const char* ssid = "DEV";
const char* password = "VgXkh1quRYh5";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
bool updateTime() {
  unsigned long start = millis();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("connecting to: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
      Serial.print(".");
      Animation::animate();

    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected.");

    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
  struct tm timeinfo;
  if (WiFi.status() == WL_CONNECTED && getLocalTime(&timeinfo)) {
    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return true;
  }
  return false;
}
bool valid_time = false;
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 1000)
    Serial.print("Connecting to ");
  Serial.println("Starting");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  Animation::begin();
  valid_time = updateTime();
  pinMode(15,OUTPUT);
}
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  // Print FPS once every second
  static Timer timecheckinterval = 30.0f;
  static Timer print_interval = 1.0f;
  Animation::animate();
  if (print_interval.update()) {
    Serial.print("fps=");
    Serial.print(Animation::fps());
    Serial.println();
  }
  // go in to deep sleep when time is after 16:00
  struct tm timeinfo;
  digitalWrite(15,!valid_time);
  if ((!valid_time || !getLocalTime(&timeinfo))) {
    if (timecheckinterval.update()) {
      Serial.println("Failed to obtain time");
      valid_time = updateTime();
    }
  } else if (timecheckinterval.update() && timeinfo.tm_hour >= 16 && timeinfo.tm_min >= 35) {
    Serial.println("Going to sleep");
    Animation::next();
    unsigned long start = millis();
    while (millis() - start < 5000) {
      Animation::animate();
    }

    ESP.deepSleep(56400000000ULL);
  }
}