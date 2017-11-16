//*************************************************************//
// TITLE
//*************************************************************//

//!t FireBot.h
//!b Namespace for RBE-2002 final project main control board.
//!a RBE-2002 B17 Team 10

//!d This code is removed from the main Arduino (ino) file in
//!d order to quickly and easily alter the robot upload code
//!d for debug purposes.

#include "IndicatorLed.h"
#include "DriveSystem.h"
#include "Odometer.h"
#include "MatlabComms.h"
#include "SonarComms.h"

//*************************************************************//
// MAIN FUNCTION DECLARATIONS
//*************************************************************//

namespace FireBot {
	void setup();
	void loop();
	void error(uint8_t);
}

