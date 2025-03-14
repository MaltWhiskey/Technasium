#include "Animation.h"

#include "Train.h"
#include "Deepsleep.h"
/*------------------------------------------------------------------------------
 * ANIMATION STATIC DEFINITIONS
 *----------------------------------------------------------------------------*/
Noise Animation::noise = Noise();
Timer Animation::animation_timer = Timer();
uint16_t Animation::animation_sequence = 0;
/*------------------------------------------------------------------------------
 * ANIMATION GLOBAL DEFINITIONS
 *----------------------------------------------------------------------------*/
Train train;
Deepsleep deepsleep;
Animation *Animations[] = {&train,&deepsleep};
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
    }
  }
  if (active_count == 0) {
      Animation::next();
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

void SEQ_TRAIN_01(void) { deepsleep.init(); }
void SEQ_TRAIN_00(void) { train.init(); }

// Animation sequencer jumptable implementation
void Animation::next() {
  static void (*jump_table[])() =  //
      {&SEQ_TRAIN_00,&SEQ_TRAIN_01};
  if (animation_sequence >= sizeof(jump_table) / sizeof(void *)) {
    animation_sequence = 0;
  }
  jump_table[animation_sequence++]();
}