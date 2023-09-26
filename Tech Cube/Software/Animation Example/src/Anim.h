#ifndef ANIM_H
#define ANIM_H

#include <math.h>
#include <stdint.h>

#include "Color.h"
#include "Math3D.h"
#include "Math8.h"
#include "main.h"
#include "img/Anim1.h"

class Anim {
 private:
  int16_t hue16;
  int16_t hue16_speed;
  uint16_t frame = 0;

 public:
  void init() {
    frame = 0;
    hue16_speed = 30 * 255;
  }

  void draw(float dt) {
    hue16 += dt * hue16_speed;
    for (uint8_t z = 0; z < 4; z++) {
      for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t y = 0; y < 4; y++) {
          uint16_t offset = (z << 2) + ((3 - y) << 4)  + x;
          uint32_t data = anim1_data[frame][offset];
          if (data >> 24 & 0xff) {
            cube[x][y][z] = CRGB(data & 0xff, data >> 8 & 0xff, data >> 16 & 0xff);
          }
        }
      }
    }
    if (++frame >= ANIM1_FRAME_COUNT)
      frame = 0;
  }
  
};
#endif