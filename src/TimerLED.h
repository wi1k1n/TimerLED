/*
	Timer for advanced LED blinking for Arduino (millis()-based)
	Github: https://github.com/wi1k1n/TimerLED

	wi1k1n, vfpkjd@gmail.com
	MIT License

	Versions:
	v2.0 - refactored + using ATimer
*/

#ifndef TIMERLED_H__
#define TIMERLED_H__ 20240714

#include <Arduino.h>

#include <atimer.h>

#ifdef TIMERLED_DEBUG
// #include <debug.h>
void printMillisAsTime(const unsigned long time) {
  const unsigned long currentMillis = time;
  const unsigned long hours = currentMillis / 3600000;
  unsigned long remainingMillis = currentMillis % 3600000;
  const unsigned long minutes = remainingMillis / 60000;
  remainingMillis %= 60000;
  const unsigned long seconds = remainingMillis / 1000;
  const unsigned long milliseconds = remainingMillis % 1000;

  // Print the time in HH:mm:ss.ms format
  Serial.print(hours);
  Serial.print(":");
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.print(seconds);
  Serial.print(".");
  if (milliseconds < 100) Serial.print("0");
  if (milliseconds < 10) Serial.print("0");
  Serial.print(milliseconds);
}
template<typename T>
void DLOGLN_helper(T v) {
	Serial.print(v);
}
template <typename T, typename... Ts>
void DLOGLN_helper(T v, Ts... ts) {
	Serial.print(v);
	DLOGLN_helper(ts...);
	Serial.println();
}
void DLOGLN_helper() {
	Serial.println();
}
template <typename... Ts>
void DLOGLN(Ts... ts) {
	Serial.print("[");
	printMillisAsTime(millis());
	Serial.print("]> ");
	DLOGLN_helper(ts...);
}
void DLOGLN() {
	Serial.print("[");
	printMillisAsTime(millis());
	Serial.println("]");
}
#else
#define DLOGLN(...)
#endif

#define LEDON LOW
#define LEDOFF HIGH

enum class TimerLEDMode {
	ONCE,
	REPEAT
};

// TODO: make variation with uint8_t intervals and multiplier (e.g. {1000, 300, 1000, 300, 1000, 1000} can be (100, {10, 3, 10, 3, 10, 10}))
// TODO: make variation with constexpr intervals (check code snippet #1 from chatgpt below)
template<uint8_t PIN, TimerLEDMode MODE = TimerLEDMode::ONCE, bool FINISH_STATE = LEDOFF>
class TimerLED_CPIN_DINT { // Constant Pin, Dynamic Intervals
public:
	// TODO: make constructor variation with variadic arguments (check code snippet #2 from chatgpt below)
	TimerLED_CPIN_DINT() {
		DLOGLN("TimerLED_CPIN_DINT<", PIN, ">::TimerLED_CPIN_DINT()");
		pinMode(PIN, OUTPUT);
		_timer.setMode(MODE == TimerLEDMode::ONCE ? ATimerMode::ONCE : ATimerMode::REPEAT); // TODO: this can be constexpr if ATimer has variation with constexpr mode
	}
	~TimerLED_CPIN_DINT() {
		DLOGLN("TimerLED_CPIN_DINT<", PIN, ">::~TimerLED_CPIN_DINT()");
		if (_intervals)
			delete[] _intervals;
	}
	
	template<typename... Intervals>
	void setIntervals(Intervals... intervals) {
		DLOGLN("TimerLED_CPIN_DINT<", PIN, ">::setIntervals(", sizeof...(intervals), "...)");
		reallocate(sizeof...(intervals));
		setIntervalsHelper(0, intervals...);
	}

	void tick() {
		if (!_timer.tick())
			return;
		
		++_idx;
		DLOGLN("TimerLED_CPIN_DINT<", PIN, ">::tick() _idx=", _idx, " _intervals[_idx]=", _intervals[_idx]);
		_idx = getNextNonEmptyIntervalIdx(_idx);
		DLOGLN("\tgetNextNonEmptyIntervalIdx() -> ", _idx);
		if (_idx >= _len) {
			_idx = 0;
			if (MODE == TimerLEDMode::ONCE)
				return stop();
		}
		_timer.setTime(_intervals[_idx]);
		_timer.restart();
		digitalWrite(PIN, _idx % 2 ? LEDOFF : LEDON);
	}

