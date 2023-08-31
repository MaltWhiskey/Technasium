
#define SOC_I2S_SUPPORTS_DAC true
#include <Arduino.h>
#include <FastLED.h>
#include <driver/dac.h>
#include <driver/i2s.h>
#include <math.h>
#include <stdint.h>

#include "magniet.h"
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
static const uint8_t LED_PIN1 = 16;
static const uint8_t LED_PIN2 = 35;
static const uint8_t DAC_PIN = 18;
static const uint8_t TOUCH_PIN = 37;
static const uint8_t width = 4;
static const uint8_t height = 4;
static const uint8_t depth = 1;
static const uint8_t ring_size = 14;
static CRGB cube[width][height][depth];
static CRGB leds[width * height * depth];
static CRGB ring[ring_size];

void magniet();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  FastLED.addLeds<WS2812B, LED_PIN1, RGB>(leds, sizeof(leds) / sizeof(CRGB));
  FastLED.addLeds<WS2812B, LED_PIN2, GRB>(ring, sizeof(ring) / sizeof(CRGB));
  FastLED.setDither(false);
  pinMode(TOUCH_PIN, INPUT);
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
  static float phase = 0;
  static unsigned long last_time = micros();

  unsigned long current_time = micros();
  unsigned long delta_time = current_time - last_time;
  last_time = current_time;
  float dt = delta_time / 1000000.0f;

  phase += dt * PI;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      float val = sinf(phase + (x / 3.5f) + (y / 3.5f));
      val = val * val * 255;
      cube[x][y][0] = CRGB(val, val, val);
    }
  }
  for (int x = 0; x < ring_size; x++) {
    float val = sinf(phase + (x / 3.5f));
    val = val * val * 255;
    ring[x] = CRGB(val, 0, 0);
  }
  display_update(0);
  display_clear();

  if (digitalRead(TOUCH_PIN)) {
    magniet();
  }
}

void magniet() {
  pinMode(DAC_PIN, OUTPUT_OPEN_DRAIN);
  uint16_t pos = 0;
  while (pos < MagNiet_len) {
    dacWrite(DAC_PIN, MagNiet[pos++]);
    delayMicroseconds(3);
  }
  pinMode(DAC_PIN, OUTPUT_OPEN_DRAIN);
}
