#ifndef TRAIN_H
#define TRAIN_H

#include "Animation.h"
#include "Palettes.h"

class Train : public Animation {
 private:
  uint16_t brightness;
  // amount of time this animation keeps running
  Timer timer_duration;
  // Color options
  Palettes palettes;
  // Current palette continually blends to target palette
  CRGBPalette16 currentPalette;
  CRGBPalette16 targetPalette;

 public:
  void init() {
    task = task_state_t::RUNNING;
    timer_duration = 10.0f;
    currentPalette = CRGBPalette16(CRGB::Black);
    targetPalette = palettes.get_next_palette();
  }

  void end() {
    // task = task_state_t::ENDING;
  }

/*
  5 naar achter (rechts)
  4 achter rechts naar links
  10 naar voor (links)
  4 voor links naar rechts
  5 achter (rechts)

  6 wiel rechts achter cw
  4 wiel rechts voor cw
  4 wiel links voor ccw
  6 wiel links achter ccw

  7 toren down up
*/
  void draw(float dt) {
    if (timer_duration.update()) {
      timer_duration = 30.0f;
      targetPalette = palettes.get_next_palette();
    }
    brightness = 20;
    uint8_t band[][14] = {{6,7},{5,8},{4,9},{3,10},{2,11},{1,12},{0,13},{27,14},{26,15},{25,16},{24,17},{23,18},{22,19},{21,20}};
    for (uint8_t x=0;x<14;x++) {
      Display::leds[band[x][0]] = ColorFromPalette(currentPalette, x + (hue16>>8), brightness);
      Display::leds[band[x][1]] = ColorFromPalette(currentPalette, x + (hue16>>8), brightness);
    }
    brightness = 100;
    // uint8_t wheel1[][6] = {{28,45},{29,44},{30,43},{31,42},{32,47},{33,46}};
    uint8_t wheel1[][6] = {{33,46},{32,47},{31,42},{30,43},{29,44},{28,45}};
    for (uint8_t x=0;x<6;x++) {
      Display::leds[wheel1[x][0]] = ColorFromPalette(currentPalette, 32 + x * 16 + (hue16>>7), brightness);
      Display::leds[wheel1[x][1]] = ColorFromPalette(currentPalette, 32 + x * 16 + (hue16>>7), brightness);
    }
    // uint8_t wheel2[][4] = {{34,38},{35,41},{36,40},{37,39}};
    uint8_t wheel2[][4] = {{39,37},{36,40},{35,41},{34,38}};
    for (uint8_t x=0;x<4;x++) {
      Display::leds[wheel2[x][0]] = ColorFromPalette(currentPalette, 48 + x * 16 + (hue16>>7), brightness);
      Display::leds[wheel2[x][1]] = ColorFromPalette(currentPalette, 48 + x * 16 + (hue16>>7), brightness);
    }
    for (uint8_t x=48;x<48+7;x++) {
      Display::leds[x] = ColorFromPalette(currentPalette, 64 + x + (hue16>>8), brightness);
    }
    hue16 -= 10000 * dt;
    // dt dependent blend towards target palette
    nblendPaletteTowardPalette(currentPalette, targetPalette, 1000 * dt);
  }
};
#endif