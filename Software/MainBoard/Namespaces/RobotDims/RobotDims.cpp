//**************************************************************/
// TITLE
//**************************************************************/

//!t RobotDims.cpp
//!a RBE-2002 B17 Team 10

#include "RobotDims.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace RobotDims {

	// Base Dimensions
	const float wheelDiameter = 0.0701;	// drive wheels (m)
	const float sonarRadiusF = 0.0489;	// from VTC (m)
	const float sonarRadiusB = 0.1284;	// from VTC (m)
	const float sonarRadiusL = 0.1248;	// from VTC (m)
	const float sonarRadiusR = 0.1248;	// from VTC (m)

	// Derived Dimensions
	const float wheelRadius = wheelDiameter * 0.5;
	const float halfWheelRadius = wheelRadius * 0.5;

	// Dimensions Used for Flame Position Calculation
	const float dBTy = 0.038;	// (m)
	const float dBTz = 0.18;	// (m)
	const float dTS = 0.05;		// (m)
}
