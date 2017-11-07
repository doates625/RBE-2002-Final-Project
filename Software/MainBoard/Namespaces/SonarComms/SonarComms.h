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

	const byte BYTE_BEGIN = 0x42; // "B"
	const byte BYTE_READY = 0x52; // "R"
	const byte BYTE_DATA  = 0x44; // "D"

	// Distance Variables
	extern float distF;
	extern float distB;
	extern float distL;
	extern float distR;

	// Function templates
	uint8_t setup();
	void loop();
}
