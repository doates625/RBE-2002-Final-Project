//*************************************************************//
// TITLE
//*************************************************************//

//!t FireBot.h
//!b Namespace for RBE-2002 final project main control board.
//!a RBE-2002 B17 Team 10

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

