#include "Display.h"

#include <Arduino.h>
/*------------------------------------------------------------------------------
 * DISPLAY CLASS
 *----------------------------------------------------------------------------*/
Color Display::cube[width][height][depth];
CRGB Display::leds[width * height * depth];
void Display::begin() {
  // Setup led controller and output pin
  FastLED.addLeds<PL9823, LED_PIN, GRB>(leds, sizeof(leds) / sizeof(CRGB));
  FastLED.setDither(false);
}

void Display::update() {
  for (uint8_t x = 0; x < Display::width; x++) {
    uint8_t xx = x * Display::height;
    for (uint8_t y = 0; y < Display::height; y++) {
      uint8_t yy = (x & 1) ? Display::height - 1 - y : y;
      for (uint8_t z = 0; z < Display::depth; z++) {
        uint8_t zz = (z + (z & 1)) * Display::height * Display::width;
        uint8_t led = (z & 1) ? zz - xx - yy - 1 : xx + yy + zz;
        leds[led] = CRGB(cube[x][y][z].g, cube[x][y][z].r, cube[x][y][z].b);
      }
    }
  }
  FastLED.show();
}

void Display::clear() { memset(cube, 0, sizeof(cube)); }

// Radiate between led's linear interpolated.
// Distance Length = 0% light
// Distance 0 = 100% light.
// Use gamma to aproximate inverse square law
void Display::radiate(const Vector3 &origin, const Color &color, float length) {
  static uint8_t x1, x2, y1, y2, z1, z2;
  Vector3 box = Vector3(length, length, length);
  Vector3 min = origin - box;
  Vector3 max = origin + box;

  // Keep within visible limits
  x1 = min.x < 0 ? 0 : roundf(min.x);
  y1 = min.y < 0 ? 0 : roundf(min.y);
  z1 = min.z < 0 ? 0 : roundf(min.z);
  x2 = max.x < Display::width - 1 ? roundf(max.x) : Display::width - 1;
  y2 = max.y < Display::height - 1 ? roundf(max.y) : Display::height - 1;
  z2 = max.z < Display::depth - 1 ? roundf(max.z) : Display::depth - 1;

  for (uint8_t x = x1; x <= x2; x++)
    for (uint8_t y = y1; y <= y2; y++)
      for (uint8_t z = z1; z <= z2; z++) {
        float radius = (Vector3(x, y, z) - origin).magnitude();
        if (radius < length) {
          Color c1 = cube[x][y][z];
          Color c2 = color.scaled((1 - (radius / length)) * 255);
          cube[x][y][z] = Color(c1.red > c2.red ? c1.red : c2.red,
                                c1.green > c2.green ? c1.green : c2.green,
                                c1.blue > c2.blue ? c1.blue : c2.blue);
        }
      }
}