
#define BAND_SIZE 8
#include <Arduino.h>
#include <AsyncUDP.h>

#include "core/Config.h"
#include "core/INMP441.h"
#include "core/WebServer.h"
#include "fft/arduinoFFT.h"
#include "space/Animation.h"
/*------------------------------------------------------------------------------
 * Globals
 *----------------------------------------------------------------------------*/
// Global configuration parameters
Config config;
// TaskHandle for running task on different core
TaskHandle_t Task;
void web_fft_task(void *);
void wav_rec_task(void *);
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
void setup() {
  // Enable console logging
  Serial.begin(115200);
  Serial.println("Booting ESP32");
  // Load config from file system
  config.load();
  // Initialize animation display
  Animation::begin();
  // Initialize web server communication
  WebServer::begin();
  // Create task1 on core 0
  xTaskCreatePinnedToCore(web_fft_task, "TASK", 50000, NULL, 10, &Task, 0);
}
/*------------------------------------------------------------------------------
 * Task Core 1 -> Animation
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
 * Audio Recorder
 * Import in Audacity as raw signed 32 bits, 3 bytes shift
 *----------------------------------------------------------------------------*/
void wav_rec_task(void *parameter) {
  Serial.printf("Audio recorder running on core %d\n", xPortGetCoreID());
  const uint16_t SAMPLES = 256;
  const uint32_t SAMPLING_FREQ = 44100;
  int32_t samples[SAMPLES];
  I2S_MIC::begin(SAMPLING_FREQ);

  // UDP Destination (ifconfig|grep mask)
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
    // Process microphone samples
    uint16_t samples_read = I2S_MIC::loop(samples, SAMPLES);
    // Log to UPD socket (keep packet size 1024 same as python script)
    udp.write((uint8_t *)samples, 1024);
  }
}
/*------------------------------------------------------------------------------
 * Webserver, Audio Analyzer
 *----------------------------------------------------------------------------*/
void web_fft_task(void *parameter) {
  Serial.printf("Webserver running on core %d\n", xPortGetCoreID());
  const uint16_t SAMPLES = 1024;
  const uint32_t SAMPLING_FREQ = 44100;

  uint32_t bands[10] = {1, 2, 4, 8, 16, 32, 64, 128, 180, 240};
  const uint16_t BANDS = sizeof(bands) / sizeof(bands[0]) - 1;

  int32_t samples[SAMPLES];
  I2S_MIC::begin(SAMPLING_FREQ);
  float vReal[SAMPLES];
  float vImag[SAMPLES];
  ArduinoFFT<float> FFT =
      ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

  while (true) {
    // Prevents watchdog timeout
    vTaskDelay(1);
    // Check for Web server events
    WebServer::update();
    // Process microphone samples
    uint16_t samples_read = I2S_MIC::loop(samples, SAMPLES);
    // Analyze audio
    for (uint16_t i = 0; i < SAMPLES; i++) {
      vReal[i] = (float)samples[i];
      vImag[i] = 0;
    }
    FFT.dcRemoval();
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    FFT.compute(FFTDirection::Forward);
    FFT.complexToMagnitude();

    float max = 0;
    for (uint16_t i = 0; i < SAMPLES / 2; i++) {
      if (vReal[i] > max) max = vReal[i];
    }
    for (uint16_t i = 0; i < SAMPLES / 2; i++) {
      vReal[i] /= max;
    }
    for (uint32_t i = 0; i < 9; i++)
      for (uint32_t j = 0; j < 9; j++) {
        if (config.devices.fft.vu[i][j] > vReal[i * 9 + j + 3]) {
          config.devices.fft.vu[i][j] *= 0.85f;
        } else {
          config.devices.fft.vu[i][j] = vReal[i * 9 + j + 3];
        }
      }
  }
}
