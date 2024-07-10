#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include <stdint.h>

#include "power/Color.h"
#include "power/Math3D.h"

class Display {
 public:
  static const uint8_t width = 9;
  static const uint8_t height = 9;
  static const uint8_t depth = 9;
  // Buffer currently being used for drawing
  static uint32_t cubeBuffer;
  // Create two cube buffers for motion blurring
  static Color cube[2][width][height][depth];

 private:
  // Special display effect for all animations
  static uint8_t motionBlur;
  static uint8_t brightness;

 public:
  // Do not use a class contructor to start the display (Arduino compatibility)
  static void begin();
  // Notifies the display that a frame is ready for displaying
  static void update();
  // Check if the display is available to accept a new frame
  static void clear();
  // Set the motion blur value higher is more blur
  static void setMotionBlur(const uint8_t);
  static uint8_t getMotionBlur();
  // Set the master display brightness value
  static void setBrightness(const uint8_t);
  static uint8_t getBrightness();
};
#endif