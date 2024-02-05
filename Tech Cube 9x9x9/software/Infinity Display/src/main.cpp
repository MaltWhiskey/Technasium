
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
TaskHandle_t FFT_Task, WAV_Task, WEB_Task;
void fft_task(void*);
void wav_task(void*);
void web_task(void*);
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
  // Create task on core 0
  xTaskCreatePinnedToCore(fft_task, "FFT", 30000, NULL, 1, &FFT_Task, 0);
  // Create task on core 0
  xTaskCreatePinnedToCore(web_task, "WEB", 10000, NULL, 8, &WEB_Task, 0);
}
/*------------------------------------------------------------------------------
 * Task Core 1 -> Animation
 *----------------------------------------------------------------------------*/
void loop() {
  Serial.printf("Animations running on core %d\n", xPortGetCoreID());
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
 * Webserver
 *----------------------------------------------------------------------------*/
void web_task(void* parameter) {
  Serial.printf("Webserver running on core %d\n", xPortGetCoreID());
  while (true) {
    // Prevents watchdog timeout
    vTaskDelay(1);
    // Check for Web server events
    WebServer::update();
  }
}
/*------------------------------------------------------------------------------
 * Audio Recorder
 * Import in Audacity as raw signed 32 bits, 3 bytes shift
 *----------------------------------------------------------------------------*/
void wav_task(void* parameter) {
  Serial.printf("Audio Recorder running on core %d\n", xPortGetCoreID());
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
    udp.write((uint8_t*)samples, 1024);
  }
}
/*------------------------------------------------------------------------------
 * Audio Analyzer
 *----------------------------------------------------------------------------*/
void fft_task(void* parameter) {
  Serial.printf("Audio Analyzer running on core %d\n", xPortGetCoreID());
  const uint16_t SAMPLES = 1024;
  const uint32_t SAMPLING_FREQ = 44100;
  const uint16_t BANDS = 9;
  float level[BANDS];
  float vReal[SAMPLES];
  float vImag[SAMPLES];
  int32_t samples[SAMPLES];
  const uint32_t bands[BANDS][2] = { {1, 3},   {4, 5},    {6, 9},
                                    {10, 16}, {17, 29},  {30, 54},
                                    {55, 98}, {99, 180}, {181, 512} };

  I2S_MIC::begin(SAMPLING_FREQ);
  ArduinoFFT<float> FFT =
    ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

  while (true) {
    // Prevents watchdog timeout
    vTaskDelay(1);
    // Process microphone samples
    uint16_t samples_read = I2S_MIC::loop(samples, SAMPLES);
    for (uint16_t i = 0; i < SAMPLES; i++) {
      vReal[i] = (float)samples[i];
      vImag[i] = 0;
    }
    // Analyze audio
    FFT.dcRemoval();
    taskYIELD();
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    taskYIELD();
    FFT.compute(FFTDirection::Forward);
    taskYIELD();
    FFT.complexToMagnitude();
    taskYIELD();
    // Keep max level for each frequency bin
    memset(level, 0, sizeof(level));
    for (uint16_t b = 0; b < BANDS; b++)
      for (uint16_t i = bands[b][0]; i <= bands[b][1]; i++)
        level[b] = max(vReal[i], level[b]);
    // Determine max level of each frequency bin
    float max_level = 1;
    for (uint16_t b = 0; b < BANDS; b++) {
      max_level = max(max_level, level[b]);
    }
    // Make all bins relative to max level
    for (uint16_t b = 0; b < BANDS; b++) {
      level[b] /= max_level;
      level[b] = level[b] * level[b] * level[b];
      if (config.devices.fft.level[b] > level[b]) {
        config.devices.fft.level[b] *= 0.96f;
      }
      else {
        config.devices.fft.level[b] = level[b];
      }
    }
  }
}