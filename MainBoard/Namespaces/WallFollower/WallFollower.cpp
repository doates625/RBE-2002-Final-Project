//**************************************************************/
// TITLE
//**************************************************************/

//!t WallFollower.cpp
//!a Dan Oates (RBE-2002 B17 Team 10)

#include "WallFollower.h"
#include "Sonar.h"
#include "Odometer.h"
#include "DriveSystem.h"
#include "PidController.h"

//**************************************************************/
// NAMESPACE FIELD DEFINITIONS
//**************************************************************/

namespace WallFollower {

	// Arduino Pin Settings
	const uint8_t PIN_CLIFFSENSE_L = A10;
	const uint8_t PIN_CLIFFSENSE_R = A11;

	// Wall-following Parameters
	const float WALL_DISTANCE 		 = 0.23;	// From VTC (m)
	const float LEFT_WALL_TOLERANCE  = 0.1;		// (m)
	const float FRONT_WALL_TOLERANCE = 0.02;	// (m)
	const float DRIVE_VELOCITY_MAX	 = 0.15;	// (m/s)
	const float PRE_TURN_DISTANCE	 = 0.047;	// (m)
	const float CLIFF_BACK_DISTANCE  =
		WALL_DISTANCE - 0.0353;				// (m)
	const float WALL_CHECK_DIST = 0.062;	// (m)

	// Derived Parameters
	const float PRE_TURN_TIME 	=
		PRE_TURN_DISTANCE / DRIVE_VELOCITY_MAX; 	// (s)
	const float CLIFF_BACK_TIME =
		CLIFF_BACK_DISTANCE / DRIVE_VELOCITY_MAX; 	// (s)
	const float WALL_CHECK_TIME =
		WALL_CHECK_DIST / DRIVE_VELOCITY_MAX;  		// (s)

	// Wall-Following State
	enum state_t {
		STATE_STOPPED = 1,
		STATE_FORWARD = 2,
		STATE_CHECK_LEFT = 3,
		STATE_PRE_TURN_LEFT = 4,
		STATE_TURN_LEFT = 5,
		STATE_POST_TURN = 6,
		STATE_BACK_FROM_CLIFF = 7,
		STATE_TURN_RIGHT = 8,
	} state, pausedState;
	enum direction_t {
		POS_Y, // +y
		POS_X, // -y
		NEG_Y, // +x
		NEG_X, // -x
	} direction;
	Timer timer;

	// Left wall-following PID Controller
	// Input: Left wall distance to VTC (m)
	// Output: Heading change (rad)
	const float L_KP = 3.5;
	const float L_KI = 0;
	const float L_KD = 0;
	const float MAX_HEADING_CHANGE = 0.2;
	PidController leftWallPid(L_KP, L_KI, L_KD,
		-MAX_HEADING_CHANGE,
		+MAX_HEADING_CHANGE);
	float headingOffset = 0;
	float driveHeading = 0;

	// Front wall distance PID controller
	// Input: Front wall distance to VTC (m)
	// Output: Forward drive velocity (m/s)
	const float F_KP = 1.5;
	const float F_KI = 0;
	const float F_KD = 0;
	PidController frontWallPid(F_KP, F_KI, F_KD,
		0, DRIVE_VELOCITY_MAX);
	float driveVelocity = DRIVE_VELOCITY_MAX;

	// Private Function Templates
	bool nearLeftWall();
	bool nearFrontWall();
	bool nearCliff();
	void setDirectionLeft();
	void setDirectionRight();
}

//**************************************************************/
// NAMESPACE FUNCTION DEFINITIONS
//**************************************************************/

//!b Initializes wallfollower namespace.
//!d Also sets state to forward.
void WallFollower::setup() {
	pinMode(PIN_CLIFFSENSE_L, INPUT);
	pinMode(PIN_CLIFFSENSE_R, INPUT);
	direction = POS_Y;
	pausedState = STATE_FORWARD;
	start();
}

//!b Initializes wall-follower to state before it was stopped.
void WallFollower::start() {
	driveVelocity = DRIVE_VELOCITY_MAX;
	timer.resume();
	state = pausedState;
}

//!b Stops drive system and sets wall-follower to stopped state.
void WallFollower::stop() {
	DriveSystem::stop();
	pausedState = state;
	timer.pause();
	state = STATE_STOPPED;
}

//!b Returns true if robot is near left wall.
//!d If left sonar is invalid, assumes true.
bool WallFollower::nearLeftWall() {
	if(Sonar::distL == 0) {
		return true;
	} else {
		return (Sonar::distL <=
			WALL_DISTANCE +
			LEFT_WALL_TOLERANCE);
	}
}

//!b Returns true if robot is near front wall.
//!b If front sonar is invalid, assumes false.
bool WallFollower::nearFrontWall() {
	if(Sonar::distF == 0) {
		return false;
	} else {
		return (Sonar::distF <=
			WALL_DISTANCE +
			FRONT_WALL_TOLERANCE);
	}
}

