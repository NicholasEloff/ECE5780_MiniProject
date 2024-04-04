/**************************************************************************************
 *
 * File:      LEDStrip.ino
 * Author:    Alex Gray & Beverly Yee
 * Date:      August 27, 2023
 * 
 * Anything and everything to do with the LED strip.
 *
 **************************************************************************************/
#include <FastLED.h>

// ================ Teensy 4.0 Stuff ================
// check the list of FastLED Parallel Output pins to get the correct one to use
#define DATA_PIN 20
#define NUM_LEDS_PER_STRIP 14
#define NUM_STRIPS 1
#define NUM_LEDS NUM_LEDS_PER_STRIP
#define STEPS 300

// declare the array of LEDs
CRGB leds[NUM_LEDS_PER_STRIP * NUM_STRIPS];

// array of colors to use on the strip
CRGB clist[10] = { CRGB::Red, CRGB::Lime, CRGB::Magenta, CRGB::Blue, CRGB::OrangeRed,
                   CRGB::ForestGreen, CRGB::Crimson, CRGB::Gold, CRGB::Cyan, CRGB::White };

// length of the color array
int arrLength;
// variables for ease of access on random colors and clearing
CRGB previous, clear, current;

// ================ Delay counters ================
int lineUpDelay;
int cycleColorsDelay, cycleColorsLength, cycleColorsEnd, cycleColorsCount;
int fadeLength, fadeInOutEnd, fadeDelay, fadeCount;
int incrementFillDelay, incrementFillLength, incrementFillEnd, incrementFillCount;
int incrementColorLength, incrementColorEnd, incrementColorCount;
int wipeDelay, wipeEnd, wipeCount, wipeLength;
int chaseDelay, chaseLength, chaseCount, chaseColor, chaseEnd;
int cycleDelay, cycleRainbowEnd, cycleStripEnd;

bool fadeOutYN, showColor, switchLED;

// master items - DO NOT CHANGE
int delayCount, colorNum, LEDNum, counter, iterateCount;


/*
 * Setup the LED strip to be called by WaterFountainMain.setup();
 */
void setupLED() {
  Serial.begin(9600);

  // get the arrLength of the color list
  arrLength = sizeof(clist) / sizeof(clist[0]);
  clear = CRGB::Black;
  previous = clear;

  // add the LED strip
  FastLED.addLeds<NUM_STRIPS, WS2811, DATA_PIN, GRB>(leds, NUM_LEDS_PER_STRIP);

  // initially set it to black
  clearStrip();

  // connect to an unconnected pin to reset random function
  randomSeed(analogRead(16));

  resetCounts();
  setupDelays();
}

/*
 *
 */
void cycleAllPatterns() {
  if (counter < lineUpDelay) {
    lineUpTest();
  } else if (counter == lineUpDelay) {
    // SETUP singleColorWipe -> chaseRandom
    delayCount = 0;
    chaseEnd = lineUpDelay + (chaseLength * chaseColor);
    clearStrip();
    getRandomColor();
  } else if (lineUpDelay < counter && counter < chaseEnd) {
    // switch LEDs are on at a given time
    chaseRandom(chaseDelay, chaseCount);
  } else if (counter == chaseEnd) {
    // SETUP lineUpTest -> fillRainbowCylcle
    cycleStripEnd = chaseEnd + cycleDelay;
    clearStrip();
  } else if (chaseEnd < counter && counter < cycleStripEnd) {
    static float startIndex = 0;
    startIndex = startIndex + 0.5;
    // individual blend into color rainbow
    fillRainbowCycle(startIndex);
  } else if (counter == cycleStripEnd) {
    // SETUP fillRainbowCycle -> cycleAllColors
    cycleColorsEnd = cycleStripEnd + (cycleColorsLength * cycleColorsCount);
    delayCount = 0;
  } else if (cycleStripEnd < counter && counter < cycleColorsEnd) {
    // cycle through all colors
    cycleAllColors(cycleColorsDelay);
  } else if (counter == cycleColorsEnd) {
    // SETUP cycleAllColors -> incrementFilltoBlack
    clearStrip();  // need to reset the brightness after fade items
    incrementFillEnd = cycleColorsEnd + (incrementFillLength * incrementFillCount);
    delayCount = 0;
    getRandomColor();
  } else if (cycleColorsEnd < counter && counter < incrementFillEnd) {
    // increment fill then to black
    incrementFillToBlack(incrementFillDelay);
  } else if (counter == incrementFillEnd) {
    // SETUP cycleAllColors -> fadeInOut
    fadeInOutEnd = incrementFillEnd + (fadeLength * fadeCount);
    delayCount = 0;
  } else if (incrementFillEnd < counter && counter < fadeInOutEnd) {
    // fade in/out random colors
    fadeInOut(fadeDelay);
  } else if (counter == fadeInOutEnd) {
    // SETUP fadeInOut -> incrementColorFill
    incrementColorEnd = fadeInOutEnd + (incrementColorLength * incrementColorCount);
    delayCount = 0;
    clearStrip();
  } else if (fadeInOutEnd < counter && counter < incrementColorEnd) {
    // increment color fill without clearing
    incrementColorFill(incrementFillDelay);
  } else if (counter == incrementColorEnd) {
    // SETUP incrementColorFill -> singleColorWipe
    delayCount = 0;
    LEDNum = 0;
    wipeEnd = incrementColorEnd + (wipeLength * wipeCount);
  } else if (incrementColorEnd < counter && counter < wipeEnd) {
    // only one LED is on at all times
    singleColorWipe(wipeDelay);
  } else if (counter == wipeEnd) {
    // SETUP chaseRandom -> fullCycle
    delayCount = 0;
    cycleRainbowEnd = wipeEnd + cycleDelay;
  } else if (wipeEnd < counter && counter < cycleRainbowEnd) {
    static float startIndex = 0;
    startIndex = startIndex + 0.25;
    // full strip blend into next color, rainbow style
    fillCycle(startIndex);
  }
  // reset everything
  else {
    resetCounts();
    clearStrip();
  }

  // increase the count with each iteration of the main loop
  counter++;
}