	void restartBlocking() {
		DLOGLN("TimerLED_CPIN_DINT<", PIN, ">::restartBlocking()");
		restart();
		while (_idx < _len) {
			const uint8_t curIdx = _idx;
			tick();
			yield();
			if (curIdx != _idx && _idx == 0)
				return stop();
		}
	}
	void restart() {
		DLOGLN("TimerLED_CPIN_DINT<", PIN, ">::restart()");
		_idx = getNextNonEmptyIntervalIdx(0);
		DLOGLN("\tgetNextNonEmptyIntervalIdx() -> ", _idx);
		if (_idx >= _len)
			return stop(); // this means all intervals are empty
		_timer.setTime(_intervals[_idx]);
		_timer.restart();
		digitalWrite(PIN, LEDON);
	}
	void stop() {
		DLOGLN("TimerLED_CPIN_DINT<", PIN, ">::stop()");
		_timer.stop();
		digitalWrite(PIN, FINISH_STATE);
	}

	bool isRunning() const { return _timer.isRunning(); }
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

	uint8_t getNextNonEmptyIntervalIdx(uint8_t idx) {
		while (idx < _len && _intervals[idx] == 0)
			idx++;
		return idx;
	}
private:
	uint16_t* _intervals = nullptr;
	int _len = 0;
	ATimer _timer;
	uint8_t _idx = 0;
};

template<uint8_t PIN, TimerLEDMode MODE = TimerLEDMode::ONCE>
using TimerLED = TimerLED_CPIN_DINT<PIN, MODE>;

// Code snippet #1 from ChatGPT:
/*
Query:
====================
I need a templated class Sequencer that has a single function "display()". Sequencer uses parameter pack for storing variable amount of int values. Each value represents the number of characters that needs to be displayed. When "display()" is executed, there's should be an output which alternates the case of the symbols. It's easier to show the usage of this class on example:

The following code:
Sequencer<3, 4, 2, 5, 1> seq;
seq.display();

should produce the following output:
'''
ooo OOOO oo OOOOO o
'''
====================

Response:
====================
#include <iostream>
#include <cctype>

template<int... N>
class Sequencer {
public:
    void display() const {
        displayImpl<0, N...>(false);
    }

private:
    // Helper function to alternate the case of characters
    void printAlternating(int count, char startChar) const {
        for (int i = 0; i < count; ++i) {
            std::cout << startChar;
        }
    }

    // Base case for the recursive display implementation
    template<int Index>
    void displayImpl(bool toggle) const {
        std::cout << std::endl;
    }

    // Recursive case for the display implementation
    template<int Index, int First, int... Rest>
    void displayImpl(bool toggle) const {
        char ch = toggle ? 'O' : 'o';
        printAlternating(First, ch);
        
        if constexpr (sizeof...(Rest) > 0) {
            std::cout << " ";
            displayImpl<Index + 1, Rest...>(!toggle);
        } else {
            std::cout << std::endl;
        }
    }
};

int main() {
    Sequencer<5, 10, 1, 1, 1, 2, 3, 5> seq;
    seq.display();
    return 0;
}
====================
*/


// Code snippet #2 from ChatGPT:
/*
Query:
====================
I need a class MyClass which takes variadic arguments of type int in the constructor and fills them in the private std::vector field.
====================

Response:
====================
#include <iostream>
#include <vector>
#include <initializer_list>

class MyClass {
private:
    std::vector<int> values;

public:
    // Variadic template constructor
    template<typename... Args>
    MyClass(Args... args) : values{args...} {
        // The constructor body is empty as the initialization list already does the work
    }

    // Function to print the contents of the vector for demonstration purposes
    void printValues() const {
        for (int value : values) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
};

int main() {
    MyClass(1, 2, 3, 4, 5).printValues();
    MyClass(3, 2, 1).printValues();
    MyClass().printValues();
    MyClass(5).printValues();

    return 0;
}
====================
*/
#endif // TIMERLED_H__