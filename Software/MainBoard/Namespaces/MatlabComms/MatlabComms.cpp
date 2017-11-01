//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.cpp
//!a RBE-2002 B17 Team 10

#include "MatlabComms.h"
#include "IndicatorLed.h"
#include "Odometer.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace MatlabComms {
	HardwareSerial* serial = &Serial;
	BinarySerial bs(
		*serial,
		BAUD);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes namespaces and waits for Matlab connection.
void MatlabComms::setup() {
	IndicatorLed::setup();
	Odometer::setup();
	digitalWrite(PIN_RESET, HIGH);
	pinMode(PIN_RESET, OUTPUT);
	bs.setup();
	bs.wait();
	if(bs.readByte() == BYTE_BEGIN) {
		return;
	}
	else
		IndicatorLed::flash();
}

//!b Performs one communication loop iteration with Matlab.
void MatlabComms::loop() {
	if(bs.available()) {
		switch(bs.readByte()) {
			case BYTE_RESET:
				resetArduino();
				break;
			case BYTE_UPDATE:
				bs.writeFloat(Odometer::heading());
				break;
			default:
				break;
		}
	}
}

//!b Resets the Arduino board.
void MatlabComms::resetArduino() {
	digitalWrite(PIN_RESET, LOW);
}
