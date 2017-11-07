//**************************************************************/
// TITLE
//**************************************************************/

//!t MotorL.h
//!b Namespace for final project robot left drive motor.
//!a RBE-2002 B17 Team 10

#pragma once
#include "DcMotor.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace MotorL {

	// Arduino Pin Connections
	const uint8_t PIN_ENABLE = 8;
	const uint8_t PIN_FORWARD = 9;
	const uint8_t PIN_REVERSE = 10;
	const uint8_t PIN_ENCODER_A = 3;
	const uint8_t PIN_ENCODER_B = 2;

	// Motor Physical Properties
	extern DcMotor motor;
	const float TERMINAL_VOLTAGE = 12.0;
	const float ENCODER_CPR = 3200.0;

	// Function templates
	void setup();
	void interruptA();
	void interruptB();
}
