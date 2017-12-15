//**************************************************************/
// TITLE
//**************************************************************/

//!t Odometer.cpp
//!a Dan Oates (RBE-2002 B17 Team 10)

#include "Odometer.h"
#include "RobotDims.h"
#include "MotorL.h"
#include "MotorR.h"
#include "Bno055.h"
#include "Timer.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace Odometer {

	// Home Distance Threshold (m)
	const float HOME_DISTANCE_THRESHOLD = 0.3;

	// Position Variables
	Vec position(2);	// Robot position vector (x,y) (m)
	Vec deltaPos(2);	// Small change in position (x,y) (m)
	Mat rotator(2,2);	// Heading rotation matrix

	// Velocity Variables
	float velocity = 0;
	Timer velocityTimer;

	// Heading Variables
	float headingCalibration = 0;
	float heading = 0;
	float lastHeading = 0;

	// Bno055 IMU
	Bno055 imu(tlb);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes and zeroes IMU.
//!d Call this method in the main setup function.
//!d Returns true if IMU is properly connected.
bool Odometer::setup() {
	if(imu.setup()) {
		headingCalibration = imu.heading();
		velocityTimer.tic();
		return true;
	} else
		return false;
}

//!b Performs one odometry iteration.
//!d Computes:
//!d - Position (x,y) (m)
//!d - Heading (rad)
//!d - Velocity (m/s)
void Odometer::loop() {

	// Get heading and change in heading
	heading = imu.heading() - headingCalibration;
	float dH = heading - lastHeading;
	lastHeading = heading;

	// Get encoder distances since last update
	float dL = MotorL::motor.encoderAngle();
	float dR = MotorR::motor.encoderAngle();
	MotorL::motor.resetEncoder();
	MotorR::motor.resetEncoder();

	// Compute velocity
	float arc = (dL + dR) * RobotDims::halfWheelRadius;
	velocity = arc / velocityTimer.toc();
	velocityTimer.tic();

	// Compute delta position vector
	if(dH == 0) {
		deltaPos(1) = 0;
		deltaPos(2) = arc;
	} else {
		float R = arc / dH;
		deltaPos(1) = R * (1.0 - cos(dH));
		deltaPos(2) = R * sin(dH);
	}

	// Rotate and add delta position vector to position
	float ch = cos(heading);
	float sh = sin(heading);

	rotator(1,1) = ch;
	rotator(1,2) = sh;
	rotator(2,1) = -sh;
	rotator(2,2) = ch;

	position = position + rotator * deltaPos;
}

//!b Returns true if robot is near home (0,0) within a threshold.
bool Odometer::nearHome() {
	return norm(position) <= HOME_DISTANCE_THRESHOLD;
}
