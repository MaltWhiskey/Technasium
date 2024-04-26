#define IN1 4
#define IN2 32
#define IN3 25
#define IN4 27

#include <Arduino.h>

namespace UL2003 {

uint8_t step = 0;
int16_t frequency = 0;
uint8_t inputs[] = {IN1, IN2, IN3, IN4};
uint8_t sequence[8][4] = {{1, 0, 0, 0},
                          {1, 1, 0, 0}, 
                          {0, 1, 0, 0}, 
                          {0, 1, 1, 0}, 
                          {0, 0, 1, 0}, 
                          {0, 0, 1, 1}, 
                          {0, 0, 0, 1}, 
                          {1, 0, 0, 1}};

void IRAM_ATTR step_interrupt() {
  frequency > 0 ? step++ : step--;
  step &= 0x7;
  for(uint8_t i = 0; i < sizeof(inputs); i++) {
    digitalWrite(inputs[i],sequence[step][i]); 
  }
}

void begin(int16_t frequency) {
  UL2003::frequency = frequency;
  for(uint8_t i = 0; i < sizeof(inputs); i++) {
    pinMode(inputs[i], OUTPUT);
    digitalWrite(inputs[i],sequence[step][i]); 
  }
  // 12.5 ns * 80 = 1000 ns
  static hw_timer_t * timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &step_interrupt, true);
  if(frequency == 0) {
    timerAlarmDisable(timer);
  }
  else {
    timerAlarmWrite(timer, 1000000 / abs(frequency), true);
    timerAlarmEnable(timer);
  }
}

} // namespace MOTOR