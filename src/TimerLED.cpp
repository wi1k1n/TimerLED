#include "TimerLED.h"

TimerLED::TimerLED(const uint8_t& pin, const boolean inverted) {
  pinMode(this->pin, OUTPUT);
  this->timer.setTimerMode();
  this->setInverted(inverted);
}

TimerLED::TimerLED(const uint8_t& pin, const uint8_t& len, const uint16_t* _intervals, const boolean inverted)
: TimerLED(pin, inverted) {
  setIntervals(len, _intervals);
}

TimerLED::~TimerLED() {
  delete [] this->intervals;
}

void TimerLED::setIntervals(const uint8_t& len, const uint16_t* _intervals) {
  if (this->len) {
    this->len = 0;
    delete [] this->intervals;
  }
  if (!len) {
    return;
  }
  
  this->len = len;
  this->intervals = new uint16_t[len];
  for (uint8_t i = 0; i < len; ++i) {
    this->intervals[i] = _intervals[i];
  }
}
void TimerLED::setIntervals(const uint16_t& int1, const uint16_t& int2) {
  const uint16_t ints[] = {int1, int2};
  this->setIntervals(2, ints);
}
void TimerLED::setIntervals(const uint16_t& int1, const uint16_t& int2, const uint16_t& int3, const uint16_t& int4) {
  const uint16_t ints[] = {int1, int2, int3, int4};
  this->setIntervals(4, ints);
}

void TimerLED::tick() {
  this->timer.tick();
  if (this->timer.ready()) {
    this->ind++;
    if (this->ind >= this->len)
      this->ind = 0;
    this->timer.setTime(this->intervals[this->ind]);
    this->timer.restart();
    digitalWrite(this->pin, this->ind % 2 ? !this->lowLevel : this->lowLevel);
  }
}

void TimerLED::restart() {
  this->ind = 0;
  this->timer.setTime(this->intervals[this->ind]);
  this->timer.restart();
  digitalWrite(this->pin, this->lowLevel);
}
void TimerLED::stop() {
  this->timer.stop();
  digitalWrite(this->pin, !this->lowLevel);
}

void TimerLED::setInverted(const boolean state) {
  this->lowLevel = state;
}