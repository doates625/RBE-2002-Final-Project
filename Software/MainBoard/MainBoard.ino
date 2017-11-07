//*************************************************************//
// TITLE
//*************************************************************//

//!t MainBoard.ino
//!b Software for RBE-2002 final project main control board.
//!a RBE-2002 B17 Team 10

#include "Arduino.h"
#include "IndicatorLed.h"
#include "MotorL.h"
#include "MotorR.h"
#include "Odometer.h"
#include "MatlabComms.h"
#include "SonarComms.h"

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

//!b Executes once on Arduino reset.
void setup() {

	// Initialize LED and Drive Motors
	IndicatorLed::setup();
	MotorL::setup();
	MotorR::setup();

	// Initialize Odometer
	switch(Odometer::setup()) {
		case 1: IndicatorLed::flash(1); break;
		default: break;
	}

	// Initialize Sonar Board
	switch(SonarComms::setup()) {
		case 1: IndicatorLed::flash(2); break;
		case 2: IndicatorLed::flash(3); break;
		default: break;
	}

	// Initialize Matlab communication over Bluetooth
	switch(MatlabComms::setup()) {
		case 1: IndicatorLed::flash(4); break;
		case 2: IndicatorLed::flash(5); break;
		default: break;
	}

	// Indicate readiness
	IndicatorLed::led.on();
}

//!b Executes repeatedly after Arduino reset.
void loop() {}