// ================ pattern functions ================
/*
 * Assign each LED with a different color from clist.
 * A test to see how the LEDs are addressable and if they all work.
 */
void lineUpTest() {
  int c = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    // since the arrLength of the color list is less than the number of LEDs we have
    if (c >= arrLength) {
      c = 0;
    }

    leds[i] = clist[c];
    c++;
  }

  FastLED.show();
}

/*
 * Cycles through all the colors of the color array
 *
 * params:  wait - how long the counter iterates before switching colors
 *                 Simulates a delay
 */
void cycleAllColors(int wait) {
  if (delayCount == wait) {
    // reset delay count when the delay is met
    delayCount = 0;
    // update the index to the color array
    colorNum++;
  }

  if (colorNum == arrLength) {
    // reset colorNum to the beginning of the array
    colorNum = 0;
  }

  fill_solid(leds, NUM_LEDS, clist[colorNum]);
  FastLED.show();
  delayCount++;
}

/*
 * Fade into color and fade out into black. When fading back into color,
 * change to a different color, chosen at random.
 *
 * params:  wait - the time between adjustments of brightness level
 */
void fadeInOut(int wait) {
  // the arrLength of half of the pattern
  int bsDelay = wait * 255;

  // utilize a boolean to determine whether to fade in or out
  if (delayCount == bsDelay) {
    // start fade out when hitting the peak of pattern
    fadeOutYN = true;
  } else if (delayCount == 0) {
    // flip the boolean to fade back into color
    fadeOutYN = false;
    // swap the color
    getRandomColor();
  }

  if (fadeOutYN) {
    // fading out, so decrement
    delayCount--;
  } else {
    delayCount++;
  }

  fill_solid(leds, NUM_LEDS, current);

  // delayCount is going to be higher than 255, so divide by wait
  FastLED.setBrightness(delayCount / wait);
  FastLED.show();
}

/*
 * Fade into color and fade out into black.
 * Override of the random color version for single color only.
 *
 * params:  wait - the time between adjustments of brightness level
 *          index - the number attached to the color in the array
 */
void fadeInOut(int wait, int index) {
  int bsDelay = wait * 255;
  if (delayCount == bsDelay) {
    fadeOutYN = true;
  } else if (delayCount == 0) {
    fadeOutYN = false;
  }

  if (fadeOutYN) {
    // fading out, so decrement
    delayCount--;
  } else {
    delayCount++;
  }

  fill_solid(leds, NUM_LEDS, clist[index]);
  FastLED.setBrightness(delayCount / wait);
  FastLED.show();
}

/*
 * Fill the entire strip with a singluar color.
 * After "wait" is met, use a different color.
 *
 * params:  wait - the time between the next color
 */
void showSingleFill(int wait) {
  if (delayCount == wait) {
    delayCount = 0;
    getRandomColor();
  }

  fill_solid(leds, NUM_LEDS, current);
  FastLED.show();

  delayCount++;
}

