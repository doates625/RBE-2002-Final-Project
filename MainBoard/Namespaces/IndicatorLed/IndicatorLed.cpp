//**************************************************************/
// TITLE
//**************************************************************/

//!t IndicatorLed.cpp
//!a Dan Oates (RBE-2002 B17 Team 10)

#include "IndicatorLed.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace IndicatorLed {
	const uint8_t PIN_LED = 13;
	Led led(PIN_LED);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes indicator LED.
//!d Call this method in the main setup function.
void IndicatorLed::setup() {
	led.setup();
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
