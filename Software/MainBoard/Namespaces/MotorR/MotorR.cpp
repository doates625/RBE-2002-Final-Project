//**************************************************************/
// TITLE
//**************************************************************/

//!t MotorR.cpp
//!a RBE-2002 B17 Team 10

#include "MotorR.h"
#include "DcMotor.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace MotorR {

	// Arduino Pin Connections
	const uint8_t PIN_ENABLE = 0; // FILL THESE IN
	const uint8_t PIN_FORWARD = 0;
	const uint8_t PIN_REVERSE = 0;
	const uint8_t PIN_ENCODER_A = 0;
	const uint8_t PIN_ENCODER_B = 0;

	// Motor Physical Properties
	const float TERMINAL_VOLTAGE = 12.0;
	const float ENCODER_CPR = 3200.0;

	// Motor object
	DcMotor motor(
		TERMINAL_VOLTAGE,
		PIN_ENABLE,
		PIN_FORWARD,
		PIN_REVERSE,
		PIN_ENCODER_A,
		PIN_ENCODER_B,
		ENCODER_CPR);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

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

//!b Initializes and enables motor.
//!d Call this method in setup.
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
