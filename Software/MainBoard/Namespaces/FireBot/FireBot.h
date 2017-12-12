//*************************************************************//
// TITLE
//*************************************************************//

//!t FireBot.h
//!b Namespace for RBE-2002 final project main control board.
//!a RBE-2002 B17 Team 10

//!d This code is removed from the main Arduino (ino) file in
//!d order to quickly and easily alter the robot upload code
//!d for debug purposes.

#include "Arduino.h"

//*************************************************************//
// NAMESPACE DECLARATION
//*************************************************************//

namespace FireBot {
	void setup();
	void loop();
	byte getState();

	bool flameDetected();
	bool flameExtinguished();

	void error(uint8_t);

	void serialFlameReadTest();
}

