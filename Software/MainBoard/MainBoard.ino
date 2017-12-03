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

Timer printTimer;

//!b Executes once on Arduino reset.
void setup() {
	FlameFinder::setup();
	Serial.begin(115200);
	printTimer.tic();
}

//!b Executes repeatedly after Arduino reset.
void loop() {
	FlameFinder::loop();
	if(printTimer.hasElapsed(0.1)) {
		Serial.println();
		Serial.println("Brightness: " +
			String(FlameFinder::getBrightness()));
		Serial.println("Pan: " +
			String(FlameFinder::pan * 57.296, 0));
		Serial.println("Tilt: " +
			String(FlameFinder::tilt * 57.296, 0));
		printTimer.tic();
	}
}
