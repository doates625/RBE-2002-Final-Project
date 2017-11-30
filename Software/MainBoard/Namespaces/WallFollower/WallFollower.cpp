//**************************************************************/
// TITLE
//**************************************************************/

//!t WallFollower.cpp
//!a RBE-2002 B17 Team 10

#include "WallFollower.h"
#include "Sonar.h"
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
		PRE_TURN_LEFT = 3,
		TURN_LEFT = 4,
		POST_TURN_LEFT = 5,
		TURN_RIGHT = 6,
	} state;
	enum direction_t {
		POS_Y,
		POS_X,
		NEG_Y,
		NEG_X,
	} direction;

	// Wall-following parameters
	const float DRIVE_VOLTAGE = 2.5; // for forward drive (V)
	const float WALL_DISTANCE = 0.2; // from VTC (m)

	// Wall Distance PID Controller
	// Input: Wall distance (m)
	// Output: Robot yawrate (rad/s)
	const float KP = 25.0;
	const float KI = 0.0;
	const float KD = 10.0;
	const float YAW_MAX = 2.0; // Max yawrate (rad/s)
	const float RESET_TIME = 0.5; // (s)

	PidController wallPid(KP, KI, KD,
		-YAW_MAX,
		+YAW_MAX,
		RESET_TIME);

	// Timer for pre- and post-turns
	Timer driveTimer;
	const float PRE_TURN_TIME = 1.5; // (s)
	const float POST_TURN_TIME = 3.0; // (s)
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
	if(Sonar::distL == 0)
		return true;
	else
		return (Sonar::distL <= WALL_DISTANCE + 0.2);
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
			if(Sonar::distL != 0) {
				DriveSystem::driveAtYawrate(
					DRIVE_VOLTAGE,
					wallPid.update(
						WALL_DISTANCE - Sonar::distL));
			}
			if(!nearLeftWall()) {
				driveTimer.tic();
				state = PRE_TURN_LEFT;
			}
			if(nearFrontWall()) {
				setDirectionRight();
				state = TURN_RIGHT;
			}
			break;

		// Drive forwards before left turn
		case PRE_TURN_LEFT:
			DriveSystem::driveAtHeading(
				DRIVE_VOLTAGE,
				targetHeading());
			if(driveTimer.hasElapsed(PRE_TURN_TIME)) {
				setDirectionLeft();
				state = TURN_LEFT;
			}
			break;

		// Make a left turn
		case TURN_LEFT:
			if(DriveSystem::driveAtHeading(0, targetHeading())) {
				driveTimer.tic();
				state = POST_TURN_LEFT;
			}
			break;

		// Drive forwards after left turn
		case POST_TURN_LEFT:
			DriveSystem::driveAtHeading(
				DRIVE_VOLTAGE,
				targetHeading());
			if(driveTimer.hasElapsed(POST_TURN_TIME)) {
				if(nearLeftWall()) {
					state = FORWARD;
				} else {
					setDirectionLeft();
					state = TURN_LEFT;
				}
			}
			break;

		// Make a right turn
		case TURN_RIGHT:
			if(DriveSystem::driveAtHeading(0, targetHeading())) {
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