/*
 * Fill the entire strip with a singular color, passed in as an argument.
 *
 * params:  index - the number attached to the color in the array.
 */
void showSolid(int index) {
  fill_solid(leds, NUM_LEDS, clist[index]);
  FastLED.show();
}

/*
 * Incrementally fill the LED strip with a color.
 * The next time the strip fills, it will be a different color.
 * i.e., after the LED is filled with a color, it doesn't clear.
 * 
 * params:    wait - how long to delay the function before continuing
 */
void incrementColorFill(int wait) {
  // the time between lighting the previous LED to lighting the next LED
  int fillDelay = wait * NUM_LEDS;

  if (delayCount == fillDelay) {
    delayCount = 0;
  }

  if (delayCount == 0) {
    // change the color at the start of the new pattern
    // this also allows a random color at startup
    getRandomColor();
  }

  // the delayCount works on the index of the strip
  leds[delayCount / wait] = current;
  FastLED.show();

  delayCount++;
}

/*
 * Incrementally fill the LED strip with a color then increment fill to black.
 * When the pattern executes again, it will be a different color.
 * 
 * params:    index - number attached to a color in clist
 *            wait - how long to delay the function before continuing
 */
void incrementFillToBlack(int wait) {
  // the time between lighting the previous LED to lighting the next LED
  int fillDelay = wait * NUM_LEDS;

  if (delayCount == fillDelay) {
    // flip the bool to start the other part of the pattern
    showColor = !showColor;
    getRandomColor();
    delayCount = 0;
  }

  // show the color
  if (showColor) {
    // the delayCount works on the index of the strip
    leds[delayCount / wait] = current;
    FastLED.show();
  } else {
    // when showColor == false, clear it
    leds[delayCount / wait] = clear;
    FastLED.show();
  }

  delayCount++;
}

/*
 * params:    wait - delay between switching the LEDs 
 *            index - number of the color attached to the array
 */
void chaseIndex(int wait, int index) {
  if (delayCount == wait) {
    clearStrip();
    // change the LEDs that light up
    switchLED = !switchLED;
    delayCount = 0;
  }

  if (switchLED) {
    // turn on the even numbered LEDs
    for (int i = 0; i < NUM_LEDS; i += 2) {
      leds[i] = clist[index];
    }
  } else {
    for (int i = 1; i < NUM_LEDS; i += 2) {
      leds[i] = clist[index];
    }
  }

  FastLED.show();
  delayCount++;
}

/*
 * params:    wait - delay between switching the LEDs 
 *            iterate - how many times to iterate before changing colors
 */
void chaseRandom(int wait, int iterate) {
  int chaseCount = iterate * 2;  // x2 to account for the full strip (half otherwise)
  if (iterateCount == chaseCount) {
    // reset the count and get another color
    iterateCount = 0;
    getRandomColor();
  }

  // delay between odd and even LED indexes
  if (delayCount == wait) {
    // change the LEDs that light up
    switchLED = !switchLED;
    iterateCount++;  // one side is done, iterate
    delayCount = 0;
  }

  if (switchLED) {
    // turn on the even numbered LEDs
    for (int i = 0; i < NUM_LEDS; i += 2) {
      leds[i] = current;
      leds[i + 1] = clear;
    }
  } else {
    for (int i = 1; i < NUM_LEDS; i += 2) {
      leds[i] = current;
      leds[i - 1] = clear;
    }
  }

  FastLED.show();
  delayCount++;
}

/*
 * Incrementally fill the LED strip with a color then increment fill to black.
 * 
 * params:    wait - how long to delay the function before continuing
 *            index - number attached to the color in the array
 */
void incrementFillToBlack(int wait, int index) {
  // the time between lighting the previous LED to lighting the next LED
  int fillDelay = wait * NUM_LEDS;

  if (delayCount == fillDelay) {
    // flip the bool to start the other part of the pattern
    showColor = !showColor;
    delayCount = 0;
  }

  // show the color
  if (showColor) {
    // the delayCount works on the index of the strip
    leds[delayCount / wait] = clist[index];
    FastLED.show();
  } else {
    // when showColor == false, clear it
    leds[delayCount / wait] = clear;
    FastLED.show();
  }

  delayCount++;
}

/*
 * Only one LED is illuminated the entire time and runs across the strip.
 * 
 * params:    index - number attached to a color in clist
 *            wait - how long to delay the function before continuing
 */
