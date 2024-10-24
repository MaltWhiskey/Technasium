#include "Display.h"

#include <Arduino.h>
/*------------------------------------------------------------------------------
 * DISPLAY CLASS
 *----------------------------------------------------------------------------*/
CRGB Display::leds[Display::PIXELS];
void Display::begin() {
  // Setup led controller and output pin
  FastLED.addLeds<PL9823, LED_PIN, RGB>(leds, sizeof(leds) / sizeof(CRGB));
  FastLED.setDither(false);
}

void Display::update() {
  FastLED.show();
}

void Display::clear() { memset(leds, 0, sizeof(leds)); }