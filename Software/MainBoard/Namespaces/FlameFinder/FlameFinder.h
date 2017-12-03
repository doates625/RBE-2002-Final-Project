//**************************************************************/
// TITLE
//**************************************************************/

//!t FlameFinder.h
//!b Namespace for final project flame finding system.
//!a RBE-2002 B17 Team 10

//!d This namespace controls the pan-tilt system, flame sensor,
//!d and fan for finding and putting out the flame. The pan-tilt
//!d system consists of 2 DS3218 servos.

#pragma once

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace FlameFinder {
	void setup();
	void loop();
	int getBrightness();
	bool aimedAtTarget();

	extern float pan;
	extern float tilt;
	extern bool foundFlame;
	extern bool extinguishedFlame;
	extern float flamePan;
	extern float flameTilt;
}