void singleColorWipe(int index, int wait) {

  if (delayCount == wait) {
    // delay met, turn off the current LED
    leds[LEDNum] = clear;
    FastLED.show();
    // and prepare to light the next one
    LEDNum++;
    // and reset the delayCount
    delayCount = 0;
  }

  if (LEDNum == NUM_LEDS) {
    LEDNum = 0;  // reset the index as to not go out of bounds
  }

  // make this an else statement?
  leds[LEDNum] = clist[index];
  FastLED.show();

  delayCount++;
}

/*
 * Only one LED is illuminated the entire time and runs across the strip.
 * 
 * params:    wait - how long to delay the function before continuing
 */
void singleColorWipe(int wait) {
  if (delayCount == wait) {
    // delay met, turn off the current LED
    leds[LEDNum] = clear;
    FastLED.show();
    // and prepare to light the next one
    LEDNum++;
    // and reset the delayCount
    delayCount = 0;
  }

  if (LEDNum == NUM_LEDS) {
    LEDNum = 0;  // reset the index as to not go out of bounds
    getRandomColor();
  }

  // make this an else statement?
  leds[LEDNum] = current;
  FastLED.show();

  delayCount++;
}

/*
 * Fills the entire strip with one color and blends it to the next color.
 * Goes through the colors of the rainbow.
 */
void fillCycle(uint8_t colorIndex) {
  fill_solid(leds, NUM_LEDS, ColorFromPalette(RainbowColors_p, colorIndex, 255, LINEARBLEND));
  FastLED.show();
}

/*
 * The entire strip is colored as a rainbow with each color blending into the next.
 * At the same time, the rainbow moves along the strip.
 */
void fillRainbowCycle(uint8_t colorIndex) {
  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds[i] = ColorFromPalette(RainbowColors_p, colorIndex, 255, LINEARBLEND);
    colorIndex += STEPS;
  }
  FastLED.show();
}


// ================ helper functions ================
/*
 * Fill the entire strip with black to clear the strip.
 */
void clearStrip() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.setBrightness(255);
  FastLED.show();
}

/*
 * Helper function to grab a random color then determine whether it is the same as
 * the previously generated color. While it is still the same, keep randomizing 
 * until the current color is different than the previous.
 */
void getRandomColor() {
  current = clist[random(arrLength)];

  // change this into a while loop in case more than 2 in a row are the same numbers?
  while (current == previous) {
    // grab another random color if it ends up being the same as the previous color
    current = clist[random(arrLength)];
  }

  // set the previous color to the current color so the next call will have a different color
  // regardless of where it is called from
  previous = current;
}

/*
 *
 */
void setupDelays() {
  // line up test delay
  lineUpDelay = 2000;

  // cycle all colors calculations
  cycleColorsDelay = 500;
  cycleColorsLength = cycleColorsDelay * arrLength;
  cycleColorsCount = 2;

  // fade in out calculations
  fadeDelay = 5;
  fadeLength = (fadeDelay * 255) * 2;  // 255 = max brightness, x2 to account for in then out times
  fadeCount = 5;                       // iterations of fade

  // increment fill to black calculations
  incrementFillDelay = 50;
  incrementFillLength = (incrementFillDelay * NUM_LEDS) * 2;  // x2 to do color then black
  incrementFillCount = 5;

  // increment fill color to color
  incrementColorLength = (incrementFillDelay * NUM_LEDS);
  incrementColorCount = 8;

  // single LED color wipe
  wipeDelay = 40;
  wipeLength = (wipeDelay * NUM_LEDS);
  wipeCount = 10;

  // chase/switching LEDs
  chaseDelay = 70;  // delay between the odd and even LEDs
  chaseCount = 10;   // how many times to show before switching colors
  chaseLength = chaseDelay * (chaseCount * 2);
  chaseColor = 5;  // number of colors to show

  // rainbow cycle calculations
  cycleDelay = 10000;
}

/*
 * 
 */
void resetCounts() {
  // master counter for cycleAllPatterns
  counter = 0;

  // master delay counter
  delayCount = 0;
  iterateCount = 0;

  // LED strip index
  LEDNum = 0;

  // color array index
  colorNum = 0;

  // used for increment to fill then to black functions
  // must reset to true to show the color
  showColor = true;

  // end of pattern variables
  cycleColorsEnd = 0;
  fadeInOutEnd = 0;
  incrementFillEnd = 0;
  incrementColorEnd = 0;
  wipeEnd = 0;
  chaseEnd = 0;
  cycleRainbowEnd = 0;
  cycleStripEnd = 0;

  // this one may not matter, but good for initialization
  switchLED = true;
}
