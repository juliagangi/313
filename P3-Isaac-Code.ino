#include <Adafruit_CircuitPlayground.h>

#define RECEPTOR_HUE       280  // Primary hue of the flame.  This is a value in
                                // degrees from 0.0 to 360.0, see HSV color space
                                // for different hue values:
                                //   https://en.wikipedia.org/wiki/HSL_and_HSV#/media/File:Hsl-hsv_models.svg
                                // The value 35 degrees is a nice orange in the
                                // middle of red and yellow hues that will look like
                                // a flame flickering as the hues animate.
                                // For the flicker effect the pixels will cycle 
                                // around hues that are +/-10 degrees of this value.
                                // For the solid effect the pixels will be set
                                // to this hue (at full saturation and value).
                                // Rainbow effect ignores this hue config and
                                // cycles through all of them.

// A few music note frequencies as defined in this tone example:
//   https://www.arduino.cc/en/Tutorial/toneMelody
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

// Define note durations.  You only need to adjust the whole note
// time and other notes will be subdivided from it directly.
#define WHOLE         2200       // Length of time in milliseconds of a whole note (i.e. a full bar).
#define HALF          WHOLE/2
#define QUARTER       HALF/2
#define EIGHTH        QUARTER/2
#define EIGHTH_TRIPLE QUARTER/3
#define SIXTEENTH     EIGHTH/2

// Global program state.
bool dose_given = false;
uint32_t last_inactivity = millis();
uint32_t last_dose = millis();
uint32_t last_change = millis();
int active_receptors = 10; 
float frequencies[10] = {0};     // Random frequencies and phases will be generated for each pixel to
float phases[10] = {0};          // define an organic but random looking flicker effect.
enum AnimationType { SOLID, FLICKER, RAINBOW };  // Which type of animation is currently running.
AnimationType animation = SOLID;

// Color gamma correction table, this makes the hues of the NeoPixels
// a little more accurate by accounting for our eye's non-linear color
// sensitivity.  See this great guide for more details:
//   https://learn.adafruit.com/led-tricks-gamma-correction/the-issue
const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

// HSV to RGB color space conversion function taken directly from:
//  https://www.cs.rit.edu/~ncs/color/t_convert.html
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
  int i;
  float f, p, q, t;
  if( s == 0 ) {
    // achromatic (grey)
    *r = *g = *b = v;
    return;
  }
  h /= 60;      // sector 0 to 5
  i = floor( h );
  f = h - i;      // factorial part of h
  p = v * ( 1 - s );
  q = v * ( 1 - s * f );
  t = v * ( 1 - s * ( 1 - f ) );
  switch( i ) {
    case 0:
      *r = v;
      *g = t;
      *b = p;
      break;
    case 1:
      *r = q;
      *g = v;
      *b = p;
      break;
    case 2:
      *r = p;
      *g = v;
      *b = t;
      break;
    case 3:
      *r = p;
      *g = q;
      *b = v;
      break;
    case 4:
      *r = t;
      *g = p;
      *b = v;
      break;
    default:    // case 5:
      *r = v;
      *g = p;
      *b = q;
      break;
  }
}

// Helper to change the color of a NeoPixel on the Circuit Playground board.
// Will automatically convert from HSV color space to RGB and apply gamma
// correction.
void setPixelHSV(int i, float h, float s, float v) {
  // Convert HSV to RGB
  float r, g, b = 0.0;
  HSVtoRGB(&r, &g, &b, h, s, v);
  // Lookup gamma correct RGB colors (also convert from 0...1.0 RGB range to 0...255 byte range).
  uint8_t r1 = pgm_read_byte(&gamma8[int(r*255.0)]);
  uint8_t g1 = pgm_read_byte(&gamma8[int(g*255.0)]);
  uint8_t b1 = pgm_read_byte(&gamma8[int(b*255.0)]);
  // Set the color of the pixel.
  CircuitPlayground.strip.setPixelColor(i, r1, g1, b1);
}

// Simple solid lit candle effect.
// No animation, each pixel is lit with the specified receptor hue, until they're all gone.
void showLitSolid(float color) {
  for (int i=0; i<10; ++i) {
    if (i < active_receptors) {
      // This pixel should be lit.
      setPixelHSV(i, color, 1.0, 1.0);
    }
    else {
      // This pixel has been blown out, just turn it off.
      setPixelHSV(i, 0, 0, 0);
    }
  }
  CircuitPlayground.strip.show();
}

// Play a note of the specified frequency and for the specified duration.
// Hold is an optional bool that specifies if this note should be held a
// little longer, i.e. for eigth notes that are tied together.
// While waiting for a note to play the waitBreath delay function is used
// so breath detection and pixel animation continues to run.  No tones
// will play if the slide switch is in the -/off position or all the
// candles have been blown out.
void playNote(int frequency, int duration, bool hold=false, bool measure=true) {
  // Check if the slide switch is off or the candles have been blown out
  // and stop immediately without playing anything.
  if (!CircuitPlayground.slideSwitch()) {
    return;
  }
  if (hold) {
    // For a note that's held play it a little longer so it blends into the next tone 
    CircuitPlayground.playTone(frequency, duration + duration/32, false);
  }
  else {
    // For a note that isn't held just play it for the specified duration.
    CircuitPlayground.playTone(frequency, duration, false);
  }
  // Use waitBreath instead of Arduino's delay because breath detection and
  // pixel animation needs to occur while music plays.  However skip this logic
  // if not asked for (measure = false, only needed when playing celebration
  // song so as to not continue waiting for breaths).
  if (measure) {
    delay(duration + duration/16);
  }
  else {
    delay(duration + duration/16);
  }
}

