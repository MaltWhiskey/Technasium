#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include <FastLED.h>
#include <stdint.h>

#include "power/Color.h"
#include "power/Math3D.h"

class Display {
 public:
  static const uint8_t LED_PIN = 16;
  const static uint8_t PIXELS = 28+4+4+6+6+7;
  static CRGB leds[PIXELS];

 public:
  // Do not use a class contructor to start the display (Arduino compatibility)
  static void begin();
  // Notifies the display that a frame is ready for displaying
  static void update();
  // Clear the cube so a new frame can be created fresh
  static void clear();
};
#endif