//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.h
//!b Namespace for final project Matlab communication system.
//!a RBE-2002 B17 Team 10

#pragma once
#include "Hc06.h"
#include "BinarySerial.h"

//**************************************************************/
// NAMESPACE DECLARATION
//**************************************************************/

namespace MatlabComms {

	// Communication objects and parameters
	extern HardwareSerial* serial;
	extern BinarySerial bs;
	const unsigned long BAUD = 115200;
	const byte BYTE_BEGIN = 0x01;
	const byte BYTE_RESET = 0x02;
	const byte BYTE_UPDATE = 0x03;

	// Arduino pins
	const uint8_t PIN_RESET = 11;

	// Function templates
	void setup();
	void loop();
	void resetArduino();
}
