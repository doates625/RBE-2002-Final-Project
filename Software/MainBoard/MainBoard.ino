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
	FireBot::setup();
	printTimer.tic();
	Serial.begin(115200);
}

//!b Executes repeatedly after Arduino reset.
void loop() {
	Sonar::loop();
	if(printTimer.hasElapsed(0.25)) {
		printTimer.tic();
		Serial.println("Time: " + String(clockTime(), 0) + "s");
		Serial.println("F: " + String(Sonar::distF, 2) + "m");
		Serial.println("B: " + String(Sonar::distB, 2) + "m");
		Serial.println("L: " + String(Sonar::distL, 2) + "m");
		Serial.println("R: " + String(Sonar::distR, 2) + "m");
	}
}
