
#include <Arduino.h>

#include "Core/Config.h"
#include "Core/WebServer.h"
#include "Space/animation.h"
/*------------------------------------------------------------------------------
 * Globals
 *----------------------------------------------------------------------------*/
// Global configuration parameters
Config config;
// TaskHandle for running task on different core
TaskHandle_t Task;
void task(void *);
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  // Enable console logging
  Serial.begin(115200);
  Serial.println("Booting...");
  // Load config from file system
  config.load();
  // Initialize animation display
  Animation::begin();
  // Initialize web server communication
  WebServer::begin();
  // Create task1 on core 0
  xTaskCreatePinnedToCore(task, "API", 10000, NULL, 10, &Task, 0);
}
/*------------------------------------------------------------------------------
 * Loop Core 1 -> Animation
 *----------------------------------------------------------------------------*/
void loop() {
  Serial.printf("Animation running on core %d\n", xPortGetCoreID());
  static Timer print_interval = 1.0f;
  while (true) {
    // Run animation rountines and update the display
    Animation::loop();
    // Print FPS once every second
    if (print_interval.update()) {
      Serial.printf("fps=%f\n", Animation::fps());
    }
  }
}
/*------------------------------------------------------------------------------
 * Task1 Core 0 -> Webserver
 *----------------------------------------------------------------------------*/
void task(void *parameter) {
  Serial.printf("Webserver running on core %d\n", xPortGetCoreID());
  while (true) {
    // Check for Web server events
    WebServer::update();
    // Prevents watchdog timeout
    vTaskDelay(1);
  }
}
