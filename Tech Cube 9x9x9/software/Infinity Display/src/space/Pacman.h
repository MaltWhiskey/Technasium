#ifndef PACMAN_H
#define PACMAN_H

#include "Animation.h"
#include "gfx/Pacman.h"
#include "power/Math8.h"

class Pacman : public Animation {
private:
  float angle;
  float angle_speed;

  float radius_start;
  float radius_max;
  float arc;

  Timer timer_interval;

  uint8_t frame = 0;
  uint8_t frame_display[4] = { 0, 1, 2, 1 };

  static constexpr auto& settings = config.animation.pacman;

public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    timer_interval = settings.interval;
    angle = 0;
  }

  void draw(float dt) {
    angle_speed = settings.angle_speed;
    radius_max = settings.radius;
    radius_start = settings.radius_start;
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness * getBrightness();

    float radius = radius_max;

    if (state == state_t::STARTING) {
      if (timer_starting.update()) {
        state = state_t::RUNNING;
        timer_running.reset();
      }
      else {
        brightness *= timer_starting.ratio();
        radius *= timer_starting.ratio();
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
        radius *= (1 - timer_ending.ratio());
      }
    }
    if (radius < radius_start) {
      radius = radius_start;
    }

    if (timer_interval.update()) {
      if (++frame >= sizeof(frame_display)) frame = 0;
    }

    angle += dt * angle_speed;
    arc = 2 * (180 / M_PI) * asinf(0.5f / radius);

    for (uint8_t y = 0; y < FRAME_HEIGHT; y++) {
      for (uint8_t x = 0; x < FRAME_WIDTH; x++) {
        uint32_t data = pacman_data[frame_display[frame]][y * FRAME_WIDTH + x];
        Color c = Color(0, 0, 0);
        if (data >> 24 & 0xff) {
          c = Color(data & 0xff, data >> 8 & 0xff, data >> 16 & 0xff);
          if (c.isBlack())
            c = Color(0, 0, 0);
          else {
            c.gamma().scale(brightness);
          }
        }
        // Map to coordinates with center (0,0,0) scaled by radius
        // Project on a line at the left of the coordinate system
        Vector3 point = Vector3(-radius, (CY - y) / CY * radius, 0);
        Quaternion q = Quaternion(angle - (arc * x), Vector3::Y);
        radiate(q.rotate(point), c, 1.0f);
      }
    }
  }
};
#endif