//**************************************************************/
// TITLE
//**************************************************************/

//!t MatlabComms.cpp
//!a RBE-2002 B17 Team 10

#include "MatlabComms.h"
#include "Hc06.h"
#include "BinarySerial.h"
#include "Odometer.h"
#include "Sonar.h"
#include "DriveSystem.h"
#include "WallFollower.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace MatlabComms {

	// Communication parameters
	HardwareSerial*
		PORT = &Serial;
	const unsigned long
		BAUD = 57600;
	const float
		TIMEOUT = 1.0; // (s)

	// Byte message definitions
	const byte BYTE_CONNECT = 0x01;
	const byte BYTE_GETDATA = 0x02;
	const byte BYTE_DISCONNECT = 0x03;

	// Communication objects
	BinarySerial bSerial(*PORT, BAUD);
	Hc06 hc06(*PORT, BAUD);
	Timer timer;

	// Teleop Parameters
	bool disconnected = false;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes namespace and waits for Matlab connection.
//!d Call this method in the main setup function.
//!d Returns true or false indicating Hc06 connection status.
bool MatlabComms::setup() {
	if(hc06.setup()) {
		bSerial.setup();
		return true;
	} else
		return false;
}

//!b Waits for begin message from Matlab.
//!d Returns false if Matlab sent wrong connect byte.
bool MatlabComms::waitForBegin() {
	bSerial.flush();
	bSerial.wait();
	if(bSerial.readByte() == BYTE_CONNECT) {
		bSerial.writeByte(BYTE_CONNECT);
		timer.tic();
		return true;
	} else
		return false;
}

//!b Runs one iteration of Matlab communication loop.
//!d Call this method in the main loop function.
//!d Return codes:
//!d - 0: Complete success
//!d - 1: No message received within timeout
//!d - 2: Invalid message type byte received
//!d - 3: Teleop data timeout
uint8_t MatlabComms::loop() {
	if(bSerial.available()) {
		while(bSerial.available()) {

			// Check message type byte
			switch(bSerial.readByte()) {

				// Robot data request
				case BYTE_GETDATA:
					bSerial.writeByte(BYTE_GETDATA);
					bSerial.writeFloat(Odometer::robotPos(1));
					bSerial.writeFloat(Odometer::robotPos(2));
					bSerial.writeFloat(Odometer::heading);
					bSerial.writeFloat(Sonar::distF);
					bSerial.writeFloat(Sonar::distB);
					bSerial.writeFloat(Sonar::distL);
					bSerial.writeFloat(Sonar::distR);
					bSerial.writeByte(WallFollower::getState());
					break;

				// Disconnect message
				case BYTE_DISCONNECT:
					disconnected = true;
					break;

				// Invalid message type
				default:
					return 2;
			}
		}
		timer.tic();
	} else {
		if(timer.hasElapsed(TIMEOUT))
			return 1;
	}
	return 0;
}
