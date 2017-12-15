//**************************************************************/
// TITLE
//**************************************************************/

//!t MotorR.h
//!b Namespace for final project robot right drive motor.
//!a Dan Oates (RBE-2002 B17 Team 10)

//!d This namespace contains a DcMotor object for controlling
//!d the right drive wheel and initializes encoder ISRs in its
//!d setup method.

#pragma once
#include "DcMotor.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace MotorR {
	extern DcMotor motor;
	void setup();
}
