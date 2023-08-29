#ifndef HELIX_H
#define HELIX_H

#include <math.h>
#include <stdint.h>

#include "Color.h"
#include "Math3D.h"
#include "Math8.h"
#include "main.h"

class Helix {
 private:
  float radius;
  float resolution;
  float phase;
  float phase_speed;
  int16_t hue16;
  int16_t hue16_speed;

 public:
  void init() {
    phase_speed = 3.0f;
    hue16_speed = 30 * 255;
    radius = 1.5f;
    resolution = 5;
    phase = 0;
  }

  void draw(float dt) {
    phase += dt * phase_speed;
    hue16 += dt * hue16_speed;

    Quaternion q1 = Quaternion(180, Vector3(0, 1, 0));
    for (uint16_t y = 0; y <= resolution; y++) {
      float xf = sinf(phase + mmapf(y, 0, resolution, 0, 2 * PI));
      float zf = cosf(phase + mmapf(y, 0, resolution, 0, 2 * PI));
      Vector3 p = Vector3(xf, 2 * (y / resolution) - 1, zf) * radius;
      Color c = Color((hue16 >> 8) + y * 2 + 000, RainbowGradientPalette);
      p += Vector3(radius, radius, radius);
      cube[(int)roundf(p.x)][(int)roundf(p.y)][(int)roundf(p.z)] =
          CRGB(c.r, c.g, c.b);
    }
  }
};
#endif