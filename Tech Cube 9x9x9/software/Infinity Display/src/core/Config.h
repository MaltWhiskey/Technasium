#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <stdint.h>

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
      char hostname[64] = "InfinityCube";
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
      uint8_t brightness = 200;
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
      uint8_t brightness = 200;
      uint8_t motionBlur = 200;
    } atoms;
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
      uint8_t brightness = 200;
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
      uint8_t brightness = 200;
      uint8_t motionBlur = 220;
    } mario;
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
      uint8_t motionBlur = 0;
      uint8_t brightness = 255;
    } plasma;
    struct {
      float starttime = 2.0f;
      float runtime = 30.0f;
      float endtime = 2.0f;
      int8_t hue_speed = 25;
      uint8_t brightness = 200;
      uint8_t motionBlur = 200;
    } pong;
    struct {
      float starttime = 0.5f;
      float runtime = 30.0f;
      float endtime = 2.0f;
      float rotation_speed = 90.0f;
      float radius = 8.0f;
      uint8_t brightness = 200;
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
      uint8_t brightness = 200;
      uint8_t motionBlur = 0;
    } sinus;
    struct {
      float starttime = 5.0f;
      float runtime = 30.0f;
      float endtime = 5.0f;
      int8_t hue_speed = -25;
      uint8_t brightness = 200;
      uint8_t motionBlur = 220;
    } spectrum;
    struct {
      float starttime = 3.0f;
      float runtime = 30.0f;
      float endtime = 5.0f;
      float phase_speed = 0.3f;
      int8_t hue_speed = 30;
      uint8_t brightness = 200;
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
      int8_t x = 0;
      int8_t y = 0;
      boolean z = false;
      boolean a = false;
      boolean b = false;
      boolean c = false;
    } button;
    struct {
      float x = 0;
      float y = 0;
      float z = 0;
    } accelerometer;
    struct {
      uint8_t data[64];
      boolean updated = false;
    } fft;
  } hid;

  File open(const char* name, const char* mode) {
    if (!SPIFFS.begin()) {
      Serial.println("Error mounting SPIFFS");
      return (File)0;
    }
    File file = SPIFFS.open(name, mode);
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

  void serialize(String& buffer) {
    DynamicJsonDocument doc(CONFIG_DOC_SIZE);
    // JsonObject settings = doc.createNestedObject("settings");
  };

  void deserialize(String buffer) {
    DynamicJsonDocument doc(CONFIG_DOC_SIZE);
    DeserializationError err = deserializeJson(doc, buffer);
    if (err) {
      Serial.printf("Deserialization error: %s\n", err.c_str());
      return;
    }

    power.max_milliamps =
        doc["power"]["max_milliamps"]["value"] | power.max_milliamps;

    strlcpy(network.wifi.ssid,
            doc["network"]["wifi"]["ssid"]["value"] | network.wifi.ssid,
            sizeof(network.wifi.ssid));
    strlcpy(network.wifi.password,
            doc["network"]["wifi"]["password"]["value"] | network.wifi.password,
            sizeof(network.wifi.password));
    strlcpy(
        network.server.hostname,
        doc["network"]["server"]["hostname"]["value"] | network.server.hostname,
        sizeof(network.server.hostname));
    network.server.port =
        doc["network"]["server"]["port"]["value"] | network.server.port;
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
    } else if (event.equals("update")) {
      String string_buffer;
      doc.remove("event");
      serializeJson(doc, string_buffer);
      deserialize(string_buffer);
    } else if (event.equals("save")) {
      save();
    }
  }
};
// All cpp files that include this link to a single config struct
extern struct Config config;
#endif