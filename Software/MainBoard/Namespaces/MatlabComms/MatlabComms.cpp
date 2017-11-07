//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.cpp
//!a RBE-2002 B17 Team 10

#include "MatlabComms.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace MatlabComms {

	// Communication parameters
	HardwareSerial* serialPort = &Serial;
	const unsigned long BAUD = 115200;

	// Communication objects
	BinarySerial serial(*serialPort, BAUD);
	Hc06 hc06(*serialPort, BAUD);
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes namespaces and waits for Matlab connection.
//!d Return codes:
//!d - 0: All success
//!d - 1: Hc06 connection failed
//!d - 2: Wrong initialized byte received
uint8_t MatlabComms::setup() {
	if(!hc06.setup()) return 1;
	serial.setup();
	serial.wait();
	if(serial.readByte() != BYTE_CONNECT) return 2;
	serial.writeByte(BYTE_CONNECT);
	return 0;
}