void birthdaySong() {
  // Play happy birthday tune, from:
  //  http://www.irish-folk-songs.com/happy-birthday-tin-whistle-sheet-music.html#.WXFJMtPytBw
  // Inside each playNote call it will play a note and drive the NeoPixel animation
  // and check for a breath against the sound sensor.  Once all the candles are blown out
  // the playNote calls will stop playing music.
  playNote(NOTE_D4, EIGHTH, true);
  playNote(NOTE_D4, EIGHTH);
  playNote(NOTE_E4, QUARTER);       // Bar 1
  playNote(NOTE_D4, QUARTER);
  playNote(NOTE_G4, QUARTER);
  playNote(NOTE_FS4, HALF);         // Bar 2
  // playNote(NOTE_D4, EIGHTH, true);
  // playNote(NOTE_D4, EIGHTH);
  // playNote(NOTE_E4, QUARTER);       // Bar 3
  // playNote(NOTE_D4, QUARTER);
  // playNote(NOTE_A4, QUARTER);
  // playNote(NOTE_G4, HALF);          // Bar 4
  // playNote(NOTE_D4, EIGHTH, true);
  // playNote(NOTE_D4, EIGHTH);
  // playNote(NOTE_D5, QUARTER);       // Bar 5
  // playNote(NOTE_B4, QUARTER);
  // playNote(NOTE_G4, QUARTER);
  // playNote(NOTE_FS4, QUARTER);      // Bar 6
  // playNote(NOTE_E4, QUARTER);
  // playNote(NOTE_C5, EIGHTH, true);
  // playNote(NOTE_C5, EIGHTH);
  // playNote(NOTE_B4, QUARTER);       // Bar 7
  // playNote(NOTE_G4, QUARTER);
  // playNote(NOTE_A4, QUARTER);
  // playNote(NOTE_G4, HALF);          // Bar 8
}

// Song to play when the candles are blown out.
void celebrateSong() {
  // Play a little charge melody, from:
  //  https://en.wikipedia.org/wiki/Charge_(fanfare)
  // Note the explicit boolean parameters in particular the measure=false
  // at the end.  This means the notes will play without any breath measurement
  // logic.  Without this false value playNote will try to keep waiting for candles
  // to blow out during the celebration song!
  playNote(NOTE_G4, EIGHTH_TRIPLE, true, false);
  playNote(NOTE_C5, EIGHTH_TRIPLE, true, false);
  playNote(NOTE_E5, EIGHTH_TRIPLE, false, false);
  playNote(NOTE_G5, EIGHTH, true, false);
  playNote(NOTE_E5, SIXTEENTH, false);
  playNote(NOTE_G5, HALF, false);
  
}

void setup() {
  // Initialize serial output and Circuit Playground library.
  Serial.begin(115200);
  CircuitPlayground.begin();
  // Check if a button is being pressed at startup and change the
  // animation mode accordingly.
  if (CircuitPlayground.leftButton()) {
    // Rainbow animation on left button press at startup.
    animation = SOLID;
  }
  else if (CircuitPlayground.rightButton()) {
    // Solid color animation on right button press at startup.
    animation = SOLID;
    // Since the solid animation doesn't update every frame, bootstrap it by
    // turning all the pixels on at the start.  As candles are blown out the
    // pixels will be updated (see the waitBreath function's loop).
    showLitSolid(RECEPTOR_HUE);
  }
  uint32_t last_inactivity = millis(); //
  uint32_t last_dose = millis();
}

bool activeDelay(uint32_t milliseconds) {
  bool dose_given = CircuitPlayground.leftButton();
  uint32_t last_dose = millis();
  uint32_t start = millis();
  uint32_t current = start;
  if (dose_given) {
    while ((current - start) < milliseconds) {
      // can do something like animatePixels <- ****
      current = millis();
    }
  }
  return dose_given;
}

void loop() {
  uint32_t second = 1000;
  bool dose_given = activeDelay(second);
  // Serial.println(dose_given);
  // Serial.println(last_dose);
  if (dose_given) {
    Serial.println(last_dose);
    // Change color of receptors
    showLitSolid(RECEPTOR_HUE - 245);
    // Play sound
    celebrateSong();

    // Eventually decrement lit
    active_receptors = max(active_receptors - 1, 1);

    dose_given = false;
  } else {
    showLitSolid(RECEPTOR_HUE);
    // if there's no dose, create withdrawal symptom
    if (millis() - last_dose > 2000*active_receptors) {
      birthdaySong();
      // Serial.println("withdrawal!");
    }
    // if no doses for a while, add a receptor
    if (millis() - max(last_dose, last_change) > 8000) {
      last_change = millis();
      active_receptors = min(active_receptors + 1, 10);
    }
  }
}