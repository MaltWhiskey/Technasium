#include <Arduino.h>
#include <stdint.h>
#include <math.h>
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
class led {
  public:
    union {
      uint8_t colors[3];
      struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
      };
    };
};

// aantal etages met leds
static led etage[4];
// etage pinnen, daarna RGB
static uint8_t pin[4] = {10,17,40,1};
static uint8_t pot = 16;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(pot, INPUT);

  etage[0] = {0x00, 0x00, 0x00};
  etage[1] = {0x00, 0x00, 0x00};
  etage[2] = {0x00, 0x00, 0x00};
  etage[3] = {0x00, 0x00, 0x00};
}

/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void blink() {
  static unsigned long ms = millis();
  static float phase = 0;
  static uint8_t line = 0;

  phase += 0.001f * (millis() - ms); ms = millis();
  float s1 = sinf(phase * 1.0f);
  float s2 = sinf(phase * 1.1f);
  float s3 = sinf(phase * 1.2f);

  etage[0].green = s1 * s1 * 255;
  etage[1].blue = s2 * s2 * 255;
  etage[2].red = s3 * s3 * 255;
  etage[3].green = etage[0].green;
  etage[3].blue = etage[1].blue;
  etage[3].red = etage[2].red;

  // SCAN LINE SELECTIE, OUTPUT HIGH
  pinMode(pin[line], OUTPUT);
  digitalWrite(pin[line], 1);

  uint8_t colors[3];

  for(uint8_t x = 0; x < 3 ; x++) {
    colors[x] = etage[line].colors[x];
    uint8_t index = (line + 1 + x)&3;
    if(etage[line].colors[x]) {
      pinMode(pin[index], OUTPUT);
      digitalWrite(pin[index], 0);
    }
    else 
      pinMode(pin[index], INPUT);
  }
  for(uint16_t t = 0; t < 0x100; t++) {
    delayMicroseconds(15);
    for(uint8_t x = 0; x < 3 ; x++) {
      uint8_t index = (line + 1 + x)&3;
      if(colors[x]-- == 0)
        pinMode(pin[index], INPUT);
    }
  }
  delayMicroseconds(pow(1.0015f, analogRead(pot)) - 1);
  // DISCONNECT LED AT SCAN LINE FIRST
  pinMode(pin[line], INPUT);
  line = (line+1)&3;
}

void loop() {
  while(1) blink();
}