#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <stdint.h>
#include <string>
#include "WebServer.h"

// Json document size to hold the commands send between client/server
#define COMMAND_DOC_SIZE 200
// Json document size to hold the config (depends on config size)
#define CONFIG_DOC_SIZE 20000
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
    char ssid[32] = "-^..^-";
    char password[64] = "qazwsxedc";
    char hostname[64] = "icube";
    uint16_t port = 80;
  } network;

  struct {
    // Do not serialize changed to disk/gui
    boolean changed = false;
    boolean play_one = false;
    // Only serialize animation to disk
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
      float body_diagonal = 6.5f;
      int8_t hue_speed = 30;
      uint8_t brightness = 255;
      uint8_t motionBlur = 230;
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

  // Do not serialize devices
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
      DynamicJsonDocument doc(CONFIG_DOC_SIZE);
      DeserializationError err = deserializeJson(doc, file);
      if (err)
        Serial.printf("Deserialization error while loading file: %s\n", err.c_str());
      else
        deserialize(doc);
      file.close();
    }
  }

  void save() {
    if (File file = open("/config.json", FILE_WRITE)) {
      DynamicJsonDocument doc(CONFIG_DOC_SIZE);
      JsonObject root = doc.to<JsonObject>();
      serialize(root);
      // Save active animation to fs, but don't create a slider on the gui.
      JsonObject obj = root["settings"]["display"];
      slider(obj, "animation", "Active Animation", animation.animation, 0, 14, 1);
      size_t size = serializeJson(root, file);
      Serial.printf("%u bytes written to config.json\n", size);
      file.close();
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

  void serialize(JsonObject& root) {
    JsonObject settings = root.createNestedObject("settings");
    settings["name"] = "Configuration Settings";
    JsonObject animations = root.createNestedObject("animations");
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
      text(obj, "ssid", "Network SSID", network.ssid, 32);
      text(obj, "password", "Password", network.password, 64);
      text(obj, "hostname", "Hostname", network.hostname, 64);
      number(obj, "port", "Port", network.port, 0x0000, 0xffff, 1);
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
    { // ANIMATIONS.ATOMS
      obj = animations.createNestedObject("atoms");
      auto& cfg = animation.atoms;
      obj["name"] = "Atoms";
      obj["index"] = 2;
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
    { // ANIMATIONS.CUBE
      obj = animations.createNestedObject("cube");
      auto& cfg = animation.cube;
      obj["name"] = "Cube";
      obj["index"] = 3;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "angle_speed", "Angle Speed", cfg.angle_speed, -180, 180, 1);
      slider(obj, "radius", "Radius", cfg.radius, 0.0f, 16.0f, 0.25f);
      slider(obj, "radius_start", "Radius Start", cfg.radius_start, 0.0f, 16.0f, 0.25f);
      slider(obj, "hue_speed", "Hue Speed", cfg.hue_speed);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.FIREWORKS
      obj = animations.createNestedObject("Fireworks");
      auto& cfg = animation.fireworks;
      obj["name"] = "Fireworks";
      obj["index"] = 4;
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "radius", "Radius", cfg.radius, 0.0f, 16.0f, 0.25f);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.HELIX
      obj = animations.createNestedObject("helix");
      auto& cfg = animation.helix;
      obj["name"] = "Double Helix";
      obj["index"] = 5;
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "interval", "Interval", cfg.interval, 0.025f, 16.0f, 0.025f);
      slider(obj, "phase_speed", "Phase Speed", cfg.phase_speed, 0.0f, 16.0f, 0.25f);
      slider(obj, "angle", "Angle", cfg.angle, -180, 180, 1);
      slider(obj, "angle_speed", "Angle Speed", cfg.angle_speed, -180, 180, 1);
      slider(obj, "radius", "Radius", cfg.radius, 0.0f, 16.0f, 0.25f);
      slider(obj, "resolution", "Resolution", cfg.resolution, 1.0f, 50.0f, 1.0f);
      slider(obj, "thickness", "Thickness", cfg.thickness, 1.0f, 16.0f, 1.0f);
      slider(obj, "hue_speed", "Hue Speed", cfg.hue_speed);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.HELIX
      obj = animations.createNestedObject("life");
      auto& cfg = animation.life;
      obj["name"] = "Life";
      obj["index"] = 6;
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "interval", "Interval", cfg.interval, 0.025f, 16.0f, 0.025f);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.PACMAN
      obj = animations.createNestedObject("pacman");
      auto& cfg = animation.pacman;
      obj["name"] = "Pacman";
      obj["index"] = 7;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "interval", "Interval", cfg.interval, 0.025f, 16.0f, 0.025f);
      slider(obj, "angle_speed", "Angle Speed", cfg.angle_speed, -180, 180, 1);
      slider(obj, "radius", "Radius", cfg.radius, 0.0f, 16.0f, 0.25f);
      slider(obj, "radius_start", "Radius Start", cfg.radius_start, 0.0f, 16.0f, 0.25f);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.PLASMA
      obj = animations.createNestedObject("plasma");
      auto& cfg = animation.plasma;
      obj["name"] = "Plasma";
      obj["index"] = 8;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "scale_p", "Scale", cfg.scale_p, 0.0f, 1.0f, 0.001f);
      slider(obj, "speed_x", "X-Speed", cfg.speed_x, -3.0f, 3.0f, 0.01f);
      slider(obj, "speed_y", "Y-Speed", cfg.speed_y, -3.0f, 3.0f, 0.01f);
      slider(obj, "speed_z", "Z-Speed", cfg.speed_z, -3.0f, 3.0f, 0.01f);
      slider(obj, "speed_w", "W-Speed", cfg.speed_w, -3.0f, 3.0f, 0.01f);
      slider(obj, "speed_offset_speed", "Dynamic Speed", cfg.speed_offset_speed, 0.00f, 0.10f, 0.001f);
      slider(obj, "hue_speed", "Hue Speed", cfg.hue_speed);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.PONG
      obj = animations.createNestedObject("pong");
      auto& cfg = animation.pong;
      obj["name"] = "Pong";
      obj["index"] = 9;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "hue_speed", "Hue Speed", cfg.hue_speed);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.SCROLLER
      obj = animations.createNestedObject("scroller");
      auto& cfg = animation.scroller;
      obj["name"] = "Scroller";
      obj["index"] = 10;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "rotation_speed", "Rotation Speed", cfg.rotation_speed, -180, 180, 1);
      slider(obj, "radius", "Radius", cfg.radius, 0.0f, 16.0f, 0.25f);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.SINUS
      obj = animations.createNestedObject("sinus");
      auto& cfg = animation.sinus;
      obj["name"] = "Sinus";
      obj["index"] = 11;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "phase_speed", "Phase Speed", cfg.phase_speed, 0.0f, 16.0f, 0.25f);
      slider(obj, "resolution", "Resolution", cfg.resolution, 1.0f, 50.0f, 1.0f);
      slider(obj, "radius", "Radius", cfg.radius, 0.0f, 16.0f, 0.25f);
      slider(obj, "hue_speed", "Hue Speed", cfg.hue_speed);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.SPECTRUM
      obj = animations.createNestedObject("spectrum");
      auto& cfg = animation.spectrum;
      obj["name"] = "Spectrum";
      obj["index"] = 12;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "hue_speed", "Hue Speed", cfg.hue_speed);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.SPECTRUM
      obj = animations.createNestedObject("starfield");
      auto& cfg = animation.starfield;
      obj["name"] = "Starfield";
      obj["index"] = 13;
      slider(obj, "starttime", "Starttime", cfg.starttime);
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "endtime", "Endtime", cfg.endtime);
      slider(obj, "phase_speed", "Phase Speed", cfg.phase_speed, 0.0f, 16.0f, 0.25f);
      slider(obj, "hue_speed", "Hue Speed", cfg.hue_speed);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
    { // ANIMATIONS.TWINKELS
      obj = animations.createNestedObject("twinkels");
      auto& cfg = animation.twinkels;
      obj["name"] = "Twinkels";
      obj["index"] = 14;
      slider(obj, "runtime", "Runtime", cfg.runtime);
      slider(obj, "interval", "Interval", cfg.interval, 0.025f, 16.0f, 0.025f);
      slider(obj, "fade_in_speed", "Fade In Speed", cfg.fade_in_speed, 0.0f, 16.0f, 0.25f);
      slider(obj, "fade_out_speed", "Fade In Speed", cfg.fade_out_speed, 0.0f, 16.0f, 0.25f);
      slider(obj, "brightness", "Brightness", cfg.brightness);
      slider(obj, "motionblur", "Motion Blur", cfg.motionBlur);
    }
  };

  void deserialize(JsonDocument& doc) {
    { // SETTINGS.DISPLAY
      JsonObject obj = doc["settings"]["display"];
      power.max_milliamps = obj["max_milliamps"]["value"] | power.max_milliamps;
      if (obj["play_one"]) {
        animation.play_one = !obj["play_one"]["value"];
        animation.changed = true;
      }
      animation.animation = obj["animation"]["value"] | animation.animation;
    }
    { // SETTINGS.NETWORK
      JsonObject obj = doc["settings"]["network"];
      strlcpy(network.ssid, obj["ssid"]["value"] |
        network.ssid, sizeof(network.ssid));
      strlcpy(network.password, obj["password"]["value"] |
        network.password, sizeof(network.password));
      strlcpy(network.hostname, obj["hostname"]["value"] |
        network.hostname, sizeof(network.hostname));
      network.port = obj["port"]["value"] | network.port;
    }
    { // SETTINGS.ACCELEROMETER
      JsonObject obj = doc["animations"]["accelerometer"];
      auto& cfg = animation.accelerometer;
      cfg.runtime = obj["runtime"]["value"] | cfg.runtime;
      cfg.radius = obj["radius"]["value"] | cfg.radius;
      cfg.brightness = obj["brightness"]["value"] | cfg.brightness;
      cfg.motionBlur = obj["motionblur"]["value"] | cfg.motionBlur;
    }
    { // SETTINGS.ARROWS
      JsonObject obj = doc["animations"]["arrows"];
      auto& cfg = animation.arrows;
      cfg.starttime = obj["starttime"]["value"] | cfg.starttime;
      cfg.runtime = obj["runtime"]["value"] | cfg.runtime;
      cfg.endtime = obj["endtime"]["value"] | cfg.endtime;
      cfg.angle_speed = obj["angle_speed"]["value"] | cfg.angle_speed;
      cfg.radius = obj["radius"]["value"] | cfg.radius;
      cfg.radius_start = obj["radius_start"]["value"] | cfg.radius_start;
      cfg.distance = obj["distance"]["value"] | cfg.distance;
      cfg.hue_speed = obj["hue_speed"]["value"] | cfg.hue_speed;
      cfg.brightness = obj["brightness"]["value"] | cfg.brightness;
      cfg.motionBlur = obj["motionblur"]["value"] | cfg.motionBlur;
    }
  }

  void execute(uint8_t* payload) {
    StaticJsonDocument<COMMAND_DOC_SIZE>cmd;
    DeserializationError err = deserializeJson(cmd, payload);
    if (err) {
      Serial.printf("Deserialization error (execute): %s\n", err.c_str());
      return;
    }

    String event = cmd["event"];
    if (event.equals("activate")) {
      animation.changed = true;
      animation.play_one = true;
      animation.animation = cmd["target"] | animation.animation;
      // Synchronize all clients to turn off cycle animations
      cmd.clear();
      cmd["event"] = "update";
      JsonObject settings = cmd.createNestedObject("settings");
      JsonObject display = settings.createNestedObject("display");
      JsonObject object = display.createNestedObject("play_one");
      object["value"] = !animation.play_one;
      uint8_t buffer[COMMAND_DOC_SIZE];
      serializeJson(cmd, buffer);
      WebServer::broadcast(buffer);
    }
    else if (event.equals("update")) {
      cmd.remove("event");
      deserialize(cmd);
    }
    else if (event.equals("save")) {
      save();
    }
  }
};
// All cpp files that include this link to a single config struct
extern struct Config config;
#endif