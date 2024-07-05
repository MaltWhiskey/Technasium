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
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
StaticJsonDocument<500> config;

/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
static const char *ap_password = "4*4*4 ledcube";
static const uint8_t LED_PIN = 16;
static const uint8_t width = 4;
static const uint8_t height = 4;
static const uint8_t depth = 4;
static CRGB cube[width][height][depth];
static CRGB leds[width * height * depth];
static CRGB frames[60][width][height][depth];
char hostname[27];
unsigned long lastmils = millis();
String data = "";
String wsmsg = data;
uint8_t current = 0;
uint8_t framessize = 0;
float fps = 3;
bool apmode = false;
bool playing = false;
void setup();
void loop();
void notifyClients(String msg);
void display_clear() { memset(cube, 0, sizeof(cube)); }
void display_update();
int hex_to_val(const char ch);
void pushframe(uint16_t frame);
void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload,size_t length);
String bench(String filename);
bool connectwifi(String ssid, String password);
void initwifi();
void puship(IPAddress IP);




void setup()
{
  pinMode(15, OUTPUT);
  pinMode(0, INPUT);
  digitalWrite(15, true);
  Serial.begin(115200);
  unsigned long start = millis();
  bool failed = false;
  while (!Serial && !failed)
  {
    digitalWrite(15, millis() % 200 > 100);
    if (millis() - start > 5000)
    {
      failed = true;
    }
  }
  digitalWrite(15, true);
  // delay(1000);
  Serial.println("Starting");
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, sizeof(leds) / sizeof(CRGB));
  FastLED.setDither(false);
  // FastLED.setBrightness(0x60);
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  String configraw;
  File file = SPIFFS.open("/config.json");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
  }
  else
  {
    Serial.println("SPIFFS working");
  }
  while (file.available())
  {
    configraw = file.readString();
  }
  file.close();
  DeserializationError error = deserializeJson(config, configraw);
  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  initwifi();
  webSocket.onEvent(onWebSocketEvent);
  webSocket.begin();
  server.on("/hostname", HTTP_GET, [](AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", String(hostname)+".local");
});
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.begin();
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword("NotGonnaGuessThis");
  ArduinoOTA
      .onStart([]()
               {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.begin();
  digitalWrite(15, false);
}
void loop()
{
  webSocket.loop();
  ArduinoOTA.handle();
  if (apmode)
  {
    digitalWrite(15, (millis() % 2500 > 2200 && millis() % 2500 < 2300) || millis() % 2500 > 2400);
  }
  else
  {
    digitalWrite(15, millis() % 2500 > 2400);
  }
  if (digitalRead(0)==HIGH)
  {
    puship(apmode ? WiFi.softAPIP() : WiFi.localIP());
  }
  if (playing && millis() - lastmils > (1000 / fps))
  {
    current++;
    if (current > framessize - 1)
    {
      current = 0;
    }
    notifyClients("{\"current\":"+String(current)+"}");
    Serial.println(current);
    pushframe(current);
    lastmils = millis();
  }
}
void notifyClients(String msg)
{
  webSocket.broadcastTXT(msg);
}
void display_update()
{
  for (uint8_t x = 0; x < width; x++)
  {
    uint8_t xx = x * height;
    for (uint8_t y = 0; y < height; y++)
    {
      uint8_t yy = (x & 1) ? height - 1 - y : y;
      for (uint8_t z = 0; z < depth; z++)
      {
        uint8_t zz = (z + (z & 1)) * height * width;
        uint8_t led = (z & 1) ? zz - xx - yy - 1 : xx + yy + zz;
        leds[led] = cube[x][y][z];
      }
    }
  }
  FastLED.show();
}
int hex_to_val(const char ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0'; /* Simple ASCII arithmetic */
  else if (ch >= 'a' && ch <= 'f')
    return 10 + ch - 'a'; /* Because hex-digit a is ten */
  else if (ch >= 'A' && ch <= 'F')
    return 10 + ch - 'A'; /* Because hex-digit A is ten */
  else
    return 0; /* Not a valid hexadecimal digit */
}
void pushframe(uint16_t frame)
{
  for (uint8_t x = 0;x<4;x++){
    for (uint8_t y = 0;y<4;y++){
      for (uint8_t z = 0;z<4;z++){
        cube[x][y][z] = frames[frame][x][y][z];
      }
    }
  }
  display_update();
  display_clear();
}
void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload,
                      size_t length)
{
  switch (type)
  {
  // Client has disconnected
  case WStype_DISCONNECTED:
  {
    Serial.printf("Disconnected client %u\n", client_num);
  }
  break;
  // New client has connected
  case WStype_CONNECTED:
  {
    Serial.printf("Connection from client %u\n", client_num);
    if (wsmsg != "")
    {
      notifyClients(wsmsg);
    }
  }
  break;
  // Text received from a connected client
  case WStype_TEXT:
  {

    data = String((char *)payload);
    Serial.print("data: ");
    Serial.println(data);
    Serial.println(length);
    notifyClients(data);
    // if (data == "{\"playing\":false}")
    // {
    //   playing = false;
    //   return;
    // }
    wsmsg = data;
    DynamicJsonDocument datajson(length * 3);
    DeserializationError error = deserializeJson(datajson, data);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    } else {Serial.println("jsonloaded");}
    if (datajson.containsKey("fps")){
      fps = datajson["fps"].as<float>();
    }
    if (datajson.containsKey("playing")){
    playing = datajson["playing"].as<bool>();
    }
    if (datajson.containsKey("current")){
    Serial.print("current: ");
    Serial.println(current);
    current = datajson["current"].as<int16_t>();
    Serial.print("current: ");
    Serial.println(current);
    pushframe(current);
    }
    if (datajson.containsKey("frames")){
      memset(frames, 0, sizeof(frames));
      uint8_t iframe = 0;
      framessize = datajson["frames"].size();
      for (JsonArray frame : datajson["frames"].as<JsonArray>())
      {
        uint8_t iz = 0;
        for (JsonArray z : frame)
        {
          uint8_t iy = 0;
          for (JsonArray y : z)
          {
            uint8_t ix = 0;
            for (JsonVariant x : y)
            {
              String hexstring = x.as<String>();
              uint32_t hex = 0;
              for (int j = 0; j < 6; j++)
              {
                int chr = round(hex_to_val(hexstring[j + 1]));
                hex <<= 4;
                hex |= chr;
              }
              frames[iframe][ix][3-iy][iz] = hex;
              ix++;
            }
            iy++;
          }
          iz++;
        }
        iframe++;
      }
      pushframe(current);
    }
    datajson.clear();

  }
  break;
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

String bench(String filename)
{
  unsigned long before = millis();
  File file = SPIFFS.open(filename);
  unsigned long open = millis();
  if (!file)
  {
    Serial.println("Failed to open file for reading");
  }
  else
  {
    Serial.println("SPIFFS working");
  }
  while (file.available())
  {
    file.read();
  }
  file.close();

  return filename + " open in " + String(open - before) + "ms\t read in " + String(millis() - open) + "ms";
}
bool connectwifi(String ssid, String password)
{
  Serial.println("connecting to " + ssid);
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  unsigned long last = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - last > 200)
    {
      Serial.print(".");
      last = millis();
    }
    digitalWrite(15, millis() % 500 > 250);
    if (millis() - start > 5000)
    {
      Serial.println("\ntimeout for " + ssid);
      return false;
    }
  }
  Serial.println("\nconnected");
  return true;
}
void initwifi(){
  bool done = false;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(hostname, "esp32-led-cube-%02x%02x%02x%02x%02x%02x", mac[0], mac[1],mac[2], mac[3],mac[4], mac[5]);
  WiFi.setHostname(hostname);
  for (JsonVariant value : config["wifi"].as<JsonArray>())
  {
    if (!done)
    {
      done = connectwifi(value["ssid"].as<String>(), value["password"].as<String>());
    }
  }
  config.clear();
  IPAddress IP;
  if (!done)
  {
    // WiFi.softAPConfig();
    WiFi.disconnect(false, true);
    WiFi.softAP(hostname, ap_password);
    // WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    // dnsServer.start(53, "*", apIP);
    IP = WiFi.softAPIP();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/indexap.html", "text/html"); });
    apmode = true;
  }
  else
  {
    IP = WiFi.localIP();
    apmode = false;
  }
  Serial.print("IP address: ");
  Serial.println(IP);
  Serial.println("MAC address: "+String(WiFi.macAddress()));
  Serial.println("hostname: "+String(hostname)+".local");
  puship(IP);
}
void puship(IPAddress IP)
{
  char bits[8];
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      for (int z = 0; z < 4; z++)
      {
        cube[x][y][z] = 0x000000;
      }
    }
  }
  for (int i = 0; i < 4; i++)
  {
    uint8_t mask = 0b10000000;
    for (int x = 0; x < 4; x++)
    {
      cube[3 - x][2][3 - i] = ((uint8_t)IP[i] & mask) ? 0xffffff : 0x0;
      mask = mask >> 1;
    }
    for (int x = 0; x < 4; x++)
    {
      cube[3 - x][3][3 - i] = ((uint8_t)IP[i] & mask) ? 0xffffff : 0x0;
      mask = mask >> 1;
    }
  }
  display_update();
  display_clear();
}