//!b Returns true if robot is near a cliff.
bool WallFollower::nearCliff() {
	return
		analogRead(PIN_CLIFFSENSE_L) >= 500 ||
		analogRead(PIN_CLIFFSENSE_R) >= 500;
}

//!b Performs wall-following loop.
void WallFollower::loop() {
	switch(state) {

		// Stopped (no movement)
		case STATE_STOPPED:
			break;

		// Driving forwards
		case STATE_FORWARD:
			// Wall following
			if(Sonar::distL != 0) {
				headingOffset = leftWallPid.update(
					WALL_DISTANCE - Sonar::distL);
				driveHeading =
					targetHeading() + headingOffset;
			} else {
				driveHeading = targetHeading();
			}
			if(Sonar::distF != 0) {
				driveVelocity = frontWallPid.update(
					Sonar::distF - WALL_DISTANCE);
			}
			DriveSystem::drive(
				driveHeading,
				driveVelocity);

			// State changes
			if(nearCliff()) {
				stop();
				timer.tic();
				state = STATE_BACK_FROM_CLIFF;
			}
			if(!nearLeftWall()) {
				timer.tic();
				state = STATE_CHECK_LEFT;
			}
			if(nearFrontWall()) {
				setDirectionRight();
				state = STATE_TURN_RIGHT;
			}
			break;

		// Drive straight then re-check left side
		case STATE_CHECK_LEFT:
			DriveSystem::drive(
				targetHeading(),
				DRIVE_VELOCITY_MAX);
			if(nearCliff()) {
				stop();
				timer.tic();
				state = STATE_BACK_FROM_CLIFF;
			}
			if(nearLeftWall()) {
				driveVelocity = DRIVE_VELOCITY_MAX;
				state = STATE_FORWARD;
			} else if(timer.hasElapsed(WALL_CHECK_TIME)) {
				timer.tic();
				state = STATE_PRE_TURN_LEFT;
			}
			break;

		// Drive forwards before left turn
		case STATE_PRE_TURN_LEFT:
			DriveSystem::drive(
				targetHeading(),
				DRIVE_VELOCITY_MAX);
			if(nearCliff()) {
				stop();
				timer.tic();
				state = STATE_BACK_FROM_CLIFF;
			}
			if(nearFrontWall()) {
				setDirectionRight();
				state = STATE_TURN_RIGHT;
			}
			if(timer.hasElapsed(PRE_TURN_TIME)) {
				setDirectionLeft();
				state = STATE_TURN_LEFT;
			}
			break;

		// Make a 90-degree left turn
		case STATE_TURN_LEFT:
			if(DriveSystem::drive(targetHeading())) {
				state = STATE_POST_TURN;
			}
			break;

		// Drive forwards after a turn
		case STATE_POST_TURN:
			DriveSystem::drive(
				targetHeading(),
				DRIVE_VELOCITY_MAX);
			if(nearCliff()) {
				stop();
				timer.tic();
				state = STATE_BACK_FROM_CLIFF;
			}
			if(nearLeftWall()) {
				driveVelocity = DRIVE_VELOCITY_MAX;
				state = STATE_FORWARD;
			}
			if(nearFrontWall()) {
				setDirectionRight();
				state = STATE_TURN_RIGHT;
			}
			break;

		// Back away from a cliff
		case STATE_BACK_FROM_CLIFF:
			DriveSystem::drive(
				targetHeading(),
				-DRIVE_VELOCITY_MAX);
			if(timer.hasElapsed(CLIFF_BACK_TIME)) {
				setDirectionRight();
				state = STATE_TURN_RIGHT;
			}
			break;

		// Make a 90-degree right turn
		case STATE_TURN_RIGHT:
			if(DriveSystem::drive(targetHeading())) {
				state = STATE_POST_TURN;
			}
			break;
	}
}

//!b Returns byte indicating current state.
//!d See state enumeration above for mapping details.
byte WallFollower::getState() {
	return (byte)state;
}

//!b Returns true if wall follower can be paused without issues.
bool WallFollower::inPausableState() {
	return
		state == STATE_FORWARD ||
		state == STATE_POST_TURN;
}

//!b Returns heading (rad) based on wall-following direction.
float WallFollower::targetHeading() {
	switch(direction) {
		case POS_Y: return 0.000000;
		case POS_X: return 1.570796;
		case NEG_Y: return 3.141593;
		case NEG_X: return 4.712389;
	}
	return 0;
}

//!b Sets wall-following direction to 90 degrees to the left.
void WallFollower::setDirectionLeft() {
	switch(direction) {
		case POS_Y: direction = NEG_X; break;
		case POS_X: direction = POS_Y; break;
		case NEG_Y: direction = POS_X; break;
		case NEG_X: direction = NEG_Y; break;
	}
}

//!b Sets wall-following direction to 90 degrees to the right.
void WallFollower::setDirectionRight() {
	switch(direction) {
		case POS_Y: direction = POS_X; break;
		case POS_X: direction = NEG_Y; break;
		case NEG_Y: direction = NEG_X; break;
		case NEG_X: direction = POS_Y; break;
	}
}
