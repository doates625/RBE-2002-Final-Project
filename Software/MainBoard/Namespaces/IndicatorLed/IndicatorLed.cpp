//**************************************************************/
// TITLE
//**************************************************************/

//!t IndicatorLed.cpp
//!a RBE-2002 B17 Team 10

#include "IndicatorLed.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace IndicatorLed {
	bool isSetup = false;

	Led led(PIN_LED);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes namespace and LED.
void IndicatorLed::setup() {
	if(!isSetup) {
		isSetup = true;
		led.setup();
	}
}

//!b Flashes LED n times in a loop forever.
void IndicatorLed::flash(uint8_t n) {
	while(1) {
		for(uint8_t i=1; i<=n; i++) {
			led.on(); delay(100);
			led.off(); delay(250);
		}
		delay(1000);
	}
}
