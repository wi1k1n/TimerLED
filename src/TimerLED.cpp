#include "TimerLED.h"

TimerLED::TimerLED(const uint8_t& pin) {
  pinMode(this->pin, OUTPUT);
  this->timer.setTimerMode();
}

TimerLED::TimerLED(const uint8_t& pin, const uint8_t& len, const uint16_t* _intervals) : TimerLED(pin) {
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

void TimerLED::tick() {
  this->timer.tick();
  if (this->timer.ready()) {
    this->ind++;
    if (this->ind >= this->len)
      this->ind = 0;
    this->timer.setTime(this->intervals[this->ind]);
    this->timer.restart();
    digitalWrite(this->pin, (this->ind + 1) % 2);
  }
}

void TimerLED::restart() {
  this->ind = 0;
  this->timer.setTime(this->intervals[this->ind]);
  this->timer.restart();
  digitalWrite(this->pin, 0);
}
void TimerLED::stop() {
  this->timer.stop();
  digitalWrite(this->pin, 1);
}
