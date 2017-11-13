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
	float robotX = 0;
	float robotY = 0;

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

	// Get heading and change in heading
	h = getHeading();
	//float dH = h - hLast;
	//hLast = h;

	// Get encoder distances since last update
	float dL = MotorL::motor.encoderAngle();
	float dR = MotorR::motor.encoderAngle();
	MotorL::motor.resetEncoder();
	MotorR::motor.resetEncoder();

	// Compute delta position vector
	float arc = (dL + dR) * 0.5 * wheelRadius;
	robotX += (arc * sin(h));
	robotY += (arc * cos(h));

	/*
	float dx, dy;
	if(true) {
		dx = 0;
		dy = arc;
	} else {
		float R = arc / dH;
		dx = R * (cos(dH) - 1.0);
		dy = R * sin(dH);
	}
	*/

	// Rotate and add delta position vector
	// float ch = cos(h);
	// float sh = sin(h);
	//robotX += ((ch * dx) + (sh * dy));
	//robotY += ((ch * dy) - (sh * dx));

	// robotX = dL;
	// robotY = dR;
}
