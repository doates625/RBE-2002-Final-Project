//**************************************************************/
// TITLE
//**************************************************************/

//!t Odometer.cpp
//!a RBE-2002 B17 Team 10

#include "Odometer.h"
#include "LinearAtmel.h"
#include "Bno055.h"
#include "MotorL.h"
#include "MotorR.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace Odometer {
	bool isSetup = false;

	// Robot Physical Properties
	const float wheelRadius = 0.034925; // (m)
	const float wheelTrack = 0.254; // (m) INCORRECT
	const float halfWheelTrack = wheelTrack * 0.5;

	// Field position
	Vec robotPos(2);
	Vec deltaPos(2);
	Mat rotator(2,2);
	float h0 = 0;

	// Bno055 IMU
	Bno055 imu(trb); // UPDATE ORIENTATION
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes IMU and namespace.
//!d Call this method in setup.
void Odometer::setup() {
	if(!isSetup) {
		isSetup = true;
		imu.setup();
		h0 = imu.heading();
	}
}

//!b Returns IMU heading relative to starting orientation.
float Odometer::heading() {
	return imu.heading() - h0;
}

//!b Updates robot position from IMU and encoders.
void Odometer::update() {
	float dL = MotorL::motor.encoderAngle() * wheelRadius;
	float dR = MotorR::motor.encoderAngle() * wheelRadius;
	float rT = halfWheelTrack * (dR + dL) / (dR - dL);
	float dT = dR / (rT + halfWheelTrack);

	deltaPos(1) = rT * sin(dT);
	deltaPos(2) = rT * (1.0 - cos(dT));

	float h = heading();
	float ch = cos(h);
	float sh = sin(h);

	rotator(1,1) = ch;
	rotator(1,2) = -sh;
	rotator(2,1) = sh;
	rotator(2,2) = ch;

	robotPos = robotPos + rotator * deltaPos;
}
