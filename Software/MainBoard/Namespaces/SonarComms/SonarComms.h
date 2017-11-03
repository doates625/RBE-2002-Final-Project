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

	// Arduino pins
	const uint8_t PIN_RESET = 2;

	// Communication objects and parameters
	extern HardwareSerial* serial;
	extern BinarySerial bs;
	const unsigned long BAUD = 115200;
	const float TIMEOUT = 1.0; // (s)

	const byte BYTE_BEGIN = 0x42; // "B"
	const byte BYTE_READY = 0x52; // "R"
	const byte BYTE_DATA  = 0x44; // "D"

	// Distance Variables
	extern float distF;
	extern float distB;
	extern float distL;
	extern float distR;

	// Function templates
	void setup();
	void loop();
}
