//**************************************************************/
// TITLE
//**************************************************************/

//!t MotorL.cpp
//!a RBE-2002 B17 Team 10

#include "MotorL.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace MotorL {

	// Arduino Pin Connections
	const uint8_t PIN_ENABLE = 8;
	const uint8_t PIN_FORWARD = 9;
	const uint8_t PIN_REVERSE = 10;
	const uint8_t PIN_ENCODER_A = 3;
	const uint8_t PIN_ENCODER_B = 2;

	// Motor Physical Properties
	const float TERMINAL_VOLTAGE = 12.0;
	const float ENCODER_CPR = 3200.0;

	// Motor Object
	DcMotor motor(
		TERMINAL_VOLTAGE,
		PIN_ENABLE,
		PIN_FORWARD,
		PIN_REVERSE,
		PIN_ENCODER_A,
		PIN_ENCODER_B,
		ENCODER_CPR);

	// Private function templates
	void interruptA();
	void interruptB();
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes and enables motor and encoder ISRs.
//!d Call this method in the main setup function.
void MotorL::setup() {
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
void MotorL::interruptA() {
	motor.isrA();
}

//!b Performs ISR for motor encoder B.
//!d Do not call this method. It is only used internally.
void MotorL::interruptB() {
	motor.isrB();
}
