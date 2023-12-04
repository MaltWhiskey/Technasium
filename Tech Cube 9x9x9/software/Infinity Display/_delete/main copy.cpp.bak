
#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/soc.h>

#include "Core/Config.h"
#include "Core/INMP441.h"
#include "Core/WebServer.h"
#include "Space/animation.h"
#include "fft/FFT.h"
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
  uint32_t brownout_reg = READ_PERI_REG(RTC_CNTL_BROWN_OUT_REG);
  // Enable console logging
  Serial.begin(115200);
  Serial.println("Booting ESP32");
  // Load config from file system
  config.load();
  // Initialize animation display
  Animation::begin();
  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
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
  // Memory for audio and fft data
  const uint16_t buffer_size = 512;
  const uint32_t audio_frequency = 44100;
  const uint32_t sample_count = audio_frequency;
  static int32_t *samples =
      (int32_t *)heap_caps_malloc(audio_frequency * 4, MALLOC_CAP_SPIRAM);
  uint32_t samples_remaining = sample_count;
  // Initialize microphone
  I2S_MIC::begin(audio_frequency);

  WiFiClient wifi;
  while (true) {
    // Prevents watchdog timeout
    vTaskDelay(1);
    // Check for Web server events
    // WebServer::update();
    if (samples == 0) {
      Serial.println("NULL");
      continue;
    }

    // Process microphone samples
    if (samples_remaining > (buffer_size >> 2)) {
      Serial.printf("Remaining = %d\n", samples_remaining);
      uint16_t samples_read = I2S_MIC::loop(
          samples + sample_count - samples_remaining, buffer_size);
      samples_remaining -= samples_read;
    } else {
      Serial.println("Posting data");
      // Initialize wifi client to send sample data
      HttpClient http = HttpClient(wifi, "192.168.178.25", 5003);
      http.post("/i2s_samples/", "application/octet-stream", sample_count * 4,
                (uint8_t *)samples);
      http.flush();
      samples_remaining = sample_count;
    }
  }
}
//// pio device monitor > floats.txt

// Process FFT on samples
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
