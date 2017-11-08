//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.h
//!b Namespace for final project Matlab communication system.
//!a RBE-2002 B17 Team 10

#pragma once
#include "Hc06.h"
#include "BinarySerial.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace MatlabComms {
	extern bool disconnected;
	extern float driveVoltageL;
	extern float driveVoltageR;

	uint8_t setup();
	void loop();
}
