//*************************************************************//
// TITLE
//*************************************************************//

//!t FireBot.cpp
//!a RBE-2002 B17 Team 10

#include "FireBot.h"

//*************************************************************//
// NAMESPACE FUNCTION DEFINITIONS
//*************************************************************//

//!b Initializes FireBot.
//!d Call this method in the main setup function.
//!d Tasks:
//!d - Initialize all namespaces for robot
//!d - Check for communication or initialization errors
//!d - Blink indicator LED corresponding to any errors.
void FireBot::setup() {

	// Initialize Namespaces
	IndicatorLed::setup();
	DriveSystem::setup();
	WallFollower::setup();

	// Initialize Odometer
	switch(Odometer::setup()) {
		case 1: error(1); break; // IMU connection failure
		default: break;
	}

	// Initialize Sonar Board
	switch(SonarComms::setup()) {
		case 1: error(2); break; // Sonar board timeout
		case 2: error(3); break; // Sonar bad byte
		default: break;
	}

	// Initialize Matlab communication over Bluetooth
	switch(MatlabComms::setup()) {
		case 1: error(4); break; // Hc06 AT command failure
		default: break;
	}

	// Indicate readiness for Matlab begin
	IndicatorLed::led.on();

	// Wait for begin message from Matlab
	switch(MatlabComms::waitForBegin()) {
		case 1: error(5); break;
		default: break;
	}

	// Begin wall-following.
	WallFollower::begin();
}

//!b Executes repeatedly after Arduino reset.
//!d Call this method in the main loop function.
//!d Tasks:
//!d - Update odometer and position estimation
//!d - Check messages from Matlab
//!d - Drive motors with Matlab teleop commands
//!d - Stop robot if Matlab disconnects
void FireBot::loop() {

	// Update odometer position estimation
	Odometer::loop();

	// Check if sonar board has new data
	switch(SonarComms::loop()) {
		case 1: error(1); break; // Sonar board timeout
		case 2: error(2); break; // Sonar bad byte
		default: break;
	}

	// Check messages from Matlab
	switch(MatlabComms::loop()) {
		case 1: error(3); break; // No messages timeout
		case 2: error(4); break; // Invalid message type byte
		case 3: error(5); break; // Teleop data timeout
		default: break;
	}
	if(MatlabComms::disconnected) {
		DriveSystem::stop();
		IndicatorLed::led.off();
		while(1);
	}

	// Run one iteration of wall-follower
	WallFollower::loop();
}

//!b Stops robot driving and flashes LED n times in a loop.
void FireBot::error(uint8_t n) {
	DriveSystem::stop();
	IndicatorLed::flash(n);
}
