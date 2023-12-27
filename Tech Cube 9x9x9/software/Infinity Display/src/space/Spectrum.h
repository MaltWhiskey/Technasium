#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "Animation.h"

class Spectrum : public Animation {
 private:
  static constexpr auto &settings = config.animation.spectrum;

 public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    hue16_speed = settings.hue_speed * 255;
    setMotionBlur(settings.motionBlur);
  }

  void draw(float dt) {
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness * getBrightness();
    hue16 += dt * hue16_speed;

    if (state == state_t::STARTING) {
      if (timer_starting.update()) {
        state = state_t::RUNNING;
        timer_running.reset();
      } else {
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
      } else {
        brightness *= (1 - timer_ending.ratio());
      }
    }
    auto &fft = config.devices.fft;

    // Adjust bars and draw Spectrum

    for (uint8_t x = 0; x < 9; x++) {
      for (uint8_t z = 0; z < 9; z++) {
        uint16_t magnitude = round(fft.vu[x][z] * 9);
        for (uint8_t y = 0; y < magnitude; y++) {
          Color c = Color(x * 20 + y * 20 + z * 20, LavaPalette);
          Color d = Color((hue16 >> 8) + x * 5 + z * 5, RainbowGradientPalette);
          Color e = Color(25, c, d).scale(brightness);
          voxel(x, y, z, e);
        }
      }
    }
  }
};
#endif