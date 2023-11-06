#include "Animation.h"

#include "Accelerometer.h"
#include "Arrows.h"
#include "Atoms.h"
#include "Fireworks.h"
#include "Helix.h"
#include "Life.h"
#include "Mario.h"
#include "Plasma.h"
#include "Pong.h"
#include "Scroller.h"
#include "Sinus.h"
#include "Spectrum.h"
#include "Starfield.h"
#include "Twinkels.h"
/*------------------------------------------------------------------------------
 * ANIMATION STATIC DEFINITIONS
 *----------------------------------------------------------------------------*/
Noise Animation::noise = Noise();
Timer Animation::animation_timer = Timer();
uint16_t Animation::animation_sequence = 0;
/*------------------------------------------------------------------------------
 * ANIMATION GLOBAL DEFINITIONS
 *----------------------------------------------------------------------------*/
Accelerometer accelerometer;
Arrows arrows;
Atoms atoms;
Fireworks fireworks1;
Fireworks fireworks2;
Helix helix;
Life life;
Mario mario;
Plasma plasma;
Pong pong;
Scroller scroller;
Sinus sinus;
Spectrum spectrum;
Starfield starfield;
Twinkels twinkels;

Animation *Animations[] = {&atoms,      &sinus,    &starfield,    &fireworks1,
                           &fireworks2, &twinkels, &helix,        &arrows,
                           &plasma,     &mario,    &life,         &pong,
                           &spectrum,   &scroller, &accelerometer};

const uint8_t ANIMATIONS = sizeof(Animations) / sizeof(Animation *);
/*----------------------------------------------------------------------------*/
// Start display asap to minimize PL9823 blue startup
void Animation::begin() { Display::begin(); }

// Render an animation frame, but only if the display allows it (non blocking)
void Animation::loop() {
  // Only draw to the display if it's available
  if (/*Display::available()*/ true) {
    // Update the animation timer to determine frame deltatime
    animation_timer.update();
    // Clear the display before drawing any animations
    Display::clear();
    // Draw all active animations from the animation pool
    uint8_t active_animation_count = 0;
    for (uint8_t i = 0; i < ANIMATIONS; i++) {
      Animation &animation = *Animations[i];
      if (animation.state != state_t::INACTIVE) {
        animation.draw(animation_timer.dt());
      }
      // Animation can become inactive after drawing so check again
      if (animation.state != state_t::INACTIVE) {
        active_animation_count++;
        if (settings.changed) animation.end();
      }
    }
    // Clear the settings changed flag. Animations are ended allready
    settings.changed = false;
    // Select the next or specific animation from the sequence list
    if (active_animation_count == 0) {
      Animation::next(settings.play_one, settings.animation);
    }
    // Commit current animation frame to the display
    Display::update();
  }
}

// Override end method if more is needed than changing state and ending timer.
void Animation::end() {
  if (state == state_t::STARTING) {
    timer_ending = Timer(2.0f, timer_starting.ratio(), true);
  } else if (state == state_t::RUNNING) {
    timer_ending = 2.0f;
  } else if (state == state_t::ENDING && !time_reduction) {
    timer_ending = Timer(2.0f, timer_ending.ratio(), false);
  }
  state = state_t::ENDING;
  time_reduction = true;
}

// Get fps, if animate has been called than t > 0
float Animation::fps() {
  if (animation_timer.dt() > 0) {
    return 1 / animation_timer.dt();
  }
  return 0;
}

void FIREWORKS() {
  fireworks2.init();
  fireworks2.timer_running = fireworks1.timer_running;
}
void SCROLLER() { scroller.set_text("#MALT WHISKEY"); }
void TWINKELS1() {
  twinkels.set_mode(true, false);
  twinkels.set_color(Color(255, 150, 30));
  twinkels.set_clear();
}
void TWINKELS2() { twinkels.set_mode(false, true); }

jump_item_t Animation::get_item(uint16_t index) {
  const jump_item_t jump_table[] = {
      {"Scroller", "Circulair text scroller ", &SCROLLER, &scroller},
      {"Pong", "The classical game of Pong", 0, &pong},
      {"Mario", "Super Mario Run", 0, &mario},
      {"Accelerometer", "Test accelerometer", 0, &accelerometer},
      {"Arrows", "Moving arrows", 0, &arrows},
      {"Atoms", "Electons arround nucleas", 0, &atoms},
      {"Fireworks", "Fireing Fireworks", &FIREWORKS, &fireworks1},
      {"Helix", "Double strand DNA", 0, &helix},
      {"Life", "Game of Life 3D", 0, &life},
      {"Mario", "Super Mario Run", 0, &mario},
      {"Plasma", "Perlin noise plasma field", 0, &plasma},
      {"Sinus", "3D Wave Function", 0, &sinus},
      {"Spectrum", "WiFi Spectrum Analyser", 0, &spectrum},
      {"Starfield", "To boldly go...", 0, &starfield},
      {"Fairylights", "Beautifull fairylights", &TWINKELS1, &twinkels},
      {"Multilights", "Multicolor fairylights", &TWINKELS2, &twinkels},
      {0, 0, 0, 0}};
  const uint16_t JUMPITEMS = sizeof(jump_table) / sizeof(jump_item_t) - 1;
  if (index > JUMPITEMS)
    return jump_table[JUMPITEMS];
  else
    return jump_table[index];
}

void Animation::next(bool play_one, uint16_t index) {
  if (play_one) {
    // Play one specific animation endlessly
    jump_item_t jump = Animation::get_item(index);
    if (jump.object) {
      jump.object->init();
      jump.object->time_reduction = false;
      jump.object->timer_running = 0;
    }
    if (jump.custom_init) jump.custom_init();
  } else {
    // Play the next animation from the sequence
    jump_item_t jump = get_item(animation_sequence++);
    if (!jump.object) {
      animation_sequence = 0;
      jump = get_item(animation_sequence++);
    }
    if (jump.object) {
      jump.object->init();
      jump.object->time_reduction = false;
    }
    if (jump.custom_init) jump.custom_init();
  }
}