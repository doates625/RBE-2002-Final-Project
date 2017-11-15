//**************************************************************/
// TITLE
//**************************************************************/

//!t DriveSystem.h
//!b Namespace for final project robot drive system.
//!a RBE-2002 B17 Team 10

#pragma once
#include "PidController.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace DriveSystem {
	extern PidController headingPid;
	extern float targetHeading;
	extern float driveVoltage;

	void setup();
	void loop();

	bool atTargetHeading();
	void resetPids();
	void stop();
}
