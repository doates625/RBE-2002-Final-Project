//**************************************************************/
// TITLE
//**************************************************************/

//!t RobotDims.cpp
//!a RBE-2002 B17 Team 10

#include "RobotDims.h"
#include "Odometer.h"
#include "FlameFinder.h"

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

	// Vector from robot VTC base to pan servo
	const Vec rBP(3, new float[3]{
		+0.0000,
		-0.0370,
		+0.1315
	});

	// Vector from pan servo to tilt servo
	const Vec rPT(3, new float[3]{
		+0.0130,
		+0.0335,
		+0.0553
	});

	// Vector from tilt servo to flame sensor
	const Vec rTS(3, new float[3]{
		-0.0164,
		+0.0711,
		+0.0071
	});

	// Flame Position
	Vec flamePos(3);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Calculates position vector (x, y, z) of flame.
//!d Stores vector in "flamePos". Requires flame be found.
void RobotDims::computeFlamePos() {

	// Origin to robot base
	Vec rOB(3, new float[3]{
		Odometer::position(1),
		Odometer::position(2),
		0.000
	});

	// Flame sensor to flame
	Vec rSF(3, new float[3]{
		0.000,
		FlameFinder::flameDistance,
		0.000
	});

	// Heading rotation matrix
	float cosH = cos(Odometer::heading);
	float sinH = sin(Odometer::heading);
	Mat rotH(3, 3, new float[9]{
		+cosH, +sinH, 0.000,
		-sinH, +cosH, 0.000,
		0.000, 0.000, 1.000
	});

	// Pan rotation matrix
	float cosP = cos(FlameFinder::flamePan);
	float sinP = sin(FlameFinder::flamePan);
	Mat rotP(3, 3, new float[9]{
		+cosP, +sinP, 0.000,
		-sinP, +cosP, 0.000,
		0.000, 0.000, 1.000
	});

	// Tilt rotation matrix
	float cosT = cos(FlameFinder::flameTilt);
	float sinT = sin(FlameFinder::flameTilt);
	Mat rotT(3, 3, new float[9]{
		1.000, 0.000, 0.000,
		0.000, +cosT, +sinT,
		0.000, -sinT, +cosT
	});

	// Flame position calculation
	flamePos = rOB + rotH * (
		rBP + rotP * (
			rPT + rotT * (
				rTS + rSF)));
}
