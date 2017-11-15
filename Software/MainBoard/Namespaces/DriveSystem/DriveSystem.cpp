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

	// Heading PID Controller
	// Input: Angular error (rad)
	// Output: Differential motor voltage (V)

	float PID_KP = 6.0;
	float PID_KI = 1.5;
	float PID_KD = 0.5;
	float PID_VMAX = 4.0;
	float PID_TRST = 1.0;

	PidController headingPid(
		PID_KP,
		PID_KI,
		PID_KD,
		-PID_VMAX,
		+PID_VMAX,
		PID_TRST);

	float targetHeading = 0.0;
	float driveVoltage = 0.0;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes drive motors (call in setup).
void DriveSystem::setup() {
	MotorL::setup();
	MotorR::setup();
}

//!b Drives motors and steers towards target heading
void DriveSystem::loop() {

	// Compute error
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

	// If angle is stabilized, reset PID
	if(atTargetHeading())
		headingPid.reset();
}

//!b Returns true if drive system is at target heading.
bool DriveSystem::atTargetHeading() {
	return headingPid.steadyState(0.02, 0.001);
}

//!b Resets all PID controllers in namespace.
void DriveSystem::resetPids() {
	headingPid.reset();
}

//!b Immediately stops drive motors.
void DriveSystem::stop() {
	MotorL::motor.brake();
	MotorR::motor.brake();
}
