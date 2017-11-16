//**************************************************************/
// TITLE
//**************************************************************/

//!t IndicatorLed.h
//!b Namespace for final project robot indicator LED.
//!a RBE-2002 B17 Team 10

//!d The indicator LED is used to indicate robot state and
//!d errors without a complex external interface.

#pragma once
#include "Led.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace IndicatorLed {

	// Arduino LED
	const uint8_t PIN_LED = 13;
	extern Led led;

	void setup();
	void flash(uint8_t = 1);
}
