#include "Display.h"

#include <Arduino.h>
#include <FastLED.h>

/*------------------------------------------------------------------------------
 * DISPLAY CLASS
 *----------------------------------------------------------------------------*/
uint8_t Display::motionBlur = 240;
float Display::brightness = 1;
uint32_t Display::cubeBuffer = 0;
Color Display::cube[2][width][height][depth];
static CRGB leds[Display::width * Display::height * Display::depth];

void Display::begin() {
  const static uint16_t FRAME = 9 * 9;
  FastLED.addLeds<WS2812B, 19, GRB>(leds + FRAME * 0, FRAME * 3);
  FastLED.addLeds<WS2812B, 18, GRB>(leds + FRAME * 3, FRAME * 3);
  FastLED.addLeds<WS2812B, 26, GRB>(leds + FRAME * 6, FRAME * 3);
  FastLED.setDither(false);
}

void Display::update() {
  for (uint16_t z = 0; z < Display::depth; z++) {
    bool ss = (z % 3 & 1);
    uint16_t zz = (z + ss) * Display::height * Display::width;
    for (uint16_t x = 0; x < Display::width; x++) {
      uint16_t xx = x * Display::height;
      for (uint16_t y = 0; y < Display::height; y++) {
        uint16_t yy = (x & 1) ? Display::height - 1 - y : y;
        uint16_t led = ss ? zz - xx - yy - 1 : xx + yy + zz;
        Color c = cube[cubeBuffer][x][y][z].blend(
            motionBlur, cube[1 - cubeBuffer][x][y][z]);
        leds[led] = CRGB(c.g, c.r, c.b);
      }
    }
  }
  FastLED.show();
  cubeBuffer = 1 - cubeBuffer;
}

void Display::clear() { memset(cube[cubeBuffer], 0, sizeof(cube[0])); }
// Set the master display brightness value
void Display::setBrightness(const float value) { brightness = value; }
float Display::getBrightness() { return brightness; }
// Set the motion blur value
void Display::setMotionBlur(const uint8_t value) { motionBlur = value; }
uint8_t Display::getMotionBlur() { return motionBlur; }