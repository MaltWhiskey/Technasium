
#include <Arduino.h>

#include "Core/Config.h"
#include "Space/animation.h"
#include "WebServer.h"
/*------------------------------------------------------------------------------
 * Globals
 *----------------------------------------------------------------------------*/
// Global configuration parameters
Config config;
// TaskHandles for running tasks on different cores
TaskHandle_t Task1;
TaskHandle_t Task2;
// Forward declare task functions
void task1(void *);
void task2(void *);
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  // Enable console logging
  Serial.begin(115200);
  // Initialize animation display
  Animation::begin();
  // Load config from file system
  // config.load();
  // Initialize web server communication
  WebServer::begin();
  // Create task1 on core 0 (higher number is higher priority)
  xTaskCreatePinnedToCore(task1, "Task1", 10000, NULL, 10, &Task1, 0);
  delay(500);
  // Create task2 on core 1 (higher number is higher priority)
  xTaskCreatePinnedToCore(task2, "Task2", 10000, NULL, 20, &Task2, 1);
  delay(500);
}
/*------------------------------------------------------------------------------
 * Loop Core 1
 *----------------------------------------------------------------------------*/
void loop() { delay(1000); }
/*------------------------------------------------------------------------------
 * Task1 Core 0
 *----------------------------------------------------------------------------*/
void task1(void *parameter) {
  Serial.printf("Task1: Wifi running on core %d\n", xPortGetCoreID());
  while (true) {
    // Check for Web server events
    WebServer::update();
    // Prevents watchdog timeout
    vTaskDelay(1);
  }
}
/*------------------------------------------------------------------------------
 * Task2 Core 1
 *----------------------------------------------------------------------------*/
void task2(void *parameter) {
  Serial.printf("Task2: Animations running on core %d\n", xPortGetCoreID());
  // Run animation on blocking fastled. Interrupts mess up the led data timeing
  // Need a way to disable interrupts and watchdog on Core 1 or use a
  // nonblocking FastLed controller (DMA based?)
  while (true) {
    // Print FPS once every second
    static Timer print_interval = 1.0f;
    // Run animation rountines and update the display
    Animation::loop();
    if (print_interval.update()) {
      Serial.printf("fps=%f\n", Animation::fps());
    }
  }
}