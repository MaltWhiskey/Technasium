#define IN1 4
#define IN2 32
#define IN3 25
#define IN4 27

#include <Arduino.h>

namespace MOTOR {

int16_t speed = 0;
uint8_t inputs[] = {IN1, IN2, IN3, IN4};
uint8_t sequence[8][4] = {{1, 0, 0, 0},
                          {1, 1, 0, 0}, 
                          {0, 1, 0, 0}, 
                          {0, 1, 1, 0}, 
                          {0, 0, 1, 0}, 
                          {0, 0, 1, 1}, 
                          {0, 0, 0, 1}, 
                          {1, 0, 0, 1}};
uint8_t step = 0;
unsigned long step_time;

void begin(int16_t speed) {
  MOTOR::speed = speed;
  for(uint8_t i = 0; i < sizeof(inputs); i++) {
    pinMode(inputs[i], OUTPUT);
    digitalWrite(inputs[i],sequence[step][i]); 
  }
  step_time = micros();
}

void loop() {
  if((micros() - step_time) > abs(speed)) {
    if(speed !=0) {
      speed > 0 ? step++ : step--;
      step &= 0x7;
      for(uint8_t i = 0; i < sizeof(inputs); i++) {
        digitalWrite(inputs[i],sequence[step][i]); 
      }
      step_time = micros();
    }
  }
}

} // namespace MOTOR