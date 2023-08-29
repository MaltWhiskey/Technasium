#ifndef ANIM_H
#define ANIM_H

#include <math.h>
#include <stdint.h>

#include "Color.h"
#include "Math3D.h"
#include "Math8.h"
#include "main.h"
#include "Charset.h"

class Anim {
 private:
  int16_t hue16;
  int16_t hue16_speed;
  uint16_t frame = 0;

 public:
  void init() {
    hue16_speed = 30 * 255;
  }

  void draw(float dt) {
    hue16 += dt * hue16_speed;
    for (uint8_t z = 0; z < 4; z++) {
      for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 4; y++) {
          uint16_t offset = x + (24 - y * 8) + ((z&1) << 2) + ((z&2) << 4);
          uint32_t data = charset_data[33][offset];
          if (data >> 24 & 0xff) {
            cube[x][y][z] = CRGB(data & 0xff, data >> 8 & 0xff, data >> 16 & 0xff);
          }
        }
      }
    }
  }
};
#endif