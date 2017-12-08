//*************************************************************//
// TITLE
//*************************************************************//

//!t FireBot.cpp
//!a RBE-2002 B17 Team 10

#include "FireBot.h"
#include "IndicatorLed.h"
#include "Odometer.h"
#include "Sonar.h"
#include "DriveSystem.h"
#include "WallFollower.h"
#include "FlameFinder.h"
#include "MatlabComms.h"

//*************************************************************//
// NAMESPACE FIELD DEFINITIONS
//*************************************************************//

namespace FireBot {

	// State Machine
	enum {
		STATE_FIND_FIRE = 1,
		STATE_EXTINGUISH_FIRE = 2,
		STATE_RETURN_HOME = 3,
		STATE_AT_HOME = 4
	} state;

	// Constants
	const float HOME_DISTANCE_THRESHOLD = 0.2; // (m)
}

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
	FlameFinder::setup();

	IndicatorLed::led.on();	// Indicate setup is complete

#ifdef MATLAB_ENABLED

	// Wait for Matlab begin message
	if(!MatlabComms::waitForBegin()) {
		error(3);	// Indicate Matlab sent wrong byte
	}

#endif

	// Initialize state machine
	state = STATE_FIND_FIRE;
}

//!b Executes repeatedly after Arduino reset.
//!d Call this method in the main loop function.
void FireBot::loop() {

	// Subsystem Updates
	Odometer::loop();		// Update position and angle
	FlameFinder::loop();	// Run flame-finder state machine
	if(state != STATE_EXTINGUISH_FIRE) {
		Sonar::loop();			// Update robot sonar
		WallFollower::loop();	// Run wall-follow state machine
	}

	// State Machine
	switch(state) {

		// Wall-follow until fire detected
		case STATE_FIND_FIRE:
			if(false) {
				WallFollower::stop();
				state = STATE_EXTINGUISH_FIRE;
			}
			break;

		// Stop and put out flame
		case STATE_EXTINGUISH_FIRE:
			if(FlameFinder::extinguishedFlame) {
				WallFollower::start();
				state = STATE_RETURN_HOME;
			}
			break;

		// Wall-follow back to home
		case STATE_RETURN_HOME:
			if(Odometer::nearHome()) {
				WallFollower::stop();
				state = STATE_AT_HOME;
			}
			break;

		// Do nothing (at home)
		case STATE_AT_HOME:
			break;
	}

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

//!b Returns byte representing state of robot (see above).
byte FireBot::getState() {
	return (byte)state;
}

//!b Stops robot driving and flashes LED n times in a loop.
void FireBot::error(uint8_t n) {
	DriveSystem::stop();
	IndicatorLed::flash(n);
}
