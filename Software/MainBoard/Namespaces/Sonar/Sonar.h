//**************************************************************/
// TITLE
//**************************************************************/

//!t SonarComms.h
//!b Namespace for final project sonar system.
//!a Dan Oates (RBE-2002 B17 Team 10)

//!d This namespace handles the sonar system of the robot,
//!d which consists of 4 Hc-Sr04 sonar sensors mounted at the
//!d front, rear, left, and right of the robot. The loop
//!d method of this function continuously updates the variables
//!d distF, distB, distL, and distR (front, back, left, and
//!d right sonar distances) which reflect the distances from the
//!d VTC of the robot (not the sensors themselves).

#pragma once

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace Sonar {
	extern float distF;
	extern float distB;
	extern float distL;
	extern float distR;

	void setup();
	void loop();
	float pingFront();
}
