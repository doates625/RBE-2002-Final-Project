//**************************************************************/
// TITLE
//**************************************************************/

//!t MotorR.cpp
//!a RBE-2002 B17 Team 10

#include "MotorR.h"

//**************************************************************/
// NAMESPACE OBJECT DEFINITIONS
//**************************************************************/

// Motor object
namespace MotorR {
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
