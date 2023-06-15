#ifndef SINUS_H
#define SINUS_H

#include "Animation.h"

class Sinus : public Animation {
 private:
  float x_min = -2;
  float x_max = 2;
  float y_min = -2;
  float y_max = 2;
  float phase = 0.0f;
  float phase_speed = 0.0f;
  int16_t hue_speed = 0;

  Timer timer;
  float time_duration;
  float time_fade_out;

 public:
  void init(float fade_in, float duration, float fade_out) {
    task = task_state_t::STARTING;
    timer = fade_in;

    phase = 0.0f;
    time_duration = duration;
    time_fade_out = fade_out;
  }

  void speed(float phase_speed_, int16_t hue_speed_) {
    phase_speed = phase_speed_;
    hue_speed = hue_speed_;
  }

  void draw(float dt) {
    phase += phase_speed * dt;
    hue16 += (int16_t)(255 * hue_speed * dt);

    uint8_t scale = 0;
    if (task == task_state_t::ENDING) {
      if (timer.update()) {
        task = task_state_t::INACTIVE;
        scale = 0;
      } else
        scale = (1 - timer.percent()) * 255;
    }
    if (task == task_state_t::RUNNING) {
      if (timer.update()) {
        task = task_state_t::ENDING;
        timer = time_fade_out;
      }
      scale = 255;
    }
    if (task == task_state_t::STARTING) {
      if (timer.update()) {
        task = task_state_t::RUNNING;
        timer = time_duration;
        scale = 255;
      } else
        scale = timer.percent() * 255;
    }
    for (uint8_t x = 0; x < Display::width; x++) {
      // convert cube x to floating point coordinate between x_min and x_max
      float xprime = mapf(x, 0, Display::width - 1, x_min, x_max);
      for (uint8_t y = 0; y < Display::height; y++) {
        // convert cube z to floating point coordinate between z_min and z_max
        float yprime = mapf(y, 0, Display::depth - 1, y_min, y_max);
        // determine y floating point coordinate
        float zprime = sinf(phase + sqrtf(xprime * xprime + yprime * yprime));
        // convert floating point y back to cube y
        float z = mapf(zprime, -1, 1, 0, Display::height - 1);
        // display voxel on the cube
        Color c = Color((hue16 >> 8) + y * 10, RainbowGradientPalette);
        Display::radiate(Vector3(x, z, y), c.scale(scale), 1.0f);
      }
    }
  }
};
#endif