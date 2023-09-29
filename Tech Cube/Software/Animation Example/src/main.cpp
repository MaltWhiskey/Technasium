
#include <Arduino.h>
#include <FastLED.h>
#include <stdint.h>
#include "main.h"
#include "Helix.h"
#include "Anim.h"

/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
static const uint8_t LED_PIN = 16;
CRGB cube[width][height][depth];
CRGB leds[width * height * depth];

Helix helix;
Anim anim;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, sizeof(leds) / sizeof(CRGB));
  FastLED.setDither(false);
  helix.init();
  anim.init();
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
  static unsigned long last_time = micros();
  unsigned long current_time = micros();
  unsigned long delta_time = current_time - last_time;
  last_time = current_time;
  float dt = delta_time / 1000000.0f;

  //helix.draw(dt);
  if (ANIM1_DATA_FRAME_RATE != 0) {
    anim.draw(dt);
    display_update(1000/ANIM1_DATA_FRAME_RATE);
    display_clear();
  }
}