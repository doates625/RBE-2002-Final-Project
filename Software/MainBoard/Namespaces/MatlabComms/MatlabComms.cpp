//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.cpp
//!a RBE-2002 B17 Team 10

#include "MatlabComms.h"
#include "Odometer.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace MatlabComms {

	// Communication parameters
	HardwareSerial* serialPort = &Serial;
	const unsigned long BAUD = 115200;
	const byte BYTE_CONNECT = 0x01;
	const byte BYTE_TELEOP = 0x02;
	const byte BYTE_ODOMETRY = 0x03;
	const byte BYTE_DISCONNECT = 0x04;

	// Communication objects
	BinarySerial serial(*serialPort, BAUD);
	Hc06 hc06(*serialPort, BAUD);

	// Teleop Parameters
	bool disconnected = false;
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

			// Teleop command
			case BYTE_TELEOP:
				serial.wait(8);
				driveVoltageL = serial.readFloat();
				driveVoltageR = serial.readFloat();
				serial.writeByte(BYTE_TELEOP);
				break;

			// Odometry info request
			case BYTE_ODOMETRY:
				serial.writeByte(BYTE_ODOMETRY);
				serial.writeFloat(Odometer::robotPos(1));
				serial.writeFloat(Odometer::robotPos(2));
				serial.writeFloat(Odometer::h);
				break;

			// Disconnect message
			case BYTE_DISCONNECT:
				disconnected = true;
				break;
		}
}
