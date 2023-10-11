#include <Arduino.h>
#include <stdint.h>
#include "sample.h"
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
static const uint8_t DAC_PIN = 18;
static const uint8_t LDR_PIN = 16;
static const uint8_t OLED_PIN = 33;
static const uint8_t WLED_PIN = 35;

const sample_dsc_t sample[] = {  scream_dsc, hiss_dsc, thunder1_dsc};

void play();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(DAC_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(OLED_PIN, OUTPUT);
  pinMode(WLED_PIN, OUTPUT);
}
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  static boolean play = false;
  static uint8_t nr = 0;
  static uint8_t wave;
  static unsigned long startTime = 0;
  // Allow for 1 second LDR calibration
  static unsigned long coolDown = millis() + 1000;

  static double ldr_avg = 0.0;
  const float alpha = 0.3f;
  const double deviation = 40;
  
  if(!play) {
    uint16_t ldr_value = analogRead(LDR_PIN);
    ldr_avg = (ldr_value * alpha) + (ldr_avg * (1 - alpha));
    if(millis() >= coolDown) {
      if(abs(ldr_avg - ldr_value) > deviation) {
        play = true;
        startTime = micros();
      }
    } 
  }
  else {
    unsigned long pos = (micros() - startTime) / (1000000 / sample[nr].rate);
    if(pos < sample[nr].size) {
      wave = sample[nr].data[pos];
      dacWrite(DAC_PIN, wave);
      if (wave < 80 || wave > 255-80) {
        digitalWrite(OLED_PIN, LOW);
        digitalWrite(WLED_PIN, HIGH);
      }
      else {
        digitalWrite(WLED_PIN, LOW);
        digitalWrite(OLED_PIN, HIGH);
      }
    }
    else
    {
      nr=(++nr)%3;
      play = false;
      coolDown = millis() + 1000;
      digitalWrite(WLED_PIN, LOW);
      digitalWrite(OLED_PIN, HIGH);
    } 
  }
}