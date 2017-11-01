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

//*************************************************************//
// MAIN FUNCTION DEFINITIONS
//*************************************************************//

//!b Executes once on Arduino reset.
//!d Tasks:
//!d - Initialize odometer.
//!d - Initialize Matlab communication over Bluetooth.
void setup() {
	MatlabComms::setup();
}

//!b Executes repeatedly after Arduino reset.
//!d Tasks:
//!d - None
void loop() {

}
