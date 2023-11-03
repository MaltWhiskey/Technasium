#include <Arduino.h>
#include <stdint.h>

#include "sample.h"
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
// RIGHT -> 39,37,35,33,18,16,GND,VBUS
// LEFT  -> EN,3,5,7,9,11,12,3V3
static const uint8_t LDR_PIN = 3;
static const uint8_t DAC_PIN = 18;
static const uint8_t PIR_PIN = 33;
static const uint8_t OLED_PIN = 37;
static const uint8_t WLED_PIN = 39;

const sample_dsc_t sample[] = {hiss_dsc, howl_dsc, magniet_dsc, scream_dsc,
                               thunder_dsc};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(DAC_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(OLED_PIN, OUTPUT);
  pinMode(WLED_PIN, OUTPUT);
  digitalWrite(WLED_PIN, LOW);
  digitalWrite(OLED_PIN, HIGH);
}
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  static boolean play = false;
  static uint8_t nr = 0;
  static uint8_t wave;
  static unsigned long startTime = 0;
  static unsigned long coolDown = 0;
  static float fast_avg = 0.0;
  static float slow_avg = 0.0;
  const float fast_alpha = 0.050f;
  const float slow_alpha = 0.005f;
  const uint16_t threshold = 200;

  static long idx = 0;

  if (!play) {
    uint16_t ldr_value = analogRead(LDR_PIN);
    uint16_t pir_value = digitalRead(PIR_PIN);
    fast_avg = (ldr_value * fast_alpha) + (fast_avg * (1 - fast_alpha));
    slow_avg = (ldr_value * slow_alpha) + (slow_avg * (1 - slow_alpha));
    uint16_t deviation = abs(slow_avg - fast_avg);

    if (millis() >= coolDown && (deviation > threshold)) {
      play = true;
      startTime = micros();
    }
    Serial.printf("pir #%d, val=%d\n",idx++,pir_value);

  } else {
    digitalWrite(OLED_PIN, LOW);
    unsigned long pos = (micros() - startTime) / (1000000 / sample[nr].rate);
    if (pos < sample[nr].size) {
      wave = sample[nr].data[pos];
      dacWrite(DAC_PIN, wave);
      if (wave < 90 || wave > 255 - 90) {
        digitalWrite(WLED_PIN, HIGH);
      } else {
        digitalWrite(WLED_PIN, LOW);
      }
    } else {
      play = false;
      nr = (++nr) % 5;  // sizeof(sample);
      digitalWrite(WLED_PIN, LOW);
      digitalWrite(OLED_PIN, HIGH);
      coolDown = millis() + 1000;
    }
  }
}