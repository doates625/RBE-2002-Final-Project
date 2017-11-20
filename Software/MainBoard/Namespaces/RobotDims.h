//**************************************************************/
// TITLE
//**************************************************************/

//!t RobotDims.h
//!b Namespace for final project robot physical dimensions.
//!a RBE-2002 B17 Team 10

//!d This namespace contains all physical dimensions of the
//!d robot relevant to internal mathematical operations.

#pragma once

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace RobotDims {

	// Base Dimensions
	const float wheelDiameter = 0.0701; // drive wheels (m)
	const float sonarRadiusF = 0.1309; // from VTC (m)
	const float sonarRadiusB = 0.1332; // from VTC (m)
	const float sonarRadiusL = 0.1310; // from VTC (m)
	const float sonarRadiusR = 0.1380; // from VTC (m)

	// Derived Dimensions
	const float wheelRadius = wheelDiameter * 0.5;
	const float halfWheelRadius = wheelRadius * 0.5;
}
