
#include <Arduino.h>
#include <FastLED.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 * Initialize setup parameters
 *----------------------------------------------------------------------------*/
// Dit is de pin van de microcontroller aangesloten op de eerste led.
// Dit is dus de DIN (data in) van de led.
static const uint8_t LED_PIN = 16;

// Variable die gebruikt worden in plaats van de "hard" gecodeerde waarde
// Dit maakt onderhouden van de code eenvoudiger en het is beter leesbaar
static const uint8_t width = 4;
static const uint8_t height = 4;
static const uint8_t depth = 4;

// 3D array. Dit is een stukje geheugen met 3 indexen, zodat op een makkelijke
// manier de juiste led aangezet kan worden. Net zoals met wiskunde gebruik
// je een assenstelsel, maar behalve x en y is er nog een z voor diepte en
// er kunnen alleen gehele getallen gebruikt worden als index.
// De computer telt vanaf 0, dus 0,1,2,3 zijn mogelijk voor de kubus die wij maken
// x = 0 tot width
// y = 0 tot height
// z = 0 tot depth
static CRGB cube[width][height][depth];

// Dit is een 1D array, de 3D array wordt in deze array in de juiste volgorde
// gezet. Alle leds hebben hun data serieel aangeloten. Deze data gaat via de
// LED_PIN naar de leds.
static CRGB leds[width * height * depth];

// Arduino framework maakt gebruik van een setup functie. Deze wordt tijdens het
// opstarten van de arduino eenmalig uitgevoerd.
// Hier kun je wat start instellingen klaar zetten.
void setup() {

  // Dit start de seriele monitor op 11520 Baud. De seriele monitor kun je gebruiken
  // om je volgen wat het programma aan het doen is. Of om te debbuggen = problemen
  // oplossen in het programma.
  Serial.begin(115200);

  // Stuurt info over de seriele monitor
  Serial.println("Starting");

  // Dit is geavanceerd C++. Het is een template die er voor zorgt dat
  // er voor het TYPE WS2812 een stukje geheugen wordt gereserveerd
  // en tevens dat de manier (protocol) van data doorsturen voor dit type
  // led beschikbaar is.
  //
  // FastLED is een libary ontwikkeld door iemand anders, dit wij gebruiken
  // in platformio.ini geven we aan dat we deze gebruiken.
  //
  // Effectief geef je aan wat voor soort led je gebruikt, op welke pin deze zit
  // waar je data staat dit je wilt doorsturen en om hoeveel data dit gaat.
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, sizeof(leds) / sizeof(CRGB));

  // Als je dit niet doet, kan FastLED mogelijk andere waarden (interpoleren tussen updates)
  // naar de leds sturen, terwijl we zelf controlle willen hebben.
  FastLED.setDither(false);
}

// Dit verplaatst onze data uit de 3D array naar de 1D array
// De functie heeft een parameter ms_delay. Dit vertraagt de update
// met een aantal milli seconden. Zonder deze wachttijd gaan animaties
// mogelijk te snel
//
// Effectief worden alle leds van de 3D array afgelopen en in volgorde
// gezet waarin de leds serieel zijn aangeloten.
//
// De volgorde is startend van de voorste linker led = led 0 (z=0)
// 0 1  2  3 = x
// leds
// 3 4 11 12 (y=3)
// 2 5 10 13 (y=2)
// 1 6 9  14 (y=1)
// 0 7 8  15 (y=0)
// Daarna boven rechts = led 11 naar achter (z=1)
//  0  1  2  3 = x
// leds
// 28 27 20 19 (y=3)
// 29 26 21 18 (y=2)
// 30 25 22 17 (y=1)
// 31 24 23 16 (y=0)
// Van led 31 naar achter 
// Etc.
// Dus op led[26] wordt cube[1][2][1] gezet
void display_update(int ms_delay) {
  
  // Deze for lus loopt de x coordinaten af x = 0,1,2,3
  for (uint8_t x = 0; x < width; x++) {

    // Om 1x op te shuiven moeten er x * 4 leds verder worden geteld
    // de 1D array
    uint8_t xx = x * height;

    // Deze for lus loopt de y coordinaten af y = 0,1,2,3
    for (uint8_t y = 0; y < height; y++) {

      // x = 0 en x = 2 zijn omhoog, x = 1 en x = 3 zijn omlaag
      // Dit is wel afhankelijk van z (z=0, z=2)
      // Binary is heeft een oneven getal een 1 in het laagste bit
      // Bij oneven flip dan de y
      // Er is hier enigszins geavanceerde c gebruikt,
      // de ? is te vervangen door if else
      // conditie ? doe dit bij waar : doe dit bij onwaar
      // & is een binaire and operator
      uint8_t yy = (x & 1) ? height - 1 - y : y;

      // Deze for lus loopt de z coordinaten af z = 0,1,2,3
      for (uint8_t z = 0; z < depth; z++) {

        // Om 1 z op te schuiven moeten er 16 leds verder worden geteld
        // Bij de oneven lagen voor z wordt er acheruit geteld en moet
        // een exta laag opgeschoven worden.
        uint8_t zz = (z + (z & 1)) * height * width;

        // Als de z laag oneven is dan achteruit tellen, anders vooruit
        uint8_t led = (z & 1) ? zz - xx - yy - 1 : xx + yy + zz;

        // Nu is de juiste led bepaald in de 1D array
        leds[led] = cube[x][y][z];
      }
    }
  }

  // Gebruik de 1D array on dit over de LED_PIN naar de 1e led te sturen
  FastLED.show();
  
  // Wacht een bepaalde tijd
  delay(ms_delay);
}

// Maak de 3D cube leeg, memset is een functie die standaard in elke c compiler
// beschikbaar is
void display_clear() { memset(cube, 0, sizeof(cube)); }
/*------------------------------------------------------------------------------
 * Start the main loop
 *----------------------------------------------------------------------------*/
// Arduino framework maakt gebruik van een loop functie. Deze functie wordt
// voordurend aangeroepen. Dus zodra de functie klaar is, doet het arduino
// framework wat "boekhouding" en roept daarna deze functie direct weer aan.
void loop() {

  // deze variablen zijn static, in deze context betekend dit dat er eenmalig
  // een geheugen plek voor wordt gereserveerd en dat ze eenmalig worden
  // geinitializeerd. Dus als loop voor de eerste keer wordt aangeroepen,
  // dan wordt z op 0 en a op 1 gezet. Maar elke volgende keer gebeurd dit niet
  // deze variablen behouden dus hun waarden.
  static int z = 0;

  // a wordt gebruikt voor de richting (naar voren of naar achter)
  static int a = 1;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      cube[x][y][z] = CRGB((x + 1)  * 20, y * 20, z * 0);
    }
  }
 
  // t is een temp variable
  int t = z + a;
 
  // bij groter dan 3 of kleiner dan 0 flip de richting
  if (t > depth - 1 || t < 0) a = -a;
 
  // z wordt 1 goter of 1 kleiner afhankelijk of a = 1 of a = -1
  z += a;
 
  // Teken de Kubus
  display_update(500);
 
  // Maak kubus geheugen schoon voor nieuwe tekening
  display_clear();
}