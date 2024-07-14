#include <Arduino.h>

#include "TimerLED.h"

TimerLED_CPIN_DINT<LED_BUILTIN> led;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting \"timerled-testing-proj\"");
  
  led.setIntervals(1000, 1000, 1000, 1000, 1000, 3000);
  led.startBlocking();


  led.setIntervals(1000, 250);
  led.start();
}

void loop() {
  led.tick();
}