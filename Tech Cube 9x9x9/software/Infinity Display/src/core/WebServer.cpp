#include "core/WebServer.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <WiFi.h>

#include "core/Config.h"

#define WEBSERVER_PORT 80
#define WEBSOCKET_PORT 1337
#define ACCESSPOINT_IP 192, 168, 1, 1
#define ACCESSPOINT_SSID "CUBE"
#define ACCESSPOINT_PASS "^3"
#define DNSSERVER_PORT 53

namespace WebServer {

AsyncWebServer server = AsyncWebServer(WEBSERVER_PORT);
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);
IPAddress APIP = IPAddress(ACCESSPOINT_IP);
DNSServer dnsServer;
boolean AP_MODE;

void begin() {
  // Start file system
  if (!LittleFS.begin()) {
    Serial.println("Error mounting LittleFS");
  }
  // Start WiFi
  Serial.print("Starting WiFi");
  WiFi.begin(config.network.wifi.ssid, config.network.wifi.password);
  AP_MODE = true;
  for (uint8_t i = 0; i < 70; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      AP_MODE = false;
      break;
    }
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Start as Accesspoint
  if (AP_MODE) {
    Serial.println("Starting Wifi Access Point");
    WiFi.mode(WIFI_AP);
    // Quick hack to wait for SYSTEM_EVENT_AP_START
    delay(100);
    WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(ACCESSPOINT_SSID, ACCESSPOINT_PASS);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    dnsServer.start(DNSSERVER_PORT, "*", APIP);
  }

  // Start connected to local network
  if (!AP_MODE) {
    Serial.print("Connected to ");
    Serial.println(config.network.wifi.ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    // Start MDNS
    if (MDNS.begin(config.network.server.hostname), WiFi.localIP()) {
      Serial.print("MDNS responder started. Hostname = ");
      Serial.println(config.network.server.hostname);
    }
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
  }

  // On HTTP request for root, provide index.html file
  server.on("*", HTTP_GET, onIndexRequest);
  // Start web server
  server.begin();
  // Start WebSocket server and assign callback
  webSocket.onEvent(onWebSocketEvent);
  // Start web socket
  webSocket.begin();

  // Initialize Arduino OTA
  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname(config.network.server.hostname);
  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else
          type = "filesystem";
        // vTaskSuspend(Task);
        Serial.println("Start updating " + type);
      })
      .onEnd([]() { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });
  ArduinoOTA.begin();
}

// Handle network traffic
void update() {
  // Handle WebSocket data
  webSocket.loop();
  if (AP_MODE) {
    // Handle dns requests in Access Point Mode only
    dnsServer.processNextRequest();
  } else if (WiFi.status() != WL_CONNECTED) {
    // Handle reconnects only when connected to Lan
    begin();
  }
}

void onIndexRequest(AsyncWebServerRequest* request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " +
                 request->url());
  if (request->url().equals("/"))
    request->send(LittleFS, "/index.html", "text/html");
  else if (request->url().equals("/gui.json")) {
    String buffer;
    config.serialize(buffer);
    request->send(200, "application/json", buffer);
  } else if (LittleFS.exists(request->url()))
    if (request->url().endsWith(".html")) {
      request->send(LittleFS, request->url(), "text/html");
    } else if (request->url().endsWith(".css")) {
      request->send(LittleFS, request->url(), "text/css");
    } else if (request->url().endsWith(".json")) {
      request->send(LittleFS, request->url(), "application/json");
    } else if (request->url().endsWith(".ico")) {
      request->send(LittleFS, request->url(), "image/x-icon");
    } else if (request->url().endsWith(".png")) {
      request->send(LittleFS, request->url(), "image/png");
    } else {
      request->send(404, "text/plain", "Mime-Type Not Found");
    }
  else {
    request->redirect("/index.html");
  }
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
      config.execute(payload);
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

}  // namespace WebServer