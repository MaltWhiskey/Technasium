#include <Arduino.h>
#include <FastLED.h>
#include <stdint.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
// DNSServer dnsServer;
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);


/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
static const char *ap_ssid = "ESP32-Led-Cube";
static const char *ap_password = "4*4*4 ledcube";
// IPAddress apIP(8,8,4,4); // The default android DNS
static const uint8_t LED_PIN = 16;
static const uint8_t width = 4;
static const uint8_t height = 4;
static const uint8_t depth = 4;
static CRGB cube[width][height][depth];
static CRGB leds[width * height * depth];
bool apmode = false;
char *wsmsg;
void notifyClients(String msg) {
  webSocket.broadcastTXT(msg);
}
void display_clear() { memset(cube, 0, sizeof(cube)); }
// Move 3D Cube data to 1D LED array
void display_update() {
  for (uint8_t x = 0; x < width; x++) {
    uint8_t xx = x * height;
    for (uint8_t y = 0; y < height; y++) {
      uint8_t yy = (x & 1) ? height - 1 - y : y;
      for (uint8_t z = 0; z < depth; z++) {
        uint8_t zz = (z + (z & 1)) * height * width;
        uint8_t led = (z & 1) ? zz - xx - yy - 1 : xx + yy + zz;
        leds[led] = cube[x][y][z];
      }
    }
  }
  Serial.println("led show");
  FastLED.show();
}
int hex_to_val(const char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';  /* Simple ASCII arithmetic */
    else if (ch >= 'a' && ch <= 'f')
        return 10 + ch - 'a';  /* Because hex-digit a is ten */
    else if (ch >= 'A' && ch <= 'F')
        return 10 + ch - 'A';  /* Because hex-digit A is ten */
    else
        return 0;  /* Not a valid hexadecimal digit */
}

void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t* payload,
                      size_t length) {
  switch (type) {
    // Client has disconnected
    case WStype_DISCONNECTED: {
      Serial.printf("Disconnected client %u\n", client_num);
    } break;
    // New client has connected
    case WStype_CONNECTED: {
      Serial.printf("Connection from client %u\n", client_num);
    } break;
    // Text received from a connected client
    case WStype_TEXT: {
      String data = String((char*) payload);
      Serial.println(data);
      Serial.println(length);
      notifyClients(data);
      byte x = 0;
      byte y = 0;
      byte z = 0;
      for (int i = 0 ; i<length; i++){
        uint32_t hex = 0;
        if (data[i] == '#' && i+7 < length && length == 603){
          Serial.println(i);
          for(int j=0;j<6;j++) {
            int chr = round(hex_to_val(data[j+i+1]));
            Serial.print(chr+" ");
            hex<<=4;
            hex |= chr;
          }
          Serial.println(hex);
          i += 7;
          cube[x][3-y][z] = hex;
          x++;
          if (x > 3){
            y++;
            x=0;
            if (y > 3){
              z++;
              y=0;
              if (z > 3){
                z = 3;
              }
            }
          }
        }
      }
      Serial.println("update display");
      display_update();
      Serial.println("clear display buffer");
      display_clear();
      Serial.println("frame done");
    } break;
    case WStype_BIN:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    case WStype_ERROR:
    case WStype_PING:
    case WStype_PONG:
      break;
  }
}



String bench(String filename) {
  unsigned long before = millis();
  File file = SPIFFS.open(filename);
  unsigned long open = millis();
  if(!file){
    Serial.println("Failed to open file for reading");
  } else{
    Serial.println("SPIFFS working");
  }
  while (file.available())
  {
    file.read();
  }
  file.close();
  
  return filename + " open in " + String(open - before) + "ms\t read in " + String(millis()-open) + "ms";
}
bool connectwifi(String ssid,String password){
  Serial.println("connecting to " + ssid);
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  unsigned long last = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis()-last > 200){
      Serial.print(".");
      last = millis();
    }
    digitalWrite(15,millis() % 500 > 250);
    if (millis() - start > 5000){
      Serial.println("\ntimeout for "+ssid);
      return false;
    }
  }
  Serial.println("\nconnected");
  return true;
}
StaticJsonDocument<500> config;
void setup() {
  pinMode(15,OUTPUT);
  digitalWrite(15,true);
  Serial.begin(115200);
  unsigned long start = millis();
  bool failed = false;
  while (!Serial && !failed){
    digitalWrite(15,millis() % 200 > 100);
    if (millis()-start > 5000){
      failed = true;
    }
  }
  digitalWrite(15,true);
  // delay(1000);
  Serial.println("Starting");
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, sizeof(leds) / sizeof(CRGB));
  FastLED.setDither(false);
  // FastLED.setBrightness(0x60);
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  String configraw;
  File file = SPIFFS.open("/config.json");
  if(!file){
    Serial.println("Failed to open file for reading");
  } else{
    Serial.println("SPIFFS working");
  }
  while (file.available())
  {
    configraw = file.readString();
  }
  file.close();
  DeserializationError error = deserializeJson(config, configraw);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  bool done = false;
  for (JsonVariant value : config["wifi"].as<JsonArray>()){
    if (!done){
      done = connectwifi(value["ssid"].as<String>(),value["password"].as<String>());
    }
  }
  IPAddress IP;
  if (!done){
    // WiFi.softAPConfig();
    WiFi.disconnect(false,true);
    WiFi.softAP(ap_ssid, ap_password);
    // WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    // dnsServer.start(53, "*", apIP);
    IP = WiFi.softAPIP();
    server.on("/",HTTP_GET,[](AsyncWebServerRequest * request){
      request->send(SPIFFS,"/indexap.html","text/html");
    });
    apmode = true;
  } else {
    IP = WiFi.localIP();
    apmode = false;
  }
  Serial.print("IP address: ");
  Serial.println(IP);
  webSocket.onEvent(onWebSocketEvent);
  webSocket.begin();
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.begin();
  ArduinoOTA.setHostname("ESP32-Led-Cube");
  ArduinoOTA.setPassword("NotGonnaGuessThis");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  digitalWrite(15,false);
}


/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  webSocket.loop();
  ArduinoOTA.handle();
  if (apmode) {
    // dnsServer.processNextRequest();
    digitalWrite(15,(millis() % 2500 > 2200 && millis() % 2500 < 2300)||millis() % 2500 > 2400);
  } else {
    digitalWrite(15,millis() % 2500 > 2400);
  }
}