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
	const float WALL_DISTANCE 		 = 0.23;	// (m)
	const float LEFT_WALL_TOLERANCE  = 0.1;		// (m)
	const float FRONT_WALL_TOLERANCE = 0.02;	// (m)
	const float DRIVE_VELOCITY_MAX	 = 0.15;	// (m/s)
	const float PRE_TURN_DIST		 = 0.047;	// (m)
	const float CLIFF_BACK_DIST 	 =
		WALL_DISTANCE - 0.0353;				// (m)
	const float WALL_CHECK_DIST = 0.062;	// (m)
	const float PID_RESET_TIME  = 0.2;		// (s)

	// Derived Parameters
	const float PRE_TURN_TIME 	=
		PRE_TURN_DIST / DRIVE_VELOCITY_MAX; 	// (s)
	const float CLIFF_BACK_TIME =
		CLIFF_BACK_DIST / DRIVE_VELOCITY_MAX; 	// (s)
	const float WALL_CHECK_TIME =
		WALL_CHECK_DIST / DRIVE_VELOCITY_MAX;  	// (s)

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

	// Left wall-following PID Controller
	// Input: Wall distance (m)
	// Output: Heading change (rad)
	const float L_KP = 3.5;
	const float L_KI = 0;
	const float L_KD = 0;
	const float MAX_HEADING_CHANGE = 0.15;
	PidController leftWallPid(L_KP, L_KI, L_KD,
		-MAX_HEADING_CHANGE,
		+MAX_HEADING_CHANGE,
		PID_RESET_TIME);
	float headingOffset = 0;
	float driveHeading = 0;

	// Front wall distance PID controller
	// Input: Wall distance (m)
	// Output: Forward drive velocity (m/s)
	const float F_KP = 1.5;
	const float F_KI = 0;
	const float F_KD = 0;
	PidController frontWallPid(F_KP, F_KI, F_KD,
		0, DRIVE_VELOCITY_MAX,
		PID_RESET_TIME);
	float driveVelocity = DRIVE_VELOCITY_MAX;
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
	start();
}

//!b Initializes wall-follower to forward state.
void WallFollower::start() {
	driveVelocity = DRIVE_VELOCITY_MAX;
	state = FORWARD;
}

//!b Stops drive system and sets wall-follower to stopped state.
void WallFollower::stop() {
	DriveSystem::stop();
	state = STOPPED;
}

//!b Returns byte indicating current state.
//!d See state enumeration above for mapping details.
byte WallFollower::getState() {
	return (byte)state;
}

//!b Returns true if wall-follower is in forward state.
bool WallFollower::inForwardState() {
	return state == FORWARD;
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

//!b Returns true if robot is near cliff.
bool WallFollower::nearCliff() {
	return
		analogRead(PIN_CLIFFSENSE_L) >= 500 ||
		analogRead(PIN_CLIFFSENSE_R) >= 500;
}

//!b Performs wall-following loop.
void WallFollower::loop() {
	switch(state) {

		// Stopped (no movement)
		case STOPPED:
			break;

		// Driving forwards
		case FORWARD:
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
				timer.tic();
				state = BACK_FROM_CLIFF;
			}
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
				DRIVE_VELOCITY_MAX);
			if(nearCliff()) {
				timer.tic();
				state = BACK_FROM_CLIFF;
			}
			if(nearLeftWall()) {
				driveVelocity = DRIVE_VELOCITY_MAX;
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
				DRIVE_VELOCITY_MAX);
			if(nearCliff()) {
				timer.tic();
				state = BACK_FROM_CLIFF;
			}
			if(nearFrontWall()) {
				setDirectionRight();
				state = TURN_RIGHT;
			}
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
				DRIVE_VELOCITY_MAX);
			if(nearCliff()) {
				timer.tic();
				state = BACK_FROM_CLIFF;
			}
			if(nearLeftWall()) {
				driveVelocity = DRIVE_VELOCITY_MAX;
				state = FORWARD;
			}
			if(nearFrontWall()) {
				setDirectionRight();
				state = TURN_RIGHT;
			}
			break;

		// Back away from a cliff
		case BACK_FROM_CLIFF:
			DriveSystem::drive(
				targetHeading(),
				-DRIVE_VELOCITY_MAX);
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

//!b Runs test of cliff sensor at Serial 115200
void WallFollower::serialTest() {
	setup();
	Serial.begin(115200);
	Serial.println("Cliff Sensor Test");

	Timer timer;
	timer.tic();

	while(1) {
		if(timer.hasElapsed(0.2)) {
			timer.tic();
			Serial.println("L: " + String(
				analogRead(PIN_CLIFFSENSE_L)));
			Serial.println("R: " + String(
				analogRead(PIN_CLIFFSENSE_R)));
			Serial.println("Cliff Status: " + String(nearCliff()));
		}
	}
}
