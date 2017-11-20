//**************************************************************/
// TITLE
//**************************************************************/

//!t WallFollower.h
//!b Namespace for final project robot wall following algorithm.
//!a RBE-2002 B17 Team 10

//!d This namespace utilizes sonar and odometry to perform
//!d left-sided wall following.

#pragma once
#include "DriveSystem.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace WallFollower {
	void setup();
	void begin();
	byte getState();

	void loop();
	bool nearLeftWall();
	bool nearFrontWall();

	float targetHeading();
	void setDirectionLeft();
	void setDirectionRight();
}
