//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.h
//!b Namespace for final project Matlab communication system.
//!a RBE-2002 B17 Team 10

#pragma once
#include "Arduino.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace MatlabComms {
	extern bool disconnected;

	uint8_t setup();
	uint8_t waitForBegin();
	uint8_t loop();
}
