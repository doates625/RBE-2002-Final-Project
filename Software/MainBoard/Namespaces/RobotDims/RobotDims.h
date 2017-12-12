//**************************************************************/
// TITLE
//**************************************************************/

//!t RobotDims.h
//!b Namespace for final project robot physical dimensions.
//!a RBE-2002 B17 Team 10

//!d This namespace contains all physical dimensions of the
//!d robot relevant to internal mathematical operations. It also
//!d contains a function to calculate the absolute position of
//!d the flame once it has been found.

#pragma once
#include "LinearAtmel.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace RobotDims {
	extern const float wheelDiameter;
	extern const float sonarRadiusF;
	extern const float sonarRadiusB;
	extern const float sonarRadiusL;
	extern const float sonarRadiusR;

	extern const float wheelRadius;
	extern const float halfWheelRadius;

	extern const float dBTy;
	extern const float dBTz;
	extern const float dTS;
}
