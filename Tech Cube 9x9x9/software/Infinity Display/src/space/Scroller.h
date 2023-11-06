#ifndef SCROLLER_H
#define SCROLLER_H

#include "Animation.h"
#include "gfx/Charset2.h"
#include "power/Math8.h"

class Scroller : public Animation {
 private:
  float radius;
  float text_rotation;
  float text_rotation_speed;
  String text = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  static constexpr auto &settings = config.animation.scroller;

 public:
  void init() {
    state = state_t::STARTING;
    timer_starting = settings.starttime;
    timer_running = settings.runtime;
    timer_ending = settings.endtime;
    text_rotation = -100.0f;
    text_rotation_speed = settings.rotation_speed;
    radius = settings.radius;
  }
  void set_text(String s) { text = s; }

  void draw(float dt) {
    setMotionBlur(settings.motionBlur);
    uint8_t brightness = settings.brightness;

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

    // Amount of degrees the text has been rotated
    text_rotation += text_rotation_speed * dt;
    // Full circle resolution in amount of pixels
    float circle_resolution = 2 * PI * radius;
    // Angle adjustment in degrees for each line
    float line_angle_adj = 360 / circle_resolution;
    // Amount of pixels the text has been rotated
    float pixel_start = text_rotation / line_angle_adj;
    // Start of the line_angle at a bit over 90 degrees
    float line_angle = 100.0f;
    // Fine adjustment for angles between lines.
    // Ignore because this will add a lot of jittering.
    // line_angle += fmod(text_rotation, line_angle_adj);

    // Adjust for negative rotation (inserts blanks at begin text)
    int16_t pixel_line = pixel_start;
    if (pixel_line < 0) {
      line_angle += line_angle_adj * pixel_line;
      pixel_line = 0;
    }
    //  Insert an amount of blank_lines after every character
    const uint16_t blank_lines = 1;

    uint16_t text_lines = (CHARSET_FRAME_HEIGHT + blank_lines) * text.length();
    while (line_angle > -line_angle_adj) {
      uint16_t text_offset = pixel_line % text_lines;
      uint16_t char_offset = text_offset / (CHARSET_FRAME_HEIGHT + blank_lines);
      uint16_t t = match_char(text.charAt(char_offset));
      uint16_t y = pixel_line++ % (CHARSET_FRAME_HEIGHT + blank_lines);
      if (y < CHARSET_FRAME_HEIGHT) {
        Quaternion q = Quaternion(line_angle, Vector3::X);
        for (uint8_t x = 0; x < CHARSET_FRAME_WIDTH; x++) {
          uint32_t data = charset_data[t][y * CHARSET_FRAME_WIDTH + x];
          if (data & 0xff000000) {
            Color c = Color(data & 0xff, data >> 8 & 0xff, data >> 16 & 0xff);
            Vector3 pixel = q.rotate(
                Vector3(x / (CHARSET_FRAME_WIDTH - 1.0f), 0, -1) * radius);
            pixel += Vector3(-radius / 2, -radius / 2, radius / 2);
            voxel(pixel, c.scale(brightness).gamma());
          }
        }
      }
      line_angle -= line_angle_adj;
    }
  }

  uint16_t match_char(uint16_t chr) {
    if (chr >= ' ' && chr <= 'Z')
      return chr - ' ';
    else
      return '#' - ' ';
  }
};
#endif