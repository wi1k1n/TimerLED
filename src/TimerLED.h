/*
	Timer for advanced LED blinking for Arduino (millis()-based)
	Github: https://github.com/wi1k1n/TimerLED

	wi1k1n, vfpkjd@gmail.com
	MIT License

	Versions:
	v1.0 - release
*/

#ifndef TIMERLED_H__
#define TIMERLED_H__

#include <atimer.h>

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
	ATimer timer;
	uint16_t* intervals;
	uint8_t _len = 0;
	uint8_t ind = 0;
	uint8_t pin = LED_BUILTIN;
	bool lowLevel = false;
};

TimerLED::TimerLED(const uint8_t& pin, const boolean inverted) {
	pinMode(pin, OUTPUT);
	timer.setMode(ATimerMode::ONCE);
	setInverted(inverted);
}

TimerLED::TimerLED(const uint8_t& pin, const uint8_t& len, const uint16_t* _intervals, const boolean inverted)
: TimerLED(pin, inverted) {
	setIntervals(len, _intervals);
}

TimerLED::~TimerLED() {
	delete [] intervals;
}

void TimerLED::setIntervals(const uint8_t& len, const uint16_t* _intervals) {
	if (_len) {
		_len = 0;
		delete [] intervals;
	}
	if (!len) {
		return;
	}
	
	_len = len;
	intervals = new uint16_t[len];
	for (uint8_t i = 0; i < len; ++i) {
		intervals[i] = _intervals[i];
	}
}
void TimerLED::setIntervals(const uint16_t& int1, const uint16_t& int2) {
	const uint16_t ints[] = {int1, int2};
	setIntervals(2, ints);
}
void TimerLED::setIntervals(const uint16_t& int1, const uint16_t& int2, const uint16_t& int3, const uint16_t& int4) {
	const uint16_t ints[] = {int1, int2, int3, int4};
	setIntervals(4, ints);
}

void TimerLED::tick() {
	if (timer.tick()) {
		ind++;
		if (ind >= _len)
			ind = 0;
		timer.setTime(intervals[ind]);
		timer.restart();
		digitalWrite(pin, ind % 2 ? !lowLevel : lowLevel);
	}
}

void TimerLED::restart() {
	ind = 0;
	timer.setTime(intervals[ind]);
	timer.restart();
	digitalWrite(pin, lowLevel);
}
void TimerLED::stop() {
	timer.stop();
	digitalWrite(pin, !lowLevel);
}

void TimerLED::setInverted(const boolean state) {
	lowLevel = state;
}


// ==================================================================================================================
// ==================================================================================================================
// ==================================================================================================================

template<int PIN>
class TimerLED_CPIN_DINT { // constant pin, dynamic intervals
public:
	TimerLED_CPIN_DINT() {
		pinMode(PIN, OUTPUT);
		_timer.setMode(ATimerMode::ONCE);
	}

	~TimerLED_CPIN_DINT() {
		if (_intervals)
			delete[] _intervals;
	}
	
	template<typename... Intervals>
	void setIntervals(Intervals... intervals) {
		reallocate(sizeof...(intervals));
		setIntervalsHelper(0, intervals...);
	}

	void tick() {
		if (!_timer.tick())
			return;
			
		_idx++;
		if (_idx >= _len)
			_idx = 0;
		_timer.setTime(_intervals[_idx]);
		_timer.restart();
		digitalWrite(PIN, _idx % 2 ? HIGH : LOW);
	}

	void start() {
		restart();
	}
	void startBlocking() {
		restart();
		while (_idx < _len) {
			const uint8_t curIdx = _idx;
			tick();
			yield();
			if (curIdx != _idx && _idx == 0)
				return;
		}
	}
	void restart() {
		_idx = 0;
		_timer.setTime(_intervals[_idx]);
		_timer.restart();
		digitalWrite(PIN, LOW);
	}
	void stop() {
		_timer.stop();
		digitalWrite(PIN, HIGH);
	}
private:
	template<typename... Intervals>
	void setIntervalsHelper(int idx, uint16_t first, Intervals... rest) {
		_intervals[idx] = first;
		setIntervalsHelper(idx + 1, rest...);
	}
	void setIntervalsHelper(int idx) {}

	void reallocate(uint16_t size) {
		if (_intervals)
			delete[] _intervals;
		_intervals = new uint16_t[size];
		_len = size;
	}
private:
	uint16_t* _intervals = nullptr;
	int _len = 0;
	ATimer _timer;
	uint8_t _idx = 0;
};


#endif // TIMERLED_H__