//**************************************************************/
// TITLE
//**************************************************************/

//!t Odometer.h
//!b Namespace for final project robot position tracking.
//!a RBE-2002 B17 Team 10

#pragma once
#include "Arduino.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace Odometer {
	uint8_t setup();
	float heading();
	void update();
}
