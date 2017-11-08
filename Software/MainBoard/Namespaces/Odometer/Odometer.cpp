//**************************************************************/
// TITLE
//**************************************************************/

//!t Odometer.cpp
//!a RBE-2002 B17 Team 10

#include "Odometer.h"
#include "Bno055.h"
#include "MotorL.h"
#include "MotorR.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace Odometer {

	// Robot Physical Properties
	const float wheelRadius = 0.034925; // (m)
	const float wheelTrack = 0.254; // (m) INCORRECT
	const float halfWheelTrack = wheelTrack * 0.5;

	// Field position
	Vec robotPos(2);
	Vec deltaPos(2);
	Mat rotator(2,2);
	float headingCalibration = 0;
	float h = 0;
	float hLast = 0;

	// Bno055 IMU
	Bno055 imu(trb); // UPDATE ORIENTATION
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes IMU and namespace.
//!d Call this method in setup.
//!d Return codes:
//!d - 0: Everything worked.
//!d - 1: IMU connection failed.
uint8_t Odometer::setup() {
	if(imu.setup()) {
		headingCalibration = imu.heading();
		return 0;
	} else
		return 1;
}

//!b Returns IMU heading relative to starting orientation.
float Odometer::getHeading() {
	return imu.heading() - headingCalibration;
}

//!b Updates robot position from IMU and encoders.
void Odometer::loop() {

	// Compute heading, change in heading, travel arc length
	h = getHeading();
	float dH = h - hLast;
	float arc = (
		MotorL::motor.encoderAngle() +
		MotorR::motor.encoderAngle()
	) * 0.5 * wheelRadius;

	// Compute delta position vector
	if(true) { // dH < 0.001
		deltaPos(1) = 0;
		deltaPos(2) = arc;
	} else {
		float R = arc / dH;
		deltaPos(1) = R * (cos(dH) - 1.0);
		deltaPos(2) = R * sin(dH);
	}

	// Rotate delta position vector
	float ch = cos(h);
	float sh = sin(h);

	rotator(1,1) = ch;
	rotator(1,2) = sh;
	rotator(2,1) = -sh;
	rotator(2,2) = ch;

	// Add rotated delta vector to robot position
	robotPos = robotPos + rotator * deltaPos;
	hLast = h;

	// Reset encoders
	MotorL::motor.resetEncoder();
	MotorR::motor.resetEncoder();
}
