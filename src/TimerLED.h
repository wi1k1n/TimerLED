/*
  Timer for advanced LED blinking for Arduino (millis()-based)
  Github: https://github.com/wi1k1n/TimerLED

  wi1k1n, vfpkjd@gmail.com
  MIT License

  Versions:
  v1.0 - release
*/
#ifndef TIMERLED_H_
#define TIMERLED_H_

#include <TimerMs.h>

class TimerLED {
public:
  TimerLED(const uint8_t& pin);
  TimerLED(const uint8_t& pin, const uint8_t& len, const uint16_t* intervals);
  ~TimerLED();

  void setIntervals(const uint8_t& len, const uint16_t* intervals);

  void tick();
  void restart();
  void stop();
private:
  TimerMs timer;
  uint16_t* intervals;
  uint8_t len{0};
  uint8_t ind{0};
  uint8_t pin{LED_BUILTIN};
};

#endif /* TIMERLED_H_ */
