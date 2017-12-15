//**************************************************************/
// TITLE
//**************************************************************/

//!t DriveSystem.h
//!b Namespace for final project robot drive system.
//!a Dan Oates (RBE-2002 B17 Team 10)

//!d This namespace controls the robot drive system via the
//!d MotorL and MotorR namespaces. It initializes the drive
//!d motors and PID controls robot heading and drive velocity.

#pragma once
#include "PidController.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace DriveSystem {
	void setup();
	bool drive(float, float = 0);
	void stop();
}
