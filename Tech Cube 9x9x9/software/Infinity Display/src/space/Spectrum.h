#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "Animation.h"

class Spectrum : public Animation {
private:
  static constexpr auto& settings = config.animation.spectrum;

public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
  }

  void draw(float dt) {
    hue16_speed = settings.hue_speed * 255;
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness * getBrightness();
    hue16 += dt * hue16_speed;

    if (state == state_t::STARTING) {
      if (timer_starting.update()) {
        state = state_t::RUNNING;
        timer_running.reset();
      }
      else {
        brightness *= timer_starting.ratio();
      }
    }
    if (state == state_t::RUNNING) {
      if (timer_running.update()) {
        state = state_t::ENDING;
        timer_ending.reset();
      }
    }
    if (state == state_t::ENDING) {
      if (timer_ending.update()) {
        state = state_t::INACTIVE;
        brightness = 0;
      }
      else {
        brightness *= (1 - timer_ending.ratio());
      }
    }
    auto& fft = config.devices.fft;

    for (uint8_t y = 0; y < 9; y++) {
      uint16_t x = fft.level[y] * CX;
      uint16_t z = fft.level[y] * CZ;
      float radius = 1.0f;
      if (x + z > 0) {
        Color e = Color((hue16 >> 8) + (y * 28), RainbowGradientPalette);
        Vector3 a = Vector3(-x, y - CY, -z);
        Vector3 b = Vector3(+x, y - CY, -z);
        Vector3 c = Vector3(+x, y - CY, +z);
        Vector3 d = Vector3(-x, y - CY, +z);
        line(a, b, e, radius);
        line(b, c, e, radius);
        line(c, d, e, radius);
        line(d, a, e, radius);
      }
    }
  }
};
#endif