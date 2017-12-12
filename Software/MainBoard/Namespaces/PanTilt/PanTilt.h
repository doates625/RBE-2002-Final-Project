//**************************************************************/
// TITLE
//**************************************************************/

//!t PanTilt.h
//!b Namespace for final project pan-tilt servo system.
//!a RBE-2002 B17 Team 10

//!d This namespace controls the pan-tilt system, flame sensor,
//!d and fan for finding and putting out the flame. The pan-tilt
//!d system consists of 2 DS3218 servos. The fan is controlled
//!d by a brushless motor.

#pragma once
#include "Arduino.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace PanTilt {
	extern const float PAN_MIN;
	extern const float PAN_MAX;
	extern const float TILT_MIN;
	extern const float TILT_MAX;

	extern float pan;
	extern float tilt;

	void setup();
	void loop();
	void sweep();

	void setPan(float);
	void setTilt(float);
	void stopTilt();

	bool isAimed();
}
