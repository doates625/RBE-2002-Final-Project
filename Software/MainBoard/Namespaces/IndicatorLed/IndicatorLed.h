//**************************************************************/
// TITLE
//**************************************************************/

//!t IndicatorLed.h
//!b Namespace for final project robot indicator LED.
//!a RBE-2002 B17 Team 10

#pragma once
#include "Led.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace IndicatorLed {

	// Arduino LED pin
	const uint8_t PIN_LED = 13;
	extern Led led;

	// Function templates
	void setup();
	void flash(uint8_t = 1);
}
