
#define BAND_SIZE 8
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <AsyncUDP.h>
#include <soc/rtc_cntl_reg.h>
#include <soc/soc.h>

#include "core/Config.h"
#include "core/INMP441.h"
#include "core/WebServer.h"
#include "fft/approx_fft.h"
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
  xTaskCreatePinnedToCore(web_fft_task, "TASK", 50000, NULL, 10, &Task, 0);
  // Restore brownout detector
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, brownout_reg);
  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname(config.network.server.hostname);
  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else
          type = "filesystem";
        vTaskSuspend(Task);
        Serial.println("Start updating " + type);
      })
      .onEnd([]() { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });
  ArduinoOTA.begin();
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
 *----------------------------------------------------------------------------*/
void wav_rec_task(void *parameter) {
  Serial.printf("Audio recorder running on core %d\n", xPortGetCoreID());
  const uint16_t SAMPLES = 512;
  const uint32_t SAMPLING_FREQ = 44100;
  int16_t samples[SAMPLES];
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
  const uint16_t SAMPLES = 512;
  const uint32_t SAMPLING_FREQ = 44100;
  int32_t samples[SAMPLES];
  I2S_MIC::begin(SAMPLING_FREQ);

  int32_t vReal[SAMPLES];
  int32_t vImag[SAMPLES];
  int32_t vEnergy[SAMPLES / 2];

  while (true) {
    // Prevents watchdog timeout
    vTaskDelay(1);
    // Check for Web server events
    WebServer::update();
    // Process microphone samples
    uint16_t samples_read = I2S_MIC::loop(samples, SAMPLES);
    // Analyze audio
    Approx_FFT(samples, vReal, vImag, SAMPLES, SAMPLING_FREQ);
    for (uint16_t i = 0; i < 5; i++) {
      vEnergy[i] = vReal[i] >> 10;
      Serial.printf("%d:%d,", i, vEnergy[i]);
    }
    Serial.println();
  }
}