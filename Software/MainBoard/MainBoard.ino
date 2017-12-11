//*************************************************************//
// TITLE
//*************************************************************//

//!t MainBoard.ino
//!b Software for RBE-2002 final project main control board.
//!a RBE-2002 B17 Team 10

#include "FireBot.h"
#include "FlameFinder.h"
#include "Odometer.h"
#include "Timer.h"
#include "Sonar.h"

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

//!b Executes once on Arduino reset.
void setup() {
	// FireBot::setup();
	// FlameFinder::serialDistanceTest();
	Sonar::serialTest();
}

//!b Executes repeatedly after Arduino reset.
void loop() {
	// FireBot::loop();
}
