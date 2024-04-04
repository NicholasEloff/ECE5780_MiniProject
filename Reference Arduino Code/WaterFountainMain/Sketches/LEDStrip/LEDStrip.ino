/* 
 * 
 */

#include <FastLED.h>

// ================ Teensy 4.0 Stuff ================
// check the list of FastLED Parallel Output pins to get the correct one to use
#define DATA_PIN 7 
#define NUM_LEDS_PER_STRIP 14
#define NUM_STRIPS 1
#define NUM_LEDS NUM_LEDS_PER_STRIP

CRGB leds[NUM_LEDS_PER_STRIP * NUM_STRIPS];
CRGB clist[10] = { CRGB::Red, CRGB::Lime, CRGB::Magenta, CRGB::Blue, CRGB::OrangeRed, CRGB::ForestGreen, CRGB::Crimson, CRGB::Gold, CRGB::Cyan, CRGB::White };

// get the length of the color list
int length = sizeof(clist) / sizeof(clist[0]);

void setupLED() {
  delay(1000);

  // add the LED strip
  FastLED.addLeds<NUM_STRIPS, WS2811, DATA_PIN, GRB>(leds, NUM_LEDS_PER_STRIP);

  // initially set it to black
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

//==================== Functions ===============================
