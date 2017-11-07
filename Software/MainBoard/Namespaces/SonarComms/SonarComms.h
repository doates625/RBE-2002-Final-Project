//**************************************************************/
// TITLE
//**************************************************************/

//!t SonarComms.h
//!b Namespace for final project sonar board communication.
//!a RBE-2002 B17 Team 10

#pragma once
#include "BinarySerial.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace SonarComms {

	// Distance Variables
	extern float distF;
	extern float distB;
	extern float distL;
	extern float distR;

	// Function templates
	uint8_t setup();
	uint8_t loop();
}
