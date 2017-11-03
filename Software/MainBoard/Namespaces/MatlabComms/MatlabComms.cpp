//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.cpp
//!a RBE-2002 B17 Team 10

#include "MatlabComms.h"
#include "IndicatorLed.h"
#include "Odometer.h"
#include "SonarComms.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace MatlabComms {
	bool isSetup = false;

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
	if(!isSetup) {
		isSetup = true;
		IndicatorLed::setup();
		Odometer::setup();
		SonarComms::setup();

		bs.setup();
		bs.wait();
		if(bs.readByte() == BYTE_BEGIN)
			return;
		else
			IndicatorLed::flash(4);
	}
}

//!b Performs one communication loop iteration with Matlab.
void MatlabComms::loop() {
	if(bs.available()) {
		switch(bs.readByte()) {
			case BYTE_UPDATE:
				bs.writeFloat(clockTime());
				bs.writeFloat(Odometer::heading());
				bs.writeFloat(SonarComms::distF);
				bs.writeFloat(SonarComms::distB);
				bs.writeFloat(SonarComms::distL);
				bs.writeFloat(SonarComms::distR);
				break;
			default:
				break;
		}
	}
}
