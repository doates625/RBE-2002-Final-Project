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
	// Output: Differential drive voltage (V)
	const float H_KP = 50.0;
	const float H_KI = 0.1;
	const float H_KD = 1.0;
	const float H_VMAX = 3.5; // Max output voltage (V)
	const float H_TRST = 0.5; // Reset time (s)

	PidController headingPid(H_KP, H_KI, H_KD,
		-H_VMAX,
		+H_VMAX,
		H_TRST);

	// Yawrate PID Controller
	// Input: Angular velocity (rad/s)
	// Output: Differential drive voltage (V)
	const float Y_KP = 1.5;
	const float Y_KI = 30.0;
	const float Y_KD = 0.0;
	const float Y_VMAX = 3.5; // Max output voltage (V)
	const float Y_TRST = 0.5; // Reset time (s)

	PidController yawratePid(Y_KP, Y_KI, Y_KD,
		-Y_VMAX,
		+Y_VMAX,
		Y_TRST);
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

//!b Drives robot and turns towards target heading.
//!d Returns true if robot is stabilized at that heading.
//!i Forward drive voltage (- for reverse drive)
//!i Target heading in rad (PID-controlled)
bool DriveSystem::driveAtHeading(float vDrive, float ht) {

	// Compute PID error
	float err;
	float hc = Odometer::heading;
	if(ht <= PI) {
		if(hc <= ht + PI) err = ht - hc;
		else err = ht + TWO_PI - hc;
	} else {
		if(hc <= ht - PI) err = ht - TWO_PI - hc;
		else err = ht - hc;
	}

	// Update PID controller and drive motors
	float vDiff = headingPid.update(err);
	MotorL::motor.setVoltage(vDrive + vDiff);
	MotorR::motor.setVoltage(vDrive - vDiff);

	// Check if robot is at steady state
	return headingPid.steadyState(0.05, 0.001);
}

//!b Drives robot and turns at target yawrate.
//!i Forward drive voltage (- for reverse drive)
//!i Target yawrate in rad/s (PID-controlled)
void DriveSystem::driveAtYawrate(float vDrive, float yt) {
	float yc = Odometer::getYawrate();
	float vDiff = yawratePid.update(yt - yc);
	MotorL::motor.setVoltage(vDrive + vDiff);
	MotorR::motor.setVoltage(vDrive - vDiff);
}

//!b Resets all PID controllers in namespace.
void DriveSystem::resetPids() {
	headingPid.reset();
	yawratePid.reset();
}

//!b Immediately stops drive motors and resets PID controllers.
void DriveSystem::stop() {
	MotorL::motor.brake();
	MotorR::motor.brake();
	resetPids();
}
