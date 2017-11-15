//*************************************************************//
// TITLE
//*************************************************************//

//!t MainBoard.ino
//!b Software for RBE-2002 final project main control board.
//!a RBE-2002 B17 Team 10

#include "FireBot.h"

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

using namespace DriveSystem;

//!b Executes once on Arduino reset.
void setup() {
	FireBot::setup();
}

//!b Executes repeatedly after Arduino reset.
void loop() {
	FireBot::loop();
}
