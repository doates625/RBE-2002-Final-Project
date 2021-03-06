//*************************************************************//
// TITLE
//*************************************************************//

//!t MainBoard.ino
//!b Software for RBE-2002 final project main control board.
//!a Dan Oates (RBE-2002 B17 Team 10)

//!d This code is uploaded to the Arduino Mega 2560 that
//!d controls the firefighter robot.

#include "FireBot.h"

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

//!b Executes once on Arduino reset.
void setup() {
	FireBot::setup();
}

//!b Executes repeatedly after Arduino reset.
void loop() {
	FireBot::loop();
}
