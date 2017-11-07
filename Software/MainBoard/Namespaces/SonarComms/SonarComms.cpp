//**************************************************************/
// TITLE
//**************************************************************/

//!t SonarComms.cpp
//!a RBE-2002 B17 Team 10

#include "SonarComms.h"
#include "IndicatorLed.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace SonarComms {

	// Sonar board reset pin
	const uint8_t PIN_RESET = 4;

	// Serial communication parameters
	HardwareSerial* serial = &Serial2;
	const unsigned long BAUD = 115200;
	const float TIMEOUT = 1.0; // (s)
	const byte BYTE_SONAR = 0x01;

	// Binary serial interface
	BinarySerial bs(
		*serial,
		BAUD);

	// Sonar distance variables
	float distF = 0;
	float distB = 0;
	float distL = 0;
	float distR = 0;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes namespace
//!d Return codes:
//!d - 0: Everything worked
//!d - 1: Sonar board timed out
//!d - 2: Sonar board sent bad byte
uint8_t SonarComms::setup() {

	// Reset the sonar board
	pinMode(PIN_RESET, OUTPUT);
	digitalWrite(PIN_RESET, HIGH);
	delay(1500);

	// Start serial communication
	bs.setup();
	bs.flush();
	bs.writeByte(BYTE_SONAR);
	if(!bs.wait(1, TIMEOUT)) return 1;
	if(bs.readByte() != BYTE_SONAR) return 2;
	return 0;
}

//!b Performs one communication loop iteration with sonar board.
//!d Return codes:
//!d - 0: Complete success
//!d - 1: Sonar board timed out
//!d - 2: Sonar board sent bad byte
uint8_t SonarComms::loop() {
	bs.writeByte(BYTE_SONAR);
	if(bs.wait(17, TIMEOUT)) {
		if(bs.readByte() != BYTE_SONAR)
			return 2;
		distF = bs.readFloat();
		distB = bs.readFloat();
		distL = bs.readFloat();
		distR = bs.readFloat();
		return 0;
	} else
		return 1;
}
