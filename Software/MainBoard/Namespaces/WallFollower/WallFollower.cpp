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

	// State
	enum state_t {
		STOPPED = 1,
		FORWARD = 2,
		CHECK_LEFT = 3,
		PRE_TURN_LEFT = 4,
		TURN_LEFT = 5,
		POST_TURN_LEFT = 6,
		TURN_RIGHT = 7,
	} state;
	enum direction_t {
		POS_Y,
		POS_X,
		NEG_Y,
		NEG_X,
	} direction;

	// Wall-following parameters
	const float WALL_DISTANCE = 0.23; // from VTC (m)
	const float WALL_TOLERANCE = 0.1; // (m)

	// Timer for pre- and post-turns
	const float PRE_TURN_TIME = 1.5; // (s)
	Timer driveTimer;

	// Left wall distance checking
	const float LEFT_WALL_CHECK_TIME = 0.5; // (s)
	Timer leftWallTimer;

	// Wall following PID
	// Input: Wall distance (m)
	// Output: Heading change (rad)
	const float KP = 3.5;
	const float KI = 0;
	const float KD = 0;
	const float MAX_HEADING_CHANGE = 0.15; // rad
	const float RESET_TIME = 0.2;
	PidController wallPid(KP, KI, KD,
		-MAX_HEADING_CHANGE,
		+MAX_HEADING_CHANGE,
		RESET_TIME);
	float headingOffset = 0;
	float wallFollowHeading = 0;
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes wallfollower namespace.
void WallFollower::setup() {
	state = STOPPED;
	direction = POS_Y;
}

//!b Initializes wallfollower state machine.
void WallFollower::begin() {
	state = FORWARD;
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
				wallFollowHeading =
					targetHeading() + headingOffset;
				if(wallFollowHeading < 0) {
					wallFollowHeading += TWO_PI;
				} else
				if(wallFollowHeading > TWO_PI) {
					wallFollowHeading -= TWO_PI;
				}
			} else {
				wallFollowHeading = targetHeading();
			}
			DriveSystem::driveAtHeading(
				wallFollowHeading,
				true);

			// State changes
			if(!nearLeftWall()) {
				leftWallTimer.tic();
				state = CHECK_LEFT;
			}
			if(nearFrontWall()) {
				setDirectionRight();
				state = TURN_RIGHT;
			}
			break;

		// Drive straight then re-check left side
		case CHECK_LEFT:
			DriveSystem::driveAtHeading(
				targetHeading(),
				true);
			if(nearLeftWall()) {
				state = FORWARD;
			} else if(leftWallTimer.hasElapsed(
				LEFT_WALL_CHECK_TIME))
			{
				driveTimer.tic();
				state = PRE_TURN_LEFT;
			}
			break;

		// Drive forwards before left turn
		case PRE_TURN_LEFT:
			DriveSystem::driveAtHeading(
				targetHeading(),
				true);
			if(driveTimer.hasElapsed(PRE_TURN_TIME)) {
				setDirectionLeft();
				state = TURN_LEFT;
			}
			break;

		// Make a left turn
		case TURN_LEFT:
			if(DriveSystem::driveAtHeading(
				targetHeading(),
				false))
			{
				state = POST_TURN_LEFT;
			}
			break;

		// Drive forwards after left turn
		case POST_TURN_LEFT:
			DriveSystem::driveAtHeading(
				targetHeading(),
				true);
			if(nearLeftWall()) {
				state = FORWARD;
			}
			break;

		// Make a right turn
		case TURN_RIGHT:
			if(DriveSystem::driveAtHeading(
				targetHeading(),
				false))
			{
				state = FORWARD;
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
