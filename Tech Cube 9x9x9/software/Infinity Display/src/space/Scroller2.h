#ifndef SCROLLER2_H
#define SCROLLER2_H

#include "Animation.h"
#include "gfx/font.h"
#include "power/Math8.h"

class Scroller2 : public Animation {
private:
  // amount of time before moving text_position
  Timer timer_interval;
  uint16_t running_pos;
  uint16_t offset_pos;
  uint8_t rotation[32] =
    {0,0,0,0,0,0,0,0, 0,1,2,3,4,5,6,7, 8,8,8,8,8,8,8,8, 8,7,6,5,4,3,2,1};
  String text;

  static constexpr auto& settings = config.animation.scroller2;
public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    timer_interval = settings.interval;
    running_pos = 0;
    offset_pos = 32;
  }
  void set_text(String s) { text = s; }

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

    for(uint8_t b = offset_pos; b<32; b++) {
      uint16_t chr_offset = (b + running_pos - offset_pos) / 8;
      uint16_t bit_offset = (b + running_pos - offset_pos) % 8;

      if(chr_offset >= text.length()) continue;
      uint8_t chr = text[chr_offset];

      for(int y=7;y>=0;y--) {
        uint8_t bitset = charset[chr][7-y];
        if(bitset & (1 << bit_offset)) {
          int x = rotation[(b)%32];
          int z = rotation[(24+b)%32];
          Color c = Color((hue16 >> 8), RainbowGradientPalette);
          if(chr==0x1F) {
            c = Color::RED;
          }
          voxel(x,y,z, c.scale(brightness));
        }
      }
    }

    if (timer_interval.update()) {
      offset_pos > 0 ? offset_pos-- : running_pos++;
    }
    if((running_pos >> 3) >= text.length()) {
      running_pos = 0;
      offset_pos = 32;
    }
  }
};
#endif