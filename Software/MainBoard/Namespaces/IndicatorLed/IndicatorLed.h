//**************************************************************/
// TITLE
//**************************************************************/

//!t IndicatorLed.h
//!b Namespace for final project robot indicator LED.
//!a Dan Oates (RBE-2002 B17 Team 10)

//!d The indicator LED is used to indicate robot state and
//!d errors without a complex external interface.

#pragma once
#include "Led.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace IndicatorLed {
	extern Led led;

	void setup();
	void flash(uint8_t);
}
