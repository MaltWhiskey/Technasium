#include <Arduino.h>
#include <stdint.h>
/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
class led {
  public:
    union {
      uint8_t colors[3];
      struct {
        uint8_t blue ;
        uint8_t green;
        uint8_t red;
      };
    };
};

// aantal etages met leds
static led etage[4];
// etage pinnen, daarna BGR
static uint8_t pin[4] = {10,17,40,1};
static uint8_t pot = 16;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(pot, INPUT);

  etage[0].blue = 0xff;
  etage[1].green = 0xff;
  etage[2].red = 0xff;
  etage[3].blue = 0xff;
  etage[3].green = 0xff;
  etage[3].red = 0xff;
}

/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
void loop() {
  static uint8_t line = 0;

  // ETAGE SELECTIE
  pinMode(pin[line], OUTPUT);
  digitalWrite(pin[line], 1);

  for(uint8_t x = 0; x < 3 ; x++) {
    if(etage[line].colors[x]) {
      pinMode(pin[(line + 1 + x)&3], OUTPUT);
      digitalWrite(pin[(line + 1 + x)&3], 0);
    }
    else 
      pinMode(pin[(line + 1 + x)&3], INPUT);
  }
  line = (line+1)&3;
  uint16_t wait = analogRead(pot);
  delay(wait/50);
}
