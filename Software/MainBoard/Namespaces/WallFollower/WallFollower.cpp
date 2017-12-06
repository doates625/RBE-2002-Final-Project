//**************************************************************/
// TITLE
//**************************************************************/

//!t WallFollower.cpp
//!a RBE-2002 B17 Team 10

#include "WallFollower.h"
#include "Sonar.h"
#include "Odometer.h"
#include "DriveSystem.h"
#include "PidController.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace WallFollower {

	// Arduino Pin Definitions
	const uint8_t PIN_CLIFFSENSE_L = A10;
	const uint8_t PIN_CLIFFSENSE_R = A11;

	// Wall-following Parameters
	const float WALL_DISTANCE 	= 0.23;	// (m)
	const float WALL_TOLERANCE 	= 0.1;	// (m)
	const float DRIVE_VELOCITY	= 0.1;	// (m/s)
	const float PRE_TURN_TIME 	= 1.5; 	// (s)
	const float CLIFF_BACK_TIME = 1.8;	// (s)
	const float WALL_CHECK_TIME = 0.5;  // (s)

	// Wall-Following State
	enum state_t {
		STOPPED = 1,
		FORWARD = 2,
		CHECK_LEFT = 3,
		PRE_TURN_LEFT = 4,
		TURN_LEFT = 5,
		POST_TURN = 6,
		BACK_FROM_CLIFF = 7,
		TURN_RIGHT = 8,
	} state;
	enum direction_t {
		POS_Y,
		POS_X,
		NEG_Y,
		NEG_X,
	} direction;
	Timer timer;

	// Wall following PID Controller
	// Input: Wall distance (m)
	// Output: Heading change (rad)
	const float KP = 3.5;
	const float KI = 0;
	const float KD = 0;
	const float MAX_HEADING_CHANGE = 0.15;
	const float RESET_TIME = 0.2;
	PidController wallPid(KP, KI, KD,
		-MAX_HEADING_CHANGE,
		+MAX_HEADING_CHANGE,
		RESET_TIME);
	float headingOffset = 0;
	float driveHeading = 0;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes wallfollower namespace.
void WallFollower::setup() {
	pinMode(PIN_CLIFFSENSE_L, INPUT);
	pinMode(PIN_CLIFFSENSE_R, INPUT);
	state = FORWARD;
	direction = POS_Y;
}

//!b Returns byte indicating current state.
//!d See state enumeration above for mapping details.
byte WallFollower::getState() {
	return (byte)state;
}

//!b Returns true if robot is near left wall.
//!d If left sonar is invalid, assumes true.
bool WallFollower::nearLeftWall() {
	if(Sonar::distL == 0) {
		return true;
	} else {
		return (Sonar::distL <= WALL_DISTANCE + WALL_TOLERANCE);
	}
}

//!b Returns true if robot is near front wall.
//!b If front sonar is invalid, assumes false.
bool WallFollower::nearFrontWall() {
	if(Sonar::distF == 0)
		return false;
	else
		return (Sonar::distF <= WALL_DISTANCE);
}

//!b Returns true if robot is near cliff.
bool WallFollower::nearCliff() {
	int floorDarkness =
		analogRead(PIN_CLIFFSENSE_L) +
		analogRead(PIN_CLIFFSENSE_R);
	return floorDarkness >= 1400;
}

//!b Performs wall-following loop.
void WallFollower::loop() {
	switch(state) {

		// Stopped (no movement)
		case STOPPED:
			DriveSystem::stop();
			break;

		// Driving forwards
		case FORWARD:
			// Wall following
			if(Sonar::distL != 0) {
				headingOffset = wallPid.update(
					WALL_DISTANCE - Sonar::distL);
				driveHeading =
					targetHeading() + headingOffset;
			} else {
				driveHeading = targetHeading();
			}
			DriveSystem::drive(
				driveHeading,
				DRIVE_VELOCITY);

			// State changes
			if(!nearLeftWall()) {
				timer.tic();
				state = CHECK_LEFT;
			}
			if(nearFrontWall()) {
				setDirectionRight();
				state = TURN_RIGHT;
			}
			break;

		// Drive straight then re-check left side
		case CHECK_LEFT:
			DriveSystem::drive(
				targetHeading(),
				DRIVE_VELOCITY);
			if(nearLeftWall()) {
				state = FORWARD;
			} else if(timer.hasElapsed(WALL_CHECK_TIME)) {
				timer.tic();
				state = PRE_TURN_LEFT;
			}
			break;

		// Drive forwards before left turn
		case PRE_TURN_LEFT:
			DriveSystem::drive(
				targetHeading(),
				DRIVE_VELOCITY);
			if(timer.hasElapsed(PRE_TURN_TIME)) {
				setDirectionLeft();
				state = TURN_LEFT;
			}
			break;

		// Make a left turn
		case TURN_LEFT:
			if(DriveSystem::drive(targetHeading())) {
				state = POST_TURN;
			}
			break;

		// Drive forwards after a turn
		case POST_TURN:
			DriveSystem::drive(
				targetHeading(),
				DRIVE_VELOCITY);
			if(nearCliff()) {
				timer.tic();
				state = BACK_FROM_CLIFF;
			}
			if(nearLeftWall()) {
				state = FORWARD;
			}
			break;

		// Back away from a cliff
		case BACK_FROM_CLIFF:
			DriveSystem::drive(
				targetHeading(),
				-DRIVE_VELOCITY);
			if(timer.hasElapsed(CLIFF_BACK_TIME)) {
				setDirectionRight();
				state = TURN_RIGHT;
			}
			break;

		// Make a right turn
		case TURN_RIGHT:
			if(DriveSystem::drive(targetHeading())) {
				state = POST_TURN;
			}
			break;
	}
}

//!b Returns target heading (rad) based on current direction.
float WallFollower::targetHeading() {
	switch(direction) {
		case POS_Y: return 0.000000;
		case POS_X: return 1.570796;
		case NEG_Y: return 3.141593;
		case NEG_X: return 4.712389;
	}
	return 0;
}

//!b Sets direction to 90 degrees to the left.
void WallFollower::setDirectionLeft() {
	switch(direction) {
		case POS_Y: direction = NEG_X; break;
		case POS_X: direction = POS_Y; break;
		case NEG_Y: direction = POS_X; break;
		case NEG_X: direction = NEG_Y; break;
	}
}

//!b Sets direction to 90 degrees to the right.
void WallFollower::setDirectionRight() {
	switch(direction) {
		case POS_Y: direction = POS_X; break;
		case POS_X: direction = NEG_Y; break;
		case NEG_Y: direction = NEG_X; break;
		case NEG_X: direction = POS_Y; break;
	}
}
