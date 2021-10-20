![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![author](https://img.shields.io/badge/author-wi1k1n-informational.svg)
# TimerLED
Timer for advanced LED blinking for Arduino (millis()-based)

### Compatibility
Compatible with all Arduino platforms

<a id="install"></a>
## Installation
[https://www.arduino.cc/en/guide/libraries]

<a id="example"></a>
## Example
```cpp
#include "TimerLED.h"

TimerLED t(LED_BUILTIN);

void setup() {
  Serial.begin(9600);
  t.setIntervals(18, (const uint16_t[]){100, 50, 100, 50, 100, 100, 500, 50, 500, 50, 500, 100, 100, 50, 100, 50, 100, 1000});
  t.restart();
}

void loop() {
  t.tick();
}
```

<a id="versions"></a>
## Versions
- v1.0