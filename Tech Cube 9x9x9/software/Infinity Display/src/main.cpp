
#include <Arduino.h>

#include "Core/Config.h"
#include "Space/animation.h"
/*---------------------------------------------------------------------------------------
 * Globals
 *-------------------------------------------------------------------------------------*/
Config config;
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  Animation::begin();
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting");
}
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  // Print FPS once every second
  static Timer print_interval = 1.0f;
  Animation::loop();
  if (print_interval.update()) {
    Serial.print("fps=");
    Serial.print(Animation::fps());
    Serial.println();
  }
}