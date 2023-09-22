
#include <Arduino.h>
#include <FastLED.h>
#include <stdint.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
// Replace with your network credentials
const char* ssid     = "ESP32-Led-Cube";
const char* password = "4*4*4 ledcube";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
static const uint8_t LED_PIN = 16;
static const uint8_t width = 4;
static const uint8_t height = 4;
static const uint8_t depth = 4;
static CRGB cube[width][height][depth];
static CRGB leds[width * height * depth];
String wsmsg = "hello";
void notifyClients(String msg) {
  ws.textAll(msg);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      wsmsg = (char*)data;
      Serial.println(wsmsg);
      notifyClients(wsmsg);
    //}
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      notifyClients(wsmsg);
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
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
void setup() {
  delay(700);
  Serial.begin(115200);
  Serial.println("Starting");
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, sizeof(leds) / sizeof(CRGB));
  FastLED.setDither(false);
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println(bench("/index.html"));
  Serial.println(bench("/style.css"));
  Serial.println(bench("/favicon.ico"));
  Serial.println(bench("/script/ws.js"));
  Serial.println(bench("/script/sketch.js"));
  Serial.println(bench("/script/setup.js"));
  Serial.println(bench("/libraries/p5.min.js")); 

  Serial.println("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  initWebSocket();
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/script/ws.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script/ws.js", "text/javascript");
  });
  server.on("/script/setup.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script/setup.js", "text/javascript");
  });
  server.on("/script/sketch.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script/sketch.js", "text/javascript");
  });
  server.on("/libraries/p5.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/libraries/p5.min.js", "text/javascript");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.ico", "image/x-icon");
  });

  server.begin();
}

// Move 3D Cube data to 1D LED array
void display_update(int ms_delay) {
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
  FastLED.show();
  delay(ms_delay);
}

void display_clear() { memset(cube, 0, sizeof(cube)); }
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  static int y = 0;
  static int a = 1;

  for (int x = 0; x < width; x++) {
    cube[x][y][0] = CRGB(40, 0, 40);
    cube[x][y][1] = CRGB(50, 0, 0);
    cube[x][y][2] = CRGB(0, 80, 0);
    cube[x][y][3] = CRGB(0, 0, 200);
  }
  int t = y + a;
  if (t > height - 1 || t < 0) a = -a;
  y += a;
  display_update(200);
  display_clear();
}