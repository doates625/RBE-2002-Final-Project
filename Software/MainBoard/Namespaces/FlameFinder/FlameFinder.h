//**************************************************************/
// TITLE
//**************************************************************/

//!t FlameFinder.h
//!b Namespace for final project flame finding system.
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

namespace FlameFinder {
	extern bool foundFlame;
	extern bool extinguishedFlame;
	extern float flamePan;
	extern float flameTilt;
	extern float flameDistance;

	void setup();
	void loop();
	byte getState();
	float getFlameDistance();

	void serialDistanceTest();
	void serialExtinguishTest();
}
