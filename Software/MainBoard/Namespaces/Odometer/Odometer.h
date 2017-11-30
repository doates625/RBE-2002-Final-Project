//**************************************************************/
// TITLE
//**************************************************************/

//!t Odometer.h
//!b Namespace for final project robot position tracking.
//!a RBE-2002 B17 Team 10

//!d This namespace uses motor encoders from MotorL and MotorR
//!d and a Bno055 9-DOF IMU to track the robot's heading and
//!d position relative to its starting point.

#pragma once
#include "LinearAtmel.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace Odometer {
	extern Vec robotPos;
	extern float heading;

	bool setup();
	float getHeading();
	float getYawrate();
	void loop();
}
