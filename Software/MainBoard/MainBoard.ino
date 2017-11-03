//*************************************************************//
// TITLE
//*************************************************************//

//!t MainBoard.ino
//!b Software for RBE-2002 final project main control board.
//!a RBE-2002 B17 Team 10

#include "IndicatorLed.h"
#include "Arduino.h"
#include "MotorL.h"
#include "MotorR.h"
#include "Odometer.h"
#include "MatlabComms.h"
#include "SonarComms.h"

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

//!b Executes once on Arduino reset.
//!d Tasks:
//!d - Initialize odometer.
//!d - Initialize Matlab communication over Bluetooth.
void setup() {
	Serial.begin(115200);
	SonarComms::setup();
}

//!b Executes repeatedly after Arduino reset.
//!d Tasks:
//!d - Loop Matlab communication.
void loop() {
	SonarComms::loop();
	using namespace SonarComms;
	Serial.println("F: " + String(distF));
	// Serial.println("B: " + String(distB));
	// Serial.println("L: " + String(distL));
	// Serial.println("R: " + String(distR));
	delay(250);
}
