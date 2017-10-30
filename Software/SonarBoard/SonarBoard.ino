//*************************************************************//
// TITLE
//*************************************************************//

//!t SonarBoard.ino
//!b Software for RBE-2002 final project sonar board.
//!a RBE-2002 B17 Team 10

#include "Arduino.h"

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

//!b Executes once on Arduino reset.
//!d Tasks:
//!d - Initialize LED on pin 13.
void setup() {
	pinMode(13, OUTPUT);
}

//!b Executes repeatedly after Arduino reset.
//!d Tasks:
//!d - Blink LED at 2Hz.
void loop() {
	digitalWrite(13, HIGH);
	delay(100);
	digitalWrite(13, LOW);
	delay(400);
}
