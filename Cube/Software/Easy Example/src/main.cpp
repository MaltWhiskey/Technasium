
#include <Arduino.h>
#include <FastLED.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
static const uint8_t LED_PIN = 16;
static const uint8_t width = 4;
static const uint8_t height = 4;
static const uint8_t depth = 4;
static CRGB cube[width][height][depth];
static CRGB leds[width * height * depth];

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, sizeof(leds) / sizeof(CRGB));
  FastLED.setDither(false);
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
  static int s = 0;

  if (s++ < 10) {
    for (int x = 0; x < width; x++) {
      cube[x][y][0] = CRGB(100, 80 * x, 0);
    }
    int t = y + a;
    if (t > height - 1 || t < 0) a = -a;
    y += a;
    display_update(1000);
    display_clear();
  } else {
    static int xx = 0;
    static int yy = 0;
    static int zz = 0;
    cube[xx++][yy][zz] = CRGB((xx + 1) * 40, (yy) * 10, (zz) * 10);
    if (xx > width - 1) {
      xx = 0;
      yy++;
    }
    if (yy > height - 1) {
      yy = 0;
      zz++;
    }
    if (zz > depth - 1) {
      zz = 0;
      s = 0;
      display_update(3000);
      display_clear();
    }
    display_update(20);
  }
}