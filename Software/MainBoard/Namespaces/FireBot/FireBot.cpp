//*************************************************************//
// TITLE
//*************************************************************//

//!t FireBot.cpp
//!a RBE-2002 B17 Team 10

#include "FireBot.h"

//*************************************************************//
// NAMESPACE FUNCTION DEFINITIONS
//*************************************************************//

//!b Executes once on Arduino reset.
//!d Tasks:
//!d - Initialize all namespaces for robot
//!d - Check for communication or initialization errors
//!d - Blink indicator LED if error occurs
void FireBot::setup() {

	// Initialize LED and Drive Motors
	IndicatorLed::setup();
	MotorL::setup();
	MotorR::setup();

	// Initialize Odometer
	switch(Odometer::setup()) {
		case 1: error(1); break;
		default: break;
	}

	// Initialize Sonar Board
	switch(SonarComms::setup()) {
		case 1: error(2); break;
		case 2: error(3); break;
		default: break;
	}

	// Initialize Matlab communication over Bluetooth
	switch(MatlabComms::setup()) {
		case 1: error(4); break;
		default: break;
	}

	// Indicate readiness for Matlab begin
	IndicatorLed::led.on();

	// Wait for begin message from Matlab
	switch(MatlabComms::waitForBegin()) {
		case 1: error(5); break;
		default: break;
	}
}

//!b Executes repeatedly after Arduino reset.
//!d Tasks:
//!d - Update odometer and position estimation
//!d - Check messages from Matlab
//!d - Drive motors with Matlab teleop commands
//!d - Stop robot if Matlab disconnects
void FireBot::loop() {

	// Update odometer position estimation
	Odometer::loop();

	// Check messages from Matlab
	switch(MatlabComms::loop()) {
		case 1: error(1); break;
		case 2: error(2); break;
		case 3: error(3); break;
		default: break;
	}

	// Drive robot with teleop commands
	MotorL::motor.setVoltage(MatlabComms::driveVoltageL);
	MotorR::motor.setVoltage(MatlabComms::driveVoltageR);

	// Check if Matlab disconnected
	if(MatlabComms::disconnected) {
		MotorL::motor.brake();
		MotorR::motor.brake();
		IndicatorLed::led.off();
	}
}

//!b Stops robot motion and flashes LED n times
void FireBot::error(uint8_t n) {
	MotorL::motor.brake();
	MotorR::motor.brake();
	IndicatorLed::flash(n);
}
