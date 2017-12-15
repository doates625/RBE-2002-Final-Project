//**************************************************************/
// TITLE
//**************************************************************/

//!t WallFollower.h
//!b Namespace for final project robot wall following algorithm.
//!a Dan Oates (RBE-2002 B17 Team 10)

//!d This namespace utilizes information from sonar, odometry,
//!d and a cliff sensor to perform left-sided wall-following via
//!d direct control of the drive system.

#pragma once
#include "DriveSystem.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace WallFollower {
	void setup();
	void start();
	void stop();
	void loop();

	byte getState();
	bool inPausableState();
	float targetHeading();
}
