#include <Arduino.h>

#include "TimerLED.h"

TimerLED<LED_BUILTIN> ledStartup;
TimerLED<LED_BUILTIN, TimerLEDMode::REPEAT> ledLoop;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting \"timerled-testing-proj\"");
  
  ledStartup.setIntervals(1000, 500, 1000, 500, 1000, 2000);
	// Serial.println(ESP.getFreeHeap(), DEC);
  ledStartup.restartBlocking();
  ledStartup.setIntervals(20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20);
	// Serial.println(ESP.getFreeHeap(), DEC);
  ledStartup.restartBlocking();
  ledStartup.stop();
  ledStartup.setIntervals(); // release memory
	// Serial.println(ESP.getFreeHeap(), DEC);

  ledLoop.setIntervals(0, 0, 0, 0, 1000, 250);
  ledLoop.restart();
}

void loop() {
  ledLoop.tick();
}