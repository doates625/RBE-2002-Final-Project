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
	const byte BYTE_CONNECT = 0x01;
	const byte BYTE_ERROR = 0x02;
	const byte BYTE_STATE = 0x03;

	const byte ERROR_IMU = 0x01;
	const byte ERROR_SONAR_TIMEOUT = 0x02;
	const byte ERROR_SONAR_BADBYTE = 0x03;

	const byte STATE_INITIALIZE = 0x01;
	const byte STATE_DIAGNOSTICS = 0x02;
	const byte STATE_TELEOPERATED = 0x03;

	extern BinarySerial serial;

	uint8_t setup();
	void imuError();
	void sonarTimeOutError();
	void sonarBadByteError();
}
