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
  static const uint8_t width = 4;
  static const uint8_t height = 4;
  static const uint8_t depth = 4;
  static Color cube[width][height][depth];
  static CRGB leds[width * height * (depth + 1)];

 public:
  // Do not use a class contructor to start the display (Arduino compatibility)
  static void begin();
  // Notifies the display that a frame is ready for displaying
  static void update();
  // Clear the cube so a new frame can be created fresh
  static void clear();
  // Radiate light from a pixel in the cube
  static void radiate(const Vector3 &, const Color &, float length);
};
#endif