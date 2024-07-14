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

// ==================================================================================================================
// ==================================================================================================================
// ==================================================================================================================

enum class TimerLEDMode {
	ONCE,
	REPEAT
};

// TODO: make variation with uint8_t intervals and multiplier (e.g. {1000, 300, 1000, 300, 1000, 1000} can be (100, {10, 3, 10, 3, 10, 10}))
// TODO: make variation with constexpr intervals (check code snippet #1 from chatgpt below)
template<uint8_t PIN, TimerLEDMode MODE = TimerLEDMode::ONCE>
class TimerLED_CPIN_DINT { // Constant Pin, Dynamic Intervals
public:
	// TODO: make constructor variation with variadic arguments (check code snippet #2 from chatgpt below)
	TimerLED_CPIN_DINT() {
		pinMode(PIN, OUTPUT);
		_timer.setMode(MODE == TimerLEDMode::ONCE ? ATimerMode::ONCE : ATimerMode::REPEAT); // TODO: this can be constexpr if ATimer has variation with constexpr mode
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

	void restartBlocking() {
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