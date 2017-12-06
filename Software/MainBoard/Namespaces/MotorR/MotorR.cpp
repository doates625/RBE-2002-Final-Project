//**************************************************************/
// TITLE
//**************************************************************/

//!t MotorR.cpp
//!a RBE-2002 B17 Team 10

#include "MotorR.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace MotorR {

	// Arduino Pin Connections
	const uint8_t PIN_PWM = 5;
	const uint8_t PIN_FORWARD = 49;
	const uint8_t PIN_REVERSE = 47;
	const uint8_t PIN_ENCODER_A = 2;
	const uint8_t PIN_ENCODER_B = 3;

	// Motor Physical Properties
	const float TERMINAL_VOLTAGE = 12.0;
	const float ENCODER_CPR = 3200.0;

	// Motor Object
	DcMotor motor(
		TERMINAL_VOLTAGE,
		PIN_PWM,
		PIN_FORWARD,
		PIN_REVERSE,
		PIN_ENCODER_A,
		PIN_ENCODER_B,
		ENCODER_CPR);

	// Private Function Templates
	void interruptA();
	void interruptB();
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes and enables motor and sets up encoder ISRs.
//!d Call this method in the main setup function.
void MotorR::setup() {
	motor.setup();
	motor.enable();
	attachInterrupt(
		motor.getInterruptA(),
		interruptA,
		CHANGE);
	attachInterrupt(
		motor.getInterruptB(),
		interruptB,
		CHANGE);
}

//!b Performs ISR for motor encoder A.
//!d Do not call this method. It is only used internally.
void MotorR::interruptA() {
	motor.isrA();
}

//!b Performs ISR for motor encoder B.
//!d Do not call this method. It is only used internally.
void MotorR::interruptB() {
	motor.isrB();
}
