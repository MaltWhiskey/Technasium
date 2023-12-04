
#include <Arduino.h>
#include <AsyncUDP.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/soc.h>

#include "arduinoFFT.h"
#include "core/Config.h"
#include "core/INMP441.h"
#include "core/WebServer.h"
#include "space/Animation.h"
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
  // Save brownout register
  // uint32_t brownout_reg = READ_PERI_REG(RTC_CNTL_BROWN_OUT_REG);
  // Enable console logging
  Serial.begin(115200);
  Serial.println("Booting ESP32");
  // Load config from file system
  config.load();
  // Initialize animation display
  Animation::begin();
  // Disable brownout detector
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  // Initialize web server communication
  WebServer::begin();
  // Create task1 on core 0
  xTaskCreatePinnedToCore(task, "TASK", 10000, NULL, 10, &Task, 0);
  // Restore brownout detector
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, brownout_reg);
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
  const uint16_t SAMPLES = 512;
  int16_t samples[SAMPLES];
  const uint32_t audio_frequency = 44100;
  I2S_MIC::begin(audio_frequency);

  // UDP Destination
  IPAddress udpAddress(192, 168, 178, 25);
  const int udpPort = 3333;
  AsyncUDP udp;
  if (!udp.connect(udpAddress, udpPort)) {
    Serial.println("UDP error, exit task");
    return;
  }

  while (true) {
    // Prevents watchdog timeout
    vTaskDelay(1);
    // Check for Web server events
    WebServer::update();
    // Process microphone samples
    uint16_t samples_read = I2S_MIC::loop(samples, SAMPLES);
    // Log to UPD socket (keep packet size 1024 same python script)
    udp.write((uint8_t *)samples, 1024);
  }
}

// Process FFT on samples
/*    for (int i = 0; i < SAMPLES; i++) {
      samples16[i] = samples[i] >> 11;
    }
    samples_read = I2S_MIC::loop(samples, SAMPLES);
    for (int i = 0; i < SAMPLES; i++) {
      samples16[SAMPLES + i] = samples[i] >> 11;
    }*/
// Approx_FFT(fft_in, fft_real, fft_imag, audio_samples, audio_frequency);
//    for (uint16_t i = 0; i < 10; i++) Serial.printf("%d ", fft_real[i] >>
//    8);
//  Serial.println();
// NORMALLY bin zero has dc and mains hum, should ignore.
// NORMALLY 2nd half of bins are not usable
// THIS appox library i don't know and i don't care so lets
// just ignore bin[0] and skip 2nd half > bin[m_bins/2]
// Also m_bins = 512 so lets hard code it, to make it easy
// i need 64 channels for nice 4x16x4 analyzer
// for (uint16_t i = 0; i < 64; i++) {
//  config.hid.fft.data[i] = random(0, 8);
//  config.hid.fft.updated = true;
//}
