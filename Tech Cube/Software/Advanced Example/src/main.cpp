
#include <Arduino.h>

#include "core/Config.h"
#include "space/Animation.h"
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
  Animation::animate();
  if (print_interval.update()) {
    Serial.print("fps=");
    Serial.print(Animation::fps());
    Serial.println();
  }
}