#ifndef STARFIELD_H
#define STARFIELD_H

#include "Animation.h"

class Starfield : public Animation {
private:
  float phase;
  float phase_speed;
  float body_diagonal;

  static const int numStars = 50;
  Vector3 stars[numStars];
  bool initialized = false;

  static constexpr auto &settings = config.animation.starfield;

public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    phase = 0;
    if (!initialized) {
      for (int i = 0; i < numStars; i++) {
        stars[i] = Vector3(noise.nextRandom(-1, 1), noise.nextRandom(-1, 1),
          noise.nextRandom(-1, 1));
      }
      initialized = true;
    }
  }

  void draw(float dt) {
    phase_speed = settings.phase_speed;
    hue16_speed = settings.hue_speed * 255;
    body_diagonal = settings.body_diagonal;
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness * getBrightness();
    phase += dt * phase_speed;
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

    Quaternion q = Quaternion(25.0f * phase, Vector3(0, 1, 0));
    for (int i = 0; i < numStars; i++) {
      float r = (stars[i] * 2.0f - Vector3(0, 0, -2.0f)).magnitude();
      stars[i].z += sinf(phase) * 1.75f * dt * r;
      if (stars[i].z > 1) {
        stars[i] =
          Vector3(noise.nextRandom(-1, 1), noise.nextRandom(-1, 1), -1);
      } else if (stars[i].z < -1) {
        stars[i] = Vector3(noise.nextRandom(-1, 1), noise.nextRandom(-1, 1), 1);
      }
      Color c = Color((hue16 >> 8) + (int8_t)(r * 12), RainbowGradientPalette);
      // Multiply by something a bit bigger than sqrt(3) * radius
      voxel(q.rotate(stars[i]) * body_diagonal, c.scale(brightness));
    }
  }
};
#endif