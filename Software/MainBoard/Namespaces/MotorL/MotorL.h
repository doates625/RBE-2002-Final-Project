//**************************************************************/
// TITLE
//**************************************************************/

//!t MotorL.h
//!b Namespace for final project robot left drive motor.
//!a RBE-2002 B17 Team 10

//!d This namespace contains a DcMotor object for controlling
//!d the left drive wheel and initializes encoder ISRs.

#pragma once
#include "DcMotor.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace MotorL {
	extern DcMotor motor;
	void setup();
}
