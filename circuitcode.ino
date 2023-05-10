#include <Adafruit_CircuitPlayground.h>
#include <vector>
#include <ctime>
#include <cmath>

void setup() {
  CircuitPlayground.begin();
  pinMode(13, OUTPUT);
  pinMode(4, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLDOWN);
  Serial.begin(9600);
}

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

void playNote(int frequency, int duration, bool hold=false) {
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
}

void loop() {
  std::vector<int> notes = {NOTE_D4,NOTE_D4,NOTE_E4,NOTE_D4,NOTE_G4,NOTE_FS4};
  std::vector<int> durations = {EIGHTH,EIGHTH,QUARTER,QUARTER,QUARTER,HALF};
  std::vector<int> times;
  std::vector<int> intensities;
  if (digitalRead(4)) {
    int dose = 0;
    int currdelay = 10000;
    int currtime;
    bool noiseplaying = false;
    bool enterloop = false;
    bool withdrawals = false;
    int previntensity;
    float intensity;
    int numlights = 10;
    int prevlights = 10;
    int timeelapsed;
    delay(150);
    // play happy sound
    while (true) {
      if (digitalRead(5)) {
        break;
      }
      while (noiseplaying) {
        for (int i=0; i<notes.size(); i++) {
          for (int j = 0; j < 3; j++) {
            playNote(notes[i],durations[i]/3);
            if (digitalRead(4)) {
              enterloop = true;
              noiseplaying = false;
              break;
            }
          }
        }
      }
      if (digitalRead(4) || enterloop) {
        // stop noise
        //noiseplaying = false;
        enterloop = false;
        dose = dose + 1;
        timeelapsed = millis() - currtime;
        if (timeelapsed > 20000) {
          prevlights = prevlights + 1; 
        }
        intensity = .95-(dose*.05*abs(20000-timeelapsed)/40000) - .05*(10-prevlights);
        if (dose < 3) {
          intensity = 1;
        }
        if (dose == 3) { // noise switches to annoying
          withdrawals = true;
          // re-assign noise
        }
        numlights = round(intensity*10);
        for (int i = 0; i < 10; i++) {
          uint8_t color1;
          uint8_t color2;
          uint8_t color3;
          if (i % 2 == 0) {
            color1 = 255;
            color2 = 255;
            color3 = 0;
          }
          else {
            color1 = 255;
            color2 = 16;
            color3 = 240;
          }
          if (i >= numlights) {
            color1 = 0;
            color2 = 0;
            color3 = 0;
          }
          CircuitPlayground.setPixelColor(i, color1, color2, color3);
        }
        delay(4000);
        for (int i = 0; i < numlights; i++) {
          uint8_t color1 = 255;
          uint8_t color2 = 255;
          uint8_t color3 = 255;
          CircuitPlayground.setPixelColor(i, color1, color2, color3);
        } 
        currdelay = currdelay - 1000; // make more complicated
        if (dose < 3) {
          currdelay = 0;
        }
        if (dose == 3) {
          currdelay = 10000;
        }
        currtime = millis(); 
        times.push_back(currtime);
        intensities.push_back(intensity);
        prevlights = numlights;
        delay(currdelay);
        // start noise
        noiseplaying = true;
        if (withdrawals) {
          Serial.println("withdrawal noise");
        }
        if (!withdrawals) {
          Serial.println("happy noise");
        }
        continue;
      }     
      //if (millis() > currtime + currdelay) { // start noise; don't need this if bc delaying inside if
        // play noise
      //  Serial.println("noise starting");
      //  if (withdrawals) {
      //    int x = 1;
          // play annoying noise
      //  }
      //  else {
      //    int x = 1;
          // play happy noise
      //  }
      //  bool startnoise = true;
      //}
    }
  }
    //uint8_t color1 = 0;
    //uint8_t color2 = 0;
    //uint8_t color3 = 0;
    //CircuitPlayground.slideSwitch()
    //CircuitPlayground.setPixelColor(i, color1,   color2,   color3);
}
