//**************************************************************/
// TITLE
//**************************************************************/

//!t DriveSystem.cpp
//!a Dan Oates (RBE-2002 B17 Team 10)

#include "DriveSystem.h"
#include "MotorL.h"
#include "MotorR.h"
#include "Odometer.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace DriveSystem {

	// Drive System Parameters
	const float DRIVE_VOLTS_MAX = 9.0;	// (V)
	const float TURN_VOLTS_MAX = 3.0;	// (V)
	const float PID_RESET_TIME = 0.2;	// (s)

	// Velocity PID controller
	// Input: Velocity (m/s)
	// Output: Motor drive voltage (V)
	const float V_KP = 0.0;
	const float V_KI = 40.0;
	const float V_KD = 0.0;
	PidController velPid(V_KP, V_KI, V_KD,
		-DRIVE_VOLTS_MAX,
		+DRIVE_VOLTS_MAX,
		PID_RESET_TIME);

	// Heading PID Controller
	// Input: Angle (rad)
	// Output: Differential drive voltage (V)
	const float H_KP = 40.0;
	const float H_KI = 0.1;
	const float H_KD = 5.0;
	PidController headingPid(H_KP, H_KI, H_KD,
		-TURN_VOLTS_MAX,
		+TURN_VOLTS_MAX,
		PID_RESET_TIME);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes drive motors.
//!d Call this method in the main setup function.
void DriveSystem::setup() {
	MotorL::setup();
	MotorR::setup();
}

//!b Drives robot at given heading and velocity via PID control.
//!i Target heading (rad)
//!i Target velocity (m/s) (default 0)
bool DriveSystem::drive(float ht, float vt) {

	// Convert target heading into 0-2pi range
	if(ht > TWO_PI) {
		ht = fmod(ht, TWO_PI);
	} else if(ht < 0) {
		ht = fmod(ht, TWO_PI) + TWO_PI;
	}

	// Compute heading PID error
	float hError;
	float hc = Odometer::heading;
	if(ht <= PI) {
		if(hc <= ht + PI) hError = ht - hc;
		else hError = ht + TWO_PI - hc;
	} else {
		if(hc <= ht - PI) hError = ht - TWO_PI - hc;
		else hError = ht - hc;
	}

	// Update PID controllers
	float vDiff = headingPid.update(hError);
	float vDrive = velPid.update(vt - Odometer::velocity);

	// Drive motors
	MotorL::motor.setVoltage(vDrive + vDiff);
	MotorR::motor.setVoltage(vDrive - vDiff);

	// Check if robot is at heading steady state
	return headingPid.steadyState(0.02, 0.002);
}

//!b Immediately stops drive motors and resets PID controllers.
void DriveSystem::stop() {
	MotorL::motor.brake();
	MotorR::motor.brake();
	headingPid.reset();
	velPid.reset();
}
