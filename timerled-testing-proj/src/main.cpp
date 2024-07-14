#include <Arduino.h>

#include "TimerLED.h"

TimerLED<LED_BUILTIN> led;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting \"timerled-testing-proj\"");
  
  led.setIntervals(1000, 1000, 1000, 1000, 1000, 3000);
  led.restartBlocking();


  led.setIntervals(1000, 250);
  led.restart();
}

void loop() {
  led.tick();
}