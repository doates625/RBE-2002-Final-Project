//**************************************************************/
// TITLE
//**************************************************************/

//!t WallFollower.h
//!b Namespace for final project robot wall following algorithm.
//!a RBE-2002 B17 Team 10

//!d This namespace utilizes sonar, odometry, and a cliff sensor
//!d to perform left-sided wall following.

#pragma once
#include "DriveSystem.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace WallFollower {
	void setup();
	void start();
	void stop();
	byte getState();

	bool nearLeftWall();
	bool nearFrontWall();
	bool nearCliff();
	void loop();

	bool inForwardState();

	float targetHeading();
	void setDirectionLeft();
	void setDirectionRight();

	void serialTest();
}
