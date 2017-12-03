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
	Sonar::setup();
	WallFollower::setup();
	if(!Odometer::setup()) error(1); // IMU failure
	if(!MatlabComms::setup()) error(2); // Hc06 failure

	IndicatorLed::led.on();
#ifdef MATLAB_ENABLED
	// Wait for Matlab begin message
	if(!MatlabComms::waitForBegin()) error(3);
#endif

	// Begin wall-following
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

	// Update sonar distances
	Sonar::loop();


#ifdef MATLAB_ENABLED
	// Check messages from Matlab
	switch(MatlabComms::loop()) {
		case 1: error(1); break; // No messages timeout
		case 2: error(2); break; // Invalid message type byte
		case 3: error(3); break; // Teleop data timeout
		default: break;
	}
	if(MatlabComms::disconnected) {
		DriveSystem::stop();
		IndicatorLed::led.off();
		while(1);
	}
#endif

	// Run one iteration of wall-follower
	WallFollower::loop();
}

//!b Stops robot driving and flashes LED n times in a loop.
void FireBot::error(uint8_t n) {
	DriveSystem::stop();
	IndicatorLed::flash(n);
}
