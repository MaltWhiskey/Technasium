#define EN 4   //enable
#define MS 32  //microstep
#define SP 25  //step
#define DR 27  //direction

#include <Arduino.h>

namespace DRV8825 {

int16_t frequency = 0;
hw_timer_t * timer = 0;

void IRAM_ATTR step_interrupt() {
  GPIO.out_w1ts = ((uint32_t)1<<SP);
  GPIO.out_w1tc = ((uint32_t)1<<SP);
  
}

void begin() {
  pinMode(EN, OUTPUT);
  pinMode(MS, OUTPUT);
  pinMode(SP, OUTPUT);
  pinMode(DR, OUTPUT);
  digitalWrite(MS, LOW);
  digitalWrite(EN, true);
  digitalWrite(DR, 0);
  // 12.5 ns * 80 = 1000 ns
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &step_interrupt, true);
}

void set(int16_t frequency) {
  DRV8825::frequency = frequency;
  digitalWrite(EN, frequency == 0);
  digitalWrite(DR, frequency > 0);
  if(frequency == 0) {
    timerAlarmDisable(timer);
  }
  else {
    timerAlarmWrite(timer, 1000000 / abs(frequency), true);
    timerAlarmEnable(timer);
  }
}

} // namespace DRV8825