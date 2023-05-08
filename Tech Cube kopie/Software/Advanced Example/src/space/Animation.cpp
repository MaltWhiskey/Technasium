#include "Animation.h"

#include "Helix.h"
#include "Sinus.h"
#include "Starfield.h"
/*------------------------------------------------------------------------------
 * ANIMATION STATIC DEFINITIONS
 *----------------------------------------------------------------------------*/
Noise Animation::noise = Noise();
Timer Animation::animation_timer = Timer();
uint16_t Animation::animation_sequence = 0;
/*------------------------------------------------------------------------------
 * ANIMATION GLOBAL DEFINITIONS
 *----------------------------------------------------------------------------*/
Sinus sinus;
Starfield starfield;
Helix helix;
Animation *Animations[] = {&sinus, &starfield, &helix};
const uint8_t ANIMATIONS = sizeof(Animations) / sizeof(Animation *);
/*----------------------------------------------------------------------------*/
// Start display asap to minimize PL9823 blue startup
void Animation::begin() { Display::begin(); }
// Gracefully terminate animation
void Animation::end() { task = task_state_t::ENDING; }

// Render an animation frame, but only if the display allows it (non blocking)
void Animation::animate() {
  Display::clear();
  // Update the animation timer to determine frame deltatime
  animation_timer.update();
  // Draw all active animations from the animation pool
  uint8_t active_count = 0;
  for (uint8_t i = 0; i < ANIMATIONS; i++) {
    Animation &animation = *Animations[i];
    if (animation.task != task_state_t::INACTIVE) {
      animation.draw(animation_timer.dt());
    }
    if (animation.task != task_state_t::INACTIVE) {
      active_count++;
      if (config.animation.animation < ANIMATIONS) {
        animation.end();
      }
    }
  }
  if (active_count == 0) {
    if (config.animation.animation < ANIMATIONS) {
      Animation::select();
    } else {
      Animation::next();
    }
  }
  // Commit current animation frame to the display
  Display::update();
}

// Get fps, if animate has been called t > 0
float Animation::fps() {
  if (animation_timer.dt() > 0) {
    return 1 / animation_timer.dt();
  }
  return 0;
}

void SEQ_SINUS_00(void) {
  sinus.init(5, 20, 10);
  sinus.speed(PI, -50);
}
void SEQ_STARFIELD_00(void) {
  starfield.init(config.starfield.fade_in_speed,
                 config.starfield.timer_duration,
                 config.starfield.fade_out_speed);
  starfield.speed(config.starfield.phase_speed, config.starfield.hue_speed);
}
void SEQ_HELIX_00(void) { helix.init(); }

// Animation sequencer jumptable implementation
void Animation::next() {
  static void (*jump_table[])() =  //
      {&SEQ_HELIX_00, &SEQ_STARFIELD_00, &SEQ_SINUS_00};
  if (animation_sequence >= sizeof(jump_table) / sizeof(void *)) {
    animation_sequence = 0;
  }
  jump_table[animation_sequence++]();
}

// Animation selector jumptable implementation
void Animation::select() {
  static void (*jump_table[])() =  //
      {&SEQ_SINUS_00};
  if (config.animation.animation >= sizeof(jump_table) / sizeof(void *)) {
    Animation::next();
  } else {
    jump_table[config.animation.animation]();
  }
  config.animation.animation = 0xff;
}