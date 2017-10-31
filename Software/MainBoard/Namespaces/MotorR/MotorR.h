//**************************************************************/
// TITLE
//**************************************************************/

//!t MotorR.h
//!b Namespace for final project robot right drive motor.
//!a RBE-2002 B17 Team 10

#pragma once
#include "DcMotor.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace MotorR {

	// Arduino Pin Connections
	const uint8_t PIN_ENABLE = 0; // FILL THESE IN
	const uint8_t PIN_FORWARD = 0;
	const uint8_t PIN_REVERSE = 0;
	const uint8_t PIN_ENCODER_A = 0;
	const uint8_t PIN_ENCODER_B = 0;

	// DC Motor Object
	extern DcMotor motor;
	const float TERMINAL_VOLTAGE = 12.0;
	const float ENCODER_CPR = 3200.0;

	// Function templates
	void setup();
	void interruptA();
	void interruptB();
}