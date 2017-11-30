//**************************************************************/
// TITLE
//**************************************************************/

//!t Odometer.cpp
//!a RBE-2002 B17 Team 10

#include "Odometer.h"
#include "Bno055.h"
#include "MotorL.h"
#include "MotorR.h"
#include "RobotDims.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace Odometer {

	// Field position
	Vec robotPos(2); // Robot position vector (x,y) (m)
	Vec deltaPos(2); // Small change in position (m)
	Mat rotator(2,2); // Heading rotation matrix

	// Heading variables
	float headingCalibration = 0;
	float heading = 0;
	float lastHeading = 0;

	// Bno055 IMU
	Bno055 imu(trb); // UPDATE ORIENTATION
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes and zeroes IMU.
//!d Call this method in the main setup function.
//!d Returns true or false indicating IMU connection status.
bool Odometer::setup() {
	if(imu.setup()) {
		headingCalibration = imu.heading();
		return true;
	} else
		return false;
}

//!b Returns IMU heading relative to starting orientation.
float Odometer::getHeading() {
	return imu.heading() - headingCalibration;
}

//!b Returns IMU yawrate (rad/s).
//!d Returned rate is clockwise-positive to match with heading.
float Odometer::getYawrate() {
	return -imu.gZ();
}

//!b Updates robot position using IMU and encoders.
void Odometer::loop() {

	// Get heading and change in heading
	heading = getHeading();
	float dH = heading - lastHeading;
	lastHeading = heading;

	// Get encoder distances since last update
	float dL = MotorL::motor.encoderAngle();
	float dR = MotorR::motor.encoderAngle();
	MotorL::motor.resetEncoder();
	MotorR::motor.resetEncoder();

	// Compute delta position vector
	float arc = (dL + dR) * RobotDims::halfWheelRadius;
	if(dH == 0) {
		deltaPos(1) = 0;
		deltaPos(2) = arc;
	} else {
		float R = arc / dH;
		deltaPos(1) = R * (cos(dH) - 1.0);
		deltaPos(2) = R * sin(dH);
	}

	// Rotate and add delta position vector
	float ch = cos(heading);
	float sh = sin(heading);

	rotator(1,1) = ch;
	rotator(1,2) = sh;
	rotator(2,1) = -sh;
	rotator(2,2) = ch;

	robotPos = robotPos + rotator * deltaPos;
}
