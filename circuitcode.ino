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
  std::vector<std::vector<int>> plot;
  if (digitalRead(4)) {
    int dose = 0;
    int delay = 10000;
    int currtime;
    bool noiseplaying = false;
    bool enterloop = false;
    bool withdrawals = false;
    int previntensity;
    int intensity;
    int numlights = 10;
    int prevlights = 10;
    // play happy sound
    while (true) {
      if (digitalRead(5)) {
        break;
      }
      if (noiseplaying) {
        if (digitalRead(4)) {
          enterloop = true;
        }
      if (digitalRead(4) || enterloop) {
        dose = dose + 1;
        intensity = .95-(dose*.05*abs(20000-timeelapsed)*prevlights/40000);
        numlights = ceiling(intensity);
        // stop noise
        if (dose == 1 or dose == 2) {
          intensity = 100;
        }
        if (dose == 3) { // noise switches to annoying
          withdrawals = true;
          // re-assign noise
        }
        for (i = 0; i < numlights; i++) {
          if (i % 2 == 0) {
            uint8_t color1 = 255;
            uint8_t color2 = 255;
            uint8_t color3 = 0;
          }
          else {
            uint8_t color1 = 255;
            uint8_t color2 = 16;
            uint8_t color3 = 240;
          }
          CircuitPlayground.setPixelColor(i, color1, color2, color3);
        }
        delay = delay - 1000; 
        currtime = millis(); 
        std::vector<int> vec(millis(),intensity);
        plot.push_back(vec);
        prevlights = numlights;
        delay(2000);
      }
        for (i = 0; i < numlights; i++) {
          uint8_t color1 = 255;
          uint8_t color2 = 255;
          uint8_t color3 = 255;
          CircuitPlayground.setPixelColor(i, color1, color2, color3);
        }      
      if (millis() > currtime + delay) { // start noise
        // play noise
        if (withdrawals) {
          // play annoying noise
        }
        else {
          // play happy noise
        }
        bool startnoise = true;
      }
    }
  }
    //uint8_t color1 = 0;
    //uint8_t color2 = 0;
    //uint8_t color3 = 0;
    //CircuitPlayground.slideSwitch()
    //CircuitPlayground.setPixelColor(i, color1,   color2,   color3);
}
