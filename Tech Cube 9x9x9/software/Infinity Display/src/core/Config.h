#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <stdint.h>
#include <string>

// Json document size to hold the commands send between client/server
#define COMMAND_DOC_SIZE 255
// Json document size to hold the config (depends on config size)
#define CONFIG_DOC_SIZE 8192
/*-----------------------------------------------------------------------------
 * Global parameters
 *
 * These parameters are used for dynamically changing runtime operation. They
 * can (optionally) be loaded from and writen to persistant storage.
 *
 * Animation init or draw routines need to apply these parameters to
 * dynamically set runtime parameters. Init only gets called when an animation
 * starts or restarts draw gets called every animation frame so choose wisely
 * where to apply. And let these parameters take effect.
 *
 * After creation of the config object, call load() to load the configuration
 * from the "config.json" file and apply the values to the config struct.
 *
 * If no "config.json" exists the config structs keeps the values supplied in
 * the code. After saveing a "config.json" is freshly created.
 *---------------------------------------------------------------------------*/
struct Config {
  struct {
    uint16_t max_milliamps = 18000;
  } power;
  struct {
    struct {
      char ssid[32] = "-^..^-";
      char password[64] = "qazwsxedc";
    } wifi;
    struct {
      char hostname[64] = "icube";
      uint16_t port = 8080;
    } server;
  } network;
  struct {
    boolean changed = false;
    boolean play_one = false;
    uint8_t animation = 0;
    struct {
      float runtime = 30.0f;
      float radius = 4.0f;
      uint8_t brightness = 255;
      uint8_t motionBlur = 220;
    } accelerometer;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 3.0f;
      float angle_speed = -180.0f;
      float radius = 4.0f;
      float radius_start = 1.0f;
      float distance = 1.0f;
      int8_t hue_speed = 25;
      uint8_t brightness = 255;
      uint8_t motionBlur = 220;
    } arrows;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 3.0f;
      float angle_speed = 120.0f;
      float radius = 4.0f;
      float radius_start = 1.0f;
      float distance = 2.0f;
      int8_t hue_speed = 25;
      uint8_t brightness = 255;
      uint8_t motionBlur = 200;
    } atoms;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 5.0f;
      float angle_speed = 160.0f;
      float radius = 3.0f;
      float radius_start = 1.0f;
      int8_t hue_speed = -50;
      uint8_t brightness = 255;
      uint8_t motionBlur = 0;
    } cube;
    struct {
      float runtime = 30.0f;
      float radius = 4.0f;
      uint8_t brightness = 255;
      uint8_t motionBlur = 220;
    } fireworks;
    struct {
      float runtime = 30.0f;
      float interval = 0.2f;
      float phase_speed = 5.0f;
      float angle = 0.0f;
      float angle_speed = 0.0f;
      float radius = 4.0f;
      float resolution = 20;
      int8_t thickness = 4;
      int8_t hue_speed = 30;
      uint8_t brightness = 255;
      uint8_t motionBlur = 230;
    } helix;
    struct {
      float runtime = 30.0f;
      float interval = 0.20f;
      uint8_t brightness = 255;
      uint8_t motionBlur = 0;
    } life;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 3.0f;
      float interval = 0.15f;
      float angle_speed = -70.0f;
      float radius = 4.0f;
      float radius_start = 1.0f;
      uint8_t brightness = 255;
      uint8_t motionBlur = 200;
    } pacman;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 10.0f;
      float scale_p = 0.15f;
      float speed_x = 0.1f;
      float speed_y = 0.2f;
      float speed_z = 0.3f;
      float speed_w = 0.4f;
      float speed_offset_speed = 0.02f;
      int8_t hue_speed = 30;
      uint8_t brightness = 255;
      uint8_t motionBlur = 0;
    } plasma;
    struct {
      float starttime = 2.0f;
      float runtime = 30.0f;
      float endtime = 2.0f;
      int8_t hue_speed = 25;
      uint8_t brightness = 255;
      uint8_t motionBlur = 200;
    } pong;
    struct {
      float starttime = 0.5f;
      float runtime = 30.0f;
      float endtime = 2.0f;
      float rotation_speed = 90.0f;
      float radius = 8.0f;
      uint8_t brightness = 255;
      uint8_t motionBlur = 0;
    } scroller;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 10.0f;
      float phase_speed = PI;
      float resolution = 15;
      float radius = 4.0f;
      int8_t hue_speed = -50;
      uint8_t brightness = 255;
      uint8_t motionBlur = 0;
    } sinus;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 5.0f;
      int8_t hue_speed = -25;
      uint8_t brightness = 255;
      uint8_t motionBlur = 230;
    } spectrum;
    struct {
      float starttime = 3.0f;
      float runtime = 30.0f;
      float endtime = 5.0f;
      float phase_speed = 0.3f;
      int8_t hue_speed = 30;
      uint8_t brightness = 255;
      uint8_t motionBlur = 230;
      float body_diagonal = 6.5f;
    } starfield;
    struct {
      float runtime = 30.0f;
      float interval = 0.1f;
      float fade_in_speed = 1.0f;
      float fade_out_speed = 2.0f;
      uint8_t brightness = 255;
      uint8_t motionBlur = 0;
    } twinkels;
  } animation;
  struct {
    struct {
      float x = 0;
      float y = 0;
      float z = 0;
    } accelerometer;
    struct {
      volatile float level[9];
    } fft;
  } devices;

  File open(const char* name, const char* mode) {
    if (!LittleFS.begin()) {
      Serial.println("Error mounting LittleFS");
      return (File)0;
    }
    File file = LittleFS.open(name, mode);
    if (!file) {
      Serial.printf("Error opening file for %s\n", mode);
      return (File)0;
    }
    return file;
  }

  void load() {
    if (File file = open("/config.json", FILE_READ)) {
      String buffer = file.readString();
      Serial.printf("%u bytes read from config.json\n", buffer.length());
      file.close();
      deserialize(buffer);
    }
  }

  void save() {
    if (File file = open("/config.json", FILE_WRITE)) {
      String buffer;
      serialize(buffer);
      int bytesWritten = file.print(buffer);
      file.close();
      Serial.printf("%u bytes written to config.json\n", bytesWritten);
    }
  }

  void slider(JsonObject& node, const char* id, const char* name, float value,
    float min = 0, float max = 255, float step = 1) {
    JsonObject leaf = node.createNestedObject(id);
    leaf["name"] = name;
    leaf["type"] = "slider";
    leaf["value"] = value;
    leaf["min"] = min;
    leaf["max"] = max;
    leaf["step"] = step;
  }

  void checkbox(JsonObject& node, const char* id, const char* name,
    boolean value) {
    JsonObject leaf = node.createNestedObject(id);
    leaf["name"] = name;
    leaf["type"] = "checkbox";
    leaf["value"] = value;
  }

  void text(JsonObject& node, const char* id, const char* name,
    const char* value, uint8_t size) {
    JsonObject leaf = node.createNestedObject(id);
    leaf["name"] = name;
    leaf["type"] = "text";
    leaf["value"] = value;
    leaf["size"] = size;
  }

  void number(JsonObject& node, const char* id, const char* name, float value,
    float min = 0, float max = 255, float step = 1) {
    JsonObject leaf = node.createNestedObject(id);
    leaf["name"] = name;
    leaf["type"] = "number";
    leaf["value"] = value;
    leaf["min"] = min;
    leaf["max"] = max;
    leaf["step"] = step;
  }

  void serialize(String& buffer) {
    DynamicJsonDocument doc(CONFIG_DOC_SIZE);
    JsonObject settings = doc.createNestedObject("settings");
    settings["name"] = "Configuration Settings";
    JsonObject animations = doc.createNestedObject("animations");
    animations["name"] = "Animation Settings";
    JsonObject obj;
    { // SETTINGS.DISPLAY
      obj = settings.createNestedObject("display");
      obj["name"] = "Display Settings";
      slider(obj, "max_milliamps", "Max mAmps", power.max_milliamps, 0, 20000, 100);
      checkbox(obj, "play_one", "Cycle Animations", !animation.play_one);
    }
    { // SETTINGS.NETWORK
      obj = settings.createNestedObject("network");
      obj["name"] = "Network Settings";
      text(obj, "ssid", "Network SSID", network.wifi.ssid, 32);
      text(obj, "password", "Password", network.wifi.password, 64);
      text(obj, "hostname", "Hostname", network.server.hostname, 64);
      number(obj, "port", "Port", network.server.port, 0x0000, 0xffff, 1);
    }
    { // ANIMATIONS.ACCELEROMETER
      obj = animations.createNestedObject("accelerometer");
      auto& cfg = animation.accelerometer;
      obj["name"] = "Accelerometer";
      obj["index"] = 0;
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "radius", "Radius", cfg.radius, 0.0f, 16.0f, 0.25f);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.ARROWS
      obj = animations.createNestedObject("arrows");
      auto& cfg = animation.arrows;
      obj["name"] = "Arrows";
      obj["index"] = 1;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "angle_speed", "Angle Speed", cfg.angle_speed, -180, 180, 1);
      slider(obj, "radius", "Radius", cfg.radius, 0.0f, 16.0f, 0.25f);
      slider(obj, "radius_start", "Radius Start", cfg.radius_start, 0.0f, 16.0f, 0.25f);
      slider(obj, "distance", "Distance", cfg.distance, 0.0f, 16.0f, 0.25f);
      slider(obj, "hue_speed", "Hue Speed", cfg.hue_speed);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    // SERIALIZED CONFIG
    serializeJson(doc, buffer);
  };

  void deserialize(String buffer) {
    DynamicJsonDocument doc(CONFIG_DOC_SIZE);
    DeserializationError err = deserializeJson(doc, buffer);
    if (err) {
      Serial.printf("Deserialization error: %s\n", err.c_str());
      return;
    }
    { // SETTINGS.DISPLAY
      JsonObject obj = doc["settings"]["display"];
      power.max_milliamps =
        obj["max_milliamps"]["value"] | power.max_milliamps;
      if (obj["play_one"]) {
        animation.play_one = !obj["play_one"]["value"];
        animation.changed = true;
      }
    }
    { // SETTINGS.NETWORK
      JsonObject obj = doc["settings"]["network"];
      strlcpy(network.wifi.ssid,
        obj["ssid"]["value"] | network.wifi.ssid,
        sizeof(network.wifi.ssid));
      strlcpy(network.wifi.password,
        obj["password"]["value"] | network.wifi.password,
        sizeof(network.wifi.password));
      strlcpy(
        network.server.hostname,
        obj["hostname"]["value"] | network.server.hostname,
        sizeof(network.server.hostname));
      network.server.port =
        obj["port"]["value"] | network.server.port;
    }
  }

  void execute(uint8_t* char_buffer) {
    StaticJsonDocument<COMMAND_DOC_SIZE> doc;
    DeserializationError err = deserializeJson(doc, char_buffer);
    if (err) {
      Serial.printf("Deserialization error: %s\n", err.c_str());
      return;
    }
    serializeJson(doc, Serial);
    Serial.println();

    String event = doc["event"];
    if (event.equals("activate")) {
      animation.changed = true;
      animation.play_one = true;
      animation.animation = doc["target"] | animation.animation;
    }
    else if (event.equals("update")) {
      String string_buffer;
      doc.remove("event");
      serializeJson(doc, string_buffer);
      deserialize(string_buffer);
    }
    else if (event.equals("save")) {
      save();
    }
  }
};
// All cpp files that include this link to a single config struct
extern struct Config config;
#endif