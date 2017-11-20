//**************************************************************/
// TITLE
//**************************************************************/

//!t DriveSystem.h
//!b Namespace for final project robot drive system.
//!a RBE-2002 B17 Team 10

//!d This namespace controls the robot drive system via the
//!d MotorL and MotorR namespaces. Its behavior is controlled
//!d by assigning the variables:
//!d - targetHeading: direction to point to (rad)
//!d - driveVoltage: applied to both wheels (- for reverse)
//!d Its loop method contains iterates PID control for setting
//!d the target heading.

#pragma once
#include "PidController.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace DriveSystem {
	void setup();

	bool driveAtHeading(float, float);
	void driveAtYawrate(float, float);

	void resetPids();
	void stop();
}
