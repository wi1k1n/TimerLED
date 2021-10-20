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
  TimerLED(const uint8_t& pin, const boolean inverted = false);
  TimerLED(const uint8_t& pin, const uint8_t& len, const uint16_t* intervals, const boolean inverted = false);
  ~TimerLED();

  void setIntervals(const uint8_t& len, const uint16_t* intervals); // number of intervals and array of intervals
  void setIntervals(const uint16_t& int1, const uint16_t& int2); // shorter form of temporary array in main function
  void setIntervals(const uint16_t& int1, const uint16_t& int2, const uint16_t& int3, const uint16_t& int4); // same for 4 args

  void tick();
  void restart();
  void stop();

  void setInverted(const boolean state); // for inverted HIGH/LOW consts (like in ESP)
private:
  TimerMs timer;
  uint16_t* intervals;
  uint8_t len{0};
  uint8_t ind{0};
  uint8_t pin{LED_BUILTIN};
  boolean lowLevel{false};
};

#endif /* TIMERLED_H_ */
