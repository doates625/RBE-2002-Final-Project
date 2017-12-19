//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.h
//!b Namespace for final project Matlab communication system.
//!a Dan Oates (RBE-2002 B17 Team 10)

//!d The robot communicates with Matlab via Bluetooth serial
//!d connection over the Hc06 Bluetooth module. This namespace
//!d checks connection to the Hc06 and keeps track of messages
//!d from Matlab on the laptop control station.

#pragma once
#include "Arduino.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace MatlabComms {
	extern bool disconnected;

	bool setup();
	bool waitForBegin();
	uint8_t loop();
}
