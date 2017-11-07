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
	const byte BYTE_CONNECT = 0x01;
	const byte BYTE_TELEOP = 0x02;

	// Communication objects
	BinarySerial serial(*serialPort, BAUD);
	Hc06 hc06(*serialPort, BAUD);

	// Teleop drive voltages
	float driveVoltageL = 0;
	float driveVoltageR = 0;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes namespace and waits for Matlab connection.
//!d Return codes:
//!d - 0: Complete success
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

//!b Runs one iteration of Matlab communication loop.
void MatlabComms::loop() {
	if(serial.available())
		switch(serial.readByte()) {
			case BYTE_TELEOP:
				serial.wait(8);
				driveVoltageL = serial.readFloat();
				driveVoltageR = serial.readFloat();
				serial.writeByte(BYTE_TELEOP);
				break;
		}
}
