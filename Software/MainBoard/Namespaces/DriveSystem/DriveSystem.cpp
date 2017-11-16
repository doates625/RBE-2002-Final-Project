//**************************************************************/
// TITLE
//**************************************************************/

//!t DriveSystem.cpp
//!a RBE-2002 B17 Team 10

#include "DriveSystem.h"
#include "MotorL.h"
#include "MotorR.h"
#include "Odometer.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace DriveSystem {

	// Initial control variables
	float targetHeading = 0.0;
	float driveVoltage = 0.0;

	// Used to detect change in target heading
	float lastTargetHeading = targetHeading;

	// Heading PID Controller
	// Input: Angular error (rad)
	// Output: Differential motor voltage (V)
	const float KP = 6.0;
	const float KI = 1.5;
	const float KD = 0.5;
	const float MAX_TURNING_VOLTAGE = 4.0;
	const float PID_RESET_TIME = 1.0;

	PidController headingPid(KP, KI, KD,
		-MAX_TURNING_VOLTAGE,
		+MAX_TURNING_VOLTAGE,
		PID_RESET_TIME);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes drive system motors.
//!d Call this method in the main setup function.
void DriveSystem::setup() {
	MotorL::setup();
	MotorR::setup();
}

//!b Drives motors and steers towards target heading
//!d Call this method in the main loop function.
void DriveSystem::loop() {

	// Reset PID controller if target heading changes
	if(targetHeading != lastTargetHeading) {
		headingPid.reset();
	}
	lastTargetHeading = targetHeading;

	// Compute PID error
	float err;
	float hc = Odometer::heading;
	float ht = targetHeading;
	if(ht <= PI) {
		if(hc <= ht + PI) err = ht - hc;
		else err = ht + TWO_PI - hc;
	} else {
		if(hc <= ht - PI) err = ht - TWO_PI - hc;
		else err = ht - hc;
	}

	// Update PID controller and drive motors
	float diffVoltage = headingPid.update(err);
	MotorL::motor.setVoltage(driveVoltage + diffVoltage);
	MotorR::motor.setVoltage(driveVoltage - diffVoltage);
}

//!b Immediately stops drive motors.
void DriveSystem::stop() {
	MotorL::motor.brake();
	MotorR::motor.brake();
}
