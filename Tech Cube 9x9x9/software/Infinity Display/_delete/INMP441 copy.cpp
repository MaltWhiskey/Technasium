/*
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2s.html
https://docs.espressif.com/projects/esp-idf/en/v3.3.5/api-reference/peripherals/i2s.html
https://youtu.be/m-MPBjScNRk?si=TUU2Jdvn_8CnD2Ui
https://invensense.tdk.com/wp-content/uploads/2015/02/INMP441.pdf

INMP441 Microphone Features:
  1. Digital I2S interface with high precision 24-bit data
  2. High signal to noise ratio is 61 dBA
  3. High sensitivity - 26 dBFS
  4. Stable frequency response from 60 Hz to 15 kHz
  5. Low power consumption: low current consumption 1.4 mA
  6. High PSR: -75 dBFS

Interface definition:
  SCK: Serial data clock for I2S interface
  SD: Serial data output for I2S interface.
  WS: Serial data word select for I2S interface
  L/R: channel select:
    - When set low, the microphone outputs it's signal in the LEFT
        channel of the I2S frame.
    - When set high, the microphone outputs it's signals in the RIGHT
        channel of the I2S frame.

Hardware connection to ESP32:
  SD  >> GPIO21 >> GRAY (100k pulldown)
  SCK >> GPIO17 >> BROWN
  WS  >> GPIO16 >> WHITE
  L/R >> GND    >> PURPLE
  GND >> GND    >> BLACK
  VDD >> 3V3    >> RED

NOTE: Although the INMP441 is 24 bits, it needs 64 clock cycles for every
stereo data word. Therefore the I2S interface needs to use 32 bits per sample.
After 24 bits are transmitted, the SD line is tri-stated, it should be pulled
down with 100k to discharge the line.
*/

#include <Arduino.h>
#include <driver/i2s.h>

#define I2S_SD 21
#define I2S_SCK 17
#define I2S_WS 16
#define I2S_PORT I2S_NUM_0

namespace I2S_MIC {

void begin(uint32_t frequency) {
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = frequency,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 1024,
      .use_apll = true,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0};

  const i2s_pin_config_t pin_config = {.bck_io_num = I2S_SCK,
                                       .ws_io_num = I2S_WS,
                                       .data_out_num = I2S_PIN_NO_CHANGE,
                                       .data_in_num = I2S_SD};
  if (esp_err_t err =
          i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL) != ESP_OK)
    Serial.printf("Failed installing driver: %d\n", err);
  else if (err = i2s_set_pin(I2S_PORT, &pin_config) != ESP_OK)
    Serial.printf("Failed setting pin: %d\n", err);
  else if (err = i2s_start(I2S_NUM_0) != ESP_OK)
    Serial.printf("Failed starting I2S: %d\n", err);
  else
    Serial.println("I2S driver installed");
}

uint16_t loop(int16_t *sample_data, const uint16_t samples) {
  // Memory for audio samples (24 bits per sample stored in signed 32 bit)
  int32_t buffer[100];
  uint16_t bytes_remaining = samples << 1;
  uint16_t samples_read = 0;
  while (bytes_remaining > 0) {
    size_t bytes_read = 0;
    uint16_t bytes_to_read = min(bytes_remaining, (uint16_t)sizeof(buffer));
    if (esp_err_t err = i2s_read(I2S_PORT, buffer, bytes_to_read, &bytes_read,
                                 100) != ESP_OK) {
      Serial.printf("ERR %d\n", err);
      return 0;
    }
    bytes_remaining -= bytes_read;
    for (uint16_t i = 0; i < bytes_read >> 1; i++) {
      sample_data[samples_read++] = buffer[i] >> 16;
    }
  }
  return samples_read;
}
}  // namespace I2S_MIC
