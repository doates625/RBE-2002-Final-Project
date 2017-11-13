//**************************************************************/
// TITLE
//**************************************************************/

//!t Odometer.h
//!b Namespace for final project robot position tracking.
//!a RBE-2002 B17 Team 10

#pragma once
#include "Arduino.h"
#include "LinearAtmel.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace Odometer {
	extern Vec robotPos;
	extern float h;

	uint8_t setup();
	float getHeading();
	void loop();
}
