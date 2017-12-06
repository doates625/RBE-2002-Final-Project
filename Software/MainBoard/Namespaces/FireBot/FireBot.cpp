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
void FireBot::setup() {

	// Initialize Namespaces
	IndicatorLed::setup();
	DriveSystem::setup();
	Sonar::setup();
	WallFollower::setup();
	if(!Odometer::setup()) {
		error(1);	// Indicate IMU failure
	}
	if(!MatlabComms::setup()) {
		error(2);	// Indicate Hc06 failure
	}

	IndicatorLed::led.on();	// Indicate setup is complete

#ifdef MATLAB_ENABLED

	// Wait for Matlab begin message
	if(!MatlabComms::waitForBegin()) {
		error(3);	// Indicate Matlab sent wrong byte
	}

#endif
}

//!b Executes repeatedly after Arduino reset.
//!d Call this method in the main loop function.
void FireBot::loop() {

	Odometer::loop();		// Update position and angle
	Sonar::loop();			// Update robot sonar
	FlameFinder::loop();	// Run flame-finder state machine
	WallFollower::loop();	// Run wall-follow state machine

#ifdef MATLAB_ENABLED

	switch(MatlabComms::loop()) {	// Check Matlab messages
		case 1: error(1); break;	// No messages timeout
		case 2: error(2); break;	// Invalid message type byte
		case 3: error(3); break;	// Teleop data timeout
		default: break;
	}
	if(MatlabComms::disconnected) {	// Matlab sent DC message
		DriveSystem::stop();
		IndicatorLed::led.off();
		while(1);
	}

#endif
}

//!b Stops robot driving and flashes LED n times in a loop.
void FireBot::error(uint8_t n) {
	DriveSystem::stop();
	IndicatorLed::flash(n);
}
