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

	// Drive System Parameters
	const float DRIVE_VELOCITY = 0.1;	// (m/s)
	const float DRIVE_VOLTS_MAX = 8.0;	// (V)
	const float TURN_VOLTS_MAX = 2.0;	// (V)
	const float PID_RESET_TIME = 0.2;	// (s)

	// Velocity PID controller
	// Input: Velocity (m/s)
	// Output: Motor drive voltage (V)
	const float V_KP = 0.0;
	const float V_KI = 110.0;
	const float V_KD = 0.0;
	PidController velPid(V_KP, V_KI, V_KD,
		-DRIVE_VOLTS_MAX,
		+DRIVE_VOLTS_MAX,
		PID_RESET_TIME);

	// Heading PID Controller
	// Input: Angular error (rad)
	// Output: Differential drive voltage (V)
	const float H_KP = 50.0;
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

//!b Initializes drive system motors.
//!d Call this method in the main setup function.
void DriveSystem::setup() {
	MotorL::setup();
	MotorR::setup();
}

//!b Steers robot towards target heading
//!d Returns true if robot is stabilized at that heading.
//!i Target heading in rad (PID-controlled)
//!i True for go forward, false for turn in place
bool DriveSystem::driveAtHeading(float ht, bool goFwd) {

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
	float vDrive = 0;
	if(goFwd) {
		float vError = DRIVE_VELOCITY - Odometer::speed;
		vDrive = velPid.update(vError);
	}

	// Drive motors
	MotorL::motor.setVoltage(vDrive + vDiff);
	MotorR::motor.setVoltage(vDrive - vDiff);

	// Check if robot is at steady state
	return headingPid.steadyState(0.05, 0.001);
}

//!b Immediately stops drive motors and resets PID controllers.
void DriveSystem::stop() {
	MotorL::motor.brake();
	MotorR::motor.brake();
}
