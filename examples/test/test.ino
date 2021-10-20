/*
  Example "SOS" for TimerLED library
  Github: https://github.com/wi1k1n/TimerLED

  wi1k1n, vfpkjd@gmail.com
  MIT License
*/
#include "TimerLED.h"

TimerLED timer(LED_BUILTIN);

void setup() {
  timer.setIntervals(18, (const uint16_t[]){100, 50, 100, 50, 100, 100, 500, 50, 500, 50, 500, 100, 100, 50, 100, 50, 100, 1000});
  timer.restart();
}

void loop() {
  timer.tick();
}
