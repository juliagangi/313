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

void loop() {
  digitalWrite(13, LOW);
  std::vector<int> times;
  std::vector<int> intensities;
  if (digitalRead(4)) {
    Serial.println("in initial loop");
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
        if (digitalRead(4)) {
          enterloop = true;
          break;
        }
      }
      if (digitalRead(4) || enterloop) {
        // stop noise
        noiseplaying = false;
        enterloop = false;
        Serial.println("in loop");
        dose = dose + 1;
        timeelapsed = millis() - currtime;
        Serial.println("time elapsed");
        Serial.println(timeelapsed);
        Serial.println("prevlights");
        Serial.println(prevlights);
        Serial.println("dose");
        Serial.println(dose);
        intensity = .95-(dose*.05*abs(20000-timeelapsed)/40000) - .05*(10-prevlights);
        Serial.println("intensity");
        Serial.println(intensity);
        if (dose < 3) {
          intensity = 1;
        }
        if (dose == 3) { // noise switches to annoying
          withdrawals = true;
          // re-assign noise
        }
        numlights = round(intensity*10);
        Serial.println("# lights");
        Serial.println(numlights);
        for (int i = 0; i < numlights; i++) {
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
        Serial.println("end of loop");
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
